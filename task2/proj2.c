/**
 * File:          proj2.c
 * Version:       1.0 beta
 * Date:          1-11-2011
 * Last update:   20-11-2011
 *
 * Course:        IZP (winter semester, 2011)
 * Project:       #2 - Iterative computations
 *
 * Author:        David Kaspar (aka Dee'Kej), 1BIT
 * Faculty:       Faculty of Information Technologies,
 *                Brno University of Technologies
 * E-mail:        xkaspa34@stud.fit.vutbr.cz
 *
 * Description:   This is the main module for proj2 program containing main
 *                function and other functions for processing and handling
 *                parameters and more.                
 *
 * More info @:
 *      https://www.fit.vutbr.cz/study/courses/IZP/private/projekty/proj2.html
 *
 * Encoding:      en_US.utf8 (United States)
 * Compiler used: gcc 4.5.2 (Ubuntu/Linaro 4.5.2-8ubuntu4)
 */

/******************************************************************************
 ***[START OF PROJ2.C]*********************************************************
 ******************************************************************************/

/******************************************************************************
 ~~~[ HEADER FILES ]~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 ******************************************************************************/

#include <errno.h>
#include <float.h>
#include <math.h>
#include <limits.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "proj2_mathlib.h"
#include "proj2_geomlib.h"

/******************************************************************************
 ~~~[ LOCAL DATA TYPES DECLARATIONS ]~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 ******************************************************************************/

/**
 * Enumerate containing values associated to each function for instant access
 * to field of pointers of these functions. (Sorted alphabetically.)
 */
typedef enum functions {
  UNKNOWN = -1,             /* Unknown function. */
  HELP = 0,                 /* Help function. */

  ARC_COS,                  /* Inverse cosine. */
  ARC_COT,                  /* Inverse cotangent. */
  ARC_SIN,                  /* Inverse sine. */
  ARC_TAN,                  /* Inverse tangent. */
  COS,                      /* Cosine. */
  EXP,                      /* Exponential function. */
  HYP_COS,                  /* Hyperbolical cosine. */
  HYP_COT,                  /* Hyperbolical cotangent. */
  HYP_SIN,                  /* Hyperbolical sine. */
  HYP_TAN,                  /* Hyperbolical tangent. */
  LBL,                      /* Length of broken line. */
  LBLE,                     /* Length of broken line with absolute error. */
  LN,                       /* Natural logarithm. */
  LOGAX,                    /* General logarithm. */
  SIN,                      /* Sine. */
  SQRT,                     /* Square root. */
} TE_Functions;


/* Structure containing information about calling parameters of the program. */
typedef struct arguments {
  TE_Functions func;        /* Function to be used. */

  int sigdig;               /* Required significant digits. */
  double err_val;           /* Value of absolute error. */
  double base;              /* Base of logarithm. */
} TS_Arguments;

/* Declaration of pointer to function. */
typedef double (*p_fd)(double x, ...);

/* Field of pointers to function. */
static p_fd const func_field[] = {
  [ARC_COS] = arc_cosine,
  [ARC_COT] = arc_cotangent,
  [ARC_SIN] = arc_sine,
  [ARC_TAN] = arc_tangent,
  [COS]     = cosine,
  [EXP]     = expn,
  [HYP_COS] = hyper_cosine,
  [HYP_COT] = hyper_cotangent,
  [HYP_SIN] = hyper_sine,
  [HYP_TAN] = hyper_tangent,
  [LN]      = ln,
  [SIN]     = sine,
  [SQRT]    = sqrroot,
};

/******************************************************************************
 ~~~[ FUNCTIONAL PROTOTYPES OF LOCAL FUNCTIONS ]~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 ******************************************************************************/

void print_help(char *prog_name);
int process_args(int argc, char *argv[], TS_Arguments *p_args);

int lbl_w(void);
int lble_w(double errval);

int func_caller(TS_Arguments *p_args);


/******************************************************************************
 ~~~[ SUBSIDIARY FUNCTIONS ]~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 ******************************************************************************/

/**
 * Displays help page about how to use this program.
 */
void print_help(char *prog_name)
{
  fprintf(stdout,
  "Usage: %s [OPTION]... [SPECIFIER]...\n"
  "\n"
  "This program can be used as a filter for computing values of functions "
  "below:\n"
  "\n"
  "  -h                       displays the help and exit\n"
  "      --arccos             computes value of inverse cosine\n"
  "      --arccot             computes value of inverse cotangent\n"
  "      --arcsin             computes value of inverse sine\n"
  "      --arctan             computes value of inverse tangent\n"
  "      --cos                computes value of cosine\n"
  "      --exp                computes value of exponential function\n"
  "      --hypcos             computes value of hyperbolical cosine\n"
  "      --hypcot             computes value of hyperbolical cotangent\n"
  "      --hypsin             computes value of hyperbolical sine\n"
  "      --hyptan             computes value of hyperbolical tanget\n"
  "      --lbl                computes length of broken line\n"
  "      --lble               computes length of broken line with error\n"
  "      --ln                 computes value of natural logarithm\n"
  "      --logax              computes value of general logarithm\n"
  "      --sin                computes value of sine\n"
  "      --sqrt               computes value of square root\n"
  "\n"
  "Running program without any option is equal to -h.\n"
  "\n"
  "Except the --lbl parameter, every other requires additional specifier:\n"
  "        sigdig             represents value of required significant digits\n"
  "                           (has to be in interval of 1 to DBL_DIG - usually "
  "15)\n"
  "\n"
  "        base               is required by --logax for specifying the base of"
  "\n                           general logarithm\n"
  "\n"
  "        err                specifies the absolute error value of --lble\n"
  "\n"
  "Exact order of the parameters is required, in this format:\n"
  "  [--OPTION] [SIGDIG] [BASE/ERR]\n"
  "\n"
  "Examples:\n"
  "  %s --logax 10 2.71\n"
  "      Will compute general logarithm with base of 2.71. The result should\n"
  "      have 10 significant digits same as any other function.\n"
  "\n"
  "  %s --lble 5\n"
  "      Will compute the length of broken line with absolute error of 5.\n"
  "__________________________________________________________________________\n"
  "\n"
  "Author:                    David Kaspar (aka Dee'Kej)\n"
  "                           FIT VUT Brno, 1BIT\n"
  "Report bugs to:            xkaspa34@stud.fit.vutbr.cz\n"
  "More info @:\n"
  "    https://www.fit.vutbr.cz/study/courses/IZP/private/projekty/proj2.html\n"
  "\n"
  "Version:                   1.2 RC\n"
  "Last update:               11-20-2011\n"
  "Date of compilation:       %s (%s)\n"
  , prog_name, prog_name, prog_name, __DATE__, __TIME__);

  return;
}


/**
 * Processes the calling parameters of the program.
 */
int process_args(int argc, char *argv[], TS_Arguments *p_args)
{
  /* No parameter -> display help. */
  if (argc == 1) {
    p_args->func = HELP;
    return EXIT_SUCCESS;
  }

  int i = 1;

  /* Determining what argument was used. */
  if (strcmp("-h", argv[i]) == 0)
    p_args->func = HELP;
  else if (strcmp("--arccos", argv[i]) == 0)
    p_args->func = ARC_COS;
  else if (strcmp("--arccot", argv[i]) == 0)
    p_args->func = ARC_COT;
  else if (strcmp("--arcsin", argv[i]) == 0)
    p_args->func = ARC_SIN;
  else if (strcmp("--arctan", argv[i]) == 0)
    p_args->func = ARC_TAN;
  else if (strcmp("--cos", argv[i]) == 0)
    p_args->func = COS;
  else if (strcmp("--exp", argv[i]) == 0)
    p_args->func = EXP;
  else if (strcmp("--hypcos", argv[i]) == 0)
    p_args->func = HYP_COS;
  else if (strcmp("--hypcot", argv[i]) == 0)
    p_args->func = HYP_COT;
  else if (strcmp("--hypsin", argv[i]) == 0)
    p_args->func = HYP_SIN;
  else if (strcmp("--hyptan", argv[i]) == 0)
    p_args->func = HYP_TAN;
  else if (strcmp("--lbl", argv[i]) == 0)
    p_args->func = LBL;
  else if (strcmp("--lble", argv[i]) == 0)
    p_args->func = LBLE;
  else if (strcmp("--ln", argv[i]) == 0)
    p_args->func = LN;
  else if (strcmp("--logax", argv[i]) == 0)
    p_args->func = LOGAX;
  else if (strcmp("--sin", argv[i]) == 0)
    p_args->func = SIN;
  else if (strcmp("--sqrt", argv[i]) == 0)
    p_args->func = SQRT;
  else
    p_args->func = UNKNOWN;
  
  /* Unknown (invalid) argument was used. */
  if (p_args->func == UNKNOWN) {
    errno = EINVAL;
    return EXIT_FAILURE;
  }

  i++;

  char *endptr;

  /* Sigdig procession. */ 
  if (i < argc && ((p_args->func > HELP && p_args->func < LBL)
                   || (p_args->func > LBLE && p_args->func <= SQRT))) {
    errno = 0;
    endptr = NULL;
    int base = 10;

    long ret_val = strtol(argv[i], &endptr, base);
  
    /* Wrong sigdig? */
    if (errno == ERANGE && (ret_val == LONG_MIN || ret_val == LONG_MAX))
      return EXIT_FAILURE;
    else if (errno != 0 && ret_val == 0)
      return EXIT_FAILURE;
    else if (*endptr != '\0' || ret_val < INT_MIN || ret_val > INT_MAX) {
      errno = EINVAL;
      return EXIT_FAILURE;
    }
    
    p_args->sigdig = (int) ret_val;
    i++;
  }
  
  /* Base and error value procession. */
  if (i < argc && (p_args->func == LOGAX || p_args->func == LBLE)) {
    errno = 0;
    endptr = NULL;

    double ret_val = strtod(argv[i], &endptr);

    /* Error occurred? */
    if (errno != 0)
      return EXIT_FAILURE;
    if (*endptr != '\0') {
      errno = EINVAL;
      return EXIT_FAILURE;
    }
    
    /* What was the argument? */
    if (p_args->func == LOGAX)
      p_args->base = ret_val;
    else
      p_args->err_val = ret_val;

    i++;
  }

  /* Enough arguments? */
  if (i < argc) {
    errno = EINVAL;
    p_args->func = UNKNOWN;
    return EXIT_FAILURE;
  }
  else
    return EXIT_SUCCESS;
}

/******************************************************************************
 ~~~[ WRAPPER FUNCTIONS ]~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 ******************************************************************************/

/**
 * Wrapper function for computing length of broken line. It gets value from
 * stdin, computes new value and prints it until EOF. Uses lbl() function and
 * TS_Lobl structure.
 */
int lbl_w(void)
{
  bool print_warn = true;       /* Prints warning only once. */

  TS_Lobl lobl = {              /* Initialization. */
    .sum = 0.0,
  };

  int scan_res;                 /* Return value of fscanf. */

  errno = 0;
  scan_res = fscanf(stdin, "%lf %lf", &lobl.x_act, &lobl.y_act);

  /* Rest of initialization. */
  lobl.x_bef = lobl.x_act;
  lobl.y_bef = lobl.y_act;
  
  /* Repeats if there's still something to read. */
  while (scan_res != EOF) {
  
    /* Read values handling. */
    switch (scan_res) {
      
      /* Matching failure of first input value. */
      case 0 :

        /* Skipping the values. */
        scan_res = fscanf(stdin, "%*s %*s");

        /* Wrong value == NaN (wrong input). */
        lobl.x_act = NAN;
        lobl.y_act = NAN;

        /* Warning. */
        if (print_warn == true) {
          fprintf(stderr, "proj2: warning: Some input values "
                          "are not a number!\n");
          print_warn = false;
        }
        
        break;

      /* Matching failure on second input value. */
      case 1 :
        /* Skipping the wrong value. */
        scan_res = fscanf(stdin, "%*s");

        /* Wrong value == NaN (wrong input). */
        lobl.y_act = NAN;

        /* Warning. */
        if (print_warn == true) {
          fprintf(stderr, "proj2: warning: Some input values "
                          "are not a number!\n");
          print_warn = false;
        }

        break;

      /* None of above, proceed. */
      default :
        break;
    }
    
    /* Compute new values. */
    lbl(&lobl);
    
    /* Warning upon computation error. */
    if (errno != 0 && print_warn == true) {
      fprintf(stderr, "proj2: warning: %s\n", strerror(errno));
      print_warn = false;
    }
    
    /* Print of result. */
    fprintf(stdout, "%.10e\n", lobl.sum);

    /* Reseting errno. */
    errno = 0;

    /* Get new values. */
    scan_res = fscanf(stdin, "%lf %lf", &lobl.x_act, &lobl.y_act);
  }

  /* Error upon read? */
  if (errno != 0) {
    perror("proj2");
    return EXIT_FAILURE;
  }
  else
    return EXIT_SUCCESS;
}


/**
 * Same function as lbl_w, but this uses lble() function for computing lehgth
 * of broken line with absolute error value.
 */
int lble_w(double err_val)
{
  bool print_warn = true;       /* Prints warning only once. */

  TS_Loblwe loblwe = {          /* Initialization. */
    .err_val = err_val,
    .sum_min = 0.0,
    .sum_max = 0.0,
  };

  int scan_res;                 /* Return value of fscanf. */

  errno = 0;
  scan_res = fscanf(stdin, "%lf %lf", &loblwe.x_act, &loblwe.y_act);

  /* Rest of initialization. */
  loblwe.x_bef_min = loblwe.x_act - err_val;
  loblwe.x_bef_max = loblwe.x_act + err_val;

  loblwe.y_bef_min = loblwe.y_act - err_val;
  loblwe.y_bef_max = loblwe.y_act + err_val;
  

  /* Repeats if there's still something to read. */
  while (scan_res != EOF) {

    /* Read values handling. */
    switch (scan_res) {

      /* Matching failure of first input value. */
      case 0 :

        /* Skipping the values. */
        scan_res = fscanf(stdin, "%*s %*s");

        /* Wrong value == NaN (wrong input). */
        loblwe.x_act = NAN;
        loblwe.y_act = NAN;
        
        /* Warning. */
        if (print_warn == true) {
          fprintf(stderr, "proj2: warning: Some input values "
                          "are not a number!\n");
          print_warn = false;
        }

        break;

      /* Matching failure on second input value. */
      case 1 :

        /* Skipping the wrong value. */
        scan_res = fscanf(stdin, "%*s");

        /* Wrong value == NaN (wrong input). */
        loblwe.y_act = NAN;

        /* Warning. */
        if (print_warn == true) {
          fprintf(stderr, "proj2: warning: Some input values "
                          "are not a number!\n");
          print_warn = false;
        }

        break;

      /* None of above, proceed. */
      default :

        break;
    }

    /* Compute new values. */
    lble(&loblwe);
    
    /* Warning upon computation error. */
    if (errno != 0 && print_warn == true) {
      fprintf(stderr, "proj2: warning: %s\n", strerror(errno));
      print_warn = false;
    }
    
    /* Print of results. */
    fprintf(stdout, "%.10e\n", loblwe.sum_min);
    fprintf(stdout, "%.10e\n", loblwe.sum_max);

    /* Reseting errno. */
    errno = 0;

    /* Get new value. */
    scan_res = fscanf(stdin, "%lf %lf", &loblwe.x_act, &loblwe.y_act);
  }
  
  /* Error upon read? */
  if (errno != 0) {
    perror("proj2");
    return EXIT_FAILURE;
  }
  else
    return EXIT_SUCCESS;
}


/*
 * Wrapper function for logax. Gets value and prints result in a loop until end
 * of file.
 */
int logax_w(double base, int sigdig)
{
  double x;                             /* Input value. */
  int scan_res;                         /* Scan result. */

  errno = 0;                            /* Reseting errno. */

  scan_res = fscanf(stdin, "%lf", &x);
  
  /* Loop until end of input or error. */
  while (scan_res != EOF) {

    if (scan_res == 1) {
      /* Correct value loaded. */
      errno = 0;
      fprintf(stdout, "%.10e\n", logax(x, base, sigdig));

      if (errno != 0)
        fprintf(stderr, "proj2: warning: %s\n", strerror(errno));
    }
    else {
      /* Invalid value loaded. */
      fprintf(stdout, "%.10e\n", NAN);

      /* Skip to next value. */
      scan_res = fscanf(stdin, "%*s");
    }
    
    errno = 0;
    scan_res = fscanf(stdin, "%lf", &x);
  }
  
  /* Error upon read? */
  if (errno != 0) {
    perror("proj2");
    return EXIT_FAILURE;
  }
  else
    return EXIT_SUCCESS;
}
/******************************************************************************
 ~~~[ PRIMARY FUNCTIONS ]~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 ******************************************************************************/

/**
 * General function which calls appropriate required function.
 */
int func_caller(TS_Arguments *p_args)
{
  switch (p_args->func) {

    /* This shouldn't happen. */
    case HELP :
      return EXIT_FAILURE;

    /* lbl() */
    case LBL :
      return lbl_w();
    
    /* lble() */
    case LBLE :
      return lble_w(p_args->err_val);

    /* General logarithm. */
    case LOGAX :
      return logax_w(p_args->base, p_args->sigdig);

    /* None of above, choose function from function field. */
    default :
      break;
  }
  
  double x;                             /* Input value. */
  int scan_res;                         /* Scan result. */

  errno = 0;                            /* Reseting errno. */

  scan_res = fscanf(stdin, "%lf", &x);
  
  /* Loop until end of input or error. */
  while (scan_res != EOF) {

    if (scan_res == 1) {
      /* Correct value loaded. */
      errno = 0;
      fprintf(stdout, "%.10e\n", func_field[p_args->func](x, p_args->sigdig));

      if (errno != 0)
        fprintf(stderr, "proj2: warning: %s\n", strerror(errno));
    }
    else {
      /* Invalid value loaded. */
      fprintf(stdout, "%.10e\n", NAN);

      /* Skip to next value. */
      scan_res = fscanf(stdin, "%*s");
    }
    
    errno = 0;
    scan_res = fscanf(stdin, "%lf", &x);
  }
  
  /* Error upon read? */
  if (errno != 0) {
    perror("proj2");
    return EXIT_FAILURE;
  }
  else
    return EXIT_SUCCESS;
}

/******************************************************************************
 ~~~[ MAIN FUNCTION ]~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 ******************************************************************************/

int main(int argc, char *argv[])
{
  /* Initialization for backward testing. */
  TS_Arguments args = {
    .sigdig = 0.0,
    .err_val = -1.0,
    .base = 0.0,
  };

  /* Correct calling parameters? */
  if (process_args(argc, argv, &args) == EXIT_FAILURE) {
    perror("proj2: Error");
    return EXIT_FAILURE;
  }

  /* Display help? */
  if (args.func == HELP) {
    print_help(argv[0]);
    return (argc == 1) ? EXIT_FAILURE : EXIT_SUCCESS;
  }

  /* Sigdig testing. */
  if ((args.func > HELP && args.func < LBL)
      || (args.func > LBLE && args.func <= SQRT))
    if (args.sigdig < 1 || args.sigdig > DBL_DIG) {
      errno = EINVAL;
      perror("proj2: Error");
      return EXIT_FAILURE;
    }

  /* Base of logarithm testing. */
  if (args.func == LOGAX)
    if (fpclassify(args.base) == FP_ZERO || fpclassify(args.base) == FP_INFINITE
        || fpclassify(args.base) == FP_NAN || signbit(args.base)
        || args.base == 1.0) {
      errno = EDOM;
      perror("proj2: Error");
      return EXIT_FAILURE;
    }

  /* Correct error? */
  if (args.func == LBLE && signbit(args.err_val)) {
    errno = EINVAL;
    perror("proj2: Error");
    return EXIT_FAILURE;
  }

  /* Call required function and end program after it ends. */
  return func_caller(&args);
}
/******************************************************************************
 ***[END OF PROJ2.C]***********************************************************
 ******************************************************************************/
