
#include <stdbool.h>

#define SIZE 10
#define THETA 0.001
#define EPSILON 0.000001

/**
 * Fills the vector.
 */
void fillVector(double*);

/**
 * Fills the matrix part.
 */
void fillMatrix(double*);

/**
 * Prints the vector.
 */
void printVector(double*);

/**
 * Prints the matrix.
 */
void printMatrix(double*);

/**
 * Normalize vector
 */
double normalize(double*);

/**
 * Checks epsilon offset for finish calculating.
 */
bool isFinish(double*, double);

/**
 * Substraction of two vectors.
 */
void subVectors(double*, double*, double*);

/**
 * Mult of scalar and vector.
 */
void multScalar(double, double*, double*);

/**
 * Mult of matrix part and vector
 */
void multMatrix(double*, double*, double*);

