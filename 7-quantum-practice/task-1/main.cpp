#include <cstdlib>
#include <iostream>
#include <ctime>
#include <complex>
#include <time.h>
#include <sys/time.h>

using namespace std;

int main(int argc, char* argv[]) {
    // processing args

    if (argc != 3) {
        cerr << "Invalid number of arguments. Usage - \"./main <n> <k>\"" << endl;
        exit(1);
    }

    const uint8_t n = stoi(argv[1]);
    const uint8_t k = stoi(argv[2]);

    // creating normalized input vector a

    const uint32_t length = 1 << n;

    complex<float_t>* a = new complex<float_t>[length];

    srand(time(0));

    double_t norm = 0;

    for (uint32_t i = 0; i < length; i++) {
        a[i] = complex<float_t>(rand() % 100, rand() % 100);
        norm += a[i].imag() * a[i].imag() + a[i].real() * a[i].real();
    }

    norm = sqrt(norm);

    for (uint32_t i = 0; i < length; i++) {
        a[i] /= norm;
    }

    // one qubit evolution algorithm

    complex<float_t>* b = new complex<float_t>[length];

    const uint8_t shift = n - k;
	const uint32_t pow_shift = 1 << shift;

    const float_t adamar_coeff = 1 / sqrt(2);

    const complex<float_t> u[2][2] {
        { adamar_coeff, adamar_coeff },
        { -adamar_coeff, adamar_coeff }
    };

    struct timeval start_time;
    gettimeofday(&start_time, NULL);

    uint32_t i, i0, i1, is;

	for	(i = 0; i < length; i++) {
		i0 = i & ~pow_shift;
		i1 = i | pow_shift;
		
		is = (i & pow_shift) >> shift;

		b[i] = u[is][0] * a[i0] + u[is][1] * a[i1];
	}

    struct timeval end_time;
    gettimeofday(&end_time, NULL);

    cout << ((end_time.tv_sec * 1000000 + end_time.tv_usec) - (start_time.tv_sec * 1000000 + start_time.tv_usec)) / 1e6 << endl;

    delete[] a;
    delete[] b;
}