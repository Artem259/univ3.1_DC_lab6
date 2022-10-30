#ifndef LINEMATRIXMULTIPLICATIONMPI_H
#define LINEMATRIXMULTIPLICATIONMPI_H

#include <ctime>
#include <cmath>
#include <cassert>

#include "MatrixMultiplicationMPI.h"
#include "Matrix.h"

class LineMatrixMultiplicationMPI : public MatrixMultiplicationMPI  {
    int* pAMatrix;
    int* pBMatrix;
    int* pCMatrix;
    int Size;
    int LineSize; // Size of line on current process
    int* pALine; // Initial block of matrix A on current process
    int* pBLine; // Initial block of matrix B on current process
    int* pCLine; // Block of result matrix C on current process

    // Function for matrix multiplication
    void ResultCalculation(int iter);

    void DataInitialization();

    // Function for memory allocation and data initialization
    void ProcessInitialization();

    // Functions for matrices decomposition
    void MatrixAScatter();
    void MatrixBScatter();

    // Data distribution among the processes
    void DataDistribution();

    // Function for gathering the result matrix
    void ResultCollection();

    // Cyclic shift of blocks in the process grid dimension
    void LineBCommunication() const;

    void ParallelResultCalculation();

    // Function for computational process termination
    void ProcessTermination ();

public:
    using MatrixMultiplicationMPI::MatrixMultiplicationMPI;

    void run() override;
};


#endif //LINEMATRIXMULTIPLICATIONMPI_H
