#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <unistd.h>

int main(int argc, char *argv[]) {
    if (argc < 3) {
        fprintf(stderr, "Usage: %s k x\n", argv[0]);
        return 1;
    }

    int k = atoi(argv[1]);
    double x = atof(argv[2]);

    double PI = acos(-1.0);
    x = remainder(x, 2.0 * PI);

    double term = x;
    if (k == 0) {
    } else {
        for (int j = 1; j <= k; ++j) {
            double denom1 = 2*(j-1) + 2;
            double denom2 = 2*(j-1) + 3;
            term = term * (-1.0) * (x * x) / (denom1 * denom2);
        }
    }

    fprintf(stderr, "PID=%d term[%d] = %.12g\n", getpid(), k, term);

    ssize_t written = write(STDOUT_FILENO, &term, sizeof(term));
    if (written != sizeof(term)) {
        fprintf(stderr, "Write error\n");
        return 2;
    }

    return 0;
}