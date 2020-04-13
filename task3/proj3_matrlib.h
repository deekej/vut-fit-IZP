/**
 * File:          proj3_matrlib.h
 * Version:       1.0 RC
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
 * Description:   This header file contains global data types declarations and
 *                functional prototypes of proj3_matrlib.c file.               
 *
 * More info @:
 *      https://www.fit.vutbr.cz/study/courses/IZP/private/projekty/proj3.html
 *
 * Encoding:      en_US.utf8 (United States)
 * Compiler used: gcc 4.5.2 (Ubuntu/Linaro 4.5.2-8ubuntu4)
 */

/******************************************************************************
 ***[ START OF PROJ3_MATRLIB.H ]***********************************************
 ******************************************************************************/

/* Safety mechanism against multi-including of this header file. */
#ifndef PROJ3_MATRLIB_H
#define PROJ3_MATRLIB_H

/******************************************************************************
 ~~~[ GLOBAL DATA TYPES DECLARATIONS ]~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 ******************************************************************************/

/**
 * Structure containing information for easy matrix control, memory allocation
 * and freeing.
 */
typedef struct matrix {
  int row_num;              /* Number of rows of the matrix. */
  int col_num;              /* Number of columns of the matrix. */
  
  int *p_alloc;             /* Pointer to matrix allocated memory. */

  /*
   * Pointer to pointer used for helpful array of pointers to int. This array
   * is later used for easier matrix indexing.
   */
  int **data;
} TS_Matrix;


/**
 * Possible directions in hexagonal matrix. Used by wtrflw() and carom() funct.
 */
typedef enum direction {
  N_D = -1,   /* Not defined yet. */
  W = 0,      /* West. */
  NW,         /* North-west. */
  NE,         /* North-east. */
  E,          /* East. */
  SE,         /* South-east. */
  SW,         /* South-west. */
} TE_Direction;


/******************************************************************************
 ~~~[ FUNCTIONAL PROTOTYPES OF GLOBAL FUNCTIONS ]~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 ******************************************************************************/

void matr_set(TS_Matrix *p_matr, int value);
int matr_copy(TS_Matrix *p_matr, TS_Matrix *p_matr_cp);
int matr_dupl(TS_Matrix *p_matr, TS_Matrix *p_matr_dpl);

int matr_transp(TS_Matrix *p_matr);
int matr_add(TS_Matrix *p_matr1, TS_Matrix *p_matr2, TS_Matrix *p_matr3);
int matr_sub(TS_Matrix *p_matr1, TS_Matrix *p_matr2, TS_Matrix *p_matr3);
int matr_mult(TS_Matrix *p_matr1, TS_Matrix *p_matr2, TS_Matrix *p_matr3);

int matr_read_dim(TS_Matrix *p_matr);
int matr_read_in(TS_Matrix *p_matr);
void matr_print(TS_Matrix *p_matr);

int matr_alloc(TS_Matrix *p_matr);
void matr_free(TS_Matrix *p_matr);

#endif

/******************************************************************************
 ***[ END OF PROJ3_MATRLIB.H ]*************************************************
 ******************************************************************************/
 
