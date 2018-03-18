
#include <stdbool.h>

#define SIZE 10
#define THETA 0.001
#define EPSILON 0.000001

/**
 * Fills the vector.
 */
void fillVector(double[SIZE]);

/**
 * Fills the matrix part.
 */
void fillMatrixPart(double**, int, int);

/**
 * Fills the vector with zero.
 */
void fillZero(double[SIZE]);

/**
 * Prints the vector.
 */
void printVector(double*);

/**
 * Prints the matrix.
 */
void printMatrix(double**, int);

/**
 * Checks epsilon offset for finish calculating.
 */
bool isFinish(double[SIZE], double[SIZE]);

/**
 * Substraction of two vectors.
 */
void subVectors(double[SIZE], double[SIZE], double[SIZE]);

/**
 * Mult of scalar and vector.
 */
void multScalar(double, double[SIZE], double[SIZE]);

/**
 * Mult of matrix part and vector
 */
void multMatrixPart(double**, int, double[SIZE], double*);

