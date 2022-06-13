# Quantum practice

## Task 1

### Run on supercomputer

- `cd task1 && make omp_polus && ./main_omp`

## Task 2

### Run

- `make mpi_mac && mpirun -np <proc_num> ./main <n> <k> [test]`  

`./main <n> <k>` to generate input and output arrays, `./main <n> <k> test` to test with generated arrays.  

## Task 3

### Run

- `make mpi_mac && mpirun -np <proc_num> ./main <n> <noise> [test]`  

`./main <n> <noise>` to generate input and output arrays, `./main <n> <noise> test` to test with generated arrays.

## Task 4 + 5

- `make`

- `mpirun -np <n> ./test <mode>`, where mode - ROT, CROT, QFT, ...
