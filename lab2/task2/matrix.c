
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
	for (size_t i = 0; i < SIZE; i++)
		for (size_t j = 0; j < SIZE; j++)
			matrix[i * SIZE + j] = i == j ? 2 : 1;
}

void printVector(double* vector) {
	for (size_t i = 0; i < SIZE; i++)
		printf("%.2f ", vector[i]);
	printf("\n");
}

