#include <mpi.h>
#include <stdio.h>
#include <stdbool.h>
#include <math.h>

#define SIZE 5
#define THETA 0.001
#define EPSILON 0.000001

double multMatrixPart(double[SIZE], double[SIZE]);
void fillVector(double[SIZE]);
void fillMatrix(double[SIZE][SIZE]);
void fillZero(double[SIZE]);
void printVector(double[SIZE]);
void printMatrix(double[SIZE][SIZE]);
bool isFinish(double[SIZE], double[SIZE]);
double norm(double[SIZE]);
void subVectors(double[SIZE], double[SIZE], double[SIZE]);
void multScalar(double, double[SIZE], double[SIZE]);

int main(int argc, char* argv[]) {
	int size, rank;
	double result;
	double MATRIX_A[SIZE][SIZE]; // Matrix A
	double VECTOR_B[SIZE]; // Vector b
	double VECTOR_X[SIZE]; // Vector x
	double RESULT[SIZE]; // Inter-result

	MPI_Init(&argc, &argv);
	MPI_Comm_size(MPI_COMM_WORLD, &size);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);

	if (size != SIZE) {
		printf("Wrong sizes.\n");
		return 1;
	}

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
		result = multMatrixPart(MATRIX_A[rank], VECTOR_X);
		MPI_Allgather(&result, 1, MPI_DOUBLE, &RESULT, 1, MPI_DOUBLE, MPI_COMM_WORLD);

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

	MPI_Finalize();
	return 0;
}

double multMatrixPart(double line[SIZE], double vector[SIZE]) {
	double result = 0;
	for (size_t i = 0; i < SIZE; i++)
		result += line[i] * vector[i];
	return result;
}

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
			matrix[i][j] = i == j ? 4 : 0;
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
