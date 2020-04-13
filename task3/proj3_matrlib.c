/**
 * File:          proj3_matrlib.c
 * Version:       0.7 alpha
 * Date:          28-11-2011
 * Last update:   4-11-2011
 *
 * Course:        IZP (winter semester, 2011)
 * Project:       #3 - Matrix operations
 *
 * Author:        David Kaspar (aka Dee'Kej), 1BIT
 * Faculty:       Faculty of Information Technologies,
 *                Brno University of Technologies
 * E-mail:        xkaspa34@stud.fit.vutbr.cz
 *
 * Description:   Module containing basic functions for elementary matrix
 *                operations, e.g. memory allocation/freeing, loading, printing
 *                etc.                
 *
 * More info @:
 *      https://www.fit.vutbr.cz/study/courses/IZP/private/projekty/proj3.html
 *
 * Encoding:      en_US.utf8 (United States)
 * Compiler used: gcc 4.5.2 (Ubuntu/Linaro 4.5.2-8ubuntu4)
 */

/******************************************************************************
 ***[ START OF PROJ3_MATRLIB.C ]***********************************************
 ******************************************************************************/

/******************************************************************************
 ~~~[ HEADER FILES ]~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 ******************************************************************************/

#include <errno.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "proj3_matrlib.h"


/******************************************************************************
 ~~~[ GLOBAL MATRIX FUNCTIONS ]~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 ******************************************************************************/

/**
 * Sets the given matrix to given int value.
 */
void matr_set(TS_Matrix *p_matr, int value)
{
  for (int i = 0; i < p_matr->row_num; i++)
    for (int j = 0; j < p_matr->col_num; j++)
      p_matr->data[i][j] = value;

  return;
}


/**
 * Copies matrices values into other matrix with same dimensions.
 */
int matr_copy(TS_Matrix *p_matr, TS_Matrix *p_matr_cp)
{
  /* Specifies kind of error, if any occurred. */
  errno = 0;

  /* Test for same dimensions. */
  if (p_matr->row_num != p_matr_cp->row_num
      || p_matr->col_num != p_matr->col_num) {
    return EXIT_FAILURE;
  }

  /* memcpy() behavior is not defined for any NULL pointer. */
  if (p_matr->data == NULL || p_matr_cp->data == NULL) {
    errno = ENOMEM;
    return EXIT_FAILURE;
  }

  /* Matrix copy. */
  memcpy(p_matr_cp->data, p_matr->data,
         p_matr->row_num * p_matr->col_num * sizeof(int));

  return EXIT_SUCCESS;
}


/**
 * Creates a duplicate of given TS_Matrix.
 */
int matr_dupl(TS_Matrix *p_matr, TS_Matrix *p_matr_dpl)
{
  /* Specifies kind of error, if any occurred. */
  errno = 0;

  /* Nothing to copy? */
  if (p_matr->data == NULL)
    return EXIT_FAILURE;

  /* Dimensions copy. */
  p_matr_dpl->row_num = p_matr->row_num;
  p_matr_dpl->col_num = p_matr->col_num;

  /* Allocation for the duplicit matrix. */
  if (matr_alloc(p_matr_dpl) == EXIT_FAILURE) {
    errno = ENOMEM;
    return EXIT_FAILURE;
  }

  /* Copy of old matrix to new matrix. */
  memcpy(p_matr_dpl->data, p_matr->data,
         p_matr->row_num * p_matr->col_num * sizeof(int));

  return EXIT_SUCCESS;
}


/******************************************************************************
 ~~~[ GLOBAL MATRIX MATH FUNCTIONS ]~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 ******************************************************************************/

/**
 * Matrix transposition. The result is stored in same TS_Matrix structure, which
 * is transposed.
 */
int matr_transp(TS_Matrix *p_matr)
{
  /* Specifies kind of error, if any occurred. */
  errno = 0;

  /* Is there anything for transposition? */
  if (p_matr->data == NULL)
    return EXIT_FAILURE;

  /* Dimensions exchange in auxiliary (transposed) matrix. */
  TS_Matrix matr_t = {
    .row_num = p_matr->col_num,
    .col_num = p_matr->row_num,
  };

  /* Allocation for auxiliary (transposed) matrix. */
  if (matr_alloc(&matr_t) == EXIT_FAILURE) {
    errno = ENOMEM;
    return EXIT_FAILURE;
  }

  /* Transposition. */
  for (int i = 0; i < matr_t.row_num; i++)
    for (int j = 0; j < matr_t.col_num; j++)
      matr_t.data[i][j] = p_matr->data[j][i];

  /* Freeing allocated memory of given matrix. */
  matr_free(p_matr);

  /* Reassigning values of given matrix. */
  p_matr->row_num = matr_t.row_num;
  p_matr->col_num = matr_t.col_num;
  p_matr->p_alloc = matr_t.p_alloc;
  p_matr->data = matr_t.data;
  
  return EXIT_SUCCESS;
}


/**
 * Addition of 2 matrices, the result is stored in 3rd matrix. Matrices have to
 * have same dimensions.
 */
int matr_add(TS_Matrix *p_matr1, TS_Matrix *p_matr2, TS_Matrix *p_matr3)
{
  /* Test for same dimensions of first 2 matrices. */
  if (p_matr1->row_num != p_matr2->row_num
      || p_matr1->col_num != p_matr2->col_num) {
    return EXIT_FAILURE;
  }
  /* Test of dimension of 3rd matrix. */
  else if (p_matr1->row_num != p_matr3->row_num
           || p_matr1->col_num != p_matr3->col_num) {
    return EXIT_FAILURE;
  }

  /* Addiction. */
  for (int i = 0; i < p_matr1->row_num; i++)
    for (int j = 0; j < p_matr1->col_num; j++)
      p_matr3->data[i][j] = p_matr1->data[i][j] + p_matr2->data[i][j];

  return EXIT_SUCCESS;
}


/**
 * Subtraction of 2 matrices, the result is stored in 3rd matrix. Matrices have
 * to have same dimensions.
 */
int matr_sub(TS_Matrix *p_matr1, TS_Matrix *p_matr2, TS_Matrix *p_matr3)
{
  /* Test for same dimensions of first 2 matrices. */
  if (p_matr1->row_num != p_matr2->row_num
      || p_matr1->col_num != p_matr2->col_num) {
    return EXIT_FAILURE;
  }
  /* Test of dimension of 3rd matrix. */
  else if (p_matr1->row_num != p_matr3->row_num
           || p_matr1->col_num != p_matr3->col_num) {
    return EXIT_FAILURE;
  }

  /* Subtraction. */
  for (int i = 0; i < p_matr1->row_num; i++)
    for (int j = 0; j < p_matr1->col_num; j++)
      p_matr3->data[i][j] = p_matr1->data[i][j] - p_matr2->data[i][j];

  return EXIT_SUCCESS;
}


/**
 * Multiplication of 2 matrices, the result is stored in 3rd matrix. Laws of
 * matrix multiplication are tested.
 */
int matr_mult(TS_Matrix *p_matr1, TS_Matrix *p_matr2, TS_Matrix *p_matr3)
{
  /* Correct dimensions of first 2 matrices?. */
  if (p_matr1->col_num != p_matr2->row_num)
    return EXIT_FAILURE;
  /* Correct dimensions of result matrix? */
  else if (p_matr3->row_num != p_matr1->row_num
           || p_matr3->col_num != p_matr2->col_num) {
    return EXIT_FAILURE;
  }

  int sum;          /* Sum of actual step. */

  /* Multiplication. */
  for (int i = 0; i < p_matr1->row_num; i++) {
    for (int j = 0; j < p_matr2->col_num; j++) {

      sum = 0;

      for (int k = 0; k < p_matr2->row_num; k++)
        sum += p_matr1->data[i][k] * p_matr2->data[k][j];

      p_matr3->data[i][j] = sum;
    }
  }

  return EXIT_SUCCESS;
}


/******************************************************************************
 ~~~[ GLOBAL I/O FUNCTIONS ]~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 ******************************************************************************/

/**
 * Reads the dimensions of the matrix.
 */
int matr_read_dim(TS_Matrix *p_matr)
{
  /* Initialization. */
  p_matr->row_num = 0;
  p_matr->col_num = 0;

  /* Dimensions reading. */
  if (fscanf(stdin, "%i %i", &p_matr->row_num, &p_matr->col_num) != 2)
    return EXIT_FAILURE;
  else
    return EXIT_SUCCESS;
}


/**
 * Function reads (fills) the given matrix with values read from stdin.
 */
int matr_read_in(TS_Matrix *p_matr)
{
  int scan_res;     /* Result of fscanf. */

  errno = 0;        /* Reseting errno for testing below. */
  scan_res = 1;     /* Needed initialization. */

  /* Loads values until all values are loaded or a read error occurs. */
  for (int i = 0; i < p_matr->row_num && scan_res == 1; i++)
    for (int j = 0; j < p_matr->col_num && scan_res == 1; j++)
      scan_res = fscanf(stdin, "%i", &p_matr->data[i][j]);
  
  /* Test for read error or bigger (than expected) matrix. */
  if (scan_res != 1 || errno != 0 || fscanf(stdin, "%*i") != EOF)
    return EXIT_FAILURE;
  else
    return EXIT_SUCCESS;
}


/**
 * Prints the values of matrix. Also prints dimension of matrix first.
 */
void matr_print(TS_Matrix *p_matr)
{
  fprintf(stdout, "%i %i\n", p_matr->row_num, p_matr->col_num);

  for (int i = 0; i < p_matr->row_num; i++) {
    for (int j = 0; j < p_matr->col_num; j++) {
      fprintf(stdout, "%i ", p_matr->data[i][j]);
    }

    fprintf(stdout, "\n");
  }
  
  return;
}


/******************************************************************************
 ~~~[ GLOBAL MEMORY FUNCTIONS ]~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 ******************************************************************************/

/**
 * Allocates memory for pointers inside the given TS_Matrix structure.
 */
int matr_alloc(TS_Matrix *p_matr)
{
  /* 
   * Pointer initialization for save use below, when testing if the memory
   * allocation were successful.
   */
  p_matr->p_alloc = NULL;
  p_matr->data = NULL;

  /* One memory block for the matrix. */
  p_matr->p_alloc = (int *) malloc(p_matr->row_num * p_matr->col_num
                                   * sizeof(int));

  /* Array of pointers for easier indexing later. */
  p_matr->data = (int **) malloc(p_matr->row_num * sizeof(int *));
  
  /* Successful allocation? */
  if (p_matr->p_alloc == NULL || p_matr->data == NULL) {

    free(p_matr->p_alloc);
    free(p_matr->data);

    p_matr->p_alloc= NULL;
    p_matr->data = NULL;
    errno = ENOMEM;

    return EXIT_FAILURE;
  }

  /* Assigning of pointers from the array above to correct places. */
  for (int i = 0; i < p_matr->row_num; i++)
    p_matr->data[i] = p_matr->p_alloc + (i * p_matr->col_num);

  return EXIT_SUCCESS;
}


/**
 * Freeing of memory allocated for pointers inside given TS_Matrix structure.
 */
void matr_free(TS_Matrix *p_matr)
{
  free((void *) p_matr->data);
  free((void *) p_matr->p_alloc);

  p_matr->data = NULL;
  p_matr->p_alloc = NULL;

  return;
}


/******************************************************************************
 ***[ END OF PROJ3_MATRLIB.C ]*************************************************
 ******************************************************************************/
 
