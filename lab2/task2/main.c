
#include <omp.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "matrix.h"

void solve();

int main(int argc, char* argv[]) {
	#pragma omp parallel
	solve();
	return 0;
}

void solve() {
	double MATRIX_A[SIZE][SIZE]; // Matrix A
	double VECTOR_B[SIZE]; // Vector b
	double VECTOR_X[SIZE]; // Vector x
	double RESULT[SIZE]; // Inter-result
	double vectorBNorm; // Normalized vector b
	double sumNorm;

	omp_set_dynamic(0);
	omp_set_num_threads(4);

	// Fill start data
	fillVector(VECTOR_X);
	fillMatrix(MATRIX_A);
	fillVector(VECTOR_B);

	vectorBNorm = 0;
	#pragma omp parallel shared(vectorBNorm, sumNorm)
	{
		#pragma omp for
		for (size_t i = 0; i < SIZE; i++) {
			#pragma omp atomic
			vectorBNorm += VECTOR_B[i] * VECTOR_B[i];
		}
		#pragma omp single
		vectorBNorm = sqrt(vectorBNorm);
		#pragma omp barrier
		//printf("%f\n", vectorBNorm);
		
		while (1) {
			// Count A * x_n
			multMatrix(MATRIX_A, VECTOR_X, RESULT);

			// Count A * x_n - b
			subVectors(RESULT, VECTOR_B, RESULT);

			// Check finish
			#pragma omp single
			sumNorm = 0;
			#pragma omp barrier
			#pragma omp for
			for (size_t i = 0; i < SIZE; i++) {
				#pragma omp atomic
				sumNorm += RESULT[i] * RESULT[i];
			}
			#pragma omp single
			sumNorm = sqrt(sumNorm);
			#pragma omp barrier
			printf("%f\n", sumNorm);
			if (sumNorm / vectorBNorm < EPSILON)
				break;

			// Count theta * (A * x_n - b)
			multScalar(THETA, RESULT, RESULT);

			// Count next x_{n + 1}
			subVectors(VECTOR_X, RESULT, VECTOR_X);
		}
	}

	printVector(VECTOR_X);
}

