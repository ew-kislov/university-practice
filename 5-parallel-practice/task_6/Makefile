compile:
	mpixlc sor_rd_mpi.c
run:
	mpisubmit.bg -np 1 ./a.out --stdout 32.csv
	mpirun -np 2 ./a.out
	mpirun -np 4 ./a.out
	mpirun -np 8 ./a.out
	mpirun -np 16 ./a.out
	mpirun -np 32 ./a.out
	mpirun -np 64 ./a.out
	mpirun -np 128 ./a.out
	mpirun -np 256 ./a.out
	mpirun -np 512 ./a.out