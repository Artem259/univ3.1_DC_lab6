#include "FoxMatrixMultiplicationMPI.h"

void FoxMatrixMultiplicationMPI::ResultCalculation() {
    for (int i=0; i<BlockSize; i++) {
        for (int j=0; j<BlockSize; j++) {
            for (int k=0; k<BlockSize; k++) {
                pCblock[i*BlockSize+j] += pAblock[i*BlockSize+k]*pBblock[k*BlockSize+j];
            }
        }
    }
}

void FoxMatrixMultiplicationMPI::CreateGridCommunicators() {
    int DimSize[2]; // Number of processes in each dimension of the grid
    int Periodic[2]; // =1, if the grid dimension should be periodic
    int Subdims[2]; // =1, if the grid dimension should be fixed

    DimSize[0] = GridSize;
    DimSize[1] = GridSize;
    Periodic[0] = 0;
    Periodic[1] = 0;
    // Creation of the Cartesian communicator
    MPI_Cart_create(MPI_COMM_WORLD, 2, DimSize, Periodic, 1, &GridComm);
    // Determination of the cartesian coordinates for every process
    MPI_Cart_coords(GridComm, procRank, 2, GridCoords);

    // Creating communicators for rows
    Subdims[0] = 0; // Dimensionality fixing
    Subdims[1] = 1; // The presence of the given dimension in the subgrid
    MPI_Cart_sub(GridComm, Subdims, &RowComm);

    // Creating communicators for columns
    Subdims[0] = 1;
    Subdims[1] = 0;
    MPI_Cart_sub(GridComm, Subdims, &ColComm);
}

void FoxMatrixMultiplicationMPI::DataInitialization() {
    pAMatrix = m1.getDataAsArray();
    pBMatrix = m2.getDataAsArray();
}

void FoxMatrixMultiplicationMPI::ProcessInitialization() {
    if (procRank == 0) {
        Size = static_cast<int>(size);
        assert(Size % GridSize == 0);
    }
    MPI_Bcast(&Size, 1, MPI_INT, 0, MPI_COMM_WORLD);
    BlockSize = Size / GridSize;
    pAblock = new int[BlockSize*BlockSize];
    pBblock = new int[BlockSize*BlockSize];
    pCblock = new int[BlockSize*BlockSize];
    pMatrixAblock = new int[BlockSize*BlockSize];
    for (int i=0; i<BlockSize*BlockSize; i++) {
        pCblock[i] = 0;
    }
    if (procRank == 0) {
        pCMatrix = new int[Size*Size];
        DataInitialization();
    }
}

void FoxMatrixMultiplicationMPI::CheckerboardMatrixScatter(int* pMatrix, int* pMatrixBlock) {
    int *MatrixRow = new int[BlockSize*Size];
    if (GridCoords[1] == 0) {
        MPI_Scatter(pMatrix, BlockSize*Size, MPI_INT, MatrixRow,
                    BlockSize*Size, MPI_INT, 0, ColComm);
    }
    for (int i=0; i<BlockSize; i++) {
        MPI_Scatter(&MatrixRow[i*Size], BlockSize, MPI_INT,
                    &(pMatrixBlock[i*BlockSize]), BlockSize, MPI_INT, 0, RowComm);
    }
    delete []MatrixRow;
}

void FoxMatrixMultiplicationMPI::DataDistribution() {
    // Scatter the matrix among the processes of the first grid column
    CheckerboardMatrixScatter(pAMatrix, pMatrixAblock);
    CheckerboardMatrixScatter(pBMatrix, pBblock);
}

void FoxMatrixMultiplicationMPI::ResultCollection() {
    int* pResultRow = new int[Size*BlockSize];
    for (int i=0; i<BlockSize; i++) {
        MPI_Gather(&pCblock[i*BlockSize], BlockSize, MPI_INT,
                   &pResultRow[i*Size], BlockSize, MPI_INT, 0, RowComm);
    }
    if (GridCoords[1] == 0) {
        MPI_Gather(pResultRow, BlockSize*Size, MPI_INT, pCMatrix,
                   BlockSize*Size, MPI_INT, 0, ColComm);
    }
    delete []pResultRow;

    if (procRank == 0) {
        result.setDataFromArray(pCMatrix);
    }
}

void FoxMatrixMultiplicationMPI::ABlockCommunication(int iter) {
    // Defining the leading process of the process grid row
    int Pivot = (GridCoords[0] + iter) % GridSize;

    // Copying the transmitted block in a separate memory buffer
    if (GridCoords[1] == Pivot) {
        for (int i=0; i<BlockSize*BlockSize; i++)
            pAblock[i] = pMatrixAblock[i];
    }

    // Block broadcasting
    MPI_Bcast(pAblock, BlockSize*BlockSize, MPI_INT, Pivot, RowComm);
}

void FoxMatrixMultiplicationMPI::BblockCommunication() {
    MPI_Status Status;
    int NextProc = GridCoords[0] - 1;
    if ( GridCoords[0] == 0 ) NextProc = GridSize-1;
    int PrevProc = GridCoords[0] + 1;
    if ( GridCoords[0] == GridSize-1 ) PrevProc = 0;
    MPI_Sendrecv_replace( pBblock, BlockSize*BlockSize, MPI_INT,
                          NextProc, 0, PrevProc, 0, ColComm, &Status);
}

void FoxMatrixMultiplicationMPI::ParallelResultCalculation() {
    for (int iter = 0; iter < GridSize; iter ++) {
        // Sending blocks of matrix A to the process grid rows
        ABlockCommunication(iter);
        // Result calculation
        ResultCalculation();
        // Cyclic shift of blocks of matrix B in process grid columns
        BblockCommunication();
    }
}

void FoxMatrixMultiplicationMPI::ProcessTermination () {
    if (procRank == 0) {
        delete [] pAMatrix;
        delete [] pBMatrix;
        delete [] pCMatrix;
    }
    delete [] pAblock;
    delete [] pBblock;
    delete [] pCblock;
    delete [] pMatrixAblock;
}

void FoxMatrixMultiplicationMPI::run() {
    startTimePoint();
    GridSize = static_cast<int>(std::sqrt(procSize));
    if (procRank == 0) {
        assert(procSize == GridSize*GridSize);
    }

    // Creating the cartesian grid, row and column communicators
    CreateGridCommunicators();
    // Memory allocation and initialization of matrix elements
    ProcessInitialization();
    DataDistribution();
    // Execution of Fox method
    ParallelResultCalculation();
    ResultCollection();
    // Process Termination
    ProcessTermination();
    finishTimePoint();
}
