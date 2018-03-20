
#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "matrix.h"

int* prepareLengths(int);
int* prepareDispls(int, int*);

int main(int argc, char* argv[]) {
	int size, rank, part_size, part_start;
	int* lengths;
	int* displs;
	double* partResult; // Partical step result
	double** MATRIX_A; // Matrix A
	double* VECTOR_B; // Vector b
	double* VECTOR_X; // Vector x
	double collectedResult[SIZE]; // Inter-result
	double vectorBNorm; // Normalized vector b
	int extendedPart;

	MPI_Init(&argc, &argv);
	MPI_Comm_size(MPI_COMM_WORLD, &size);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);

	extendedPart = (int) ceil((double) SIZE / size);

	// Check process count overflow
	if (size > SIZE) {
		if (!rank)
			printf("Processes > Matrix size.\n");
		MPI_Finalize();
		return 0;
	}

	lengths = prepareLengths(size);
	displs = prepareDispls(size, lengths);
	part_size = lengths[rank];
	part_start = displs[rank];

	partResult = (double*) calloc(extendedPart, sizeof(double));

	// Init start data
	MATRIX_A = (double**) malloc(sizeof(double*) * part_size);
	for (size_t i = 0; i < part_size; i++)
		MATRIX_A[i] = (double*) malloc(sizeof(double) * SIZE);
	VECTOR_B = (double*) malloc(sizeof(double) * extendedPart);
	VECTOR_X = (double*) calloc(extendedPart, sizeof(double));

	// Fills start data
	fillMatrixPart(MATRIX_A, part_start, part_size);
	fillVectorPart(VECTOR_B, part_start,  part_size);

	// Count ||b||
	MPI_Allgatherv(VECTOR_B, part_size, MPI_DOUBLE, collectedResult, 
			lengths, displs, MPI_DOUBLE, MPI_COMM_WORLD);
	vectorBNorm = normalize(collectedResult, SIZE);

	while (1) {
		// Count A * x_n
		//printf("pr: %d multing matrix part...\n", rank);
		multMatrixPart(MATRIX_A, VECTOR_X, partResult, lengths, displs, size, rank);
		MPI_Allgatherv(partResult, part_size, MPI_DOUBLE, collectedResult, 
				lengths, displs, MPI_DOUBLE, MPI_COMM_WORLD);
		//printf("pr: %d finish mult\n", rank);

		// Count A * x_n - b
		subVectors(partResult, VECTOR_B, partResult, extendedPart);

		// Check finish
		MPI_Allgatherv(partResult, part_size, MPI_DOUBLE, collectedResult, 
				lengths, displs, MPI_DOUBLE, MPI_COMM_WORLD);
		if (isFinish(collectedResult, vectorBNorm))
			break;

		// Count theta * (A * x_n - b)
		multScalar(THETA, partResult, partResult, extendedPart);

		// Count next x_{n + 1}
		subVectors(VECTOR_X, partResult, VECTOR_X, extendedPart);
	}

	// Prints results
	
	printf("pr(%d): ", rank);
	printVector(VECTOR_X, part_size);

	free(partResult);
	for (size_t i = 0; i < part_size; i++)
		free(MATRIX_A[i]);
	free(MATRIX_A);
	free(VECTOR_B);
	free(VECTOR_X);
	free(lengths);
	free(displs);

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

