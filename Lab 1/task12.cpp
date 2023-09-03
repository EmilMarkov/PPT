#include <iostream>
#include <locale>
#include <vector>
#include <random>
#include <omp.h>
#include <chrono>

// Функция для генерации случайных натуральных чисел
int generateRandomNaturalNumber() {
    static std::random_device rd;
    static std::mt19937 gen(rd());
    std::uniform_int_distribution<int> dist(1, 100); // задаём диапазон чисел
    return dist(gen);
}

// Функция для подсчёта количетсва семёрок
int countSevensInDecimal(int number) {
    int count = 0;
    while (number > 0) {
        if (number % 10 == 7) {
            count++;
        }
        number /= 10;
    }
    return count;
}

int main() {
    setlocale(LC_ALL, "Russian");

    const int M = 1000; // Количество строк
    const int N = 1000; // Количество столбцов

    // Создание и заполнение матрицы случайными натуральными числами
    std::vector<std::vector<int>> matrix(M, std::vector<int>(N));
    for (int i = 0; i < M; ++i) {
        for (int j = 0; j < N; ++j) {
            matrix[i][j] = generateRandomNaturalNumber();
        }
    }

    // Вывод сгенерированной матрицы
    /*std::cout << "Сгенерированная матрица:" << std::endl;
    for (int i = 0; i < M; ++i) {
        for (int j = 0; j < N; ++j) {
            std::cout << matrix[i][j] << " ";
        }
        std::cout << std::endl;
    }*/

    std::cout << "\n\nРезультаты:" << std::endl;

    // С распараллеливанием

    auto start = std::chrono::high_resolution_clock::now();

    #pragma omp parallel for
    for (int i = 0; i < M; ++i) {
        int sevensCount = 0;
        for (int j = 0; j < N; ++j) {
            for (int k = j + 1; k < N; ++k) {
                int pairSum = matrix[i][j] + matrix[i][k];
                sevensCount += countSevensInDecimal(pairSum);
            }
        }
        #pragma omp critical
        {
            //std::cout << "Строка #" << i << ": Количество семёрок = " << sevensCount << std::endl;
        }
    }

    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> duration = end - start;
    std::cout << "\nВремя выполнения программы с распараллеливанием: " << duration.count() << " секунд\n\n" << std::endl;


    // Без распараллеливания


    start = std::chrono::high_resolution_clock::now();

    for (int i = 0; i < M; ++i) {
        int sevensCount = 0;
        for (int j = 0; j < N; ++j) {
            for (int k = j + 1; k < N; ++k) {
                int pairSum = matrix[i][j] + matrix[i][k];
                sevensCount += countSevensInDecimal(pairSum);
            }
        }
        //std::cout << "Строка #" << i << ": Количество семёрок = " << sevensCount << std::endl;
    }

    end = std::chrono::high_resolution_clock::now();
    duration = end - start;
    std::cout << "\nВремя выполнения программы без распараллеливания: " << duration.count() << " секунд" << std::endl;

    return 0;
}
