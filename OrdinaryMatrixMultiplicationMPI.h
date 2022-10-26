#ifndef ORDINARYMATRIXMULTIPLICATIONMPI_H
#define ORDINARYMATRIXMULTIPLICATIONMPI_H

#include "MatrixMultiplicationMPI.h"

class OrdinaryMatrixMultiplicationMPI : public MatrixMultiplicationMPI {
public:
    using MatrixMultiplicationMPI::MatrixMultiplicationMPI;
    void run() override;
};


#endif //ORDINARYMATRIXMULTIPLICATIONMPI_H
