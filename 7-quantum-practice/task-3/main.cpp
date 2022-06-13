#include <mpi.h>
#include <iostream>
#include <fstream>
#include <string>
#include <complex>

using namespace std;

complex<double>* file_to_complex_array(string inp_file, int rank, uint32_t chunk_size) {
    MPI_File file;

    MPI_File_open(MPI_COMM_WORLD, inp_file.c_str(), MPI_MODE_RDONLY, MPI_INFO_NULL, &file);
    MPI_File_set_view(file, 2 * chunk_size * rank * sizeof(double), MPI_DOUBLE, MPI_DOUBLE, "native", MPI_INFO_NULL);

    double complex_arr[2];
    complex<double>* arr = new complex<double>[chunk_size];

    for (int i = 0; i < chunk_size; i++) {
        MPI_File_read(file, &complex_arr, 2, MPI_DOUBLE, MPI_STATUS_IGNORE);
        arr[i].real(complex_arr[0]);
        arr[i].imag(complex_arr[1]);
    }

    MPI_File_close(&file);

    return arr;
}

complex<double>* random_complex_vector(uint32_t chunk_size, int rank, int size) { 
    double local_norm;
    double norm = 0;

    complex<double>* local_arr = new complex<double>[chunk_size];
    uint32_t seed = time(NULL) + rank * rank * 123456789;

    for (uint32_t i = 0; i < chunk_size; i++){
        local_arr[i] = complex<double>(rand_r(&seed) % 100, rand_r(&seed) % 100);
        local_norm += local_arr[i].imag() * local_arr[i].imag() + local_arr[i].real() * local_arr[i].real();
    }
    
    MPI_Reduce(&local_norm, &norm, 1, MPI_DOUBLE, MPI_SUM, 0, MPI_COMM_WORLD);

    norm = sqrt(norm);

    MPI_Bcast(&norm, 1, MPI_DOUBLE, 0, MPI_COMM_WORLD);

    for (uint32_t i = 0; i < chunk_size; i++) {
        local_arr[i] /= norm;
    }

    return local_arr;
}

void complex_array_to_file(string out_file, complex<double>* arr, uint8_t n, int rank, int size, uint32_t chunk_size) {
    MPI_File file;

    MPI_File_open(MPI_COMM_WORLD, out_file.c_str(), MPI_MODE_CREATE | MPI_MODE_WRONLY, MPI_INFO_NULL, &file);
    MPI_File_set_view(file, 2 * chunk_size * rank * sizeof(double), MPI_DOUBLE, MPI_DOUBLE, "native", MPI_INFO_NULL);

    double complex_arr[2];

    for (int i = 0; i < chunk_size; i++) {
        complex_arr[0] = arr[i].real();
        complex_arr[1] = arr[i].imag();
        MPI_File_write(file, &complex_arr, 2, MPI_DOUBLE, MPI_STATUS_IGNORE);
    }

    MPI_File_close(&file);
}

void one_qubit_evol(complex<double>* a, complex<double>* b, const complex<double> u[2][2], unsigned n, unsigned k, int rank, unsigned long long chunk_size) {
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

/**
 * Taken from task description
 */
double normal_distribution() {
    double s = 0.;
    for (int i = 0; i < 12; i++) {
        s += (double)rand() / RAND_MAX;
    }
    return s - 6.;
}

double fidelity(complex<double>* b, complex<double>* b_n, unsigned long long chunk_size, int rank) {
    MPI_Status status;
    double local_dot = 0, global_dot = 0;
    for (int i = 0; i < chunk_size; i++) {
        local_dot += abs(b[i] * conj(b_n[i]));
    }
    MPI_Reduce(&local_dot, &global_dot, 1, MPI_DOUBLE, MPI_SUM, 0, MPI_COMM_WORLD);
    return global_dot;
}

int main(int argc, char** argv) {
    // processing args

    if (argc > 4 || argc < 3) {
        cerr << "Invalid number of arguments. Usage - \"./main <n> <noise> [test]\"" << endl;
        exit(1);
    }

    const uint8_t n = stoi(argv[1]);
    const double noise = stod(argv[2]);
    const uint8_t is_test = (argc == 4 && string(argv[3]).compare("test") == 0);

    // init MPI
    
    MPI_Init(&argc, &argv);

    int rank, size;

    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    // creating/reading input vector a

    const uint32_t length = 1 << n;
    const uint32_t chunk_size = length / size;

    complex<double>* a = is_test ? file_to_complex_array("inp.bin", rank, chunk_size) : random_complex_vector(chunk_size, rank, size);

    const double adamar_coeff = 1 / sqrt(2);

    const complex<double> u[2][2] {
        { adamar_coeff, adamar_coeff },
        { -adamar_coeff, adamar_coeff }
    };

    // one qubit evolution algorithm

    complex<double>* b = new complex<double>[chunk_size];
    complex<double>* b_n = new complex<double>[chunk_size];

    double start_time = MPI_Wtime();

    for (int k = 1; k <= n; k++) {
        one_qubit_evol(a, b, u, n, k, rank, chunk_size);
    }

    complex<double> u_n[2][2];

    for (int k = 1; k <= n; k++) {
        double ksi;
        double theta;

        if (rank == 0) {
            ksi = normal_distribution();
            theta = noise * ksi;
        }

        MPI_Bcast(&theta, 1, MPI_DOUBLE, 0, MPI_COMM_WORLD);

        double cos_theta = cos(theta);
        double sin_theta = sin(theta);

        u_n[0][0] = u[0][0] * cos_theta - u[0][1] * sin_theta;
        u_n[0][1] = u[0][0] * sin_theta + u[0][1] * cos_theta;
        u_n[1][0] = u[1][0] * cos_theta - u[1][1] * sin_theta;
        u_n[1][1] = u[1][0] * sin_theta + u[1][1] * cos_theta;
        one_qubit_evol(a, b_n, u_n, n, k, rank, chunk_size);
    }
    
    double end_time = MPI_Wtime();
    double time = end_time - start_time;

    // for generating mode: output to files

    if (!is_test) {
        complex_array_to_file("inp.bin", a, n, rank, size, chunk_size);
        complex_array_to_file("out.bin", b, n, rank, size, chunk_size);

        double res_fidelity = fidelity(b, b_n, chunk_size, rank);

        if (rank == 0) {
            cout << noise << " " << time << " " << 1 - res_fidelity << endl;
        }

        MPI_Finalize();

        delete[] a;
        delete[] b;
        delete[] b_n;

        return 0;
    }

    // for test mode: check for errors and output time to stdin

    uint8_t is_local_error = false;
    uint8_t is_error = false;

    complex<double>* test_a = file_to_complex_array("out.bin", rank, chunk_size);

    for (int i = 0; i < chunk_size; i++) {
        if (test_a[i].real() != b[i].real() || test_a[i].imag() != b[i].imag()) {
            is_local_error = true;
            break;
        }
    }

    MPI_Reduce(&is_local_error, &is_error, 1, MPI_UINT8_T, MPI_LAND, 0, MPI_COMM_WORLD);

    if (rank == 0 && is_error) {
        cerr << "Error on processors count = " << size << endl;
    } else if (rank == 0 && !is_error) {
        cout << unsigned(n) << " " << noise << " " << size << " " << end_time - start_time << endl;
    }

    MPI_Finalize();
    
    delete[] a;
    delete[] test_a;
    delete[] b;
    delete[] b_n;

    return 0;
}