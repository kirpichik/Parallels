
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <omp.h>

#include "matrix.h"

void solve(size_t);

int main(int argc, char* argv[]) {
	if (argc < 2) {
		printf("Need argument: <threads count>\n");
		return -1;
	}
	solve(atoi(argv[1]));
	return 0;
}

void solve(size_t th) {
	double* MATRIX_A = (double*) malloc(SIZE * SIZE * sizeof(double)); // Matrix A
	double* VECTOR_B = (double*) malloc(SIZE * sizeof(double)); // Vector b
	double* VECTOR_X = (double*) malloc(SIZE * sizeof(double)); // Vector x
	double* RESULT = (double*) malloc(SIZE * sizeof(double)); // Inter-result
	double vectorBNorm; // Normalized vector b
	size_t count = 0;

	omp_set_num_threads(th);

	// Fill start data
	fillVector(VECTOR_X);
	fillMatrix(MATRIX_A);
	fillVector(VECTOR_B);
	vectorBNorm = normalize(VECTOR_B);

	while (1) {
		count++;
		// Count A * x_n
		multMatrix(MATRIX_A, VECTOR_X, RESULT);

		// Count A * x_n - b
		subVectors(RESULT, VECTOR_B, RESULT);

		// Check finish
		if (isFinish(RESULT, vectorBNorm))
			break;

		// Count theta * (A * x_n - b)
		multScalar(THETA, RESULT, RESULT);

		// Count next x_{n + 1}
		subVectors(VECTOR_X, RESULT, VECTOR_X);
	}

#ifdef DEBUG_LEVEL
	printf("Count: %lu\n", count);
	printVector(VECTOR_X);
#endif
}

