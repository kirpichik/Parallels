
#include <stdio.h>
#include <math.h>

#include "matrix.h"

void fillVector(double vector[SIZE]) {
	for (size_t i = 0; i < SIZE; i++)
		vector[i] = i;
}

void fillZero(double vector[SIZE]) {
	for (size_t i = 0; i < SIZE; i++)
		vector[i] = 0;
}

void fillMatrix(double matrix[SIZE][SIZE]) {
	for (size_t i = 0; i < SIZE; i++)
		for (size_t j = 0; j < SIZE; j++)
			matrix[i][j] = i == j ? 2 : 0;
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

bool isFinish(double vectorA[SIZE], double vectorB[SIZE]) {
	return norm(vectorA) / norm(vectorB) < EPSILON;
}

double norm(double vector[SIZE]) {
	double sum = 0;
	for (size_t i = 0; i < SIZE; i++)
		sum += vector[i] * vector[i];
	return sqrt(sum);
}

void subVectors(double vectorA[SIZE], double vectorB[SIZE], double result[SIZE]) {
	for (size_t i = 0; i < SIZE; i++)
		result[i] = vectorA[i] - vectorB[i];
}

void multScalar(double scalar, double vector[SIZE], double result[SIZE]) {
	for (size_t i = 0; i < SIZE; i++)
		result[i] = scalar * vector[i];
}
