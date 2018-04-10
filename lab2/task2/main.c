
#include <omp.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <stdbool.h>

#include "matrix.h"

int main(int argc, char* argv[]) {
	double MATRIX_A[SIZE][SIZE]; // Matrix A
	double VECTOR_B[SIZE]; // Vector b
	double VECTOR_X[SIZE]; // Vector x
	double RESULT[SIZE]; // Inter-result
	double vectorBNorm = 0; // Normalized vector b
	double sumNorm = 0;
	bool flag = true;

	if (argc < 2) {
		printf("Need argument: <threads count>\n");
		return 0;
	}

	omp_set_num_threads(atoi(argv[1]));

	// Fill start data
	fillVector(VECTOR_X);
	fillMatrix(MATRIX_A);
	fillVector(VECTOR_B);

	vectorBNorm = 0;
	#pragma omp parallel
	{
		#pragma omp for reduction(+:vectorBNorm)
		for (size_t i = 0; i < SIZE; i++)
			vectorBNorm += VECTOR_B[i] * VECTOR_B[i];

		#pragma omp single
		vectorBNorm = sqrt(vectorBNorm);
		
		while (flag) {
			#pragma omp for reduction(+:sumNorm)
			for (size_t i = 0; i < SIZE; i++) {
				double valueX = 0;
				// A * x
				for (size_t j = 0; j < SIZE; j++)
					valueX += MATRIX_A[i][j] * VECTOR_X[j];

				// A * x - b
				valueX -= VECTOR_B[i];
				RESULT[i] = VECTOR_X[i] - valueX * THETA;
				sumNorm += pow(valueX, 2);
			}

			#pragma omp single
			{
				memcpy(VECTOR_X, RESULT, sizeof(double) * SIZE);
				sumNorm = sqrt(sumNorm);
				flag = sumNorm / vectorBNorm >= EPSILON;
				sumNorm = 0;
			}
		}
	}

	printVector(VECTOR_X);

	return 0;
}

