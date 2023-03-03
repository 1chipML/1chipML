CC=gcc

CFLAGS += -g # adds debugging information
CFLAGS += -Wall # turns on most compiler warnings
CFLAGS += -std=c99# C99
CFLAGS += -I./src # included header files

# loaded libraries
LDLIBS += -lm # Math library

all: linear_congruential_random_generator gauss_elimination poly_interpolation DFT FFT lanczos monte_carlo

test: all run_all_tests

linear_congruential_random_generator: ./tests/test_linear_congruential_random_generator.c ./src/linear_congruential_random_generator.c
	$(CC) $(CFLAGS) $^ -o test_$@.out $(LDLIBS)

gauss_elimination: ./tests/test_gauss_elimination.c ./src/gauss_elimination.c
	$(CC) $(CFLAGS) $^ -o test_$@.out $(LDLIBS)

poly_interpolation: ./tests/test_poly_interpolation.c ./src/poly_interpolation.c
	$(CC) $(CFLAGS) $^ -o test_$@.out $(LDLIBS)

FFT: ./tests/test_FFT.c ./src/FFT.c ./src/DFT.c ./src/linear_congruential_random_generator.c
	$(CC) $(CFLAGS) $^ -o test_$@.out $(LDLIBS)

DFT: ./tests/test_DFT.c ./src/DFT.c ./src/linear_congruential_random_generator.c
	$(CC) $(CFLAGS) $^ -o test_$@.out $(LDLIBS)

lanczos: ./tests/test_lanczos.c ./src/lanczos.c ./src/linear_congruential_random_generator.c 
	$(CC) $(CFLAGS) $^ -o test_$@.out $(LDLIBS)


monte_carlo:
	$(CC) ./tests/test_monte_carlo.c ./src/monte_carlo.c ./src/linear_congruential_random_generator.c -lm -Wall -std=c99 -I./src -o test_monte_carlo.out

run_all_tests:
	./test_linear_congruential_random_generator.out
	./test_gauss_elimination.out
	./test_poly_interpolation.out
	./test_monte_carlo.out
	./test_DFT.out
	./test_FFT.out
	./test_lanczos.out

clean:
	rm -rf test_*.out
