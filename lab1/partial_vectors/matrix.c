
#include <stdio.h>
#include <math.h>
#include <string.h>

#include "matrix.h"

void fillVector(double vector[SIZE]) {
	for (size_t i = 0; i < SIZE; i++)
		vector[i] = i;
}

void fillZero(double vector[SIZE]) {
	for (size_t i = 0; i < SIZE; i++)
		vector[i] = 0;
}

void fillMatrixPart(double** part, int begin, int count) {
	for (size_t i = 0; i < count; i++)
		for (size_t j = 0; j < SIZE; j++)
			part[i][j] = (i + begin) == j ? (2 * SIZE) : 1;
}

void printVector(double* vector) {
	for (size_t i = 0; i < SIZE; i++)
		printf("%.2f ", vector[i]);
	printf("\n");
}

void printMatrixPart(double** part, int count) {
	for (size_t i = 0; i < count; i++)
		printVector(part[i]);
}

static double normalize(double vector[SIZE]) {
	double sum = 0;
	for (size_t i = 0; i < SIZE; i++)
		sum += vector[i] * vector[i];
	return sqrt(sum);
}

bool isFinish(double vectorA[SIZE], double vectorB[SIZE]) {
	return normalize(vectorA) / normalize(vectorB) < EPSILON;
}

void subVectors(double vectorA[SIZE], double vectorB[SIZE], double result[SIZE]) {
	for (size_t i = 0; i < SIZE; i++)
		result[i] = vectorA[i] - vectorB[i];
}

void multScalar(double scalar, double vector[SIZE], double result[SIZE]) {
	for (size_t i = 0; i < SIZE; i++)
		result[i] = scalar * vector[i];
}

void multMatrixPart(double** part, int count, double vector[SIZE], double* result) {
	memset(result, 0, count * sizeof(double));

	for (size_t i = 0; i < count; i++)
		for (size_t j = 0; j < SIZE; j++)
			result[i] += part[i][j] * vector[j];
}

