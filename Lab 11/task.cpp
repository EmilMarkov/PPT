#include <iostream>
#include <vector>
#include <random>
#include <algorithm>
#include <mpi.h>

const int N = 100000;
const int RANGE = 100000;

void merge(std::vector<int>& arr, int l, int m, int r) {
    int n1 = m - l + 1;
    int n2 = r - m;

    std::vector<int> L(n1), R(n2);

    for (int i = 0; i < n1; i++)
        L[i] = arr[l + i];
    for (int j = 0; j < n2; j++)
        R[j] = arr[m + 1 + j];

    int i = 0;
    int j = 0;
    int k = l;

    while (i < n1 && j < n2) {
        if (L[i] <= R[j]) {
            arr[k] = L[i];
            i++;
        }
        else {
            arr[k] = R[j];
            j++;
        }
        k++;
    }

    while (i < n1) {
        arr[k] = L[i];
        i++;
        k++;
    }

    while (j < n2) {
        arr[k] = R[j];
        j++;
        k++;
    }
}

void mergeSort(std::vector<int>& arr, int l, int r) {
    if (l >= r) return;

    int m = l + (r - l) / 2;
    mergeSort(arr, l, m);
    mergeSort(arr, m + 1, r);

    merge(arr, l, m, r);
}

int main(int argc, char** argv) {
    int rank, size;
    MPI_Group original_group, sub_group;
    MPI_Comm sub_comm; // communicator of subgroup

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    // Ensure size is even.
    if (size % 2 != 0) {
        if (rank == 0) {
            std::cerr << "The number of processes must be even!" << std::endl;
        }
        MPI_Finalize();
        return 0;
    }

    // Extract the group from the MPI_COMM_WORLD communicator
    MPI_Comm_group(MPI_COMM_WORLD, &original_group);
    // Split processes into two groups
    int half_size = size / 2;
    int* ranks = new int[half_size];
    if (rank < half_size) {
        for (int i = 0; i < half_size; ++i) {
            ranks[i] = i;
        }
    }
    else {
        for (int i = 0; i < half_size; ++i) {
            ranks[i] = half_size + i;
        }
    }

    MPI_Group_incl(original_group, half_size, ranks, &sub_group);
    MPI_Comm_create(MPI_COMM_WORLD, sub_group, &sub_comm);

    if (sub_comm != MPI_COMM_NULL) {
        std::vector<int> data;

        if (rank == 0 || rank == half_size) {
            std::random_device rd;
            std::mt19937 mt(rd());
            std::uniform_int_distribution<int> dist(0, RANGE);

            for (int i = 0; i < N / 2; i++) {
                data.push_back(dist(mt));
            }
        }

        int chunk_size = (N / 2) / half_size;
        std::vector<int> local_data(chunk_size);

        MPI_Scatter(data.data(), chunk_size, MPI_INT, &local_data[0], chunk_size, MPI_INT, 0, sub_comm);

        mergeSort(local_data, 0, chunk_size - 1);

        std::vector<int> sorted_data;
        if (rank == 0 || rank == half_size) {
            sorted_data.resize(N / 2);
        }

        MPI_Gather(local_data.data(), chunk_size, MPI_INT, sorted_data.data(), chunk_size, MPI_INT, 0, sub_comm);

        if (rank == 0 || rank == half_size) {
            for (int i = 0; i < half_size - 1; i++) {
                merge(sorted_data, 0, (i + 1) * chunk_size - 1, (i + 2) * chunk_size - 1);
            }
        }

        if (rank == 0) {
            std::vector<int> sorted_data_from_second_group(N / 2);
            MPI_Recv(sorted_data_from_second_group.data(), N / 2, MPI_INT, half_size, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

            std::vector<int> final_sorted_data(N);
            std::merge(sorted_data.begin(), sorted_data.end(),
                sorted_data_from_second_group.begin(), sorted_data_from_second_group.end(),
                final_sorted_data.begin());

            for (const int& val : final_sorted_data) {
                std::cout << val << " ";
            }
            std::cout << std::endl;
        }
        else if (rank == half_size) {
            MPI_Send(sorted_data.data(), N / 2, MPI_INT, 0, 0, MPI_COMM_WORLD);
        }
    }

    MPI_Group_free(&original_group);
    MPI_Group_free(&sub_group);
    if (sub_comm != MPI_COMM_NULL) {
        MPI_Comm_free(&sub_comm);
    }
    delete[] ranks;
    MPI_Finalize();

    return 0;
}
