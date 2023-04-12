CC=gcc
TEST_FOLDER = tests
BUILD_FOLDER = $(TEST_FOLDER)/build

CFLAGS += -g # adds debugging information
CFLAGS += -Wall # turns on most compiler warnings
CFLAGS += -std=c99# C99
CFLAGS += -I./src # included header files

# loaded libraries
LDLIBS += -lm # Math library

all: linear_congruential_random_generator gauss_elimination poly_interpolation DFT FFT lanczos jacobi genetic stats

test: all run_all_tests

linear_congruential_random_generator: ./$(TEST_FOLDER)/test_linear_congruential_random_generator.c ./src/linear_congruential_random_generator.c | build_folder
	$(CC) $(CFLAGS) $^ -o $(BUILD_FOLDER)/test_$@.out $(LDLIBS)
  
genetic : ./$(TEST_FOLDER)/test_genetic.c ./src/genetic.c  ./src/linear_congruential_random_generator.c | build_folder
	$(CC) $(CFLAGS) $^ -o $(BUILD_FOLDER)/test_$@.out $(LDLIBS)

gauss_elimination: ./$(TEST_FOLDER)/test_gauss_elimination.c ./src/gauss_elimination.c | build_folder
	$(CC) $(CFLAGS) $^ -o $(BUILD_FOLDER)/test_$@.out $(LDLIBS)

poly_interpolation: ./$(TEST_FOLDER)/test_poly_interpolation.c ./src/poly_interpolation.c | build_folder
	$(CC) $(CFLAGS) $^ -o $(BUILD_FOLDER)/test_$@.out $(LDLIBS)

jacobi: ./$(TEST_FOLDER)/test_jacobi.c ./src/jacobi.c | build_folder
	$(CC) $(CFLAGS) $^ -o $(BUILD_FOLDER)/test_$@.out $(LDLIBS)

FFT: ./$(TEST_FOLDER)/test_FFT.c ./src/FFT.c ./src/DFT.c ./src/linear_congruential_random_generator.c | build_folder
	$(CC) $(CFLAGS) $^ -o $(BUILD_FOLDER)/test_$@.out $(LDLIBS)

DFT: ./$(TEST_FOLDER)/test_DFT.c ./src/DFT.c ./src/linear_congruential_random_generator.c | build_folder
	$(CC) $(CFLAGS) $^ -o $(BUILD_FOLDER)/test_$@.out $(LDLIBS)

lanczos: ./$(TEST_FOLDER)/test_lanczos.c ./src/lanczos.c ./src/linear_congruential_random_generator.c | build_folder
	$(CC) $(CFLAGS) $^ -o $(BUILD_FOLDER)/test_$@.out $(LDLIBS)

run_all_tests:
	./$(BUILD_FOLDER)/test_linear_congruential_random_generator.out
	./$(BUILD_FOLDER)/test_gauss_elimination.out
	./$(BUILD_FOLDER)/test_poly_interpolation.out
	./$(BUILD_FOLDER)/test_genetic.out
	./$(BUILD_FOLDER)/test_jacobi.out
	./$(BUILD_FOLDER)/test_DFT.out
	./$(BUILD_FOLDER)/test_FFT.out
	./$(BUILD_FOLDER)/test_lanczos.out
	./$(BUILD_FOLDER)/test_stats.out

build_folder:
	mkdir -p $(BUILD_FOLDER)

clean:
	rm -rf $(BUILD_FOLDER)
