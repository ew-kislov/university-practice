all: lint build
lint:
	clang-format -i -style=Chromium src/*
build:
	clang++ src/*.cpp -o main -std=c++17 -pthread -Werror
