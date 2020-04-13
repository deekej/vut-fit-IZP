/**
 * File:          proj3.c
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
 * Description:   This program provides operations above matrices, e.g.
 *                2 matrices addition, multiplication or matrix transposition
 *                etc. For more info see the help page of the program (-h).             
 *
 * More info @:
 *      https://www.fit.vutbr.cz/study/courses/IZP/private/projekty/proj3.html
 *
 * Encoding:      en_US.utf8 (United States)
 * Compiler used: gcc 4.5.2 (Ubuntu/Linaro 4.5.2-8ubuntu4)
 */

/******************************************************************************
 ***[ START OF PROJ3.C ]*******************************************************
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
 ~~~[ LOCAL DATA TYPES DECLARATIONS ]~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 ******************************************************************************/

/**
 * Strings representing Czech acronyms of possible hexagonal directions.
 */
static const char * const directs[] = {
  "Z",                      /* West. */
  "SZ",                     /* North-west. */
  "SV",                     /* North-east. */
  "V",                      /* East. */
  "JV",                     /* South-east. */
  "JZ",                     /* South-west. */
};


/**
 * Enumerate for recognizing and accessing the array with error strings below.
 */
typedef enum error {
  N_A = -1,                 /* Not defined or associated yet. */
  INVALID_ARG = 0,          /* Invalid argument. */
  NO_FILENAME,              /* No filename(s) specified. */
  CONV_FAIL,                /* Integer conversion failed. */
  UNKNOWN_DIR,              /* Unknown hexagonal direction. */

  OPEN_FAIL,                /* File opening failed. */
  ALLOC_FAIL,               /* Memory allocation failed. */

  INVALID_DIM,              /* Invalid matrix dimension. */
  FORMAT_CORR,              /* Matrix format corrupted. */
  INVALID_COOR,             /* Invalid coordinates. */
  INVALID_POW,              /* Invalid power value. */
} TE_Error;


/**
 * Array containing error strings corresponding to TE_Error enumerate.
 */
static const char * const err_str[] = {
  ": Error: Invalid argument!\n",
  ": Error: Input file(s) not specified!\n",
  ": Error: Integer value conversion failed!\n",
  ": Error: Unknown hexagonal direction!\n",

  ": Error: Failed to open file: ",
  ": Error: Memory allocation failed!\n",
  
  ": Error: Invalid matrix dimension!\n",
  ": Error: Matrix format corrupted!\n",
  ": Error: Invalid coordinates!\n",
  ": Error: Invalid value of power!\n",
};


/**
 * Enumerate representing possible operations.
 */
typedef enum operation {
  UNKNOWN = -1,             /* Not defined operation. */

  ADD = 0,                  /* 2 matrices addition. */
  CAROM,                    /* Carom ball path. */
  EXPR,                     /* A*(B^T) + A expression. */
  MULT,                     /* 2 matrices multiplication. */
  SUB,                      /* 2 matrices subtraction. */
  TEST,                     /* Matrix format testing. */
  TRANSP,                   /* Matrix transposition. */
  WATER,                    /* Water flow. */
  HELP,                     /* Help. */
} TE_Operation;


/**
 * Array containing strings of possible program arguments corresponding to
 * TE_Operation enumerate.
 */
static const char * const params[] = {
  "--add",
  "--carom",
  "--expr",
  "--mult",
  "--sub",
  "--test",
  "--trans",
  "--water",
  "-h",
};


/**
 * Structure containing informations about processed arguments.
 */
typedef struct arguments {
  TE_Operation oper;        /* Operation to be done. */

  char *file1;              /* Pointer to 1st file-name. */
  char *file2;              /* Pointer to 2nd file-name. */
  char *prg_name;           /* Outside name of actual program. */

  int row_co;               /* Row coordinate. */
  int col_co;               /* Column coordinate. */
  int power;                /* Power of carom stroke. */
  
  TE_Direction dir;         /* Direction of carom stroke. */

  TE_Error err;             /* Error representation. */

} TS_Arguments;


/******************************************************************************
 ~~~[ FUNCTIONAL PROTOTYPES OF LOCAL FUNCTIONS ]~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 ******************************************************************************/

int process_args(int argc, char *argv[], TS_Arguments *p_args);

int process_fname(int argc, char *argv[], TS_Arguments *p_args, int pos);
int process_fname2x(int argc, char *argv[], TS_Arguments *p_args, int pos);
int process_value(int argc, char *argv[], int *p_result, int pos);
int process_direct(int argc, char *argv[], TS_Arguments *p_args, int pos);

void display_help(char *prog_name);
int matr_init(TS_Matrix *p_matr, const char *fname, const char *prg_name);

int matr_test_w(TS_Arguments *p_args);
int matr_add_w(TS_Arguments *p_args);
int matr_sub_w(TS_Arguments *p_args);
int matr_transp_w(TS_Arguments *p_args);
int matr_mult_w(TS_Arguments *p_args);
int matr_expr_w(TS_Arguments *p_args);


/******************************************************************************
 ~~~[ PROGRAMS' ARGUMENTS PROCESSING FUNCTIONS ]~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 ******************************************************************************/

/**
 * Processes arguments used when calling of the program was performed. Result
 * is stored in given TS_Arguments structure. Function returns EXIT_FAILURE,
 * if any argument procession failed etc.
 */
int process_args(int argc, char *argv[], TS_Arguments *p_args)
{
  /* TS_Arguments values initialization. */
  p_args->oper = UNKNOWN;
  p_args->file1 = NULL;
  p_args->file2 = NULL;
  p_args->prg_name = argv[0];
  p_args->row_co = -1;
  p_args->col_co = -1;
  p_args->power = -1;
  p_args->dir = N_D;
  p_args->err = N_A;

  /* No argument used - display help page. */
  if (argc == 1) {
    p_args->oper = HELP;
    return EXIT_SUCCESS;
  }

  
  TE_Operation op;
  int cmp_res;
  
  /* Comparing 1st argument with allowed arguments. */
  for (op = ADD, cmp_res = 1; cmp_res > 0 && op <= HELP; op++)
    cmp_res = strcmp(argv[1], params[op]);

  /* Correct argument used? */
  if (cmp_res != 0) {
    p_args->err = INVALID_ARG;
    return EXIT_FAILURE;
  }
  else
    /* FOR cycle above has overrun by 1. */
    p_args->oper = --op;


  /*
   * Processing next arguments correspondingly to 1st argument.
   */
  switch (p_args->oper) {

    /* -h used, no more processing needed. */
    case HELP :
      return EXIT_SUCCESS;

    /* --test or --trans used, only 1 filename is to be read. */
    case TEST :
    case TRANSP :
      return process_fname(argc, argv, p_args, 2);

    /* --add, --sub, --mult or --expr used, 2 filenames are to be read. */
    case ADD :
    case SUB :
    case MULT :
    case EXPR :
      return process_fname2x(argc, argv, p_args, 2);

    /* --water used. 2 int values and 1 filename are to be processed. */
    case WATER :
      if (process_value(argc, argv, &p_args->row_co, 2) == EXIT_FAILURE
          || process_value(argc, argv, &p_args->col_co, 3) == EXIT_FAILURE) {
        p_args->err = CONV_FAIL;
        return EXIT_FAILURE;
      }

      return process_fname(argc, argv, p_args, 4);

    /*
     * --carom used. 2 int values, one string representing direction, another
     * int value and 1 filename are to be processed.
     */
    case CAROM :
      if (process_value(argc, argv, &p_args->row_co, 2) == EXIT_FAILURE
          || process_value(argc, argv, &p_args->col_co, 3) == EXIT_FAILURE) {
        p_args->err = CONV_FAIL;
        return EXIT_FAILURE;
      }
      
      if (process_direct(argc, argv, p_args, 4) == EXIT_FAILURE)
        return EXIT_FAILURE;
      
      if (process_value(argc, argv, &p_args->power, 5) == EXIT_FAILURE) {
        p_args->err = CONV_FAIL;
        return EXIT_FAILURE;
      }
      
      return process_fname(argc, argv, p_args, 6);

    /* Default only to get rid of warning. This case should never happen!!! */
    default :
      return EXIT_FAILURE;
  }
}


/**
 * Processes one string representing filename, if it was used.
 */
int process_fname(int argc, char *argv[], TS_Arguments *p_args, int pos)
{
  /* Existing string? (-1 is used because of array indexing.) */
  if (argc - 1 != pos) {
    /* No filename or excessive argument. */
    p_args->err = (argc - 1 > pos) ? INVALID_ARG : NO_FILENAME;
    return EXIT_FAILURE;
  }
  else
    p_args->file1 = argv[pos];

  return EXIT_SUCCESS;
}


/**
 * Processes two strings representing two filenames, if they were used.
 */
int process_fname2x(int argc, char *argv[], TS_Arguments *p_args, int pos)
{
  /* Existing strings? (-1 is used because of array indexing.) */
  if (argc - 1 != pos + 1) {
    /* No filename or excessive argument. */
    p_args->err = (argc - 1 > pos + 1) ? INVALID_ARG : NO_FILENAME;
    return EXIT_FAILURE;
  }
  else {  
    p_args->file1 = argv[pos];
    p_args->file2 = argv[pos + 1];
  }

  return EXIT_SUCCESS;
}


/**
 * Processes string representing value using strtol(). Returns EXIT_FAILURE
 * upon error.
 */
int process_value(int argc, char *argv[], int *p_result, int pos)
{
  /* Existing string? (-1 is used because of array indexing.) */
  if (argc - 1 < pos)
    return EXIT_FAILURE;

  errno = 0;

  char *endptr = NULL;
  long ret_val = strtoul(argv[pos], &endptr, 10);

  /* Test for correct conversion. */
  if (errno != 0 || *endptr != '\0' || ret_val < INT_MIN || ret_val > INT_MAX)
    return EXIT_FAILURE;
  else
    *p_result = (int) ret_val;
  
  return EXIT_SUCCESS;
}


/**
 * Processes the string representing direction of ball.
 */
int process_direct(int argc, char *argv[], TS_Arguments *p_args, int pos)
{
  /* Existing string? (-1 is used because of array indexing.) */
  if (argc - 1 < pos) {
    p_args->err = UNKNOWN_DIR;
    return EXIT_FAILURE;
  }

  TE_Direction dir;
  int cmp_res;

  /* Comparing string with possible directions. */
  for (dir = W, cmp_res = 1; dir <= SW && cmp_res != 0; dir++)
    cmp_res = strcmp(argv[pos], directs[dir]);

  /* Correct direction? */
  if (cmp_res != 0) {
    p_args->err = UNKNOWN_DIR;
    return EXIT_FAILURE;
  }
  else
    /* FOR cycle above has overrun by 1. */
    p_args->dir = --dir;

  return EXIT_SUCCESS;
}


/******************************************************************************
 ~~~[ AUXILIARY FUNCTIONS ]~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 ******************************************************************************/

/**
 * Displays the help page of the program.
 */
void display_help(char *prg_name)
{
  fprintf(stdout,
  "Usage: %s [OPTION]... [FILE]...\n"
  "\n"
  "Provides basic matrix operations. The input matrices are read from specified"
  "\n"
  "file, the output is to stdout.\n"
  "\n"
  "  --test   [FILE]                        matrix format check\n"
  "  --trans  [FILE]                        matrix transposition\n"
  "  --add    [FILE] [FILE]                 addition of 2 matrices\n"
  "  --sub    [FILE] [FILE]                 subtraction of 2 matrices\n"
  "  --mult   [FILE] [FILE]                 multiplication of 2 matrices\n"
  "  --expr   [FILE] [FILE]                 (A * (B^T) + A) matrix expression\n"
  "  --water  [CO] [CO] [FILE]              water flow in high-altitude model\n"
  "  --carom  [CO] [CO] [DIR] [POW] [FILE]  carom ball path\n"
  "      -h                                 displays this help page and exit\n"
  "\n"
  "Other arguments specification:\n"
  "  [FILE]  -  input filename\n"
  "  [CO]    -  coordinate\n"
  "  [DIR]   -  direction of the ball (Z, SZ, SV, V, JV, JZ)\n"
  "  [POW]   -  power of stroke to ball\n"
  "\n"
  "Running program without any option is equal to -h.\n"
  "\n"
  "For more detail information about programs' use, visit web page below:\n"
  "__________________________________________________________________________\n"
  "\n"
  "Author:                    David Kaspar (aka Dee'Kej)\n"
  "                           FIT VUT Brno, 1BIT\n"
  "Report bugs to:            xkaspa34@stud.fit.vutbr.cz\n"
  "More info @:\n"
  "    https://www.fit.vutbr.cz/study/courses/IZP/private/projekty/proj3.html\n"
  "\n"
  "Version:                   0.7 alpha\n"
  "Last update:               12-04-2011\n"
  "Date of compilation:       %s (%s)\n"
  , prg_name, __DATE__, __TIME__);

  return;
}

/**
 * This is a wrapper function for complete matrix initialization. It also
 * allocates memory for the matrix, so upon successful return one matrix free
 * is needed!
 */
int matr_init(TS_Matrix *p_matr, const char *fname, const char *prg_name)
{
  /* Rerouting the stdin stream from the given file. */
  if (freopen(fname, "r", stdin) == NULL) {
    fprintf(stderr, "%s%s%s\n", prg_name, err_str[OPEN_FAIL], fname);
    return EXIT_FAILURE;
  }
  
  /* Matrix dimensions reading. */
  if (matr_read_dim(p_matr) == EXIT_FAILURE) {
    fprintf(stderr, "%s%s", prg_name, err_str[INVALID_DIM]);
    return EXIT_FAILURE;
  }

  /* Memory allocating for the memory. */
  if (matr_alloc(p_matr) == EXIT_FAILURE) {
    fprintf(stderr, "%s%s", prg_name, err_str[ALLOC_FAIL]);
    return EXIT_FAILURE;
  }
  
  /* Filling the matrix with the values. */
  if (matr_read_in(p_matr) == EXIT_FAILURE) {
    matr_free(p_matr);
    fprintf(stderr, "%s%s", prg_name, err_str[FORMAT_CORR]);
    return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;
}


/******************************************************************************
 ~~~[ PRIMARY FUNCTIONS ]~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 ******************************************************************************/

/**
 * Checks if the data inside the matrix are correct. Then prints the matrix 
 * to stdout correctly formated, if there was no error.
 */
int matr_test_w(TS_Arguments *p_args)
{
  TS_Matrix matr;

  /* Initialization. */
  if (matr_init(&matr, p_args->file1, p_args->prg_name) == EXIT_FAILURE)
    return EXIT_FAILURE;

  matr_print(&matr);
  matr_free(&matr);

  return EXIT_SUCCESS;
}


/**
 * Wrapper function for addition of 2 matrices.
 */
int matr_add_w(TS_Arguments *p_args)
{
  TS_Matrix matr1, matr2;

  /* 1st matrix initialization. */
  if (matr_init(&matr1, p_args->file1, p_args->prg_name) == EXIT_FAILURE)
    return EXIT_FAILURE;

  /* 2nd matrix initialization. */
  if (matr_init(&matr2, p_args->file2, p_args->prg_name) == EXIT_FAILURE) {
    matr_free(&matr1);
    return EXIT_FAILURE;
  }
  
  int ret_val;

  /* Addition. If the dimensions are not same, then prints false. */
  if ((ret_val = matr_add(&matr1, &matr2, &matr1)) == EXIT_FAILURE)
    fprintf(stdout, "false\n");
  else
    matr_print(&matr1);
  
  matr_free(&matr2);
  matr_free(&matr1);

  return ret_val;
}


/**
 * Wrapper function for subtraction of 2 matrices.
 */
int matr_sub_w(TS_Arguments *p_args)
{
  TS_Matrix matr1, matr2;

  /* 1st matrix initialization. */
  if (matr_init(&matr1, p_args->file1, p_args->prg_name) == EXIT_FAILURE)
    return EXIT_FAILURE;

  /* 2nd matrix initialization. */
  if (matr_init(&matr2, p_args->file2, p_args->prg_name) == EXIT_FAILURE) {
    matr_free(&matr1);
    return EXIT_FAILURE;
  }
  
  int ret_val;

  /* Subtraction. If the dimensions are not same, then prints false. */
  if ((ret_val = matr_sub(&matr1, &matr2, &matr1)) == EXIT_FAILURE)
    fprintf(stdout, "false\n");
  else
    matr_print(&matr1);
  
  matr_free(&matr2);
  matr_free(&matr1);

  return ret_val;
}


/**
 * Reads matrix and makes transposition of it.
 */
int matr_transp_w(TS_Arguments *p_args)
{
  TS_Matrix matr;

  /* Initialization. */
  if (matr_init(&matr, p_args->file1, p_args->prg_name) == EXIT_FAILURE)
    return EXIT_FAILURE;
  
  /* Transposition. */
  if (matr_transp(&matr) == EXIT_FAILURE) {
    matr_free(&matr);
    fprintf(stderr, "%s%s", p_args->prg_name, err_str[ALLOC_FAIL]);
    return EXIT_FAILURE;
  }

  matr_print(&matr);
  matr_free(&matr);

  return EXIT_SUCCESS;
}


/**
 * Wrapper function for multiplication of 2 matrices.
 */
int matr_mult_w(TS_Arguments *p_args)
{
  TS_Matrix matr1, matr2, matr3;

  /* 1st matrix initialization. */
  if (matr_init(&matr1, p_args->file1, p_args->prg_name) == EXIT_FAILURE)
    return EXIT_FAILURE;

  /* 2nd matrix initialization. */
  if (matr_init(&matr2, p_args->file2, p_args->prg_name) == EXIT_FAILURE) {
    matr_free(&matr1);
    return EXIT_FAILURE;
  }
  
  /* Dimensions copy for 3rd matrix. */ 
  matr3.row_num = matr1.row_num;
  matr3.col_num = matr2.col_num;

  /* Allocation of 3rd matrix. */
  if (matr_alloc(&matr3) == EXIT_FAILURE) {
    matr_free(&matr2);
    matr_free(&matr1);
    fprintf(stderr, "%s%s", p_args->prg_name, err_str[ALLOC_FAIL]);
    return EXIT_FAILURE;
  }

  int ret_val;

  /* Multiplication. If the dimensions are incorrect, then prints false. */
  if ((ret_val = matr_mult(&matr1, &matr2, &matr3)) == EXIT_FAILURE)
    fprintf(stdout, "false\n");
  else
    matr_print(&matr3);
  
  matr_free(&matr3);
  matr_free(&matr2);
  matr_free(&matr1);

  return ret_val;
}


/**
 * Wrapper function for A*(B^T) + A matrix expression.
 */
int matr_expr_w(TS_Arguments *p_args)
{
  TS_Matrix matr1, matr2, matr3;

  /* 1st matrix initialization. */
  if (matr_init(&matr1, p_args->file1, p_args->prg_name) == EXIT_FAILURE)
    return EXIT_FAILURE;

  /* 2nd matrix initialization. */
  if (matr_init(&matr2, p_args->file2, p_args->prg_name) == EXIT_FAILURE) {
    matr_free(&matr1);
    return EXIT_FAILURE;
  }

  /* Transposition of 2nd matrix. */
  if (matr_transp(&matr2) == EXIT_FAILURE) {
    matr_free(&matr2);
    matr_free(&matr1);
    fprintf(stderr, "%s%s", p_args->prg_name, err_str[ALLOC_FAIL]);
    return EXIT_FAILURE;
  }
  
  /* Dimensions copy for 3rd matrix. */ 
  matr3.row_num = matr1.row_num;
  matr3.col_num = matr2.col_num;

  /* Allocation of 3rd matrix. */
  if (matr_alloc(&matr3) == EXIT_FAILURE) {
    matr_free(&matr2);
    matr_free(&matr1);
    fprintf(stderr, "%s%s", p_args->prg_name, err_str[ALLOC_FAIL]);
    return EXIT_FAILURE;
  }

  /* Multiplication. If the dimensions are incorrect, then prints false. */
  if (matr_mult(&matr1, &matr2, &matr3) == EXIT_FAILURE) {
    matr_free(&matr3);
    matr_free(&matr2);
    matr_free(&matr1);
    fprintf(stdout, "false\n");
    return EXIT_FAILURE;
  }

  /* 2nd matrix is not needed anymore. */
  matr_free(&matr2);

  int ret_val;

  /* Addition. If the dimensions are not the same, then prints false. */
  if ((ret_val = matr_add(&matr3, &matr1, &matr3)) == EXIT_FAILURE)
    fprintf(stdout, "false\n");
  else
    matr_print(&matr3);

  matr_free(&matr3);
  matr_free(&matr1);

  return ret_val;
}


/******************************************************************************
 ~~~[ MAIN FUNCTION ]~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 ******************************************************************************/

int main(int argc, char *argv[])
{
  TS_Arguments args;
  
  /* Program arguments processing. */
  if (process_args(argc, argv, &args) == EXIT_FAILURE) {
    fprintf(stderr, "%s%s", argv[0], err_str[args.err]);
    return EXIT_FAILURE;
  }

  /* Calling of appropriate function. */
  switch (args.oper) {
    case ADD :
      return matr_add_w(&args);

    case CAROM :
      fprintf(stderr, "%s: Error: Carambole not implemented!\n", argv[0]);
      return EXIT_FAILURE;

    case EXPR :
      return matr_expr_w(&args);
    
    case MULT :
      return matr_mult_w(&args);

    case SUB :
      return matr_sub_w(&args);

    case TEST :
      return matr_test_w(&args);

    case TRANSP :
      return matr_transp_w(&args);

    case WATER :
      fprintf(stderr, "%s: Error: Watercourse not implemented!\n", argv[0]);
      return EXIT_FAILURE;

    case HELP :
      display_help(argv[0]);
      return (argc == 1) ? EXIT_FAILURE : EXIT_SUCCESS;

    default :
      return EXIT_FAILURE;
  }
}


/******************************************************************************
 ***[ END OF PROJ3.C ]*********************************************************
 ******************************************************************************/
 
