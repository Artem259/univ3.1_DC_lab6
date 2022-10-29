#include "MatrixMultiplicationMPI.h"

#include <cassert>
#include <utility>

MatrixMultiplicationMPI::MatrixMultiplicationMPI(Matrix m1, Matrix m2) : m1(std::move(m1)), m2(std::move(m2)), result(m1.getSize()) {
    assert(this->m1.getSize() == this->m2.getSize());
    startTime = 0;
    finishTime = 0;
    size = this->m1.getSize();
    procSize = 0;
    procRank = 0;
    MPI_Comm_size(MPI_COMM_WORLD, &procSize);
    MPI_Comm_rank(MPI_COMM_WORLD, &procRank);
    assert(procRank == 0);
}

MatrixMultiplicationMPI::MatrixMultiplicationMPI() : m1(0), m2(0), result(0)  {
    startTime = 0;
    finishTime = 0;
    size = 0;
    procSize = 0;
    procRank = 0;
    MPI_Comm_size(MPI_COMM_WORLD, &procSize);
    MPI_Comm_rank(MPI_COMM_WORLD, &procRank);
    assert(procRank != 0);
}

void MatrixMultiplicationMPI::startTimePoint() {
    startTime = static_cast<unsigned long long>(MPI_Wtime()*1000);
}

void MatrixMultiplicationMPI::finishTimePoint() {
    finishTime = static_cast<unsigned long long>(MPI_Wtime()*1000);
}

unsigned long long MatrixMultiplicationMPI::getRunningTime() const {
    assert(procRank == 0);
    assert(finishTime != 0);
    return finishTime - startTime;
}

Matrix MatrixMultiplicationMPI::getResult() const {
    assert(procRank == 0);
    assert(finishTime != 0);
    return result;
}
