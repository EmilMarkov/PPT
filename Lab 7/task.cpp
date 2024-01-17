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
    //srand(time(NULL));
    MPI_Init(&argc, &argv);
    

    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    while (true) {
        if (rank == 0) {
            std::cout << value << std::endl;
            if (value == -1) {
                flag = -1;
                for (int i = 1; i < size; ++i)
                {
                    MPI_Send(&flag, 1, MPI_INT, i, 0, MPI_COMM_WORLD);
                }
                std::cout << "Find -1 End" << std::endl;
                break;
            }
            else {
                ++counter;
                std::cout << "Main procces: Counter = " << counter << std::endl;
                for (int i = 1; i < size; i++)
                {
                    MPI_Send(&flag, 1, MPI_INT, i, 0, MPI_COMM_WORLD);
                    MPI_Recv(&value, 1, MPI_INT, i, 1, MPI_COMM_WORLD, &status);
                }

            }
        }
        else {
            int local_flag = 0;
            MPI_Recv(&local_flag, 1, MPI_INT, 0, 0, MPI_COMM_WORLD, &status);
            if (local_flag == -1) {
                std::cout << "Sub procces : End work" << std::endl;
                break;
            }
            else {
                int local_value = static_cast<int>(rand() % 8) - 5;
                MPI_Send(&local_value, 1, MPI_INT, 0, 1, MPI_COMM_WORLD);
            }
        }
    }
    MPI_Finalize();
    return 0;
}
