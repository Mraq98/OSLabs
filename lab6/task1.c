#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <time.h>

void print_info(const char *label) {
    struct timeval tv;
    gettimeofday(&tv, NULL);

    struct tm *tm_info = localtime(&tv.tv_sec);

    printf("[%s] PID=%d, PPID=%d, Time: %02d:%02d:%02d:%03ld\n",
           label,
           getpid(),
           getppid(),
           tm_info->tm_hour,
           tm_info->tm_min,
           tm_info->tm_sec,
           tv.tv_usec / 1000);
}

int main() {
    print_info("Parent start");

    pid_t pid1 = fork();

    if (pid1 == 0) {
        print_info("Child 1");
        exit(0);
    }

    pid_t pid2 = fork();

    if (pid2 == 0) {
        print_info("Child 2");
        exit(0);
    }

    wait(NULL);
    wait(NULL);

    print_info("Parent after children");

    printf("\n--- Running 'ps -x' ---\n");
    system("ps -x");

    return 0;
}
