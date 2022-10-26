#ifndef MATRIXMULTIPLICATIONMPI_H
#define MATRIXMULTIPLICATIONMPI_H

#include <cstdlib>
#include "Matrix.h"

class MatrixMultiplicationMPI {
private:
    unsigned long long startTime;
    unsigned long long finishTime;

protected:
    Matrix m1, m2;
    Matrix result;
    void startTimePoint();
    void finishTimePoint();

public:
    MatrixMultiplicationMPI(Matrix m1, Matrix m2);
    virtual ~MatrixMultiplicationMPI() = default;
    virtual void run() = 0;
    Matrix getResult() const;
    unsigned long long getRunningTime() const;
};


#endif //MATRIXMULTIPLICATIONMPI_H
