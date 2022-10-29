#ifndef FOXMATRIXMULTIPLICATIONMPI_H
#define FOXMATRIXMULTIPLICATIONMPI_H

#include <ctime>
#include <cmath>
#include <cassert>

#include "MatrixMultiplicationMPI.h"
#include "Matrix.h"

class FoxMatrixMultiplicationMPI : public MatrixMultiplicationMPI {
private:
    int* pAMatrix;
    int* pBMatrix;
    int* pCMatrix;
    int Size;
    int BlockSize; // Sizes of matrix blocks on current process
    int GridSize; // Size of virtual processor grid
    int GridCoords[2]; // Coordinates of current processor in grid
    int* pAblock; // Initial block of matrix A on current process
    int* pBblock; // Initial block of matrix B on current process
    int* pCblock; // Block of result matrix C on current process
    int* pMatrixAblock;
    MPI_Comm GridComm; // Grid communicator
    MPI_Comm ColComm; // Column communicator
    MPI_Comm RowComm; // Row communicator

    // Function for matrix multiplication
    void ResultCalculation();

    // Creation of two-dimensional grid communicator
    // and communicators for each row and each column of the grid
    void CreateGridCommunicators();

    void DataInitialization();

    // Function for memory allocation and data initialization
    void ProcessInitialization();

    // Function for checkerboard matrix decomposition
    void CheckerboardMatrixScatter(int* pMatrix, int* pMatrixBlock);

    // Data distribution among the processes
    void DataDistribution();

    // Function for gathering the result matrix
    void ResultCollection();

    // Broadcasting matrix A blocks to process grid rows
    void ABlockCommunication(int iter);

    // Cyclic shift of matrix B blocks in the process grid columns
    void BblockCommunication();

    void ParallelResultCalculation();

    // Function for computational process termination
    void ProcessTermination ();

public:
    using MatrixMultiplicationMPI::MatrixMultiplicationMPI;

    void run() override;
};


#endif //FOXMATRIXMULTIPLICATIONMPI_H
