#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include "omp.h"

#define Max(a, b) ((a) > (b) ? (a) : (b))
#define Min(a, b) ((a) < (b) ? (a) : (b))

#define N (2 * 2 * 2 * 2 * 2 * 2 * 2)

double maxeps = 0.1e-7;
int itmax = 100;
int i, j, k;
double eps;

double A[N][N][N];

void relax();
void init();
void verify();

int main(int an, char **as) {
	int it;

	double start_time = omp_get_wtime();

	init();

	for (it = 1; it <= itmax; it++) {
		eps = 0.;
		relax();
		if (it == itmax) {
			printf("it=%4i   eps=%f\n", it, eps);
		}
		if (eps < maxeps) {
			break;
		}
	}

	verify();

	double time = omp_get_wtime() - start_time;

	printf("Spent %f seconds\n", time);

	return 0;
}

void init() {
	#pragma omp parallel for num_threads(16) shared(A) private(i,j,k)
	for (k = 0; k <= N - 1; k++) {
		for (j = 0; j <= N - 1; j++) {
			for (i = 0; i <= N - 1; i++) {
				if (i == 0 || i == N - 1 || j == 0 || j == N - 1 || k == 0 || k == N - 1) {
					A[i][j][k] = 0.;
				} else {
					A[i][j][k] = (4. + i + j + k);
				}
			}
		}
	}
}

void relax() {
	for (int wave = 2; wave <= 2 * (N - 2); wave++) {
		#pragma omp parallel shared(A, eps) num_threads(16)
		{
			double local_eps = 0.;
			
			#pragma omp for private(i, j, k)
			for (i = Max(1, wave - (N - 2)); i <= Min((N - 2), wave - 1); i++) {
				j = wave - i;
				for (k = 1; k <= (N - 2); k++) {
					double e = A[i][j][k];
					A[i][j][k] = (A[i - 1][j][k] + A[i + 1][j][k] + A[i][j - 1][k] + A[i][j + 1][k] + A[i][j][k - 1] + A[i][j][k + 1]) / 6.0;
					local_eps = Max(local_eps, fabs(e - A[i][j][k]));
				}
			}
			
			#pragma omp critical
			{
				eps = Max(eps, local_eps);
			}
		}
	}
}

void verify() {
	double s;

	s = 0.;
	#pragma omp parallel for num_threads(16) ordered shared(A, s) private(i,j,k)
	for (k = 0; k <= N - 1; k++) {
		for (j = 0; j <= N - 1; j++) {
			for (i = 0; i <= N - 1; i++) {
				#pragma omp critical
				{
					s = s + A[i][j][k] * (i + 1) * (j + 1) * (k + 1) / (N * N * N);
				}
			}
		}
	}
	printf("  S = %f\n", s);
}