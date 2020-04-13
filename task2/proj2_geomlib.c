/**
 * File:          proj2_geomlib.c
 * Version:       1.2 final
 * Date:          1-11-2011
 * Last update:   20-11-2011
 *
 * Course:        IZP (winter semester, 2011)
 * Project:       #2 - Iterative calculations
 *
 * Author:        David Kaspar (aka Dee'Kej), 1BIT
 * Faculty:       Faculty of Information Technologies,
 *                Brno University of Technologies
 * E-mail:        xkaspa34@stud.fit.vutbr.cz
 *
 * Description:   This module contains definitions of functions declared in
 *                proj2_geomlib.h, which computes length of broken line and
 *                length of broken line with absolute error. It also contains
 *                subsidiary functions used for the computation.
 *
 * More info @:
 *      https://www.fit.vutbr.cz/study/courses/IZP/private/projekty/proj2.html
 *
 * Encoding:      en_US.utf8 (United States)
 * Compiler used: gcc 4.5.2 (Ubuntu/Linaro 4.5.2-8ubuntu4)
 */

/******************************************************************************
 ***[ START OF PROJ2_GEOMLIB.C ]***********************************************
 ******************************************************************************/

/******************************************************************************
 ~~~[ HEADER FILES ]~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 ******************************************************************************/

#include <errno.h>
#include <stdbool.h>
#include <string.h>

#include "proj2_mathlib.h"
#include "proj2_geomlib.h"


/******************************************************************************
 ~~~[ LOCAL DATA TYPES DECLARATIONS ]~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 ******************************************************************************/

/*
 * Enumerate representing position of 2 squares which are representing squares.
 */
typedef enum position {
  NO_OVERLAP,             /* No overlap. */
  X_OVERLAP,              /* Overlap on x axis. */
  Y_OVERLAP,              /* Overlap on y axis. */
  XY_OVERLAP,             /* Overlap on both axis. */
  IDENTITY,               /* Identity, special case of both axis overlap. */
} TE_Position;

/******************************************************************************
 ~~~[ FUNCTIONAL PROTOTYPES OF LOCAL SUBSIDIARY FUNCTIONS ]~~~~~~~~~~~~~~~~~~~~
 ******************************************************************************/

static inline void no_overlap_func(TS_Loblwe *p_loblwe);
static inline void x_overlap_func(TS_Loblwe *p_loblwe);
static inline void y_overlap_func(TS_Loblwe *p_loblwe);
static inline void xy_overlap_func(TS_Loblwe *p_loblwe);


/******************************************************************************
 ~~~[ PRIMARY FUNCTIONS ]~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 ******************************************************************************/

/**
 * Function computes new size of a broken line by computing size of it and
 * adding it to sum of previous sizes. Size is computed using a method for
 * computing the size of a vector (which is given in two pair of x,y
 * coordinates).
 */
void lbl(TS_Lobl *p_lobl)
{
  /* Distance between each axis coordinates. */
  p_lobl->x_dist = (p_lobl->x_act - p_lobl->x_bef);
  p_lobl->y_dist = (p_lobl->y_act - p_lobl->y_bef);

  /* Power of distances. */
  p_lobl->x_dist *= p_lobl->x_dist;
  p_lobl->y_dist *= p_lobl->y_dist;

  /* Sum + square root of distances addition. */
  p_lobl->sum += sqrroot(p_lobl->x_dist + p_lobl->y_dist);

  /* Backup of current coordinates, which can be used in next computation. */
  p_lobl->x_bef = p_lobl->x_act;
  p_lobl->y_bef = p_lobl->y_act;

  int sum_cls = fpclassify(p_lobl->sum);    /* SUM classification. */

  switch (sum_cls) {
    /* Result is NaN or any input value was NaN. */
    case FP_NAN :
      errno = EDOM;
      p_lobl->sum = abs_d(p_lobl->sum);
      break;

    /* Result has overflowed. */
    case FP_INFINITE :
      errno = ERANGE;
      break;
    
    /* Result should be correct. */
    default :
      break;
  }

  return;
}


/**
 * Function computes new size of a broken line with absolute error value by
 * computing new minimal and maximal length of between 2 points and adding it
 * to minimal and maximal sum respectively. Length is computes using a method
 * for computing size of a vector (which is given in two pair of x,y
 * coordinates).
 */
void lble(TS_Loblwe *p_loblwe)
{
  /* Relation operations with NaN would fail, result would be NaN anyway. */
  if (isnan(p_loblwe->x_act) || isnan(p_loblwe->y_act)) {
    errno = EDOM;
    p_loblwe->sum_min = NAN;
    p_loblwe->sum_max = NAN;
    return;
  }

  /* Computing new frontiers of actual point. (X min,max | Y min, max.) */
  p_loblwe->x_act_min = p_loblwe->x_act - p_loblwe->err_val;
  p_loblwe->x_act_max = p_loblwe->x_act + p_loblwe->err_val;

  p_loblwe->y_act_min = p_loblwe->y_act - p_loblwe->err_val;
  p_loblwe->y_act_max = p_loblwe->y_act + p_loblwe->err_val;

  
  /* Intervals overlay indicators. */
  bool x_axis_over = true;
  bool y_axis_over = true;

  
  /* Testing for overlay in x axis. */
  if (p_loblwe->x_bef_max < p_loblwe->x_act_min
      || p_loblwe->x_act_max < p_loblwe->x_bef_min) {
    x_axis_over = false;
  }
  
  /* Testing for overlay in y axis. */
  if (p_loblwe->y_bef_max < p_loblwe->y_act_min
      || p_loblwe->y_act_max < p_loblwe->y_bef_min) {
    y_axis_over = false;
  }
  
  /* Initialization to avoid warnings. */
  TE_Position pos = NO_OVERLAP;

  /* Determining the position of points to each other. 5 cases can occur. */
  if (x_axis_over == false && y_axis_over == false)
    pos = NO_OVERLAP;
  else if (x_axis_over == true && y_axis_over == false)
    pos = X_OVERLAP;
  else if (x_axis_over == false && y_axis_over == true)
    pos = Y_OVERLAP;
  else if (x_axis_over == true && y_axis_over == true) {
    /* Identity occurs when all 4 corners of squares are identical. */
    if (p_loblwe->x_bef_min != p_loblwe->x_act_min)
      pos = XY_OVERLAP;
    else 
      pos = IDENTITY;
  }

  /*
   * Calling function to count the minimal and maximal possible length of line.
   * Computing is different for each position, that can occur.
   */
  switch (pos) {
    
    /* No overlap at all, standard computation. */
    case NO_OVERLAP :
      no_overlap_func(p_loblwe);
      break;

    /* X intervals overlays. */
    case X_OVERLAP :
      x_overlap_func(p_loblwe);
      break;
    
    /* Y intervals overlays. */
    case Y_OVERLAP :
      y_overlap_func(p_loblwe);
      break; 
    
    /* X and Y intervals overlays. */
    case XY_OVERLAP :
      xy_overlap_func(p_loblwe);
      break;
    
    /* Points are identical, no more computation needed. */
    case IDENTITY :
      return;
  }
  
  int min_cls = fpclassify(p_loblwe->sum_min);  /* Minimal sum classification. */
  int max_cls = fpclassify(p_loblwe->sum_max);  /* Maximal sum classification. */
  
  /* Is result NaN? */
  if (min_cls == FP_NAN || max_cls == FP_NAN) {
    errno = EDOM;
    p_loblwe->sum_min = NAN;
    p_loblwe->sum_max = NAN;
  }
  /* Overflow? */
  else if (min_cls == FP_INFINITE || max_cls == FP_INFINITE) {
    errno = ERANGE;
  }

  /* Backup of current coordinates, which can be used in next computation. */
  p_loblwe->x_bef_min = p_loblwe->x_act_min;
  p_loblwe->x_bef_max = p_loblwe->x_act_max;

  p_loblwe->y_bef_min = p_loblwe->y_act_min;
  p_loblwe->y_bef_max = p_loblwe->y_act_max;

  return;
}


/******************************************************************************
 ~~~[ LOCAL SUBSIDIARY FUNCTIONS ]~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 ******************************************************************************/

/**
 * Computes minimal and maximal length of line.
 */
static inline void no_overlap_func(TS_Loblwe *p_loblwe)
{
  /*
   * Minimal length:
   */
  /* Actual point is right to predecessor. */
  if (p_loblwe->x_bef_max < p_loblwe->x_act_min)
    p_loblwe->x_dist = p_loblwe->x_act_min - p_loblwe->x_bef_max;
  /* Actual point is left to ... */
  else
    p_loblwe->x_dist = p_loblwe->x_act_max - p_loblwe->x_bef_min;
  
  /* Actual is above the predecessor. */
  if (p_loblwe->y_bef_max < p_loblwe->y_act_min)
    p_loblwe->y_dist = p_loblwe->y_act_min - p_loblwe->y_bef_max;
  /* Actual is below ... */
  else
    p_loblwe->y_dist = p_loblwe->y_act_max - p_loblwe->y_bef_min;
  
  /* Power of axis distances. */
  p_loblwe->x_dist *= p_loblwe->x_dist;
  p_loblwe->y_dist *= p_loblwe->y_dist;

  /* Sum + square root of distances addition. */
  p_loblwe->sum_min += sqrroot(p_loblwe->x_dist + p_loblwe->y_dist);

  
  /*
   * Maximal length:
   */
  /* Actual point is right to predecessor. */
  if (p_loblwe->x_bef_max < p_loblwe->x_act_min)
    p_loblwe->x_dist = p_loblwe->x_act_max - p_loblwe->x_bef_min;
  /* Actual point is left to ... */
  else
    p_loblwe->x_dist = p_loblwe->x_act_min - p_loblwe->x_bef_max;

  /* Actual is above the predecessor. */
  if (p_loblwe->y_bef_max < p_loblwe->y_act_min)
    p_loblwe->y_dist = p_loblwe->y_act_max - p_loblwe->y_bef_min;
  /* Actual is below ... */
  else
    p_loblwe->y_dist = p_loblwe->y_act_min - p_loblwe->y_bef_max;

  /* Power of axis distances. */
  p_loblwe->x_dist *= p_loblwe->x_dist;
  p_loblwe->y_dist *= p_loblwe->y_dist;

  /* Sum + square root of distances addition. */
  p_loblwe->sum_max += sqrroot(p_loblwe->x_dist + p_loblwe->y_dist);

  return;
}

/**
 * Computes maximal length of line. The minimal length is given as absolute
 * value of subtraction of overlaying sides.
 */
static inline void x_overlap_func(TS_Loblwe *p_loblwe)
{
  /* Actual point is above the predecessor. */
  if (p_loblwe->y_bef_max < p_loblwe->y_act_min) {
    p_loblwe->sum_min += abs_d(p_loblwe->y_act_min - p_loblwe->y_bef_max);
    p_loblwe->y_dist = p_loblwe->y_act_max - p_loblwe->y_bef_min;
  }
  /* Actual is below ... */
  else {
    p_loblwe->sum_min += abs_d(p_loblwe->y_act_max - p_loblwe->y_bef_min);
    p_loblwe->y_dist = p_loblwe->y_act_min - p_loblwe->y_bef_max;
  }
  
  /* Actual is left to predecessor. */
  if (p_loblwe->x_act_max < p_loblwe->x_bef_max)
    p_loblwe->x_dist = p_loblwe->x_act_min - p_loblwe->x_bef_max;
  /* Actual is right to ... */
  else
    p_loblwe->x_dist = p_loblwe->x_act_max - p_loblwe->x_bef_min;

  /* Power of axis distances. */
  p_loblwe->x_dist *= p_loblwe->x_dist;
  p_loblwe->y_dist *= p_loblwe->y_dist;

  /* Power of axis distances. */
  p_loblwe->sum_max += sqrroot(p_loblwe->x_dist + p_loblwe->y_dist);

  return;
}


/**
 * Computes maximal length of line. The minimal length is given as absolute
 * value of subtraction of overlaying sides.
 */
static inline void y_overlap_func(TS_Loblwe *p_loblwe)
{
  /* Actual is right to predecessor. */
  if (p_loblwe->x_bef_max < p_loblwe->x_act_min) {
    p_loblwe->sum_min += abs_d(p_loblwe->x_act_min - p_loblwe->x_bef_max);
    p_loblwe->x_dist = p_loblwe->x_act_max - p_loblwe->x_bef_min;
  }
  /* Actual is left to ... */
  else {
    p_loblwe->sum_min += abs_d(p_loblwe->x_act_max - p_loblwe->x_bef_min);
    p_loblwe->x_dist = p_loblwe->x_act_min - p_loblwe->x_bef_max;
  }
  
  /* Actual is above the predecessor. */
  if (p_loblwe->y_act_max > p_loblwe->y_bef_max)
    p_loblwe->y_dist = p_loblwe->y_act_max - p_loblwe->y_bef_min;
  /* Actual is below ... */
  else
    p_loblwe->y_dist = p_loblwe->y_act_min - p_loblwe->y_bef_max;

  /* Power of axis distances. */
  p_loblwe->x_dist *= p_loblwe->x_dist;
  p_loblwe->y_dist *= p_loblwe->y_dist;

  /* Power of axis distances. */
  p_loblwe->sum_max += sqrroot(p_loblwe->x_dist + p_loblwe->y_dist);

  return;
}

/**
 * Computes maximal length of line. The minimal length is zero (unchanged).
 */
static inline void xy_overlap_func(TS_Loblwe *p_loblwe)
{
  /* Actual is right to predecessor. */
  if (p_loblwe->x_bef_max < p_loblwe->x_act_max)
    p_loblwe->x_dist = p_loblwe->x_act_max - p_loblwe->x_bef_min;
  /* Actual is left to ... */
  else
    p_loblwe->x_dist = p_loblwe->x_act_min - p_loblwe->x_bef_max;

  /* Actual is above the predecessor. */
  if (p_loblwe->y_bef_max < p_loblwe->y_act_max)
    p_loblwe->y_dist = p_loblwe->y_act_max - p_loblwe->y_bef_min;
  /* Actual is below ... */
  else
    p_loblwe->y_dist = p_loblwe->y_act_min - p_loblwe->y_bef_max;

  /* Power of axis distances. */
  p_loblwe->x_dist *= p_loblwe->x_dist;
  p_loblwe->y_dist *= p_loblwe->y_dist;

  /* Power of axis distances. */
  p_loblwe->sum_max += sqrroot(p_loblwe->x_dist + p_loblwe->y_dist);

  return;
}

/******************************************************************************
 ***[ END OF PROJ2_GEOMLIB.C ]*************************************************
 ******************************************************************************/
