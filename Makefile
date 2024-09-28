parallel_sum: parallel_sum.cc
	g++ -o $@ $< -fopenmp -Wall -Wpedantic
