#ifndef MATRIX_H
#define MATRIX_H

#include <vector>
#include <iostream>
#include <random>

using vector_t = std::vector<int>;
using matrix_t = std::vector<std::vector<int>>;

class Matrix {
private:
    matrix_t matrix;
    std::size_t size;
    std::mt19937 randEng;

public:
    explicit Matrix(std::size_t size);
    void randomize(int maxV);
    std::size_t getSize() const;

    int getElementAt(std::size_t row, std::size_t column);
    void setElementAt(std::size_t row, std::size_t column, int v);

    vector_t getRowAt(std::size_t row);
    void setRowAt(std::size_t row, const vector_t& v);

    vector_t getColumnAt(std::size_t column);
    void setColumnAt(std::size_t column, const vector_t& v);

    int* getDataAsArray();
    void setDataFromArray(const int* data);

    friend std::ostream& operator << (std::ostream &out, const Matrix &m);
    friend bool operator==(const Matrix& m1, const Matrix& m2);
    friend bool operator!=(const Matrix& m1, const Matrix& m2);
};


#endif //MATRIX_H
