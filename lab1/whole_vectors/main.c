
#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "matrix.h"

int* prepareLengths(int);
int* prepareDispls(int, int*);

int main(int argc, char* argv[]) {
	int size, rank;
	int* lengths;
	int* displs;
	double* multResult;
	double** MATRIX_A; // Matrix A
	double VECTOR_B[SIZE]; // Vector b
	double VECTOR_X[SIZE]; // Vector x
	double collectedResult[SIZE]; // Inter-result
	double vectorBNorm; // Normalized vector b

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
	displs = prepareDispls(size, lengths);

	//printf("%d/%d, dip: %d, len: %d\n", rank + 1, size, displs[rank], lengths[rank]);
	//MPI_Finalize();
	//return 0;

	multResult = (double*) calloc(lengths[rank], sizeof(double));

	// Fill start data
	MATRIX_A = (double**) malloc(sizeof(double*) * lengths[rank]);
	for (size_t i = 0; i < lengths[rank]; i++)
		MATRIX_A[i] = (double*) malloc(sizeof(double) * SIZE);
	fillVector(VECTOR_X);
	fillMatrixPart(MATRIX_A, displs[rank], lengths[rank]);
	fillVector(VECTOR_B);
	vectorBNorm = normalize(VECTOR_B);

	//printMatrix(MATRIX_A, lengths[rank]);
	if (!rank) {
		printf("=====VECTOR B:=====\n");
		printVector(VECTOR_B);
		printf("Counting vector x:\n");
	}

	while (1) {
		// Count A * x_n
		multMatrixPart(MATRIX_A, lengths[rank], VECTOR_X, multResult);
		MPI_Allgatherv(multResult, lengths[rank], MPI_DOUBLE, collectedResult, 
				lengths, displs, MPI_DOUBLE, MPI_COMM_WORLD);

		// Count A * x_n - b
		subVectors(collectedResult, VECTOR_B, collectedResult);

		// Check finish
		if (isFinish(collectedResult, vectorBNorm))
			break;

		// Count theta * (A * x_n - b)
		multScalar(THETA, collectedResult, collectedResult);

		// Count next x_{n + 1}
		subVectors(VECTOR_X, collectedResult, VECTOR_X);
	}

	// Zero rank
	if (!rank)
		printVector(VECTOR_X);

	free(multResult);
	free(lengths);

	MPI_Finalize();
	return 0;
}

int* prepareDispls(int size, int* lengths) {
	int* result = (int*) malloc(sizeof(int) * size);
	result[0] = 0;
	int sum = 0;
	for (size_t i = 1; i < size; i++)
		result[i] = sum += lengths[i - 1];
	return result;
}

int* prepareLengths(int size) {
	int* result = (int*) malloc(sizeof(int) * size);
	int add = SIZE % size;
	for (size_t i = 0; i < size; i++)
		result[i] = SIZE / size + (add-- > 0 ? 1 : 0);
	return result;
}

