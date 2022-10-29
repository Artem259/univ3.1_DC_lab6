#include "OrdinaryMatrixMultiplicationMPI.h"

void OrdinaryMatrixMultiplicationMPI::run() {
    if (procRank != 0)
        return;

    startTimePoint();
    int* array1 = m1.getDataAsArray();
    int* array2 = m2.getDataAsArray();
    int* arrayRes = new int[size*size]();
    for (int i=0; i<size; i++) {
        for (int j=0; j<size; j++) {
            for (int k=0; k<size; k++) {
                arrayRes[i*size+j] += array1[i*size+k]*array2[k*size+j];
            }
        }
    }
    result.setDataFromArray(arrayRes);
    delete []array1;
    delete []array2;
    delete []arrayRes;
    finishTimePoint();
}
