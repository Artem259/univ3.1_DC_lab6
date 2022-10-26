#include "Matrix.h"

#include <cassert>

Matrix::Matrix(std::size_t size) : size(size) {
    matrix = matrix_t(size, vector_t(size, 0));
    std::random_device device;
    randEng = std::mt19937(device());
}

void Matrix::randomize(int maxV) {
    std::uniform_int_distribution<int> dist(-maxV, maxV);
    for (std::size_t i=0; i<size; i++) {
        for (std::size_t j=0; j<size; j++) {
            matrix[i][j] = dist(randEng);
        }
    }
}

std::size_t Matrix::getSize() const {
    return size;
}

int Matrix::getElementAt(size_t row, size_t column) {
    assert(row < size);
    assert(column < size);
    return matrix[row][column];
}

void Matrix::setElementAt(std::size_t row, std::size_t column, int v) {
    assert(row < size);
    assert(column < size);
    matrix[row][column] = v;
}

vector_t Matrix::getRowAt(std::size_t row) {
    assert(row < size);
    return matrix[row];
}

void Matrix::setRowAt(std::size_t row, const vector_t& v) {
    assert(row < size);
    assert(v.size() == size);
    matrix[row] = v;
}

vector_t Matrix::getColumnAt(std::size_t column) {
    assert(column < size);
    vector_t res;
    for (std::size_t i=0; i<size; i++)
        res.push_back(matrix[i][column]);
    return res;
}

void Matrix::setColumnAt(std::size_t column, const vector_t& v) {
    assert(column < size);
    assert(v.size() == size);
    for (std::size_t i=0; i<size; i++)
        matrix[i][column] = v[i];
}


std::ostream& operator << (std::ostream &out, const Matrix &m) {
    for (std::size_t i=0; i<m.size; i++) {
        for (std::size_t j=0; j<m.size; j++) {
            out << m.matrix[i][j] << " ";
        }
        if (i != m.size-1)
            out << "\n";
    }
    return out;
}

bool operator==(const Matrix& m1, const Matrix& m2) {
    if (m1.size != m2.size)
        return false;

    for (std::size_t i=0; i<m1.size; i++) {
        if (m1.matrix[i] != m2.matrix[i]) {
            return false;
        }
    }
    return true;
}

bool operator!=(const Matrix& m1, const Matrix& m2) {
    return !(m1 == m2);
}
