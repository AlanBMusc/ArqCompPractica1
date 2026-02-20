#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "counter.h"

#define CLS 64

void init_array(int *A, int size, int D) {
    for (int i = 0; i < size; i += D) {
        int val = rand();
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

    int *A = aligned_alloc(CLS, N * sizeof(int));
    int *ind = malloc(R * sizeof(int));

    if (!A || !ind) {
        printf("Error reservando memoria\n");
        return 1;
    }

    init_array(A, N, D);

    for (int i = 0; i < R; i++) {
        ind[i] = i * D;
    }

    int S[10];
    int suma;
    double ck;

    start_counter();

    for (int k = 0; k < 10; k++) {
        suma = 0;

        for (int i = 0; i < R; i++) {
            suma += A[ind[i]];
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
    free(ind);

    return 0;
}
