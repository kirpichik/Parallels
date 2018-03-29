
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "matrix.h"

void solve();

int main(int argc, char* argv[]) {
	solve();
	return 0;
}

void solve() {
	double MATRIX_A[SIZE][SIZE]; // Matrix A
	double VECTOR_B[SIZE]; // Vector b
	double VECTOR_X[SIZE]; // Vector x
	double RESULT[SIZE]; // Inter-result
	double vectorBNorm; // Normalized vector b

	// Fill start data
	fillVector(VECTOR_X);
	fillMatrix(MATRIX_A);
	fillVector(VECTOR_B);
	vectorBNorm = normalize(VECTOR_B);

	while (1) {
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

	printVector(VECTOR_X);
}

