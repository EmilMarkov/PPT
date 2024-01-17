#include <iostream>
#include <mpi.h>

int main(int argc, char** argv) {
    int rank, size;
    MPI_Comm ring_comm;
    MPI_Comm graph_comm;

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    // Проверка на 8 процессов
    if (size != 8) {
        if (rank == 0) {
            std::cerr << "This program requires exactly 8 processes!" << std::endl;
        }
        MPI_Finalize();
        return 1;
    }

    // Определите, какие процессы принадлежат кольцевой топологии, а какие - топологии графа
    int ring_group = (rank < 4) ? 1 : 0;
    int graph_group = (rank < 4) ? 0 : 1;

    // Создайте коммуникаторы для каждой группы
    MPI_Comm_split(MPI_COMM_WORLD, ring_group, rank, &ring_comm);
    MPI_Comm_split(MPI_COMM_WORLD, graph_group, rank, &graph_comm);

    if (ring_group) {
        // Создание топологии кольца с использованием MPI_Cart_create
        int dims[1] = { 4 };  // Один ранг
        int periods[1] = { 1 };  // Кольцевая топология
        int coords[1];
        MPI_Cart_create(ring_comm, 1, dims, periods, 0, &ring_comm);
        MPI_Cart_coords(ring_comm, rank, 1, coords);

        // Определение соседних процессов с использованием MPI_Cart_shift
        int source, dest;
        MPI_Cart_shift(ring_comm, 0, 1, &source, &dest);

        int data = rank;
        int recv_data;
        MPI_Status status;

        // Отправка данных к соседнему процессу
        //MPI_Send(&data, 1, MPI_INT, dest, 0, ring_comm);
        // Получение данных от соседнего процесса
        //MPI_Recv(&recv_data, 1, MPI_INT, source, 0, ring_comm, &status);

        MPI_Sendrecv(&data, 1, MPI_INT, dest, 0, &recv_data, 1, MPI_INT, source, 0, ring_comm, &status);

        std::cout << "Ring process " << rank << " received data: " << recv_data << std::endl;
    }

    if (graph_group) {
        // Создание топологии графа с использованием MPI_Graph_create
        int nnodes = 4;  // Количество вершин в графе
        int index[] = { 3, 4, 5, 6 };
        int edges[] = { 1, 2, 3, 0, 0, 0 };
        // Связи между вершинами
        MPI_Graph_create(graph_comm, nnodes, index, edges, 0, &graph_comm);

        /*int data[] = { 10, 1, 1, 1 };
        int local;
        MPI_Scatter(data, 1, MPI_INT, &local, 1, MPI_INT, 0, graph_comm);
        std::cout << "Star process " << rank << " received data: " << local << std::endl;*/

        MPI_Barrier(graph_comm);
        int nneighbors;
        int* nneighbors_arr = (int*) malloc(sizeof(int) * 3);
        int data;
        int recv_data;
        MPI_Graph_neighbors_count(graph_comm, rank - 4, &nneighbors);
        MPI_Graph_neighbors(graph_comm, rank - 4, 3, nneighbors_arr);
        for (int i = 0; i < nneighbors; i++) {
            data = rank;
            MPI_Sendrecv(&data, 1, MPI_INT, nneighbors_arr[i], 0, &recv_data, 1, MPI_INT, nneighbors_arr[i], 0, graph_comm, MPI_STATUS_IGNORE);
            std::cout << "Star process " << rank << " received data: " << recv_data << std::endl;
        }
    }

    // Освобождаем коммуникаторы
    MPI_Barrier(ring_comm);
    MPI_Barrier(graph_comm);
    MPI_Comm_free(&ring_comm);
    MPI_Comm_free(&graph_comm);
    MPI_Finalize();
}
