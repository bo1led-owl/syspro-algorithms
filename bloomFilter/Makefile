run: main
	@./main

main: main.cc
	clang++ -std=c++23 -march=native -ffast-math -O3 -fsanitize=address,undefined main.cc -o main
