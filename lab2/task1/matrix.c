
#include <omp.h>
#include <stdio.h>
#include <math.h>
#include <string.h>

#include "matrix.h"

void fillVector(double vector[SIZE]) {
<<<<<<< HEAD
	size_t i;
	#pragma omp parallel for private(i)
	for (i = 0; i < SIZE; i++)
=======
	#pragma omp parallel for shared(vector)
	for (size_t i = 0; i < SIZE; i++)
>>>>>>> ad1e86cb4208199802980904bfb479f28e2cfa82
		vector[i] = i + 1;
}

void fillMatrix(double matrix[SIZE][SIZE]) {
<<<<<<< HEAD
	size_t i;
	#pragma omp parallel for private(i)
	for (i = 0; i < SIZE; i++) {
		size_t j;
		#pragma omp parallel for private(j)
=======
	#pragma omp parallel for shared(matrix)
	for (size_t i = 0; i < SIZE; i++) {
		size_t j;
		//#pragma omp parallel for private(j)
>>>>>>> ad1e86cb4208199802980904bfb479f28e2cfa82
		for (j = 0; j < SIZE; j++)
			matrix[i][j] = i == j ? 2 : 1;
	}
}

void printVector(double vector[SIZE]) {
<<<<<<< HEAD
	size_t i;
	#pragma omp parallel for private(i)
	for (i = 0; i < SIZE; i++)
=======
	for (size_t i = 0; i < SIZE; i++)
>>>>>>> ad1e86cb4208199802980904bfb479f28e2cfa82
		printf("%.2f ", vector[i]);
	printf("\n");
}

void printMatrix(double matrix[SIZE][SIZE]) {
<<<<<<< HEAD
  size_t i;
	#pragma omp parallel for private(i)
	for (i = 0; i < SIZE; i++)
=======
	for (size_t i = 0; i < SIZE; i++)
>>>>>>> ad1e86cb4208199802980904bfb479f28e2cfa82
		printVector(matrix[i]);
}

double normalize(double vector[SIZE]) {
	double sum = 0;
<<<<<<< HEAD
	size_t i;
	#pragma omp parallel for private(i)
	for (i = 0; i < SIZE; i++)
=======
	#pragma omp parallel for shared(vector)
	for (size_t i = 0; i < SIZE; i++)
>>>>>>> ad1e86cb4208199802980904bfb479f28e2cfa82
		sum += vector[i] * vector[i];
	return sqrt(sum);
}

bool isFinish(double vectorA[SIZE], double vectorBNorm) {
	return normalize(vectorA) / vectorBNorm < EPSILON;
}

void subVectors(double vectorA[SIZE], double vectorB[SIZE], double result[SIZE]) {
<<<<<<< HEAD
	size_t i;
	#pragma omp parallel for private(i)
	for (i = 0; i < SIZE; i++)
=======
	#pragma omp parallel for shared(vectorA, vectorB, result)
	for (size_t i = 0; i < SIZE; i++)
>>>>>>> ad1e86cb4208199802980904bfb479f28e2cfa82
		result[i] = vectorA[i] - vectorB[i];
}

void multScalar(double scalar, double vector[SIZE], double result[SIZE]) {
<<<<<<< HEAD
	size_t i;
	#pragma omp parallel for private(i)
	for (i = 0; i < SIZE; i++)
=======
	#pragma omp parallel for shared(vector, result)
	for (size_t i = 0; i < SIZE; i++)
>>>>>>> ad1e86cb4208199802980904bfb479f28e2cfa82
		result[i] = scalar * vector[i];
}

void multMatrix(double matrix[SIZE][SIZE], double vector[SIZE], double result[SIZE]) {
	memset(result, 0, SIZE * sizeof(double));

<<<<<<< HEAD
	size_t i;
	size_t j;

	#pragma omp parallel for private(i)
	for (i = 0; i < SIZE; i++) {
		#pragma omp parallel for private(j)
		for (j = 0; j < SIZE; j++)
=======
	#pragma omp parallel for shared(matrix, vector, result)
	for (size_t i = 0; i < SIZE; i++) {
		//#pragma omp parallel for private(j)
		for (size_t j = 0; j < SIZE; j++)
>>>>>>> ad1e86cb4208199802980904bfb479f28e2cfa82
			result[i] += matrix[i][j] * vector[j];
	}
}

