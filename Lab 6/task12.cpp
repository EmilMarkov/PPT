#include <iostream>
#include <vector>
#include <ctime>
#include <cstdlib>
#include <omp.h>
#include <algorithm>
// Объявляем семафоры
omp_lock_t total_lock;
omp_lock_t max_lock;
void computeWithoutOMP(const int* A, const int* B, int* C, int N) {
    for (int i = 0; i < N; ++i) {
        C[i] = std::max(A[i], B[i]);
    }
}

int main() {
    setlocale(LC_ALL, "RU");
    int N;
    int i;
    std::cout << "Введите размерность массивов: ";
    std::cin >> N;

    int* A = new int[N];
    int* B = new int[N];
    int* C = new int[N];

    std::srand(static_cast<unsigned>(std::time(nullptr)));
    for (int i = 0; i < N; ++i) {
        A[i] = std::rand() % 100;
        B[i] = std::rand() % 100;
    }

    double total_atomic = 0;
    double total_critical = 0;


    double start2 = omp_get_wtime();
#pragma omp parallel shared(total_atomic, A, B, C) private(i)
    {
        double local_total = 0;

#pragma omp for
        for (int i = 0; i < N; ++i) {
            C[i] = std::max(A[i], B[i]);
            local_total += C[i];
        }

#pragma omp atomic
        total_atomic += local_total;
    }
    double end2 = omp_get_wtime();
    double time2 = end2 - start2;

    total_critical = 0;
    double max_local = 0;

#pragma omp parallel shared(total_critical, A, B, C) private(i)
    {
        double local_total = 0;
        double local_max = 0;

#pragma omp for
        for (int i = 0; i < N; ++i) {
            C[i] = std::max(A[i], B[i]);
            local_total += C[i];
            local_max = std::max<int>(local_max, C[i]);
        }

#pragma omp critical
        {
            total_critical += local_total;
            if (local_max > max_local) {
                max_local = local_max;
            }
        }
    }

    double end3 = omp_get_wtime();
    double time3 = end3 - start2;
    double total_with_semaphore = 0;
    max_local = 0;
    double start4 = omp_get_wtime();
    omp_init_lock(&total_lock);
    omp_init_lock(&max_lock);
#pragma omp parallel shared(total_with_semaphore, max_local, A, B, C) private(i)
    {
        double local_total = 0;
        int local_max = 0;

#pragma omp for
        for (int i = 0; i < N; ++i) {
            C[i] = std::max(A[i], B[i]);
            local_total += C[i];
            local_max = std::max(local_max, C[i]);
        }

        omp_set_lock(&total_lock);
        total_with_semaphore += local_total;
        omp_unset_lock(&total_lock);
    }
    omp_destroy_lock(&total_lock);
    omp_destroy_lock(&max_lock);
    double end4 = omp_get_wtime();
    double time4 = end4 - start4;
    total_critical = 0;
    max_local = 0;

    // Используем OpenMP для выполнения задачи с critical и барьерной синхронизацией
    double start_time_critical = omp_get_wtime();

#pragma omp parallel shared(total_critical, max_local, A, B, C) private(i)
    {
        double local_total = 0;
        int local_max = 0;

#pragma omp for
        for (int i = 0; i < N; ++i) {
            C[i] = std::max(A[i], B[i]);
            local_total += C[i];
            local_max = std::max<int>(local_max, C[i]);
        }

#pragma omp critical
        {
            total_critical += local_total;
        }

        // Барьерная синхронизация
#pragma omp barrier

#pragma omp single
        {
            // Выполнить действия, которые должны быть выполнены только однажды
            if (local_max > max_local) {
                max_local = local_max;
            }
        }
    }

    double end_time_critical = omp_get_wtime();
    double elapsed_time_critical = end_time_critical - start_time_critical;
    double start1 = omp_get_wtime();
    int* C_without_omp = new int[N];
    computeWithoutOMP(A, B, C_without_omp, N);
    double end1 = omp_get_wtime();
    double time1 = end1 - start1;
    std::cout << "Время выполнения с использованием atomic: " << time2 << " секунд" << std::endl;
    std::cout << "Время выполнения с использованием critical: " << time3 << " секунд" << std::endl;
    std::cout << "Время выполнения lock: " << time1 << " секунд" << std::endl;
    std::cout << "Время выполнения barrier: " << elapsed_time_critical << " секунд" << std::endl;
    //std::cout << "Время выполнения без семафоры: " << time4 << " секунд" << std::endl;
    delete[] A;
    delete[] B;
    delete[] C;
    delete[] C_without_omp;

    return 0;
}