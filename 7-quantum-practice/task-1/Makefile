omp_mac:
	clang++ -Xpreprocessor -fopenmp main_omp.cpp -o main_omp -lomp -std=c++11
serial_mac:
	g++ main.cpp -o main -std=c++11
omp_polus:
	xlc++ -qsmp=omp -fopenmp main_omp.cpp -o main_omp -std=c++11
serial_polus:
	g++ main.cpp -o main -std=c++11