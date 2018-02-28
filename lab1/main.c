
#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>

#include "matrix.h"

int* prepareLengths(int);
int* prepareDispls(int);
void multMatrixPart(double[SIZE][SIZE], int, int, double[SIZE], double*);

int main(int argc, char* argv[]) {
	int size, rank;
	int* lengths;
	int* displs;
	double* result;
	double MATRIX_A[SIZE][SIZE]; // Matrix A
	double VECTOR_B[SIZE]; // Vector b
	double VECTOR_X[SIZE]; // Vector x
	double RESULT[SIZE]; // Inter-result

	MPI_Init(&argc, &argv);
	MPI_Comm_size(MPI_COMM_WORLD, &size);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);

	// Check process count overflow
	if (size > SIZE) {
		if (!rank)
			printf("Processes > Matrix size.\n");
		MPI_Finalize();
		return 0;
	}

	lengths = prepareLengths(size);
	displs = prepareDispls(size);
	result = (double*) calloc(lengths[rank], sizeof(double));

	// Fill start data
	fillVector(VECTOR_X);
	fillMatrix(MATRIX_A);
	fillVector(VECTOR_B);

	if (!rank) {
		printf("=====MATRIX A:=====\n");
		printMatrix(MATRIX_A);
		printf("=====VECTOR B:=====\n");
		printVector(VECTOR_B);
		printf("Counting vector x:\n");
	}

	while (1) {
		// Count A * x_n
		multMatrixPart(MATRIX_A, displs[rank], lengths[rank], VECTOR_X, result);
		MPI_Allgatherv(result, lengths[rank], MPI_DOUBLE, RESULT, lengths, displs, MPI_DOUBLE, MPI_COMM_WORLD);

		// Count A * x_n - b
		subVectors(RESULT, VECTOR_B, RESULT);

		// Check finish
		if (isFinish(RESULT, VECTOR_B))
			break;

		// Count theta * (A * x_n - b)
		multScalar(THETA, RESULT, RESULT);

		// Count next x_{n + 1}
		subVectors(VECTOR_X, RESULT, VECTOR_X);
	}

	// Zero rank
	if (!rank)
		printVector(VECTOR_X);

	free(result);
	free(lengths);

	MPI_Finalize();
	return 0;
}

int* prepareDispls(int size) {
	int* result = (int*) malloc(sizeof(int) * size);
	for (size_t i = 0; i < size; i++)
		result[i] = SIZE / size * i;
	return result;
}

int* prepareLengths(int size) {
	int* result = (int*) malloc(sizeof(int) * size);
	for (size_t i = 0; i < size; i++)
		result[i] = SIZE / size + (i == (size - 1) ? SIZE % size : 0);
	return result;
}

void multMatrixPart(double matrix[SIZE][SIZE], int begin, int len, 
		double vector[SIZE], double* result) {
	for (size_t i = 0; i < len; i++)
		result[i] = 0;

	for (size_t i = 0; i < len; i++)
		for (size_t j = 0; j < SIZE; j++)
			result[i] += matrix[i + begin][j] * vector[j];
}

