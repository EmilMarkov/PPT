#include <ctime>
#include <iostream>
#include <mpi.h>

int main(int argc, char** argv) {
    int rank;
    int size;
    int flag = 0;
    int counter = 0;
    int value = 0;
    MPI_Status status;
    MPI_Init(&argc, &argv);

    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    double start_time, end_time;

    start_time = MPI_Wtime();

    while (true) {
        if (rank == 0) {
            std::cout << value << std::endl;
            if (value == -1) {
                flag = -1;
                MPI_Bcast(&flag, 1, MPI_INT, 0, MPI_COMM_WORLD);
                std::cout << "Find -1 End" << std::endl;
                break;
            }
            else {
                ++counter;
                std::cout << "Main process: Counter = " << counter << std::endl;
            }
        }

        MPI_Bcast(&flag, 1, MPI_INT, 0, MPI_COMM_WORLD);

        if (flag == -1) {
            std::cout << "Sub process : End work" << std::endl;
            break;
        }
        else {
            if (rank != 0) {
                int local_value = static_cast<int>(rand() % 18) - 1;
                MPI_Send(&local_value, 1, MPI_INT, 0, 1, MPI_COMM_WORLD);
            }
            else {
                for (int i = 1; i < size; i++) {
                    int local_value;
                    MPI_Recv(&local_value, 1, MPI_INT, i, 1, MPI_COMM_WORLD, &status);
                    value = local_value;
                }
            }
        }
    }

    end_time = MPI_Wtime();

    if (rank == 0) {
        double elapsed_time = end_time - start_time;
        std::cout << "Elapsed time: " << elapsed_time << " seconds" << std::endl;
    }

    MPI_Finalize();
    return 0;
}
