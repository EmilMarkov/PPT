#include <ctime>
#include <iostream>
#include <mpi.h>

int main(int argc, char** argv) {
    int rank;
    int size;
    int flag = 0;
    int counter = 0;
    int local_value = 0; // Локальное значение для каждого процесса
    int value = 0;
    MPI_Status status;
    MPI_Init(&argc, &argv);

    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    // Создаем массив для хранения случайных значений
    int* local_values = new int[size];
    int* scattered_values = new int[size];

    while (true) {
        // Генерируем случайные значения для всех процессов
        for (int i = 0; i < size; ++i) {
            local_values[i] = static_cast<int>(rand() % 8) - 2;
        }

        // Используем MPI_Scatter, чтобы передать случайные значения каждого процесса в главный процесс
        MPI_Scatter(local_values, 1, MPI_INT, &local_value, 1, MPI_INT, 0, MPI_COMM_WORLD);

        // Выводим информацию о том, какой процесс отправляет какое значение
        std::cout << "Process " << rank << ": Sending value = " << local_value << std::endl;

        // Обрабатываем значения в главном процессе
        MPI_Gather(&local_value, 1, MPI_INT, scattered_values, 1, MPI_INT, 0, MPI_COMM_WORLD);

        if (rank == 0) {
            for (int i = 0; i < size; ++i) {
                if (scattered_values[i] == -1) {
                    flag = -1;
                    break;
                }
            }

            MPI_Bcast(&flag, 1, MPI_INT, 0, MPI_COMM_WORLD);

            if (flag == -1) {
                std::cout << "Main process: Found -1, Ending the program" << std::endl;
                break;
            }
            else {
                ++counter;
                std::cout << "Main process: Counter = " << counter << std::endl;
            }
        }
        else {
            MPI_Bcast(&flag, 1, MPI_INT, 0, MPI_COMM_WORLD);
            if (flag == -1) {
                std::cout << "Process " << rank << ": End work" << std::endl;
                break;
            }
        }
    }

    MPI_Finalize();
    delete[] local_values;
    return 0;
}
