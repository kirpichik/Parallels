
#include <omp.h>
#include <stdio.h>
#include <math.h>
#include <string.h>

#include "matrix.h"

void fillVector(double* vector) {
	for (size_t i = 0; i < SIZE; i++)
		vector[i] = SIZE + 1;
}

void fillMatrix(double* matrix) {
	for (size_t i = 0; i < SIZE; i++) {
		for (size_t j = 0; j < SIZE; j++)
			matrix[i * SIZE + j] = i == j ? 2 : 1;
	}
}

void printVector(double* vector) {
	for (size_t i = 0; i < SIZE; i++)
		printf("%.2f ", vector[i]);
	printf("\n");
}

void printMatrix(double* matrix) {
	for (size_t i = 0; i < SIZE; i++)
		printVector(&matrix[i * SIZE]);
}

double normalize(double* vector) {
	double sum = 0;
	#pragma omp parallel for reduction(+:sum)
	for (size_t i = 0; i < SIZE; i++)
		sum += vector[i] * vector[i];
	return sqrt(sum);
}

bool isFinish(double* vectorA, double vectorBNorm) {
#ifdef DEBUG_LEVEL
	printf("ITER: %f\n", normalize(vectorA) / vectorBNorm);
#endif
	return normalize(vectorA) / vectorBNorm < EPSILON;
}

void subVectors(double* vectorA, double* vectorB, double* result) {
	#pragma omp parallel for
	for (size_t i = 0; i < SIZE; i++)
		result[i] = vectorA[i] - vectorB[i];
}

void multScalar(double scalar, double* vector, double* result) {
	#pragma omp parallel for
	for (size_t i = 0; i < SIZE; i++)
		result[i] = scalar * vector[i];
}

void multMatrix(double* matrix, double* vector, double* result) {
	memset(result, 0, SIZE * sizeof(double));

	#pragma omp parallel for
	for (size_t i = 0; i < SIZE; i++) {
		#pragma omp parallel for
		for (size_t j = 0; j < SIZE; j++)
			result[i] += matrix[i * SIZE + j] * vector[j];
	}
}

