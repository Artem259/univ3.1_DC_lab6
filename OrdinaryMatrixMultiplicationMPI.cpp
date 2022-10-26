#include "OrdinaryMatrixMultiplicationMPI.h"

void OrdinaryMatrixMultiplicationMPI::run() {
    startTimePoint();

    auto size = m1.getSize();
    for (std::size_t i=0; i<size; i++) {
        for (std::size_t j=0; j<size; j++) {
            for (std::size_t k=0; k<size; k++) {
                result.setElementAt(i, j, result.getElementAt(i,j) + m1.getElementAt(i, k)*m2.getElementAt(k, j));
            }
        }
    }

    finishTimePoint();
}
