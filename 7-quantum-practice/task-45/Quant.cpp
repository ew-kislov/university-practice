#pragma once

#include "Quant.h"

using namespace Quant;

void Quant::one_qubit_evolution(
    complex<double>* a,
    complex<double>* b,
    const complex<double> u[2][2],
    unsigned n,
    unsigned k,
    int rank,
    unsigned long long chunk_size
) {
    int shift = n - k;
    int pow_shift = 1 << shift;
    int symm_rank = ((rank * chunk_size) ^ pow_shift) / chunk_size;

    if (rank != symm_rank) {
        complex<double>* local_a = new complex<double> [chunk_size];

        MPI_Sendrecv(a, chunk_size, MPI_DOUBLE_COMPLEX, symm_rank, 0, local_a, chunk_size, MPI_DOUBLE_COMPLEX, symm_rank, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

        if (rank > symm_rank) {
            for (uint32_t i = 0; i < chunk_size; i++) {
                b[i] = u[1][0] * local_a[i] + u[1][1] * a[i];
            }
        } else {
            for (int i = 0; i < chunk_size; i++) {
                b[i] = u[0][0] * a[i] + u[0][1] * local_a[i];
            }
        }

        delete[] local_a;
    } else {
        uint32_t i, i0, i1, is;

        for (i = 0; i < chunk_size; i++) {
            i0 = i & ~pow_shift;
            i1 = i | pow_shift;

            is = (i & pow_shift) >> shift;

            b[i] = u[is][0] * a[i0] + u[is][1] * a[i1];
        }
    }
}

void Quant::two_qubit_evolution(
    complex<double>* a,
    complex<double>* b,
    const complex<double> u[4][4],
    unsigned n,
    unsigned k1,
    unsigned k2,
    int rank,
    unsigned long long chunk_size
) {
    int base_i = rank * chunk_size;
	int shift1 = n - k1;
	int shift2 = n - k2;
	int pow1 = 1 << shift1;
	int pow2 = 1 << shift2;

    complex<double> *a1, *a2, *a3, *a4;

    int rank1_change = (base_i & ~pow1 & ~pow2) / chunk_size;
    int rank2_change = ((base_i & ~pow1) | pow2) / chunk_size;
    int rank3_change = ((base_i | pow1) & ~pow2) / chunk_size;
    int rank4_change = (base_i | pow1 | pow2) / chunk_size;

    a1 = a;
    a2 = a;
    a3 = a;
    a4 = a;

    Quant::resend(a, a1, a2, a3, a4, n, k1, k2, rank, chunk_size);

    for	(int i = 0; i < chunk_size; i++) {
        int g_i = base_i + i;

        int i1 = (g_i & ~pow1 & ~pow2) - rank1_change * chunk_size;
        int i2 = ((g_i & ~pow1) | pow2) - rank2_change * chunk_size;
        int i3 = ((g_i | pow1) & ~pow2) - rank3_change * chunk_size;
        int i4 = (g_i | pow1 | pow2) - rank4_change * chunk_size;

        int is1 = (g_i & pow1) >> shift1;
        int is2 = (g_i & pow2) >> shift2;
        int is = (is1 << 1) + is2;

        b[i] =  u[is][0] * a1[i1] + u[is][0] * a2[i2] + u[is][2] * a3[i3] + u[is][3] * a4[i4];
    }
}

void Quant::resend(
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
) {
    int base_i = rank * chunk_size;
	int shift1 = n - k1;
	int shift2 = n - k2;
	int pow1 = 1 << shift1;
	int pow2 = 1 << shift2;

    int rank1_change = (base_i & ~pow1 & ~pow2) / chunk_size;
    int rank2_change = ((base_i & ~pow1) | pow2) / chunk_size;
    int rank3_change = ((base_i | pow1) & ~pow2) / chunk_size;
    int rank4_change = (base_i | pow1 | pow2) / chunk_size;

    if (rank != rank1_change) {
        a1 = new complex<double>[chunk_size];
        MPI_Sendrecv(
            a,
            chunk_size,
            MPI_DOUBLE_COMPLEX,
            rank1_change,
            0,
            a1,
            chunk_size,
            MPI_DOUBLE_COMPLEX,
            rank1_change,
            0,
            MPI_COMM_WORLD,
            MPI_STATUS_IGNORE
        );
    } else {
        a1 = a;
    }

    if (rank != rank2_change) {
        a2 = new complex<double>[chunk_size];
        MPI_Sendrecv(
            a,
            chunk_size,
            MPI_DOUBLE_COMPLEX,
            rank2_change,
            0,
            a2,
            chunk_size,
            MPI_DOUBLE_COMPLEX,
            rank2_change,
            0,
            MPI_COMM_WORLD,
            MPI_STATUS_IGNORE
        );
    } else {
        a2 = a;
    }

    if (rank != rank3_change) {
        a3 = new complex<double>[chunk_size];
        MPI_Sendrecv(
            a,
            chunk_size,
            MPI_DOUBLE_COMPLEX,
            rank3_change,
            0,
            a3,
            chunk_size,
            MPI_DOUBLE_COMPLEX,
            rank3_change,
            0,
            MPI_COMM_WORLD,
            MPI_STATUS_IGNORE
        );
    } else {
        a3 = a;
    }

    if (rank != rank4_change) {
        a4 = new complex<double>[chunk_size];
        MPI_Sendrecv(
            a,
            chunk_size,
            MPI_DOUBLE_COMPLEX,
            rank4_change,
            0,
            a4,
            chunk_size,
            MPI_DOUBLE_COMPLEX,
            rank4_change,
            0,
            MPI_COMM_WORLD,
            MPI_STATUS_IGNORE
        );
    } else {
        a4 = a;
    }
}

void Quant::NOT(complex<double>* a, complex<double>* b, unsigned n, unsigned k, int rank, int size) {
    unsigned length = 1 << n;
    unsigned chunk_size = length / size;

    complex<double> u[2][2] = {
        {0, 1},
        {1, 0}
    };
    
    one_qubit_evolution(a, b, u, n, k, rank, chunk_size);
}

void Quant::CNOT(complex<double>* a, complex<double>* b, unsigned n, unsigned k1, unsigned k2, int rank, int size) {
    unsigned length = 1 << n;
    unsigned chunk_size = length / size;

    complex<double> u[4][4] = {
        { 1, 0, 0, 0 },
        { 0, 1, 0, 0 },
        { 0, 0, 0, 1 },
        { 0, 0, 1, 0 }
    };

    two_qubit_evolution(a, b, u, n, k1, k2, rank, chunk_size);
}

void Quant::ROT(complex<double>* a, complex<double>* b, unsigned n, unsigned k, int rank, int size) {
    unsigned length = 1 << n;
    unsigned chunk_size = length / size;

    complex<double> u[2][2] = {
        {1, 0},
        {0, -1}
    };
    
    one_qubit_evolution(a, b, u, n, k, rank, chunk_size);
}

void Quant::CROT(complex<double>* a, complex<double>* b, unsigned n, unsigned k1, unsigned k2, int rank, int size) {
    unsigned length = 1 << n;
    unsigned chunk_size = length / size;

    complex<double> u[4][4] = {
        { 1, 0, 0, 0 },
        { 0, 1, 0, 0 },
        { 0, 0, 1, 0 },
        { 0, 0, 0, -1 }
    };

    two_qubit_evolution(a, b, u, n, k1, k2, rank, chunk_size);
}

void Quant::H(complex<double>* a, complex<double>* b, unsigned n, unsigned k, int rank, int size) {
    unsigned length = 1 << n;
    unsigned chunk_size = length / size;

    double koeff = 1 / sqrt(2);

    complex<double> u[2][2] = {
        {koeff, koeff},
        {koeff, -koeff}
    };
    
    one_qubit_evolution(a, b, u, n, k, rank, chunk_size);
}

void Quant::nH(complex<double>* a, complex<double>* b, unsigned n, unsigned k, int rank, int size) {
    for (int i = 1; i <= n; i++) {
        H(a, b, n, i, rank, size);
    }
}

void Quant::qft(complex<double>* a, complex<double>* b, unsigned n, int rank, int size) {
    unsigned length = 1 << n;
    unsigned chunk_size = length / size;

    double koeff = 1 / sqrt(2);

    complex<double> H[2][2] = {
        {koeff, koeff},
        {koeff, -koeff}
    };

    complex<double> R[4][4] = {
        {1, 0, 0, 0},
        {0, 1, 0, 0},
        {0, 1, 0, 0},
        {0, 0, 0, 0}
    };

    for (int k1 = 1; k1 <= n; k1++) {
        one_qubit_evolution(a, b, H, n, k1, rank, chunk_size);
        for (int k2 = k1 + 1; k2 <= n; k2++) {
            R[3][3] = exp(2 * M_PI * complex<double>(0, 1) / double(1 << (k2 - k1 + 1)));
            two_qubit_evolution(a, b, R, n, k1, k2, rank, chunk_size);
        }
    }
}