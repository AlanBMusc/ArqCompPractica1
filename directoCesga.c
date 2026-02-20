#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "counter.h"

#define CLS 64

void init_array(double *A, int size, int D) {
    for (int i = 0; i < size; i += D) {
        double val = 1.0 + (double)rand() / RAND_MAX;
        int sign = (rand() % 2) ? 1 : -1;
        A[i] = sign * val;
    }
}

int main(int argc, char **argv) {

    if (argc < 4) {
        printf("Uso: %s D R L\n", argv[0]);
        return 1;
    }

    int D = atoi(argv[1]);
    int R = atoi(argv[2]);
    int L = atoi(argv[3]);

    srand(1);

    int N = R * D;

    double *A = aligned_alloc(CLS, N * sizeof(double));

    if (!A) {
        printf("Error reservando memoria\n");
        return 1;
    }

    init_array(A, N, D);

    double S[10];
    double suma;
    double ck;

    start_counter();

    for (int k = 0; k < 10; k++) {
        suma = 0.0;

        for (int i = 0; i < R; i++) {
            suma += A[i * D];
        }

        S[k] = suma;
    }

    ck = get_counter();

    double media = 0.0;
    for (int i = 0; i < 10; i++) {
        media += S[i];
    }
    media /= 10.0;

    printf("%d,%d,%d,%.5f,%.5f,%.5f\n",
    D,
    R,
    L,
    media,
    ck / 10.0,
    ck / (10.0 * R));

    free(A);

    return 0;
}
