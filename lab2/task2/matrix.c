
#include <omp.h>
#include <stdio.h>
#include <math.h>
#include <string.h>

#include "matrix.h"

void fillVector(double vector[SIZE]) {
	#pragma omp for
	for (size_t i = 0; i < SIZE; i++)
		vector[i] = i + 1;
}

void fillMatrix(double matrix[SIZE][SIZE]) {
	#pragma omp for
	for (size_t i = 0; i < SIZE; i++) {
		for (size_t j = 0; j < SIZE; j++)
			matrix[i][j] = i == j ? 2 : 1;
	}
}

void printVector(double vector[SIZE]) {
	for (size_t i = 0; i < SIZE; i++)
		printf("%.2f ", vector[i]);
	printf("\n");
}

void printMatrix(double matrix[SIZE][SIZE]) {
	for (size_t i = 0; i < SIZE; i++)
		printVector(matrix[i]);
}

double normalize(double vector[SIZE]) {
	double sum = 0;
	#pragma omp for
	for (size_t i = 0; i < SIZE; i++)
		sum += vector[i] * vector[i];
	return sqrt(sum);
}

bool isFinish(double vectorA[SIZE], double vectorBNorm) {
	return normalize(vectorA) / vectorBNorm < EPSILON;
}

void subVectors(double vectorA[SIZE], double vectorB[SIZE], double result[SIZE]) {
	#pragma omp for
	for (size_t i = 0; i < SIZE; i++)
		result[i] = vectorA[i] - vectorB[i];
}

void multScalar(double scalar, double vector[SIZE], double result[SIZE]) {
	#pragma omp for
	for (size_t i = 0; i < SIZE; i++)
		result[i] = scalar * vector[i];
}

void multMatrix(double matrix[SIZE][SIZE], double vector[SIZE], double result[SIZE]) {
	memset(result, 0, SIZE * sizeof(double));

	#pragma omp for
	for (size_t i = 0; i < SIZE; i++) {
		for (size_t j = 0; j < SIZE; j++)
			result[i] += matrix[i][j] * vector[j];
	}
}

