#include <iostream>
#include <vector>
#include <iomanip>

#include "OrdinaryMatrixMultiplicationMPI.h"
#include "FoxMatrixMultiplicationMPI.h"
#include "CannonMatrixMultiplicationMPI.h"

// mpiexec -n 4 univ3_1_DC_lab6.exe
int main(int argc, char *argv[]) {
    MPI_Init(&argc, &argv);
    int procNum, procRank;
    MPI_Comm_size(MPI_COMM_WORLD, &procNum);
    MPI_Comm_rank(MPI_COMM_WORLD, &procRank);

    std::vector<unsigned> sizes = {504, 1002, 1500, 2004, 2502, 3000};
    std::size_t N1 = 10;
    std::size_t N2 = 10;
    using type = CannonMatrixMultiplicationMPI;

    if (procRank == 0) {
        for (std::size_t i=0; i<N1; i++) {
            Matrix m1(sizes[0]), m2(sizes[0]);
            m1.randomize(10);
            m2.randomize(10);
            MatrixMultiplicationMPI* r1 = new OrdinaryMatrixMultiplicationMPI(m1, m2);
            MatrixMultiplicationMPI* r2 = new type(m1, m2);
            r1->run();
            r2->run();
            if (r1->getResult() != r2->getResult()) {
                std::cout << std::setw(3) << i+1 << std::setw(5) << sizes[0] << std::setw(15) << "FAILED" << std::endl;
                std::cout << m1 << "\n" << std::endl;
                std::cout << m2 << "\n" << std::endl;
                std::cout << r1->getResult() << "\n" << std::endl;
                std::cout << r2->getResult() << "\n" << std::endl;
                MPI_Abort(MPI_COMM_WORLD, 1);
            }
            std::cout << std::setw(3) << i+1 << std::setw(5) << sizes[0] << std::setw(15) << "passed" << std::endl;
            delete r1;
            delete r2;
        }
        std::cout << "-----------------------" << std::endl;
        std::cout << "                 passed\n\n" << std::endl;

        for (auto& size : sizes) {
            std::size_t average = 0;
            for (std::size_t n=0; n<N2; n++) {
                Matrix m1(size), m2(size);
                m1.randomize(10);
                m2.randomize(10);
                MatrixMultiplicationMPI* r = new type(m1, m2);
                r->run();
                average += r->getRunningTime();
                std::cout << std::setw(3) << n+1 << std::setw(5) << size << std::setw(15) << r->getRunningTime() << std::endl;
                delete r;
            }
            if (N2 > 0)
                average /= N2;
            std::cout << "-----------------------" << std::endl;
            std::cout << "   " << std::setw(5) << size << std::setw(15) << average << "\n\n" << std::endl;
        }
    }
    else {
        for (std::size_t i=0; i<N1; i++) {
            MatrixMultiplicationMPI* r1 = new OrdinaryMatrixMultiplicationMPI();
            MatrixMultiplicationMPI* r2 = new type();
            r1->run();
            r2->run();
            delete r1;
            delete r2;
        }

        for (auto& size : sizes) {
            for (std::size_t n=0; n<N2; n++) {
                MatrixMultiplicationMPI* r = new type();
                r->run();
                delete r;
            }
        }
    }

    MPI_Finalize();
    return 0;
}
