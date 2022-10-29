#include "CannonMatrixMultiplicationMPI.h"

void CannonMatrixMultiplicationMPI::ResultCalculation() {
    for (int i=0; i<BlockSize; i++) {
        for (int j=0; j<BlockSize; j++) {
            for (int k=0; k<BlockSize; k++) {
                pCblock[i*BlockSize+j] += pAblock[i*BlockSize+k]*pBblock[k*BlockSize+j];
            }
        }
    }
}

void CannonMatrixMultiplicationMPI::CreateGridCommunicators() {
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

void CannonMatrixMultiplicationMPI::DataInitialization() {
    pAMatrix = m1.getDataAsArray();
    pBMatrix = m2.getDataAsArray();
}

void CannonMatrixMultiplicationMPI::ProcessInitialization() {
    if (procRank == 0) {
        Size = static_cast<int>(size);
        assert(Size % GridSize == 0);
    }
    MPI_Bcast(&Size, 1, MPI_INT, 0, MPI_COMM_WORLD);
    BlockSize = Size / GridSize;
    pAblock = new int[BlockSize*BlockSize];
    pBblock = new int[BlockSize*BlockSize];
    pCblock = new int[BlockSize*BlockSize];
    for (int i=0; i<BlockSize*BlockSize; i++) {
        pCblock[i] = 0;
    }
    if (procRank == 0) {
        pCMatrix = new int[Size*Size];
        DataInitialization();
    }
}

void CannonMatrixMultiplicationMPI::MatrixScatter(int* pMatrix, int* pMatrixBlock) {
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

void CannonMatrixMultiplicationMPI::DataDistribution() {
    // Scatter the matrix among the processes of the first grid column
    MatrixScatter(pAMatrix, pAblock);
    MatrixScatter(pBMatrix, pBblock);

    BlockCommunication(pAblock, RowComm, GridCoords[1]-GridCoords[0], GridCoords[1]+GridCoords[0]);
    BlockCommunication(pBblock, ColComm, GridCoords[0]-GridCoords[1], GridCoords[0]+GridCoords[1]);
}

void CannonMatrixMultiplicationMPI::ResultCollection() {
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

void CannonMatrixMultiplicationMPI::BlockCommunication(int* block, MPI_Comm comm, int dest, int source) const {
    while (dest >= GridSize)
        dest -= GridSize;
    while (dest < 0)
        dest += GridSize;

    while (source >= GridSize)
        source -= GridSize;
    while (source < 0)
        source += GridSize;

    MPI_Status Status;
    MPI_Sendrecv_replace( block, BlockSize*BlockSize, MPI_INT,
                          dest, 0, source, 0, comm, &Status);
}

void CannonMatrixMultiplicationMPI::ParallelResultCalculation() {
    for (int iter = 0; iter < GridSize; iter ++) {
        // Result calculation
        ResultCalculation();

        // Cyclic shift of blocks of matrices A and B in process grid columns
        BlockCommunication(pAblock, RowComm, GridCoords[1]-1, GridCoords[1]+1);
        BlockCommunication(pBblock, ColComm, GridCoords[0]-1, GridCoords[0]+1);
    }
}

void CannonMatrixMultiplicationMPI::ProcessTermination () {
    if (procRank == 0) {
        delete [] pAMatrix;
        delete [] pBMatrix;
        delete [] pCMatrix;
    }
    delete [] pAblock;
    delete [] pBblock;
    delete [] pCblock;
}

void CannonMatrixMultiplicationMPI::run() {
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
    // Execution of Cannon method
    ParallelResultCalculation();
    ResultCollection();
    // Process Termination
    ProcessTermination();
    finishTimePoint();
}
