cmake_minimum_required(VERSION 3.22)
project(univ3_1_DC_lab6)

set(CMAKE_CXX_STANDARD 14)

find_package(MPI REQUIRED)

add_executable(univ3_1_DC_lab6
        main.cpp
        MatrixMultiplicationMPI.h
        MatrixMultiplicationMPI.cpp
        Matrix.cpp
        Matrix.h
        FoxMatrixMultiplicationMPI.cpp
        FoxMatrixMultiplicationMPI.h
        OrdinaryMatrixMultiplicationMPI.cpp
        OrdinaryMatrixMultiplicationMPI.h
        CannonMatrixMultiplicationMPI.cpp
        CannonMatrixMultiplicationMPI.h
        LineMatrixMultiplicationMPI.cpp
        LineMatrixMultiplicationMPI.h)

target_link_libraries(univ3_1_DC_lab6 PUBLIC MPI::MPI_CXX)