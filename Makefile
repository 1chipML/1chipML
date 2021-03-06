CC=gcc

all: linear_congruential_random_generator gauss_elimination poly_interpolation

test: all run_all_tests

linear_congruential_random_generator:
	$(CC) ./tests/test_linear_congruential_random_generator.c ./src/linear_congruential_random_generator.c -lm -Wall -std=c99 -I./src -o test_linear_congruential_random_generator.out

gauss_elimination:
	$(CC) ./tests/test_gauss_elimination.c ./src/gauss_elimination.c -lm -Wall -std=c99 -I./src -o test_gauss_elimination.out

poly_interpolation:
	$(CC) ./tests/test_poly_interpolation.c ./src/poly_interpolation.c -lm -Wall -std=c99 -I./src -o test_poly_interpolation.out

run_all_tests:
	./test_linear_congruential_random_generator.out
	./test_gauss_elimination.out
	./test_poly_interpolation.out

clean:
	rm -rf test_*.out
