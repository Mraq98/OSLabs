#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <math.h>
#include <unistd.h>
#include <sys/syscall.h>

#define PI 3.141592653589793

typedef struct {
    int term_index;
    double x;
    int sign;
    double result;
} term_data_t;

void *compute_term(void *arg) {
    term_data_t *data = (term_data_t *)arg;

    int k = data->term_index;
    double x = data->x;

    double numerator = pow(x, 2*k + 1);

    long long fact = 1;
    for (int i = 2; i <= 2*k + 1; i++)
        fact *= i;

    data->result = data->sign * (numerator / fact);

    printf("[Thread tid=%ld] term %d = %.10f\n",
           syscall(SYS_gettid), k, data->result);

    return NULL;
}

int main() {
    int K, N, n;
    printf("Введите K, N, n: ");
    scanf("%d %d %d", &K, &N, &n);

    FILE *f = fopen("output.txt", "w");
    if (!f) {
        perror("Ошибка открытия файла");
        return 1;
    }

    for (int i = 0; i < K; i++) {
        double x = 2 * PI * i / N;

        pthread_t threads[n];
        term_data_t terms[n];

        for (int k = 0; k < n; k++) {
            terms[k].term_index = k;
            terms[k].x = x;
            terms[k].sign = (k % 2 == 0) ? 1 : -1;
            pthread_create(&threads[k], NULL, compute_term, &terms[k]);
        }

        double sum = 0;
        for (int k = 0; k < n; k++) {
            pthread_join(threads[k], NULL);
            sum += terms[k].result;
        }

        fprintf(f, "y[%d] = %.10f\n", i, sum);
    }

    fclose(f);

    printf("\nРезультаты записаны в output.txt\n");

    return 0;
}
