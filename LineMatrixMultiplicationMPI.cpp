#include "LineMatrixMultiplicationMPI.h"

void LineMatrixMultiplicationMPI::ResultCalculation(int iter) {
    for (int i=0; i<LineSize; i++) {
        for (int j=0; j<LineSize; j++) {
            for (int k=0; k<Size; k++) {
                pCLine[i*Size+iter*LineSize+j] += pALine[i*Size+k]*pBLine[k*LineSize+j];
            }
        }
    }
}

void LineMatrixMultiplicationMPI::DataInitialization() {
    pAMatrix = m1.getDataAsArray();
    pBMatrix = m2.getDataAsArray();
}

void LineMatrixMultiplicationMPI::ProcessInitialization() {
    if (procRank == 0) {
        Size = static_cast<int>(size);
        assert(Size % procSize == 0);
    }
    MPI_Bcast(&Size, 1, MPI_INT, 0, MPI_COMM_WORLD);
    LineSize = Size / procSize;
    pALine = new int[Size*LineSize];
    pBLine = new int[Size*LineSize];
    pCLine = new int[Size*LineSize]();
    if (procRank == 0) {
        pCMatrix = new int[Size*Size];
        DataInitialization();
    }
}

void LineMatrixMultiplicationMPI::MatrixAScatter() {
    MPI_Scatter(pAMatrix, Size*LineSize, MPI_INT,
                pALine, Size*LineSize, MPI_INT, 0, MPI_COMM_WORLD);
}

void LineMatrixMultiplicationMPI::MatrixBScatter() {
    for (int i=0; i<Size; i++) {
        MPI_Scatter(&pBMatrix[i*Size], LineSize, MPI_INT,
                    &(pBLine[i*LineSize]), LineSize, MPI_INT, 0, MPI_COMM_WORLD);
    }
}

void LineMatrixMultiplicationMPI::DataDistribution() {
    // Scatter matrices among the processes
    MatrixAScatter();
    MatrixBScatter();
}

void LineMatrixMultiplicationMPI::ResultCollection() {
    MPI_Gather(pCLine, Size*LineSize, MPI_INT,
                pCMatrix, Size*LineSize, MPI_INT, 0, MPI_COMM_WORLD);

    if (procRank == 0) {
        result.setDataFromArray(pCMatrix);
    }
}

void LineMatrixMultiplicationMPI::LineBCommunication() const {
    int dest = procRank-1;
    int source = procRank+1;

    while (dest >= procSize)
        dest -= procSize;
    while (dest < 0)
        dest += procSize;

    while (source >= procSize)
        source -= procSize;
    while (source < 0)
        source += procSize;

    MPI_Status Status;
    MPI_Sendrecv_replace( pBLine, LineSize*Size, MPI_INT,
                          dest, 0, source, 0, MPI_COMM_WORLD, &Status);
}

void LineMatrixMultiplicationMPI::ParallelResultCalculation() {
    for (int iter = 0; iter < procSize; iter ++) {
        // Result calculation
        ResultCalculation((iter+procRank) % procSize);

        LineBCommunication();
    }
}

void LineMatrixMultiplicationMPI::ProcessTermination() {
    if (procRank == 0) {
        delete [] pAMatrix;
        delete [] pBMatrix;
        delete [] pCMatrix;
    }
    delete [] pALine;
    delete [] pBLine;
    delete [] pCLine;
}

void LineMatrixMultiplicationMPI::run() {
    startTimePoint();

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
