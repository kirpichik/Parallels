
#include <stdbool.h>

#define SIZE 10
#define THETA 0.001
#define EPSILON 0.000001

/**
 * Fills the vector.
 */
void fillVectorPart(double*, int, int);

/**
 * Fills the matrix part.
 */
void fillMatrixPart(double**, int, int);

/**
 * Fills the vector with zero.
 */
void fillZeroVectorPart(double*, int, int);

/**
 * Prints the vector.
 */
void printVector(double*, int);

/**
 * Prints the matrix.
 */
void printMatrixPart(double**, int);

/**
 * Normalize vector
 */
double normalize(double*, int);

/**
 * Checks epsilon offset for finish calculating.
 */
bool isFinish(double, double);

/**
 * Substraction of two vectors.
 */
void subVectors(double*, double*, double*, int);

/**
 * Mult of scalar and vector.
 */
void multScalar(double, double*, double*, int);

/**
 * Mult of matrix part and vector
 */
void multMatrixPart(double**, double*, double*, int*, int*, int, int);

