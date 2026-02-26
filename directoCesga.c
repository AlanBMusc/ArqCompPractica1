#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "counter.h"

#define CLS 64   // Tamaño línea caché (bytes)

/* Inicialización completa */
void init_array(double *A, int size) {
    for (int i = 0; i < size; i++) {
        double val = 1.0 + (double)rand() / RAND_MAX;
        int sign = (rand() % 2) ? 1 : -1;
        A[i] = sign * val;
    }
}

int main(int argc, char **argv) {

    if (argc < 3) {
        printf("Uso: %s D L\n", argv[0]);
        return 1;
    }

    int D = atoi(argv[1]);
    int L = atoi(argv[2]);

    /* Cálculo de R según tu criterio */
    int R;

    if (D < 8)
        R = (L * 8) / D;   // 8 doubles por línea
    else
        R = L;

    srand(1);

    int N = R * D;

    /* Asegurar múltiplo de CLS */
    size_t bytes = N * sizeof(double);
    size_t padded = ((bytes + CLS - 1) / CLS) * CLS;

    double *A = aligned_alloc(CLS, padded);

    if (!A) {
        printf("Error reservando memoria\n");
        return 1;
    }

    init_array(A, N);

    double S[10];
    double suma;
    double ck;

    start_counter();

    for (int k = 0; k < 10; k++) {
        suma = 0.0;

        for (int i = 0; i < R; i++) {
            suma += A[i * D];   // ACCESO DIRECTO
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
