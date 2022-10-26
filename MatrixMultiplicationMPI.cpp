#include "MatrixMultiplicationMPI.h"

#include <chrono>
#include <cassert>
#include <utility>

MatrixMultiplicationMPI::MatrixMultiplicationMPI(Matrix m1, Matrix m2) : m1(std::move(m1)), m2(std::move(m2)), result(m1.getSize()) {
    assert(this->m1.getSize() == this->m2.getSize());
    startTime = 0;
    finishTime = 0;
}

void MatrixMultiplicationMPI::startTimePoint() {
    auto time = std::chrono::system_clock::now().time_since_epoch();
    startTime = std::chrono::duration_cast<std::chrono::microseconds>(time).count();
}

void MatrixMultiplicationMPI::finishTimePoint() {
    auto time = std::chrono::system_clock::now().time_since_epoch();
    finishTime = std::chrono::duration_cast<std::chrono::microseconds>(time).count();
}

unsigned long long MatrixMultiplicationMPI::getRunningTime() const {
    assert(finishTime != 0);
    return finishTime - startTime;
}

Matrix MatrixMultiplicationMPI::getResult() const {
    assert(finishTime != 0);
    return result;
}
