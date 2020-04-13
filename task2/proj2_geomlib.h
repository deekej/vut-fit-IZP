/**
 * File:          proj2_geomlib.h
 * Version:       0.2
 * Date:          1-11-2011
 * Last update:   19-11-2011
 *
 * Course:        IZP (winter semester, 2011)
 * Project:       #2 - Iterative calculations
 *
 * Author:        David Kaspar (aka Dee'Kej), 1BIT
 * Faculty:       Faculty of Information Technologies,
 *                Brno University of Technologies
 * E-mail:        xkaspa34@stud.fit.vutbr.cz
 *
 * Description:   This is header file for proj2_geomlib.h file and contains
 *                functional prototypes of used functions and global data types
 *                declarations.
 * More info @:
 *      https://www.fit.vutbr.cz/study/courses/IZP/private/projekty/proj2.html
 *
 * Encoding:      en_US.utf8 (United States)
 * Compiler used: gcc 4.5.2 (Ubuntu/Linaro 4.5.2-8ubuntu4) 
 */

/******************************************************************************
 ***[ START OF PROJ2_GEOMLIB.H ]***********************************************
 ******************************************************************************/

/*
 * Safety mechanism against multiple including of this header file.
 */
#ifndef PROJ2_GEOMLIB_H
#define PROJ2_GEOMLIB_H

/******************************************************************************
 ~~~[ GLOBAL DATA TYPES DECLARATIONS ]~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 ******************************************************************************/

/**
 * Contains values used for computing length of broken line in lbl() function.
 */
typedef struct lobl {
  double x_bef;             /* Coordinate of x predecessor. */
  double y_bef;             /* Coordinate of y predecessor. */

  double x_act;             /* Coordinate of new x. */
  double y_act;             /* Coordinate of new y. */

  double x_dist;            /* Distance (absolute value) of x coordinates. */
  double y_dist;            /* Distance (absolute value) of y coordinates. */

  double sum;               /* Sum of length of broken line. */
} TS_Lobl;


/**
 * Contains values used for computing length of broken line with error in lble()
 * function.
 */
typedef struct loblwe {
  double err_val;           /* Absolute error value. */

  double x_act;             /* Actual coordinate of x. */
  double y_act;             /* Actual coordinate of y. */

  double x_act_min;         /* The smallest coordinate of actual x. */
  double y_act_min;         /* The smallest coordinate of actual x. */
  double x_act_max;         /* The biggest coordinate of actual x. */
  double y_act_max;         /* The biggest coordinate of actual x. */

  double x_bef_min;         /* The smallest coordinate of x predecessor. */
  double y_bef_min;         /* The smallest coordinate of y predecessor. */
  double x_bef_max;         /* The biggest coordinate of x predecessor. */
  double y_bef_max;         /* The biggest coordinate of y predecessor. */

  double x_dist;            /* Distance (absolute value) of x coordinates. */
  double y_dist;            /* Distance (absolute value) of y coordinates. */

  double sum_min;           /* The smallest sum of broken line with error. */
  double sum_max;           /* The biggest sum of broken line with error. */
} TS_Loblwe;


/******************************************************************************
 ~~~[ FUNCTIONAL PROTOTYPES OF PRIMARY FUNCTIONS ]~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 ******************************************************************************/

void lbl(TS_Lobl *p_lobl);
void lble(TS_Loblwe *p_loblwe);

#endif

/******************************************************************************
 ***[ END OF PROJ2_GEOMLIB.H ]***********************************************************
 ******************************************************************************/
