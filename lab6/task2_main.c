#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <math.h>
#include <string.h>
#include <errno.h>

ssize_t read_n(int fd, void *buf, size_t n) {
    size_t total = 0;
    char *p = buf;
    while (total < n) {
        ssize_t r = read(fd, p + total, n - total);
        if (r < 0) {
            if (errno == EINTR) continue;
            return -1;
        }
        if (r == 0) break;
        total += r;
    }
    return (ssize_t) total;
}

int main() {
    int K, N, n;
    printf("Введите K N n (через пробел): ");
    if (scanf("%d %d %d", &K, &N, &n) != 3) {
        fprintf(stderr, "Bad input\n");
        return 1;
    }

    double PI = acos(-1.0);

    FILE *f = fopen("result.txt", "w");
    if (!f) {
        perror("fopen");
        return 1;
    }

    for (int i = 0; i < K; ++i) {
        double x = 2.0 * PI * i / (double)N;

        double y = 0.0;

        for (int k = 0; k < n; ++k) {
            int fd[2];
            if (pipe(fd) == -1) {
                perror("pipe");
                fclose(f);
                return 1;
            }

            pid_t pid = fork();
            if (pid < 0) {
                perror("fork");
                close(fd[0]); close(fd[1]);
                fclose(f);
                return 1;
            }

            if (pid == 0) {
                close(fd[0]);
                if (dup2(fd[1], STDOUT_FILENO) == -1) {
                    perror("dup2");
                    _exit(2);
                }
                close(fd[1]);

                char argk[32], argx[64];
                sprintf(argk, "%d", k);
                sprintf(argx, "%.17g", x);

                char *args[] = {"task2_term_calc", argk, argx, NULL};
                execvp("./task2_term_calc", args);
                perror("execvp");
                _exit(3);
            } else {
                close(fd[1]);

                double term;
                ssize_t got = read_n(fd[0], &term, sizeof(term));
                close(fd[0]);

                if (got != sizeof(term)) {
                    fprintf(stderr, "Parent: read error or incomplete read for i=%d k=%d (got=%zd)\n", i, k, got);
                    term = 0.0;
                }

                int status;
                waitpid(pid, &status, 0);

                y += term;
            }
        }

        printf("y[%d] = %.12g\n", i, y);
        fprintf(f, "y[%d] = %.12g\n", i, y);
    }

    fclose(f);
    return 0;
}