#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#include <mpi.h>

#define  Max(a,b) ((a)>(b)?(a):(b))

double   maxeps = 0.1e-7;
int itmax = 100;
int i,j,k;
double eps;

int N = 2*2*2*2*2*2*2;
double *** A;
double *** localA;
double ** matrixSlice;
double ** blockSlice;

double totalTime = 0.0;

void relax();
void init();
void verify(); 

int main(int argc, char **argv) {
	MPI_Init(&argc, &argv);

	int it;
	int rank, count;

	MPI_Comm_size(MPI_COMM_WORLD, &count);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);

	if (argc == 2) {
		if (rank == 0) {
			sscanf(argv[1], "%d", &N);
		}
		MPI_Bcast(&N, 1, MPI_INT, 0, MPI_COMM_WORLD);
	}

	init();

	for(it = 1; it <= itmax; it++) {
		eps = 0.;
		relax();

		double maxEps;
		MPI_Reduce(&eps, &maxEps, 1, MPI_DOUBLE, MPI_MAX, 0, MPI_COMM_WORLD);
		if (rank == 0) {
			eps = maxEps;
		}
		MPI_Bcast(&eps, 1, MPI_DOUBLE, 0, MPI_COMM_WORLD);
		if (eps < maxeps) break;
	}

	verify();

	if (rank == 0) {
		printf("Total time = %.6lf\n", totalTime);
	}

	for (i = 0; i < N / count; i++) {
		for (j = 0; j < N; j++) {
			free(localA[i][j]);
		}
		free(localA[i]);
	}
	free(localA);

	if (rank != count - 1) {
		for (i = 0; i < N; i++) {
			free(matrixSlice[i]);
		}
		free(matrixSlice);
	}

	if (rank != 0) {
		for (i = 0; i < N / count; i++) {
			free(blockSlice[i]);
		}
		free(blockSlice);
	}

	MPI_Finalize();
	return 0;
}


void init()
{
	int count, rank;
	MPI_Comm_size(MPI_COMM_WORLD, &count);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);

	double begin = MPI_Wtime();
	
	localA = (double ***)malloc(N / count * sizeof(double **));
	for (i = 0; i < N / count; i++) {
		localA[i] = (double **)malloc(N * sizeof(double *));
		for (j = 0; j < N; j++) {
			localA[i][j] = (double *)malloc(N * sizeof(double));
		}
	}

	if (rank != count - 1) {
		matrixSlice = (double **)malloc(N * sizeof(double *));
		for (i = 0; i < N; i++) {
			matrixSlice[i] = (double *)malloc(N * sizeof(double));
		}
	}

	if (rank != 0) {
		blockSlice = (double **)malloc(N / count * sizeof(double *));
		for (i = 0; i < N / count; i++) {
			blockSlice[i] = (double *)malloc(N * sizeof(double));
		}
	}

	for (i = 0; i < N / count; ++i)
	    for (j = 0; j < N; ++j)
	        for (k = 0; k < N; ++k) {
                double value = 4.0 + (double)((i + 1 + rank * (N / count)) + (j + 1) + (k + 1));
                localA[i][j][k] = value;
	}

	begin = MPI_Wtime() - begin;
	double initTime;

	MPI_Reduce(&begin, &initTime, 1, MPI_DOUBLE, MPI_MAX, 0, MPI_COMM_WORLD);
	if (rank == 0) {
		totalTime += initTime;
	}
} 


void relax() {
	int count, rank;
	MPI_Status st;
	MPI_Comm_size(MPI_COMM_WORLD, &count);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);

	double begin = MPI_Wtime();

	int blockSize = N / count;

	if (count != 1) {
		for (i = 0; i < N; i++) {
			if (rank % 2 == 0) {
				if (rank != 0) {
					MPI_Send(localA[0][i], N, MPI_DOUBLE, rank - 1, 0, MPI_COMM_WORLD);
				}
				MPI_Recv(matrixSlice[i], N, MPI_DOUBLE, rank + 1, 0, MPI_COMM_WORLD, &st);
			} else {
				if (rank != count - 1) {
					MPI_Recv(matrixSlice[i], N, MPI_DOUBLE, rank + 1, 0, MPI_COMM_WORLD, &st);
				}
				MPI_Send(localA[0][i], N, MPI_DOUBLE, rank - 1, 0, MPI_COMM_WORLD);
			}
		}
	}

	int blockNum;
	for (blockNum = 0; blockNum < count; blockNum++) {
		if (rank != 0) {
			for (i = 0; i < blockSize; i++) {
				MPI_Recv(blockSlice[i], N, MPI_DOUBLE, rank - 1, 0, MPI_COMM_WORLD, &st);
			}
		}

		for (i = 0; i < blockSize; i++) {
			for (j = 0; j < blockSize; j++) {
				for (k = 0; k < N; k++) {
					int realj = j + blockNum * blockSize;
					double up, down, left, right, forward, backward;
					up = i != 0 ? localA[i - 1][realj][k] : (rank == 0 ? 0.0 : blockSlice[j][k]);
					down = i != blockSize - 1 ? localA[i + 1][realj][k] : (rank == count - 1 ? 0.0 : matrixSlice[realj][k]);
					left = realj == 0 ? 0.0 : localA[i][realj - 1][k];
					right = realj == N - 1 ? 0.0 : localA[i][realj + 1][k];
					forward = k == N - 1 ? 0.0 : localA[i][realj][k + 1];
					backward = k == 0 ? 0.0 : localA[i][realj][k - 1];

					double e = localA[i][realj][k];

					localA[i][realj][k] = (up + down + left + right + forward + backward) / 6.0;
					eps = Max(eps, fabs(e - localA[i][realj][k]));
				}
			}
		}

		if (rank != count - 1) {
			for (i = 0; i < blockSize; i++) {
				MPI_Send(localA[blockSize - 1][i + blockNum * blockSize], N, MPI_DOUBLE, rank + 1, 0, MPI_COMM_WORLD);
			}
		}
	}

	begin = MPI_Wtime() - begin;
	double relaxTime;

	MPI_Reduce(&begin, &relaxTime, 1, MPI_DOUBLE, MPI_MAX, 0, MPI_COMM_WORLD);
	if (rank == 0) {
		totalTime += relaxTime;
	}    
}


void verify() { 
	double s = 0.0;

	int count, rank;
	MPI_Comm_size(MPI_COMM_WORLD, &count);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);

	double begin = MPI_Wtime();

	for (i = 0; i < N / count; i++)
	    for (j = 0; j < N; j++)
	        for (k = 0; k < N; k++) {
		s += localA[i][j][k] * ((i + rank * (N / count)) + 2) * (j + 2) * (k + 2) / ((N + 2) * (N + 2) * (N + 2));
	}

	double totalSum;
	MPI_Reduce(&s, &totalSum, 1, MPI_DOUBLE, MPI_SUM, 0, MPI_COMM_WORLD);
	if (rank == 0) {
		// printf("  S = %f\n", totalSum);
	}

	begin = MPI_Wtime() - begin;
	double verifyTime;
	MPI_Reduce(&begin, &verifyTime, 1, MPI_DOUBLE, MPI_MAX, 0, MPI_COMM_WORLD);
	if (rank == 0) {
		totalTime += verifyTime;
	}
}