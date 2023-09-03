#include <iostream>
#include <time.h>
#include <omp.h>

const int NMAX = 5000;
const int LIMIT = 5000;


int main() {
    int i, j;
    float sum;
    float** a = new float* [NMAX];
    for (int i = 0; i < NMAX; ++i) {
        a[i] = new float[NMAX];
    }

    for (i = 0; i < NMAX; ++i) {
        for (j = 0; j < NMAX; ++j) a[i][j] = i + j;
    }
    time_t start1 = clock();

    for (i = 0; i < NMAX; ++i) {
        sum = 0;
        for (j = 0; j < NMAX; ++j) {
            sum += a[i][j];
        }
        //printf ("Сумма элементов строки %d равна %f\n", i, sum);
    }
   
    time_t end1 = clock();
    float executionTime = static_cast<float>(end1 - start1) / CLOCKS_PER_SEC;
    printf("Seconds spended without threads: %f\n", executionTime);

    time_t start2 = clock();
#pragma omp parallel shared(a) if (NMAX > LIMIT)
    {
#pragma omp for private(i, j, sum) 
        for (i = 0; i < NMAX; ++i) {
            sum = 0;
            for (j = 0; j < NMAX; ++j) {
                sum += a[i][j];
            }
            //printf ("Сумма элементов строки %d равна %f\n", i, sum);
        }
    }
    time_t end2 = clock();
    executionTime = static_cast<float>(end2 - start2) / CLOCKS_PER_SEC;
    printf("Seconds spended with threads: %f\n", executionTime);
    for (int i = 0; i < NMAX; ++i) {
        delete[] a[i];
    }
    delete[] a;
    return 0;
}
