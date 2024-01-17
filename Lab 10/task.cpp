#include <iostream>
#include <vector>
#include <ctime>
#include "mpi.h"

using namespace std;

const size_t DIGITS = 40;

vector<int> multiplyLongInts(const vector<int>& a, const vector<int>& b) {
    size_t n = a.size();
    size_t m = b.size();
    vector<int> result(n + m + 1, 0);
    for (size_t i = 0; i < n; i++) {
        for (size_t j = 0; j < m; j++) {
            result[i + j] += a[i] * b[j];
            if (result[i + j] >= 10) {
                result[i + j + 1] += result[i + j] / 10;
                result[i + j] %= 10;
            }
        }
    }
    while (result.size() > 1 && result.back() == 0)
        result.pop_back();
    return result;
}

int main(int argc, char* argv[]) {
    int ProcCount, myRank;
    MPI_Status Status;

    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &ProcCount);
    MPI_Comm_rank(MPI_COMM_WORLD, &myRank);

    MPI_Datatype MPI_LONG_INT_TYPE;
    MPI_Type_contiguous(DIGITS, MPI_INT, &MPI_LONG_INT_TYPE);
    MPI_Type_commit(&MPI_LONG_INT_TYPE);

    int A = 8;
    vector<vector<int>> numbers;

    if (myRank == 0) {
        srand(static_cast<unsigned int>(time(nullptr)));
        for (int i = 0; i < A; i++) {
            numbers.push_back(vector<int>(DIGITS));
            for (size_t j = 0; j < DIGITS; j++) {
                numbers.back()[j] = rand() % 10;
            }
        }
    }

    MPI_Bcast(&A, 1, MPI_INT, 0, MPI_COMM_WORLD);
    numbers.resize(A, vector<int>(DIGITS));

    for (int i = 0; i < A; i++) {
        MPI_Bcast(&numbers[i][0], DIGITS, MPI_INT, 0, MPI_COMM_WORLD);
    }

    vector<int> myLongInt(DIGITS, 0);
    myLongInt[0] = 1;
    for (int i = 0; i < A; i++) {
        if (i % ProcCount == myRank) {
            cout << "Processor " << myRank << " has number: ";
            for (int j = numbers[i].size() - 1; j >= 0; j--)
                cout << numbers[i][j];
            cout << endl;
        }
    }
    if (ProcCount <= A) {
        if (myRank == 0) {
            // Только процесс 0 считает произведение всех чисел
            for (int i = 0; i < A; i++) {
                myLongInt = multiplyLongInts(myLongInt, numbers[i]);
            }

            // Вывод результата
            cout << "Final result: ";
            for (int i = myLongInt.size() - 1; i >= 0; i--) {
                cout << myLongInt[i];
            }
            cout << endl;
        }
    }
    else {
        // Даем каждому процессу по два числа для обработки
        for (int i = myRank * 2; i < min(A, (myRank + 1) * 2); i++) {
            myLongInt = multiplyLongInts(myLongInt, numbers[i]);
        }
        for (int i = 0; i < A; i++) {
            if (i % ProcCount == myRank) {
                cout << "Processor " << myRank << " has number: ";
                for (int j = numbers[i].size() - 1; j >= 0; j--)
                    cout << numbers[i][j];
                cout << endl;
            }
        }
        // Отправляем результаты обратно на главный процесс
        if (myRank != 0) {
            MPI_Send(&myLongInt[0], DIGITS, MPI_INT, 0, 0, MPI_COMM_WORLD);
        }
        else {
            for (int i = 1; i < ProcCount; i++) {
                vector<int> receivedLongInt(DIGITS);
                MPI_Recv(&receivedLongInt[0], DIGITS, MPI_INT, i, 0, MPI_COMM_WORLD, &Status);
                myLongInt = multiplyLongInts(myLongInt, receivedLongInt);
            }

            // Вывод результата
            cout << "Final result: ";
            for (int i = myLongInt.size() - 1; i >= 0; i--) {
                cout << myLongInt[i];
            }
            cout << endl;
        }
    }

    MPI_Type_free(&MPI_LONG_INT_TYPE);
    MPI_Finalize();

    return 0;
}
