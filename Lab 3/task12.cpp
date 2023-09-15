#include <iostream>
#include <omp.h>
#include <cstdlib>
#include <ctime>
#include <chrono>
#include "locale.h"

const int N = 1000000;

int main() {
    setlocale(LC_ALL, "Russian");

    // Выделение памяти
    int* A = new int[N];
    int* B = new int[N];
    int* C = new int[N];
    int sum = 0;

    // Формирование массивов А и В
    std::srand(std::time(nullptr));
    for (int i = 0; i < N; i++) {
        A[i] = std::rand() % 100;
    }

    for (int i = 0; i < N; i++) {
        B[i] = std::rand() % 100;
    }



    // Без распараллеливания
    sum = 0;
    auto start_time = std::chrono::high_resolution_clock::now();

    for (int i = 0; i < N; i++) {
        C[i] = std::max(A[i], B[i]);
        sum += C[i];
    }

    auto end_time = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end_time - start_time);

    std::cout << "Сумма элементов массива C: " << sum << std::endl;
    std::cout << "Время работы программы без распараллеливания: " << duration.count() << " микросекунд" << std::endl;


    // С распараллеливанием
    start_time = std::chrono::high_resolution_clock::now();

#pragma omp parallel for reduction(+:sum)
    for (int i = 0; i < N; i++) {
        C[i] = std::max(A[i], B[i]);
        sum += C[i];
    }

    end_time = std::chrono::high_resolution_clock::now();
    duration = std::chrono::duration_cast<std::chrono::microseconds>(end_time - start_time);

    std::cout << "Сумма элементов массива C: " << sum << std::endl;
    std::cout << "Время работы программы с распараллеливанием: " << duration.count() << " микросекунд" << std::endl;


    delete[] A;
    delete[] B;
    delete[] C;

    return 0;
}
