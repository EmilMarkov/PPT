#include <iostream>
#include <omp.h>
#include <cstdlib>
#include <ctime>
#include <chrono>
#include "locale.h"

const int N = 10000000;

int main() {
    setlocale(LC_ALL, "Russian");

    // Выделение памяти
    int* A = new int[N];
    int* B = new int[N];
    int* C = new int[N];
    int sum = 0; // для лаб 3

    // Формирование массивов А и В
    std::srand(std::time(nullptr));
    for (int i = 0; i < N; i++) {
        A[i] = std::rand() % 100;
    }

    for (int i = 0; i < N; i++) {
        B[i] = std::rand() % 100;
    }

    // ============================Лаб 3============================

    // Без распараллеливания
    sum = 0;
    auto start_time = std::chrono::high_resolution_clock::now();

    for (int i = 0; i < N; i++) {
        C[i] = std::max(A[i], B[i]);
        sum += C[i];
    }

    auto end_time = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end_time - start_time);

    std::cout << "========================Лаб 3========================" << std::endl << std::endl;
    std::cout << "Сумма элементов массива C: " << sum << std::endl;
    std::cout << "Время работы программы без распараллеливания: " << duration.count() << " микросекунд" << std::endl << std::endl;

    // С распараллеливанием
    sum = 0;
    start_time = std::chrono::high_resolution_clock::now();

    #pragma omp parallel reduction(+:sum)
    {
        #pragma omp for
        for (int i = 0; i < N; i++) {
            C[i] = std::max(A[i], B[i]);
            sum += C[i];
        }
    }

    end_time = std::chrono::high_resolution_clock::now();
    duration = std::chrono::duration_cast<std::chrono::microseconds>(end_time - start_time);

    std::cout << "Сумма элементов массива C: " << sum << std::endl;
    std::cout << "Время работы программы с распараллеливанием: " << duration.count() << " микросекунд" << std::endl;

    // ============================Лаб 5============================

    int sum1 = 0; // для 2 секций
    int sum2 = 0; // для 4 секций

    // 2 секции
    start_time = std::chrono::high_resolution_clock::now();

    #pragma omp parallel
    {
        #pragma omp sections reduction(+:sum1)
        {
            #pragma omp section
            {
                int local_sum1 = 0; // Локальная переменная для первой секции
                for (int i = 0; i < N / 2; ++i) {
                    C[i] = std::max(A[i], B[i]);
                    local_sum1 += C[i];
                }
                sum1 += local_sum1; // Аккумулируем результат из локальной переменной
            }

            #pragma omp section
            {
                int local_sum1 = 0; // Локальная переменная для второй секции
                for (int i = N / 2; i < N; ++i) {
                    C[i] = std::max(A[i], B[i]);
                    local_sum1 += C[i];
                }
                sum1 += local_sum1; // Аккумулируем результат из локальной переменной
            }
        }
    }

    end_time = std::chrono::high_resolution_clock::now();
    duration = std::chrono::duration_cast<std::chrono::microseconds>(end_time - start_time);

    std::cout << "========================Лаб 5========================" << std::endl << std::endl;
    std::cout << "Сумма элементов массива C: " << sum1 << std::endl;
    std::cout << "Время работы программы с двумя секциями: " << duration.count() << " микросекунд" << std::endl;

    // 4 секции
    sum1 = 0;
    sum2 = 0;

    start_time = std::chrono::high_resolution_clock::now();

    #pragma omp parallel
    {
        #pragma omp sections reduction(+:sum2)
        {
            #pragma omp section
            {
                int local_sum2 = 0; // Локальная переменная для первой секции
                for (int i = 0; i < N / 4; ++i) {
                    C[i] = std::max(A[i], B[i]);
                    local_sum2 += C[i];
                }
                sum2 += local_sum2; // Аккумулируем результат из локальной переменной
            }

            #pragma omp section
            {
                int local_sum2 = 0; // Локальная переменная для второй секции
                for (int i = N / 4; i < N / 2; ++i) {
                    C[i] = std::max(A[i], B[i]);
                    local_sum2 += C[i];
                }
                sum2 += local_sum2; // Аккумулируем результат из локальной переменной
            }

            #pragma omp section
            {
                int local_sum2 = 0; // Локальная переменная для третьей секции
                for (int i = N / 2; i < 3 * N / 4; ++i) {
                    C[i] = std::max(A[i], B[i]);
                    local_sum2 += C[i];
                }
                sum2 += local_sum2; // Аккумулируем результат из локальной переменной
            }

            #pragma omp section
            {
                int local_sum2 = 0; // Локальная переменная для четвертой секции
                for (int i = 3 * N / 4; i < N; ++i) {
                    C[i] = std::max(A[i], B[i]);
                    local_sum2 += C[i];
                }
                sum2 += local_sum2; // Аккумулируем результат из локальной переменной
            }
        }
    }

    end_time = std::chrono::high_resolution_clock::now();
    duration = std::chrono::duration_cast<std::chrono::microseconds>(end_time - start_time);

    std::cout << "Сумма элементов массива C: " << sum2 << std::endl;
    std::cout << "Время работы программы с четырьмя секциями: " << duration.count() << " микросекунд" << std::endl;

    delete[] A;
    delete[] B;
    delete[] C;

    return 0;
}