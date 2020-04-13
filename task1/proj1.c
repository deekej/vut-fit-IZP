/**
 * File:          proj1.c
 * Version:       1.2.1 final
 * Date:          4-10-2011
 * Last update:   28-10-2011
 *
 * Course:        IZP (winter semester, 2011)
 * Project:       #1 - Time entry conversion
 *
 * Author:        David Kaspar (aka Dee'Kej), 1BIT
 * Faculty:       Faculty of Information Technologies,
 *                Brno University of Technologies
 * E-mail:        xkaspa34@stud.fit.vutbr.cz
 *
 * Description:   Program processes time value(s) to human readable form. It
 *                reads time value from stdin which represents number of seconds
 *                and then prints the same information in text form as a number
 *                of weeks, days, hours, minutes and seconds. For more info see
 *                the link below or display the help page of the program by
 *                running program with --help argument.
 *
 *                NOTE: Program uses proj1timeconv module for transformations.
 * More info @:
 *      https://www.fit.vutbr.cz/study/courses/IZP/private/projekty/proj1.html
 *
 * Encoding:      en_US.utf8 (United States)
 * Compiler used: gcc 4.5.2 (Ubuntu/Linaro 4.5.2-8ubuntu4)
 */


/******************************************************************************
 ~~~[ START OF PROJ1.C ]~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 ******************************************************************************/

/******************************************************************************
 ~~~[ HEADER FILES ]~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 ******************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <limits.h>
#include <errno.h>
#include <ctype.h>
#include <string.h>

#include "proj1timeconv.h"


/******************************************************************************
 ~~~[ DATA TYPES DECLARATIONS ]~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 ******************************************************************************/

/*
 * Structure containing informations about arguments used when the calling of
 * the program was performed. It uses enumerate type defined in proj1timeconv.h
 * file.
 */
typedef struct arguments {
  TE_Max_unit max_unit;   /* Maximum time unit. */
  bool extra;             /* Argument --extra? */
  
  char *p_fname_r;        /* Pointer to filename to read from. */
  char *p_fname_w;        /* Pointer to filename to write into. */
  char *p_fname_a;        /* Pointer to filename to append to it. */
} TS_Arguments;


/******************************************************************************
 ~~~[ FUNCTIONAL PROTOTYPES OF LOCAL FUNCTIONS ]~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 ******************************************************************************/

int getargs(int argc, char *argv[], TS_Arguments *p_arg);

int process_sequence(TE_Max_unit max_unit);

void print_help(char *prog_name);


/******************************************************************************
 ~~~[ DEFINITIONS OF LOCAL FUNCTIONS]~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 ******************************************************************************/

/**
 * Function processes calling arguments of the program. Returns EXIT_SUCCESS
 * upon successful processing or EXIT_FAILURE if there was an invalid or too
 * many arguments. However --extra argument can repeat itself. Arguments order
 * doesn't matter. The processed arguments are stored in given TS_Arguments
 * structure. 
 */
int getargs(int argc, char *argv[], TS_Arguments *p_args)
{
  /*
   * Initialization of pointers which are used in conditions evaluation below.
   */
  p_args->p_fname_r = NULL;
  p_args->p_fname_w = NULL;
  p_args->p_fname_a = NULL;

  /* No extra used yet - initialization for portability. */
  p_args->extra = false;

  if (argc == 1) {      /* Setting default value when no arguments were used. */
    p_args->max_unit = WEEKS;
  }
  else {                /* Processing of used argument(s). */

    p_args->max_unit = NOT_DEF;   /* Initialization of max_unit. */

    int i = 1;          /* Iteration counter.*/


    /*
     * Processing arguments one by one.
     */
    while (i < argc) {
     
      if ((strcmp("--extra", argv[i])) == 0) {    /* Optional EXTRA argument. */
        p_args->extra = true;
      }
      else if ((strcmp("-fr", argv[i])) == 0 && p_args->p_fname_r == NULL
               && (i + 1) < argc) {               /* Optional input file. */
        i++;
        p_args->p_fname_r = argv[i];
      }
      else if ((strcmp("-fw", argv[i])) == 0 && p_args->p_fname_w == NULL
               && (i + 1) < argc){                /* Optional output file. */
        i++;
        p_args->p_fname_w = argv[i];
      }
      else if ((strcmp("-fa", argv[i])) == 0 && p_args->p_fname_w == NULL
               && (i + 1) < argc){                /* Optional output file. */
        i++;
        p_args->p_fname_a = argv[i];
      }
      else if (p_args->max_unit == NOT_DEF) {    /* No assigned argument yet. */

        if ((strcmp("--help", argv[i])) == 0) {   /* HELP argument */
          p_args->max_unit = HELP;
        }
        else if ((strcmp("-s", argv[i])) == 0) {  /* SECONDS argument */
          p_args->max_unit = SECONDS;
        }
        else if ((strcmp("-m", argv[i])) == 0) {  /* MINUTES argument */
          p_args->max_unit = MINUTES;
        }
        else if ((strcmp("-h", argv[i])) == 0) {  /* HOURS argument */
          p_args->max_unit = HOURS;
        }
        else if ((strcmp("-d", argv[i])) == 0) {  /* DAYS arguments */
          p_args->max_unit = DAYS;
        }
        else if ((strcmp("-t", argv[i])) == 0) {  /* WEEKS arguments */
          p_args->max_unit = WEEKS;
        }
        else  {                                   /* WRONG arguments */
          errno = EINVAL;
          return EXIT_FAILURE;
        }

      }
      else {                                /* Repeated or invalid argument. */
        errno = EINVAL;
        return EXIT_FAILURE;
      }
      
      i++;  /* Iteration for another argument. */
    }
    
    /*
     * If there was no --extra argument and any of the pointer are not NULL,
     * then sets errno, because file I/O can be used only with --extra argument.
     */
    if (p_args->extra == false && (p_args->p_fname_r != NULL ||
        p_args->p_fname_w != NULL || p_args->p_fname_a != NULL)) {
      errno = EINVAL;
      return EXIT_FAILURE;
    }

    /*
     * Testing for multiple use of file output argument. (File cannot be opened
     * multiple times!).
     */
    if (p_args->p_fname_w != NULL && p_args->p_fname_a != NULL) {
      errno = EINVAL;
      return EXIT_FAILURE;
    }

    /* Testing for only EXTRA argument. If so, then sets to default value. */
    if (p_args->max_unit == NOT_DEF && p_args->extra == true)
      p_args->max_unit = WEEKS;
  }

  return EXIT_SUCCESS;
}


/**
 * Function processes possible endless sequence of numbers from stdin and prints
 * processed values to stdout, if there was no error.
 */
int process_sequence(TE_Max_unit max_unit)
{
  TS_Time time;             /* Structure for saving processed value of time. */

  unsigned long seconds;    /* Variable for storing value of input number. */

  int ret_val;              /* Return value of called function. */


  /*
   * Processes the time value and prints it to stdout, if there was no error
   * while transforming the number to value.
   */
  while ((ret_val = readtoul(&seconds)) == EXIT_SUCCESS) {

    time = process_time_value(seconds, max_unit);

    print_time_value(&time);
  }

  if (ret_val == EOF)       /* No error while processing a value from stdin. */
    return EXIT_SUCCESS;
  else
    return EXIT_FAILURE;    /* An error occurred, the errno is set. */
}


/**
 * Displays help page about how to use this program.
 */
void print_help(char *prog_name)
{
  fprintf(stdout,
  "Usage: %s [OPTION]... \n"
  "\n"
  "Transforms NUMBER of seconds to human-readable form from standard input to\n"
  "standard output. The output is in Czech language. The type of NUMBER is\n"
  "unsigned long integer.\n"
  "\n"
  "  -s                       maximal displayed units - seconds\n"
  "  -m                       maximal displayed units - minutes\n"
  "  -h                       maximal displayed units - hours\n"
  "  -d                       maximal displayed units - days\n"
  "  -t                       maximal displayed units - weeks\n"
  "      --help               displays this help and exit\n"
  "\n"
  "Running program without any option is equal to -t.\n"
  "\n"
  "Options below requires --extra to be used, otherwise it will be considered\n"
  "as a wrong parameter has been used.\n"
  "  -fr [FILE]               reads input from FILE\n"
  "  -fw [FILE]               writes output to FILE, overwrites existing file\n"
  "  -fa [FILE]               appends output to FILE, do not overwrites\n"
  "      --extra              enables processing of possible endless sequence\n"
  "                           of input lines\n"
  "\n"
  "Option --extra can be combined with one [ -s | -m | -h | -d | -t ] option\n"
  "for specifying the maximal displayed units when using --extra.\n"
  "\n"
  "Examples:\n"
  "  %s --extra -d\n"
  "      Outputs' maximal displayed unit will be days, program will process\n"
  "      sequence of input lines.\n"
  "\n"
  "  %s --extra -fa FILE\n"
  "      Outputs' maximal displayed unit will be weeks, output will be append\n"
  "      to FILE, if it exists. If FILE doesn't exists, than the FILE will be\n"
  "      created. Program will also process sequence of input lines.\n"
  "__________________________________________________________________________\n"
  "\n"
  "Author:                    David Kaspar (aka Dee'Kej)\n"
  "                           FIT VUT Brno, 1BIT\n"
  "Report bugs to:            xkaspa34@stud.fit.vutbr.cz\n"
  "More info @:\n"
  "    https://www.fit.vutbr.cz/study/courses/IZP/private/projekty/proj1.html\n"
  "\n"
  "Version:                   1.2.1 final\n"
  "Last update:               10-28-2011\n"
  "Date of compilation:       %s (%s)\n"
  , prog_name, prog_name, prog_name, __DATE__, __TIME__);

  return;
}


/******************************************************************************
 ~~~[ MAIN FUNCTION ]~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 ******************************************************************************/

int main(int argc, char *argv[])
{
  TS_Arguments args;

  /*
   * Processing of arguments used when calling of the program was performed.
   */
  if (getargs(argc, argv, &args) == EXIT_FAILURE) {
    perror("proj1: Error");
    return EXIT_FAILURE;
  }
  
  if (args.max_unit == HELP) {        /* Printing only the help page. */
    print_help(argv[0]);
    return EXIT_SUCCESS;
  }


  /*
   * Processes only one value for standard calling of the program and unlimited 
   * number of values when --extra argument was used.
   */
  if (args.extra == false) {
    
    unsigned long seconds;

    int ret_val = readtoul(&seconds);

    /*
     * Jumping to proper section by return value of the function readtoul().
     */
    switch (ret_val) {
      
      /* No character was read. */ 
      case EOF :
        break;
      
      /* Error occurred. Displaying the error message. */
      case EXIT_FAILURE :
        perror("proj1: Error");
        return EXIT_FAILURE;
      
      /* No error detected. Proceeding with value processing & printing. */
      case EXIT_SUCCESS : {
        TS_Time time;
        time = process_time_value(seconds, args.max_unit);
        print_time_value(&time);
        break;
      }
      
      /* Wrong return value of readtoul(), terminating of the program. */
      default :
        fprintf(stderr, "proj1: Internal error: Not defined behaviour\n");
        return EXIT_FAILURE;
        break;
    }

  }
  else {
    
    /* Redirecting input stream from a given file name. */
    if (args.p_fname_r != NULL
        && freopen(args.p_fname_r, "r", stdin) == NULL) {
      perror("proj1: Error");
      return EXIT_FAILURE;      
    }
    
    /* Redirecting output stream to a file of a given name (write mode). */ 
    if (args.p_fname_w != NULL
        && freopen(args.p_fname_w, "w", stdout) == NULL) {
      perror("proj1: Error");
      return EXIT_FAILURE;
    }
    
    /*
     * Redirecting output stream to a file of a given name (append mode).
     * Multi-opening of the file shouldn't occur! (See getargs() function for
     * more info.) However, if it should occur, then appending to an open file
     * should be safe enough.
     */
    if (args.p_fname_a != NULL
        && freopen(args.p_fname_a, "a", stdout) == NULL) {
      perror("proj1: Error");
      return EXIT_FAILURE;
    }
    
    /*
     * Processing possible endless sequence of input values by calling
     * process_sequence() function. For more information see the description of
     * the process_sequence() function.
     */
    if (process_sequence(args.max_unit) == EXIT_FAILURE) {
      perror("proj1: Error");
      return EXIT_FAILURE;
    }

  }
  
  return EXIT_SUCCESS;
}


/******************************************************************************
 ***[END OF PROJ1.C]***********************************************************
 ******************************************************************************/
 
