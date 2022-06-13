.PHONY: all build_unix build_mac
build_unix:
	g++ -fopenmp main.cpp -o main
build_mac:
	clang++ -Xpreprocessor -fopenmp -O3  main.cpp -o main -lomp -std=c++11
clean:
	rm ./main