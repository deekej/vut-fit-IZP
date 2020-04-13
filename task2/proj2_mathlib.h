/**
 * File:          proj2_mathlib.h
 * Version:       1.2 final
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
 * Description:   This header file contains functional prototypes of PRIMARY
 *                functions of proj2_mathlib.c file and also global constants,
 *                data structures and inline functions, that can be used
 *                alongside them.
 *
 * More info @:
 *      https://www.fit.vutbr.cz/study/courses/IZP/private/projekty/proj2.html
 *
 * Encoding:      en_US.utf8 (United States)
 * Compiler used: gcc 4.5.2 (Ubuntu/Linaro 4.5.2-8ubuntu4) 
 */

/******************************************************************************
 ***[ START OF PROJ2_MATHLIB.H ]***********************************************
 ******************************************************************************/

/*
 * Safety mechanism against multi-including of this .h file.
 */
#ifndef PROJ2_MATHLIB_H
#define PROJ2_MATHLIB_H

/* Used in some inline functions below. */
#include <math.h>
/* DBL_DIG value is used below for DEFAULT_SIGDIG initialization. */
#include <float.h>

/******************************************************************************
 ~~~[ GLOBAL CONSTANTS DEFINITIONS ]~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 ******************************************************************************/

/* Default value of significant digits representing accuracy of computing. */
#define DEFAULT_SIGDIG DBL_DIG

/* Euler's number. */
#define EUL_NUM 2.7182818284590452353

/* Inverse Euler's number. */
#define EUL_NUM_INV 0.3678794411714423216

/* Value of Pi * 2. */
#define PI_2X 6.2831853071795864769

/* Value of Pi. */
#define PI 3.1415926535897932385

/* Value of Pi/2. */
#define PI_HALF 1.5707963267948966192

/* Value of Pi/4. */
#define PI_QUARTER 0.7853981633974483096

/* Value of Pi/180 used for transformation from radians to degrees. */
#define DEG_VAL 0.0174532925199432958

/* Value of 180/Pi used for transformation from degrees to radians. */
#define RAD_VAL 57.2957795130823208768


/******************************************************************************
 ~~~[ GLOBAL DATA TYPES DECLARATIONS ]~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 ******************************************************************************/

/**
 * Contains initialization values for particular expansion of Maclaurin series.
 */
typedef struct maclaur_series {
  double sum;                         /* Actual expansion sum. */
  double numer;                       /* Numerator of actual step. */
  double denom;                       /* Denominator of actual step. */

  double n;                           /* Initialization & helpful variable.*/

  bool oscillate;                     /* Should expansion oscillate? */
  bool use_fact;                      /* Should expansion use factorial? */
} TS_Maclaur_series;


/******************************************************************************
 ~~~[ FUNCTIONAL PROTOTYPES OF PRIMARY FUNCTIONS ]~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 ******************************************************************************/

double xsciexp(double x, int y);              /* x * 10^y */
double expn(double x, ...);                   /* Exponential function. */
double ln(double x, ...);                     /* Natural logarithm of x. */
double logax(double x, double base, ...);     /* General logarithm of x. */
double sqrroot(double x, ...);                /* Square root of x. */

double sine(double x, ...);                   /* Sine of x. */
double arc_sine(double x, ...);               /* Inverse sine of x. */
double hyper_sine(double x, ...);             /* Hyperbolical sine of x. */

double cosine(double x, ...);                 /* Cosine of x. */
double arc_cosine(double x, ...);             /* Inverse cosine of x. */
double hyper_cosine(double x, ...);           /* Hyperbolical cosine of x. */

double arc_tangent(double x, ...);            /* Inverse tangent of x. */
double hyper_tangent(double x, ...);          /* Hyperbolical tangent of x. */

double arc_cotangent(double x, ...);          /* Inverse cotangent of x. */
double hyper_cotangent(double x, ...);        /* Hyperbolical cotangent of x. */


/******************************************************************************
 ~~~[ FUNCTIONAL PROTOTYPES OF SHARED INLINE FUNCTIONS ]~~~~~~~~~~~~~~~~~~~~~~~
 ******************************************************************************/

static inline int abs_i(int i);
static inline float abs_f(float f);
static inline double abs_d(double d);
static inline long double abs_ld(long double ld);

static inline double todeg_d(double d);
static inline double torad_d(double d);


/******************************************************************************
 ~~~[ SHARED INLINE FUNCTIONS ]~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 ******************************************************************************/

/**
 * Returns the absolute value of given integer value.
 */
static inline int abs_i(int i)
{
  return (i < 0) ? -i : i;
}


/**
 * Returns the absolute value of given float value. If f is number negative or
 * positive infinity, then positive infinity is returned. If f is -0, then +0 is
 * returned. If f is -NaN, then NaN is returned.
 */
static inline float abs_f(float f)
{
  return (signbit(f)) ? -f : f;
}


/**
 * Same function as abs_f, but instead of type float it uses type double.
 */
static inline double abs_d(double d)
{
  return (signbit(d)) ? -d : d;
}


/**
 * Same function as abs_f, but instead of type float it uses type long double.
 */
static inline long double abs_ld(long double ld)
{
  return (signbit(ld)) ? -ld : ld;
}


/**
 * Transforms double value from radians to degrees.
 */
static inline double todeg_d(double d)
{
  return d * DEG_VAL;
}


/**
 * Transforms double value from degrees to radians.
 */
static inline double torad_d(double d)
{
  return d * RAD_VAL;
}

#endif

/******************************************************************************
 ***[ END OF PROJ2_MATHLIB.H ]*************************************************
 ******************************************************************************/
