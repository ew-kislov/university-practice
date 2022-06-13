#include <mpi.h>
#include <iostream>
#include <fstream>
#include <cmath>
#include <string>
#include "stdlib.h"
#include "string.h"
#include <map>
#include <complex>

#include "Quant.cpp"

using namespace std;

int prank, psize;

double sqr(double d) {
    return d * d;
}

bool test(char* gate) {
    map<string, complex<double>* > output_map {
        {"H", new complex<double>[4] { 0.141421, 0.0707107, 0.141421, -0.0707107 } },
        {"nH", new complex<double>[4] { 0.141421, 0.141421, 0.0707107, -0.0707107 } },
        {"ROT", new complex<double>[4] { 0.2, 0, 0, -0.1 } },
        {"CROT", new complex<double>[4] { 0.2, 0, 0, -0.1 } },
        {"NOT", new complex<double>[4] { 0, 0.1, 0.2, 0 } },
        {"CNOT", new complex<double>[4] { 0.2, 0, 0.1, 0 } },
        {"QFT", new complex<double>[4] { 0.141421, 0.141421, 0.0707107, -0.0707107 } }
    };

    const double eps = 1e-6;

    if (psize > 4) {
        throw runtime_error("Process size must be one of 1, 2, 4");
    }

    complex<double>* a_total = new complex<double>[4] { 0.2, 0, 0, 0.1 };
    complex<double>* b = new complex<double>[4 / psize];

    complex<double>* a = new complex<double>[4 / psize];
    memcpy(a, &(a_total[(prank) * 4 / psize]), sizeof(complex<double>) * 4 / psize);



    if (strcmp(gate, "CROT") == 0) {
        Quant::CROT(a, b, 2, 1, 2, prank, psize);
    } else if (strcmp(gate, "CNOT") == 0) {
        Quant::CNOT(a, b, 2, 1, 2, prank, psize);
    } else if (strcmp(gate, "ROT") == 0) {
        Quant::ROT(a, b, 2, 1, prank, psize);
    } else if (strcmp(gate, "NOT") == 0) {
        Quant::NOT(a, b, 2, 1, prank, psize);
    } else if (strcmp(gate, "H") == 0) {
        Quant::H(a, b, 2, 1, prank, psize);
    } else if (strcmp(gate, "nH") == 0) {
        Quant::nH(a, b, 2, 1, prank, psize);
    } else if (strcmp(gate, "QFT") == 0) {
        Quant::qft(a, b, 2, prank, psize);
    }

    double norm_local = 0.0;
    double norm = 0.0;


    for (int i = (prank) * 4 / psize; i < (prank + 1) * 4 / psize; i++) {
        complex<double> output_element = output_map[gate][i];
        complex<double> test_element = b[i - (prank) * 4 / psize];
        norm_local += sqr(abs(output_map[gate][i]) - abs(b[i - (prank) * 4 / psize]));
    }

    MPI_Reduce(&norm_local, &norm, 1, MPI_DOUBLE, MPI_SUM, 0, MPI_COMM_WORLD);

    norm = sqrt(norm);

    delete[] a;
    delete[] b;

    return norm < eps;
}

int main(int argc, char** argv) {
    MPI_Init(&argc, &argv);

    MPI_Comm_rank(MPI_COMM_WORLD, &prank);
    MPI_Comm_size(MPI_COMM_WORLD, &psize);

    if (argc != 2) {
        throw runtime_error("Program accepts one argument");
    }

    char* gate = argv[1];

    bool test_result = test(gate);

    assert(test_result == true);

    MPI_Finalize();

    return 0;
}