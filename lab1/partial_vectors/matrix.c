
#include <stdio.h>
#include <math.h>
#include <string.h>
#include <mpi.h>

#include "matrix.h"

void fillVectorPart(double* vector, int begin, int count) {
	for (size_t i = 0; i < count; i++)
		vector[i] = i + 1 + begin;
}

void fillZeroVectorPart(double* vector, int begin, int count) {
	for (size_t i = 0; i < count; i++)
		vector[i] = 0;
}

void fillMatrixPart(double** part, int begin, int count) {
	for (size_t i = 0; i < count; i++)
		for (size_t j = 0; j < SIZE; j++)
			part[i][j] = (i + begin) == j ? (2) : 1;
}

void printVector(double* vector, int count) {
	for (size_t i = 0; i < count; i++)
		printf("%.2f ", vector[i]);
	printf("\n");
}

void printMatrixPart(double** part, int count) {
	for (size_t i = 0; i < count; i++)
		printVector(part[i], SIZE);
}

double normalize(double* vector, int count) {
	double sum = 0;
	for (size_t i = 0; i < count; i++)
		sum += vector[i] * vector[i];
	return sqrt(sum);
}

bool isFinish(double vectorA[SIZE], double vectorBNorm) {
	return normalize(vectorA, SIZE) / vectorBNorm < EPSILON;
}

void subVectors(double* vectorA, double* vectorB, double* result, int size) {
	for (size_t i = 0; i < size; i++)
		result[i] = vectorA[i] - vectorB[i];
}

void multScalar(double scalar, double* vector, double* result, int size) {
	for (size_t i = 0; i < size; i++)
		result[i] = scalar * vector[i];
}

void multMatrixPart(double** matrix, double* vector, double* result, int* lengths, int* displs, int pr_size, int rank) {
	int rows = lengths[rank];
	int begin = displs[rank];
	int length = rows;
	int send_len = (int) ceil((double) SIZE / pr_size);
	MPI_Status mpi_status;

	for (int shift = 0; shift < pr_size; shift++) {
		for (int i = 0; i < rows; i++)
			for (int j = begin; j < begin + length; j++)
				result[i] += matrix[i][j] * vector[j - begin];

		// Sent circle our vector part
		int pos = (rank + pr_size - shift - 1) % pr_size;
		begin = displs[pos];
		length = lengths[pos] % SIZE;
		int send_id = (rank + 1) % pr_size;
		int recv_id = (rank + pr_size - 1) % pr_size;
		if (send_id != recv_id) {
			MPI_Send(vector, send_len, MPI_DOUBLE, send_id, 777, MPI_COMM_WORLD);
			MPI_Recv(vector, send_len, MPI_DOUBLE, recv_id, 777, MPI_COMM_WORLD, &mpi_status);
		}
	}
}

