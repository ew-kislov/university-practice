#ifndef QUANT_H
#define QUANT_H

#include <mpi.h>
#include <complex>

using namespace std;

namespace Quant {
    void one_qubit_evolution(
        complex<double>* a,
        complex<double>* b,
        const complex<double> u[2][2],
        unsigned n,
        unsigned k,
        int rank,
        unsigned long long chunk_size
    );

    void two_qubit_evolution(
        complex<double>* a,
        complex<double>* b,
        const complex<double> u[4][4],
        unsigned n,
        unsigned k1,
        unsigned k2,
        int rank,
        unsigned long long chunk_size
    );

    void resend(
        complex<double>* a,
        complex<double>* a1,
        complex<double>* a2,
        complex<double>* a3,
        complex<double>* a4,
        unsigned n,
        unsigned k1,
        unsigned k2,
        int rank,
        unsigned long long chunk_size
    );

    /**
     * Gates
     */
    void NOT(complex<double>* a, complex<double>* b, unsigned n, unsigned k, int rank, int size);
    void CNOT(complex<double>* a, complex<double>* b, unsigned n, unsigned k1, unsigned k2, int rank, int size);
    void H(complex<double>* a, complex<double>* b, unsigned n, unsigned k, int rank, int size);
    void nH(complex<double>* a, complex<double>* b, unsigned n, unsigned k, int rank, int size);
    void ROT(complex<double>* a, complex<double>* b, unsigned n, unsigned k, int rank, int size);
    void CROT(complex<double>* a, complex<double>* b, unsigned n, unsigned k1, unsigned k2, int rank, int size);

    /**
     * Quantum fourier transform
     */

    void qft(complex<double>* a, complex<double>* b, unsigned n, int rank, int size);
}

#endif