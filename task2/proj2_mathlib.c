/**
 * File:          proj2_mathlib.c
 * Version:       1.1 rc
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
 * Description:   This module contains mathematical functions and other
 *                subsidiary functions which are used for main computing. This
 *                module was designed for use mostly by proj2.c module, but it
 *                can be used by other modules too. For list of functions 
 *                usable outside this module see the proj2_mathlib.h header
 *                file.
 *
 * More info @:
 *      https://www.fit.vutbr.cz/study/courses/IZP/private/projekty/proj2.html
 *
 * Encoding:      en_US.utf8 (United States)
 * Compiler used: gcc 4.5.2 (Ubuntu/Linaro 4.5.2-8ubuntu4)
 */


/******************************************************************************
 ***[ START OF MATHLIB.C ]*****************************************************
 ******************************************************************************/

/******************************************************************************
 ~~~[ HEADER FILES ]~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 ******************************************************************************/

#include <errno.h>
#include <fenv.h>
#include <float.h>
#include <math.h>
#include <stdarg.h>
#include <stdbool.h>

#include "proj2_mathlib.h"

/******************************************************************************
 ~~~[ MACRO DEFINITIONS ]~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 ******************************************************************************/

/**
 * This macro is used when function can have one last optional argument which
 * represents the value of sigdig (significant digit, see below). LFA stands
 * for last fixed argument (of the actual function).
 */
#define va_sigdig(lfa, sigdig)       \
  (sigdig) = DEFAULT_SIGDIG;          /* Sigdig default initialization. */\
  va_list ap;                         /* Stack pointer. */\
                                     \
  va_start(ap, (lfa));                /* Stack pointer initialization. */\
  (sigdig) = (int) va_arg(ap, int);   /* Accessing last stack argument. */\
                                     \
  /*
   * Testing for cor correct sigdig value from interval of zero exclusive to
   * DBL_DIG inclusive.
   */\
  if ((sigdig) < 1) \
    (sigdig) = DEFAULT_SIGDIG;        /* Wrong sigdig = default value. */\
  else if ((sigdig) > DBL_DIG)       \
    (sigdig) = DBL_DIG;               /* Maximal possible sigdig. */\
                                     \
  va_end(ap)                          /* End of work with stack. */


/******************************************************************************
 ~~~[ FUNCTIONAL PROTOTYPES OF LOCAL FUNCTIONS ]~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 ******************************************************************************/

static inline double expn_heurstc(double *p_x);
static inline double ln_heurstc(double *p_x);
static inline void co_sine_heurstc(double *p_x);

static inline double mclrn_expan(double x, TS_Maclaur_series expan, int sigdig);


/******************************************************************************
 ~~~[ PRIMARY FUNCTIONS ]~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 ******************************************************************************/

/**
 * Function computes: x * 10^y (scientific exponent of y multiplied by x). For
 * easier computation the y must be an integer. If need of y as floating point
 * number, than try using functions from standard math library.
 */
double xsciexp(double x, int y)
{
  int xcls = fpclassify(x);     /* x value classifying. */
  
  /* Domain error checking. */
  if ((xcls == FP_NAN || xcls == FP_ZERO || xcls == FP_INFINITE) && y == 0) {
    errno = EDOM;
    feraiseexcept(FE_INVALID);
    return (signbit(x)) ? -NAN : NAN;
  }

  if (y == 0)                   /* No more computing needed: 10^0 == 1. */
    return x;

  double exp = 1.0;             /* Starting value of exponent. */

  int z = abs_i(y);             /* Absolute value of y for iterations. */

  for (int i = 0; i < z; i++)   /* Shifting the number's set. */
    exp *= 10;          

  /*
   * Returning the appropriate value depending on sign of the exponent y.
   */
  if (y < 0)
    return x / exp;
  else
    return x * exp;
}


/**
 * Computes value of exponential function (e^x where e is Euler's number) by 
 * double calling the mclrn_expan() function, first time over the even order
 * expansion and second time over the odd order expansion. (For better
 * understanding see Maclaurin expansion for exponential function.) Function
 * sets errno and raises FP exception upon error. One optional argument
 * representing number of required significant digits of the result can be used
 * (sigdig).
 */
double expn(double x, ...)
{
  int xcls = fpclassify(x);       /* x value classification. */

  /* Error handling. */
  switch (xcls) {

    /* NaN */
    case FP_NAN :
      return x;
    
    /* e^inf == inf, e^-inf == 0.0 */
    case FP_INFINITE :
      return (signbit(x)) ? 0.0 : x;

    /* e^0 = 1.0 */
    case FP_ZERO :
      return 1.0;
    
    /* No error. */
    default :
      break;
  }

  
  /*
   * Safety mechanism against too much long looping in expn_heurstc() for
   * bigger values. The result of expn() will overflow/underflow for values
   * outside of this interval.
   */
  if (x > 1000) {
    errno = ERANGE;
    feraiseexcept(FE_OVERFLOW);
    return HUGE_VAL;
  }
  else if (x < -1000) {
    errno = ERANGE;
    feraiseexcept(FE_UNDERFLOW);
    return 0.0;
  }


  int sigdig;                     /* Variable for va_sigdig() macro below. */
  fenv_t env;                     /* FP environment storage. */

  va_sigdig(x, sigdig);           /* Processes the optional sigdig argument. */
  feholdexcept(&env);             /* Backup of FP environment. */

  double y = expn_heurstc(&x);    /* Heuristic for expn(x). */


  /* Even order expansion initialization. */
  TS_Maclaur_series expnt1 = {
    .sum = 1.0,
    .numer = 1.0,
    .denom = 1.0,

    .n = 0.0,

    .oscillate = false,
    .use_fact = true,
  };

  /* Odd order expansion initialization. */
  TS_Maclaur_series expnt2 = {
    .sum = x,
    .numer = x,
    .denom = 1.0,

    .n = 1.0,

    .oscillate = false,
    .use_fact = true,
  };


  /* Parts of the result. */
  expnt1.sum = mclrn_expan(x, expnt1, sigdig);
  expnt2.sum = mclrn_expan(x, expnt2, sigdig);

  x = (expnt1.sum + expnt2.sum) * y;  /* Finalizing of the result. */

  errno = 0;                          /* Reseting errno. */
  fesetenv(&env);                     /* Restoring FP environment. */


  xcls = fpclassify(x);               /* Result classification. */

  /*
   * Detection of an error by return value of fpclassify(). Sets errno and
   * raises exception correspondingly to the error, if there was any, and
   * returns proper value as described in "man 7 math_error".
   */
  switch (xcls) {
    /* Indeterminate form appeared. */
    case FP_NAN :
      errno = ERANGE;
      feraiseexcept(FE_INVALID);
      return x;

    /* Overflow occurred. */
    case FP_INFINITE :
      errno = ERANGE;
      feraiseexcept(FE_OVERFLOW);
      return HUGE_VAL;

    /* Underflow occurred. */
    case FP_ZERO :
    case FP_SUBNORMAL :
      errno = ERANGE;
      feraiseexcept(FE_UNDERFLOW);
      return 0.0;

    /* Zero and other results are correct. */
    case FP_NORMAL :
      return x;

    /* Default only for warning disposal. This case should never occur! */
    default :
      return x;
  }
}


/**
 * Computes value of natural logarithm by using transformed version of Maclaurin
 * series expansion. Function sets errno and raises FP exception upon error. One
 * optional argument representing number of required significant digits of the 
 * result can be used (sigdig).
 */
double ln(double x, ...)
{
  /* Relation operations with NaN would fail. */
  if (isnan(x)) {
    return x;
  }
  /* Pole error - ln(0) is exact minus infinity. */
  else if (fpclassify(x) == FP_ZERO) {
    errno = ERANGE;
    feraiseexcept(FE_DIVBYZERO);
    return -HUGE_VAL;
  }
  /* Any negative values is out of any logarithm domain. */
  else if (signbit(x)) {
    errno = EDOM;
    feraiseexcept(FE_INVALID);
    return NAN;
  }
  /* Positive infinity of any logarithm is positive infinity. */
  else if (isinf(x)) {
    return HUGE_VAL;
  }


  int sigdig;                   /* Variable for va_sigdig() macro below. */
  
  va_sigdig(x, sigdig);         /* Processes the optional sigdig argument. */


  double h = ln_heurstc(&x);    /* Heuristic for ln(x). */

  double y = (x - 1) / (x + 1); /* Transform. for quicker convergence rate. */

  TS_Maclaur_series logarithm = {
    .sum = y,
    .numer = y,
    .denom = 1.0,

    .n = 1.0,

    .oscillate = false,
    .use_fact = false,
  };

  
  logarithm.sum = mclrn_expan(y, logarithm, sigdig);

  logarithm.sum *= 2.0;         /* Correction because of used transform. */

  return logarithm.sum + h;     /* Computed value + result of heuristic. */
}


/**
 * Computes general logarithm of x using relation: loga(x) = ln(x) / ln(base),
 * where the general logarithm is defined for base in interval of zero exclusive
 * to infinity exclusive, without 1.0. The value x has to be in interval of zero
 * inclusive to infinity inclusive. Function sets errno and raise FP exception
 * upon error. One optional argument representing number of required significant
 * digits of the result can be used (sigdig). 
 */
double logax(double x, double base, ...)
{
  /* 
   * Testing for correct base of logarithm. Testing of value x is performed in
   * ln() function.
   */
  if (fpclassify(base) == FP_ZERO || fpclassify(base) == FP_INFINITE
      || fpclassify(base) == FP_NAN || signbit(base) || base == 1.0) {
    errno = EDOM;
    feraiseexcept(FE_INVALID);
    return NAN;
  }

  int sigdig;                   /* Variable for va_sigdig() macro below. */

  va_sigdig(base, sigdig);      /* Processes the optional sigdig argument. */

  double numer = ln(x, sigdig);
  double denom = ln(base, sigdig);

  return numer / denom;         /* Value of general logarithm. */
}


/**
 * Computes square root by using this identity: square root(x) = e^(ln(x)/2).
 * The value of x has to be in interval of 0 inclusive to infinity inclusive,
 * otherwise a domain error occurs. 
 */
double sqrroot(double x, ...)
{
  /* Return for NaN, zero and infinite value. */
  if (isnan(x) || isinf(x) == 1 || fpclassify(x) == FP_ZERO) {
    return x;
  }
  /* Negative value -> domain error. */
  else if (signbit(x)) {
    errno = EDOM;
    feraiseexcept(FE_INVALID);
    return -NAN;
  }


  int sigdig;                 /* Variable for va_sigdig() macro below. */
  fenv_t env;                 /* FP environment storage. */

  va_sigdig(x, sigdig);       /* Processes the optional sigdig argument. */
  feholdexcept(&env);         /* Backup of current FP environment. */

  double result;
  
  result = ln(x, sigdig) / 2.0;     /* Intermediate step. */
  result = expn(result, sigdig);    /* Final value. */
  
  errno = 0;                  /* Reseting errno. */
  fesetenv(&env);             /* Restoring of FP environment. */

  return result;
}

/**
 * Computes value of sine by calling mclrn_expan() with properly initialized
 * values. The result of function is in radians, if the given value is in
 * radians. (Same applies for degrees.) Function sets errno and raises FP
 * exception upon error. One optional argument representing number of required
 * significant digits of the result can be used (sigdig).
 */
double sine(double x, ...)
{
  int xcls = fpclassify(x);       /* x classification. */

  switch (xcls) {

    /* Relation operations with NaN would fail. */
    case FP_NAN :
      return x;
    
    /* Domain error. */
    case FP_INFINITE :
      errno = EDOM;
      feraiseexcept(FE_INVALID);
      return -NAN;

    /* sin(0.0) == 0.0 */
    case FP_ZERO :
      return x;
    
    /* None of above. */
    default :
      break;
  }

  co_sine_heurstc(&x);            /* Heuristic for sine & cosine. */

  /* Underflow occured? */
  if (fpclassify(x) == FP_ZERO) {
    errno = ERANGE;
    feraiseexcept(FE_UNDERFLOW);
    return x;
  }


  int sigdig;                     /* Variable for va_sigdig() macro below. */
  
  va_sigdig(x, sigdig);           /* Processes the optional sigdig argument. */
  
  TS_Maclaur_series sin_x = {
    .sum = x,
    .numer = x,
    .denom = 1.0,

    .n = 1.0,

    .oscillate = true,
    .use_fact = true,
  };

  /* Return of final result. */
  return mclrn_expan(x, sin_x, sigdig);
}


/**
 * Computes inverse sine of x (arcus sine) using special case of Maclaurin
 * expansion for values in interval -1.0 exclusive to 1.0 exclusive (without
 * factorial in denominator). Because of that the input value is transformed
 * into this interval and back after getting the result. Function computes
 * in radians. Also sets errno and raises FP exception upon error. One
 * optional argument representing the number of required significant digits
 * of the result can be used (sigdig).
 */
double arc_sine(double x, ...)
{
  int xcls = fpclassify(x);        /* x classification. */

  switch (xcls) {

    /* Relation operations with NaN would fail. asin(0.0) == 0.0 */
    case FP_NAN :
    case FP_ZERO :
      return x;
    
    /* Domain error. */
    case FP_INFINITE :
      errno = EDOM;
      feraiseexcept(FE_INVALID);
      return NAN;
    
    /* None of above. Proceed with controls. */
    default :
      break;
  }
  
  /* Domain error? */
  if (abs_d(x) > 1.0) {
    errno = EDOM;
    feraiseexcept(FE_INVALID);
    return NAN;
  }
  
  /* asin(-1.0) == -PI/2 | asin(1.0) == PI/2 */
  if (abs_d(x) == 1.0)
    return (signbit(x)) ? -PI_HALF : PI_HALF;


  int sigdig;                     /* Variable for va_sigdig() macro below. */
  fenv_t env;                     /* FP environment storage. */

  va_sigdig(x, sigdig);           /* Processes the optional sigdig argument. */
  feholdexcept(&env);             /* Backup of current FP environment. */

  /*
   * Input value transformation for quicker and more accurate computation:
   * y = x / (1.0 + sqrt(1.0 - x^2))
   */
  double y = x / (1.0 + sqrroot(1.0 - (x * x)));

  TS_Maclaur_series arc_sin = {
    .sum = y,
    .numer = y,
    .denom = 1.0,

    .n = 1.0,

    .oscillate = true,
    .use_fact = false,
  };

  arc_sin.sum = mclrn_expan(y, arc_sin, sigdig);

  errno = 0;                      /* Reseting errno. */
  fesetenv(&env);                 /* Restoring FP environment. */

  return arc_sin.sum * 2.0;       /* Return of final correct value. */
}


/**
 * Computes value of hyperbolical sine of given value using this relation:
 * sinh(x) = (e^(2*x) - 1)/(2*e^x) | With this representation function can 
 * computes to bigger values without overflowing than would be sooner while
 * using Maclaurin expansion for hyperbolical sine. Function sets errno and
 * raises FP exception upon error. One optional argument representing number
 * of required significant digits of the result can be used (sigdig).
 */
double hyper_sine(double x, ...)
{
  /*
   * Relation operations with NaN would fail,
   * sinh(inf) == inf, sinh(-inf) == -inf.
   */
  if (isnan(x) || isinf(x))
    return x;

  
  int sigdig;                 /* Variable for va_sigdig() macro below. */
  fenv_t env;                 /* FP environment storage. */

  va_sigdig(x, sigdig);       /* Processes the optional sigdig argument. */
  feholdexcept(&env);         /* Backup of current FP environment. */

  double numer = expn(x * 2, sigdig) - 1;   /*  e^(2 * x) - 1  */
  double denom = expn(x, sigdig) * 2;       /*  2 * e^(x)  */

  errno = 0;                  /* Reseting errno. */
  fesetenv(&env);             /* Restoring FP environment. */

  /* Testing for overflow. */
  if (isinf(numer) || isinf(denom)) {
    errno = ERANGE;
    feraiseexcept(FE_OVERFLOW);
    return (signbit(x)) ? -HUGE_VAL : HUGE_VAL;  
  }

  return numer / denom;       /* No overflow occurred, return final result. */
}


/**
 * Same as sine function, but instead of computing sine this function computes
 * cosine of given value.
 */
double cosine(double x, ...)
{
  int xcls = fpclassify(x);       /* x classification. */

  switch (xcls) {

    /* Relation operations with NaN would fail. */
    case FP_NAN :
      return x;
    
    /* Domain error. */
    case FP_INFINITE :
      errno = EDOM;
      feraiseexcept(FE_INVALID);
      return -NAN;

    /* cos(0.0) == 1.0 */
    case FP_ZERO :
      return 1.0;

    default :
      break;
  }

  co_sine_heurstc(&x);            /* Heuristic for sine & cosine. */
  
  /* Underflow error? */
  if (fpclassify(x) == FP_ZERO) {
    errno = ERANGE;
    feraiseexcept(FE_UNDERFLOW);
    return x;
  }


  int sigdig;                     /* Variable for va_sigdig() macro below. */

  va_sigdig(x, sigdig);           /* Processes the optional sigdig argument. */

  TS_Maclaur_series cos_x = {
    .sum = 1.0,
    .numer = 1.0,
    .denom = 1.0,

    .n = 0.0,

    .oscillate = true,
    .use_fact = true,
  };
  
  /* Return of final result. */
  return mclrn_expan(x, cos_x, sigdig);
}


/**
 * Same function as arc_sine(), but instead of computing inverse sine this 
 * function computes inverse cosine of x (arcus cosine).
 */
double arc_cosine(double x, ...)
{
  int xcls = fpclassify(x);       /* x classification. */

  switch (xcls) {

    /* Relation operations with NaN would fail. */
    case FP_NAN :
      return x;

    /* Domain error. */
    case FP_INFINITE :
      errno = EDOM;
      feraiseexcept(FE_INVALID);
      return NAN;
    
    /* acos(0.0) == PI/2 */
    case FP_ZERO :
      return PI_HALF;

    /* None of above, proceed with controls. */
    default :
      break;
  }

  /* Domain error? */
  if (abs_d(x) > 1.0) {
    errno = EDOM;
    feraiseexcept(FE_INVALID);
    return NAN;
  }
  
  /* asin(-1.0) == PI | asin(1.0) == 0.0 */
  if (abs_d(x) == 1.0)
    return (signbit(x)) ? PI : 0.0;

   
  int sigdig;                     /* Variable for va_sigdig() macro below. */
  fenv_t env;                     /* FP environment storage. */

  va_sigdig(x, sigdig);           /* Processes the optional sigdig argument. */
  feholdexcept(&env);             /* Backup of current FP environment. */
  
  /*
   * Input value transformation for quicker and more accurate computation:
   * y = x / (1.0 + sqrt(1.0 - x^2))
   */
  double y = x / (1.0 + sqrroot(1.0 - (x * x)));

  TS_Maclaur_series arc_cos = {
    .sum = y,
    .numer = y,
    .denom = 1.0,

    .n = 1.0,

    .oscillate = true,
    .use_fact = false,
  };

  arc_cos.sum = mclrn_expan(y, arc_cos, sigdig);

  errno = 0;                      /* Reseting errno. */
  fesetenv(&env);                 /* Restoring FP environment. */
  
  /* Return of final correct result. */
  return PI_HALF - (arc_cos.sum * 2.0);
}


/**
 * Same function as hyper_sine(), but instead of computing hyperbolic sine this
 * function computes hyperbolic cosine of given value using similar relation:
 * cosh(x) = (e^(2*x) + 1)/(2*e^x)
 */
double hyper_cosine(double x, ...)
{
  /* Relation operations with NaN would fail. */
  if (isnan(x))
    return x;
  /* cosh(+/-inf) == inf. */
  else if (isinf(x))
    return abs_d(x);


  int sigdig;                 /* Variable for va_sigdig() macro below. */
  fenv_t env;                 /* FP environment storage. */

  va_sigdig(x, sigdig);       /* Processes the optional sigdig argument. */
  feholdexcept(&env);         /* Backup of current FP environment. */

  double numer = expn(x * 2, sigdig) + 1;   /*  e^(2 * x) + 1  */
  double denom = expn(x, sigdig) * 2;       /*  2 * e^(x)  */
  
  errno = 0;                  /* Reseting errno. */
  fesetenv(&env);             /* Restoring FP environment. */

  /* Testing for overflow. */
  if (isinf(numer) || isinf(denom)) {
    errno = ERANGE;
    feraiseexcept(FE_OVERFLOW);
    return HUGE_VAL;
  }

  return numer / denom;       /* No overflow occurred, return final result. */
}


/**
 * Computes inverse tangent of x (arcus tangent) using special case of
 * Maclaurin expansion for values in interval -1.0 exclusive to 1.0 exclusive
 * (without factorial in denominator). Because of that the input value is
 * transformed into this interval and back after getting the result. Function
 * computes in radians. Also sets errno and raises FP exception upon error.
 * One optional argument representing the number of required significant digits
 * of the result can be used (sigdig).
 */
double arc_tangent(double x, ...)
{
  int xcls = fpclassify(x);       /* x classification. */

  switch (xcls) {

    /* Relation operations with NaN would fail. atan(0.0) == 0 */
    case FP_NAN :
    case FP_ZERO :
      return x;

    /* atan(-inf) == -PI/2 | atan(inf) == PI/2 */
    case FP_INFINITE :
      return (signbit(x)) ? -PI_HALF : PI_HALF;
    
    /* None of above, proceed with controls. */
    default :
      break;
  }
  
  /*
   * Huge values approximates +-PI/2 and very slows the computation and still
   * the output result wouldn't be significant compared to +-PI/2.
   */
  if (abs_d(x) >= 800000)
    return (signbit(x)) ? -PI_HALF : PI_HALF;


  int sigdig;                     /* Variable for va_sigdig() macro below. */
  fenv_t env;                     /* FP environment storage. */

  va_sigdig(x, sigdig);           /* Processes the optional sigdig argument. */
  feholdexcept(&env);             /* Backup of current FP environment. */

  /*
   * Input value transformation for quicker and more accurate computation:
   * y = x / (1.0 + sqrt(1.0 + x^2))
   */
  double y = x / (1.0 + sqrroot(1.0 + (x * x)));

  TS_Maclaur_series arc_tan = {
    .sum = y,
    .numer = y,
    .denom = 1.0,

    .n = 1.0,

    .oscillate = true,
    .use_fact = false,
  };

  arc_tan.sum = mclrn_expan(y, arc_tan, sigdig);

  errno = 0;                      /* Reseting errno. */
  fesetenv(&env);                 /* Restoring FP environment. */

  return arc_tan.sum * 2.0;       /* Return of the final correct result. */
}


/**
 * Computes value of hyperbolical tangent of given value using this relation:
 * tanh(x) = (e^(2*x) - 1) / (e^(2*x) + 1) | Function sets errno and raises
 * FP exception upon error. One optional argument representing number of
 * required significant digits of the result can be used (sigdig).
 */
double hyper_tangent(double x, ...)
{
  int xcls = fpclassify(x);     /* x value classification. */

  switch (xcls) {

    /* Relation operations with NaN would fail. tanh(0) == 0. */
    case FP_NAN :
    case FP_ZERO :
      return x;
    
    /* tanh(inf) == 1.0, tanh(-inf) == -1.0 */
    case FP_INFINITE :
      return (signbit(x)) ? -1.0 : 1.0;
    
    /* None of above. Proceed with computation. */
    default :
      break;
  }

  
  int sigdig;                   /* Variable for va_sigdig() macro below. */
  fenv_t env;                   /* FP environment storage. */

  va_sigdig(x, sigdig);         /* Processes the optional sigdig argument. */
  feholdexcept(&env);           /* Backup of current FP environment. */

  double step = expn(x * 2, sigdig);      /*  e^(2 * x)  */

  errno = 0;                    /* Reseting errno. */
  fesetenv(&env);               /* Restoring FP environment. */

  /* Testing for overflow. Huge values approximate to +/- 1.0. */
  if (isinf(step))
    return (signbit(x)) ? -1.0 : 1.0;

  /* No overflow occurred. Return of final result. */
  return (step - 1) / (step + 1);
}


/**
 * Same function as arc_tangent(), but instead of computing inverse tangent
 * this function computes inverse cotangent (arcus cotangent) using same 
 * relations, because of relation that acot(x) equals to atan() of inverse x.
 * However, inverse cotangent has different domain and image.
 */
double arc_cotangent(double x, ...)
{
  int xcls = fpclassify(x);       /* x classification. */

  switch (xcls) {
    /* Relation operations with NaN would fail. */
    case FP_NAN : 
      return x;

    /* acot(0.0) == PI/2 */
    case FP_ZERO :
      return PI_HALF;

    /* acot(+-inf) == 0.0 */
    case FP_INFINITE :
      return 0.0;
    
    /* None of above, proceed with controls. */
    default :
      break;
  }
  
  /*
   * Very small values approximates PI/2 (negative PI/2 for negative values,
   * positive PI/2 for positive values) and very slows down the computation
   * and still the result wouldn't be significant compared to +- PI/2.
   */
  if (abs_d(x) < 0.00001)
    return (signbit(x)) ? -PI_HALF : PI_HALF;

  
  int sigdig;                     /* Variable for va_sigdig() macro below. */
  fenv_t env;                     /* FP environment storage. */

  va_sigdig(x, sigdig);           /* Processes the optional sigdig argument. */
  feholdexcept(&env);             /* Backup of current FP environment. */

  x = 1.0 / x;                    /* acot(x) == atan(x^-1) */

  /*
   * Input value transformation for quicker and more accurate computation:
   * y = x / (1.0 + sqrt(1.0 + x^2))
   */
  double y = x / (1.0 + sqrroot(1.0 + (x * x)));

  TS_Maclaur_series arc_cot = {
    .sum = y,
    .numer = y,
    .denom = 1.0,

    .n = 1.0,

    .oscillate = true,
    .use_fact = false,
  };

  arc_cot.sum = mclrn_expan(y, arc_cot, sigdig);

  errno = 0;                        /* Reseting errno. */
  fesetenv(&env);                   /* Restoring FP environment. */

  return arc_cot.sum * 2;           /* Return of the final correct result. */
}


/**
 * Same function as hyper_tangent(), but instead of computing hyperbolic
 * tangent this function computes hyperbolic cotangent of given value using
 * similar relation: coth(x) = (e^(2*x) + 1) / (e^(2*x) + 1)
 */
double hyper_cotangent(double x, ...)
{
  int xcls = fpclassify(x);       /* x value classification. */

  switch (xcls) {
    /* Relation operations with NaN would fail. */
    case FP_NAN :
      return x;

    /* coth(inf) == 1.0, coth(-inf) == -1.0 */
    case FP_INFINITE :
      return (signbit(x)) ? -1.0 : 1.0;
    
    /* coth(0) == complex infinity, NaN for real numbers. */
    case FP_ZERO:
      errno = EDOM;
      feraiseexcept(FE_INVALID);
      return -NAN;
    
    /* None of above. Proceed with computation. */
    default :
      break;
  }


  int sigdig;                     /* Variable for va_sigdig() macro below. */
  fenv_t env;                     /* FP environment storage. */

  va_sigdig(x, sigdig);           /* Processes the optional sigdig argument. */
  feholdexcept(&env);             /* Backup of current FP environment. */

  double step = expn(x * 2, sigdig);      /*  e^(2 * x)  */

  errno = 0;                      /* Reseting errno. */
  fesetenv(&env);                 /* Restoring FP environment. */

  /* Testing for overflow. Huge values approximate to +/- 1.0. */
  if (isinf(step))
    return (signbit(x)) ? -1.0 : 1.0;

  /* No overflow occurred. Return of final result. */
  return (step + 1) / (step - 1);
}


/******************************************************************************
 ~~~[ HEURISTIC FUNCTIONS ]~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 ******************************************************************************/

/**
 * Heuristic for the exponential function (e^x). This function modifies the
 * input value towards 0.0 using relation of: exp(x+y) = exp(x) * exp(y) for
 * gaining the best rate of convergence. Function modifies the input value into
 * the interval of -1.0 exclusive to 1.0 exclusive and returns value of e^y,
 * where y represents number of periods (value 1.0) subtracted from the input
 * value (e^0 = 1.0).
 */
static inline double expn_heurstc(double *p_x)
{
  double y = 1.0;           /* e^y initialization. */
  double i = trunc(*p_x);   /* Number of periods, iteration counter. */

  *p_x -= i;                /* Lowering x value -> periods subtraction. */

  /* Negative value? */
  if (signbit(i))
    /* e^y for y below zero. */
    while (i <= -1.0) {
      y *= EUL_NUM_INV;
      i++;
  }
  else
    /* e^y for y above zero. */
    while (i >= 1.0) {
      y *= EUL_NUM; 
      i--;
    }

  return y;                 /* e^y to be multiplied with e^x later. */
}


/**
 * Heuristic for natural logarithm. This function modifies the input value of 
 * ln() by adjusting value towards 1.0 for gaining the best rate of convergence.
 * Return value represents the quantity of how many times the input value was
 * modified. Heuristic exploits this relation: ln(e) = 1, where e is the Euler's
 * number.
 */
static inline double ln_heurstc(double *p_x)
{
  double x_pred = 0;        /* Previous value of x. See the use below. */
  double i = 0;             /* Modification counter. */

  /*
   * If input value is less than 1.0, then multiplying is required to get closer
   * to 1.0, else the number is greater and dividing is required...
   */
  if (*p_x < 1.0) {
    while (*p_x < 1.0) {
      x_pred = *p_x;        /* Backup of previous value of x. */
      *p_x *= EUL_NUM;      /* Multiplying by Euler's number. */
      i--;                  /* Decreasing for proper value when adding later. */
    }

    /* Determining which value is closer to 1.0. */
    if (abs_d(1.0 - *p_x) > abs_d(1.0 - x_pred)) {
      *p_x = x_pred;        /* Previous value was closer. */
      i++;                  /* Count correcting. */
    }
  }
  else {
    while (*p_x > 1.0) {
      x_pred = *p_x;        /* Backup of previous value of x. */
      *p_x /= EUL_NUM;      /* Dividing by Euler's number. */
      i++;                  /* Increasing for proper value when adding later. */
    }
    
    /* Determining which value is closer to 1.0. */
    if (abs_d(1.0 - *p_x) > abs_d(1.0 - x_pred)) {
      *p_x = x_pred;        /* Previous value was closer. */
      i--;                  /* Count correcting. */
    }
  }
  
  /*
   * Value to be add to the outcome of natural logarithm of new x to get a
   * proper result of unmodified x.
   */
  return i;
}


/**
 * Heuristic for sine & cosine functions. This functions shifts the value into
 * interval of zero inclusive to 2 * PI exclusive because of periodicity of
 * trigonometric functions. Sine and cosine functions convergence is the fastest
 * for this interval.
 */
static inline void co_sine_heurstc(double *p_x)
{
  double k = trunc(*p_x / PI_2X);   /* Detection of number of periods. */
  double periods = k * PI_2X;       /* Value of periods. */

  *p_x -= periods;                  /* Subtracting of the periods. */

  /* Negative value? (Not in interval of 0 inclusive to 2 * PI exclusive?) */
  if (signbit(*p_x)) {
    *p_x += PI_2X;                  /* Shift to positive value. */
  }

  return;
}


/******************************************************************************
 ~~~[ SUBSIDIARY FUNCTIONS ]~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 ******************************************************************************/

/**
 * Computes value of Maclaurin series (special type of Taylor series where
 * a == 0) for the most elementary expansions. The exact expansion depends on
 * values in given TS_Maclaur_series structure. When true is used for the 
 * oscillate, then function computes with oscillating sign of every addition.
 * When true is used for use_fact, then function computes with factorial as a
 * denominator. Sigdig represents number of digits of result that are
 * significant.
 */
static inline double mclrn_expan(double x, TS_Maclaur_series expan, int sigdig)
{
  if (isnan(x))           /* Relation operations with NaN would fail. */
    return x;

  double sum_pred;        /* Sum of all values before. */
  double eps_factor;      /* Value used to compute new epsilon. */
  double x2;              /* Value representing +/-(x^2), see below. */
  double eps;             /* Wanted accuracy (epsilon) of actual step. */

  sum_pred = 0.0;                       /* No sum yet. */
  eps_factor = xsciexp(1.0, -sigdig);   /* 1*10^(-sigdig) */


  /*
   * Preprocess the value x^2. The sign of x2 depends on if there was a
   * requirement for an oscillation of the sign.
   */
  if (expan.oscillate == true)
    x2 = -(x * x);        /* Expansion with sign oscillation of each step.*/
  else
    x2 = x * x;           /* Expansion without sign oscillation. */


  /*
   * Selects correct expansion depending on value of expan.use_fact.
   */
  if (expan.use_fact == true) {

    /*
     * Computation with factorial as a denominator.
     */
    do {
      sum_pred = expan.sum;                       /* Backup of actual SUM. */

      expan.n += 2.0;                             /* New value of N. */
      expan.numer *= x2;                          /* New value of power. */
      expan.denom *= (expan.n) * (expan.n - 1.0); /* New value of factorial. */
      expan.sum += expan.numer / expan.denom;     /* New value of expansion. */

      eps = abs_d(expan.sum * eps_factor);        /* New value of epsilon. */
    
    /*
     * If |actual value - previous value| > required accuracy (epsilon) and if
     * fpclassify() of actual sum is == FP_NORMAL, then repeat.
     */
    } while (isnormal(expan.sum) && abs_d(expan.sum - sum_pred) > eps);

  }
  else {

    /*
     * Computation with only N as a denominator. For this kind of expansion
     * input value should be transformed to interval of -1.0 exclusive to
     * 1.0 inclusive and back, otherwise the expansion would loop infinitely.
     * Also, the closer value is to the absolute value of +/-1, the faster 
     * convergence is.
     */
    do {
      sum_pred = expan.sum;                       /* Backup of actual SUM. */

      expan.numer *= x2;                          /* New value of power. */
      expan.denom += 2.0;                         /* New value of N. */
      expan.sum += expan.numer / expan.denom;     /* New value of expansion. */

      eps = abs_d(expan.sum * eps_factor);        /* New value of epsilon. */

    /*
     * If |actual value - previous value| > reqired accuracy (epsilon) and if
     * fpclassify() of actual sum is == FP_NORMAL, then repeat.
     */
    } while (isnormal(expan.sum) && abs_d(expan.sum - sum_pred) > eps);

  }


  int sum_cls = fpclassify(expan.sum);            /* Result classification. */

  /*
   * Detection of an error by return value of fpclassify(). Sets errno and
   * raises exception correspondingly to the error, if there was any, and
   * returns proper value as described in "man 7 math_error".
   */
  switch (sum_cls) {

    /* Indeterminate form appeared. */
    case FP_NAN :
      errno = ERANGE;
      feraiseexcept(FE_INVALID);
      return expan.sum;

    /* Overflow occurred. */
    case FP_INFINITE :
      errno = ERANGE;
      feraiseexcept(FE_OVERFLOW);
      return (signbit(expan.sum)) ? -HUGE_VAL : HUGE_VAL;

    /* Underflow occurred. */
    case FP_SUBNORMAL :
      errno = ERANGE;
      feraiseexcept(FE_UNDERFLOW);
      return 0.0;

    /* Zero and other results are correct. */
    case FP_ZERO :
    case FP_NORMAL :
      return expan.sum;

    /* Default only for warning disposal. This case should never occur! */
    default :
      return expan.sum;
      break;
  }
}


/******************************************************************************
 ***[ END OF MATHLIB.C ]*******************************************************
 ******************************************************************************/
