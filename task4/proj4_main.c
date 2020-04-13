/**
 * File:          proj4_main.c
 * Version:       1.2 Final
 * Date:          07-12-2011
 * Last update:   18-12-2011
 *
 * Course:        IZP (winter semester, 2011)
 * Project:       #4 - Czech sorting
 *
 * Author:        David Kaspar (aka Dee'Kej), 1BIT
 * Faculty:       Faculty of Information Technologies,
 *                Brno University of Technologies
 * E-mail:        xkaspa34@stud.fit.vutbr.cz
 *
 * Description:   This program reads lines from given file and saves them sorted
 *                in the output file. Sorting is done by collation of Czech
 *                strings corresponding to CSN_97_6030. Program supports various
 *                encodings of the file. For more info see the help page of
 *                the program (-h or --help) or the web page below.
 *
 * More info @:
 *      https://www.fit.vutbr.cz/study/courses/IZP/private/projekty/proj4.html
 *
 * File encoding: en_US.utf8 (United States)
 * Compiler used: gcc 4.5.2 (Ubuntu/Linaro 4.5.2-8ubuntu4)
 */


/******************************************************************************
 ***[ START OF PROJ4_MAIN.C ]**************************************************
 ******************************************************************************/


/******************************************************************************
 ~~~[ HEADER FILES ]~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 ******************************************************************************/

#include <errno.h>
#include <locale.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <wchar.h>

#include "proj4_linkedlist.h"


/******************************************************************************
 ~~~[ LOCAL DATA TYPES DECLARATIONS ]~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 ******************************************************************************/

/**
 * Enumerate representing error which has occurred.
 */
typedef enum err {
 NA = -1,                     /* Not defined or associated yet. */
 NO_ARG = 0,                  /* No argument used. */
 INVAL_ARG,                   /* Invalid argument. */
 FNAME_R,                     /* No input filename. */
 FNAME_W,                     /* No output filename. */
 NO_LOCALE,                   /* No localization specifier. */
 LOCALE_RE,                   /* Locale redefined. */
 HELP_USED,                   /* -h or --help used alongside other arguments. */

 SET_LOCALE,                  /* Failed to set locale. */
 USING_ENV,                   /* Using current environment settings. */

 FILE_CLOSE,                  /* File closing failed. */
 DATA_CORRUPT,                /* Possible data corruption. */
} TE_Err;


/* Array of strings corresponding to TE_Err enumerate. */
static const wchar_t * const err_str[] = {
  L": No file names specified\n",
  L": Invalid option: ",
  L": Input filename not specified\n",
  L": Output filename not specified\n",

  L": Missing --loc specification\n",
  L": Locale redefined\n",

  L": Warning: -h or --help used, displaying help page\n",

  L": Warning: Failed to set localization: ",
  L": Warning: Using implicit environment configuration\n",

  L": Failed to close file: ",
  L": Warning: Data might be corrupted\n",
};


/**
 * Structure containing informations about processed program's parameters used
 * when calling of the program was performed.
 */
typedef struct arguments {
  bool help;                  /* Display help? */
  bool usort;                 /* Optional argument - use of unique sorting? */

  TE_Err error;               /* Error representation, if occurred. */
  char *p_wr_arg;             /* Pointer to wrong argument. */

  char *p_fname_r;            /* Input file - first filename found. */
  char *p_fname_w;            /* Output file - second filename found. */
  char *p_locale;             /* Optional argument - locale specification. */
} TS_Arguments;


/******************************************************************************
 ~~~[ FUNCTIONAL PROTOTYPES OF LOCAL FUNCTIONS ]~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 ******************************************************************************/

int process_args(int argc, char *argv[], TS_Arguments *p_args);
void disp_help(char *prg_name);


/******************************************************************************
 ~~~[ AUXILIARY FUNCTIONS ]~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 ******************************************************************************/

/**
 * Processes program's calling parameters. Also initializes the given
 * TS_Arguments structure
 */
int process_args(int argc, char *argv[], TS_Arguments *p_args)
{{{
  /* Initialization of given TS_Arguments structure. */
  p_args->help = false;
  p_args->usort = false;
  p_args->error = NA;
  p_args->p_wr_arg = NULL;
  p_args->p_fname_r = NULL;
  p_args->p_fname_w = NULL;
  p_args->p_locale = NULL;

  /* No arguments used. */
  if (argc == 1) {
    p_args->error = NO_ARG;
    return EXIT_FAILURE;
  }
  
  /* Goes through arguments array and processes each argument, one by one. */
  for (int i = 1; i < argc; i++) {

    /* Help parameter used? */
    if (strcmp(argv[i], "-h") == 0 || strcmp(argv[i], "--help") == 0)
      p_args->help = true;
    /* --usort parameter used? */
    else if (strcmp(argv[i], "--usort") == 0)
      p_args->usort = true;
    /* --loc parameter used? */
    else if (strcmp(argv[i], "--loc") == 0) {
      /* Increase of iterator to access another argument, if any. */
      i++;

      /* No locale specified yet? */
      if (p_args->p_locale == NULL) {

        /* Is there anything to process? */
        if (i < argc) {
          
          /* Missing --loc specification? (-h, --help, --usort used) */
          if (strcmp(argv[i], "-h") == 0 || strcmp(argv[i], "--help") == 0)
            p_args->help = true;
          else if (strcmp(argv[i], "--usort") == 0)
            p_args->error = NO_LOCALE;
          else
            p_args->p_locale = argv[i];   /* None of above. */

        }
        else if (p_args->error == NA)
          p_args->error = NO_LOCALE;      /* No --loc specification. */
      }
      else {
        p_args->error = LOCALE_RE;        /* --loc redefinition. */
      }
    }
    /* First unknown parameter -> input filename. */
    else if (p_args->p_fname_r == NULL)
      p_args->p_fname_r = argv[i];
    /* Second unknown parameter -> output filename. */
    else if (p_args->p_fname_w == NULL)
      p_args->p_fname_w = argv[i];
    /* Invalid parameter. */
    else {
      p_args->error = INVAL_ARG;
      p_args->p_wr_arg = argv[i];         /* Pointer to invalid parameter. */
    }

  }

  /* Only help parameter used? */
  if (p_args->help == true && argc == 2)
    return EXIT_SUCCESS;

  /* Missing filenames? */ 
  if (p_args->p_fname_r == NULL)
    p_args->error = FNAME_R;
  else if (p_args->p_fname_w == NULL)
    p_args->error = FNAME_W;
    
  /* Help parameter used with another parameters? */
  if (p_args->help == true)
    p_args->error = HELP_USED;

  /* Error occurred? */
  return (p_args->error != NA) ? EXIT_FAILURE : EXIT_SUCCESS;
}}}


/** TODO: FINISH ME!
 * Displays the program help page.
 */
void disp_help(char *prg_name)
{{{
  fwprintf(stdout, L"%s: THIS IS BIG HELP.\n", prg_name);
  return;
}}}


/******************************************************************************
 ~~~[ MAIN FUNCTION ]~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 ******************************************************************************/

int main(int argc, char *argv[])
{{{
  /* Setting localization to environment's actual setting. */
  if ((setlocale(LC_CTYPE, "")) == NULL) {
    fprintf(stderr, "%s: Failed to set current environment's localization.\n",
            argv[0]);
    return EXIT_FAILURE;
  }


  TS_Arguments args;

  /* Arguments processing. */
  if (process_args(argc, argv, &args) == EXIT_FAILURE) {

    /* Error handling. */
    switch (args.error) {

      /* Invalid option has been used. */
      case INVAL_ARG :
        fwprintf(stderr, L"%s%ls%s\n", argv[0],
                 err_str[args.error], args.p_wr_arg);
        break;

      /* Printing message for other errors. */
      default :
        fwprintf(stderr, L"%s%ls", argv[0], err_str[args.error]);
        break;
    }
    
    /* Additional message printing or help page displaying. */
    if (args.error != HELP_USED)
      fwprintf(stderr, L"Try '%s --help' for more information.\n", argv[0]);
    else
      disp_help(argv[0]);

    return EXIT_FAILURE;
  }

  
  /* Display help page? */
  if (args.help == true) {
    disp_help(argv[0]);
    return EXIT_SUCCESS;
  }


  int ret_val = EXIT_SUCCESS;         /* Program's default return value. */


  /* Sets new locale, if required. */
  if (args.p_locale != NULL) {

    /* Test for locale setting. */
    if (setlocale(LC_CTYPE, args.p_locale) == NULL) {
      fwprintf(stderr, L"%s%ls%s\n%s%ls", argv[0], err_str[SET_LOCALE],
               args.p_locale, argv[0], err_str[USING_ENV]);
      ret_val = EXIT_FAILURE;
    }

  }


  FILE *f_read;
  FILE *f_write;

  errno = 0;    /* Reseting errno for recognizing below if any error occurs. */

  /* Read file opening test. */
  if ((f_read = fopen(args.p_fname_r, "r")) == NULL) {
    fwprintf(stderr, L"%s: %s: %s\n", argv[0], args.p_fname_r, strerror(errno));
    return EXIT_FAILURE;
  }
  
  /* Write file opening test. */
  if ((f_write = fopen(args.p_fname_w, "w")) == NULL) {
    fwprintf(stderr, L"%s: %s: %s\n", argv[0], args.p_fname_w, strerror(errno));
    fclose(f_read);
    return EXIT_FAILURE;
  }


  /*
   * Calling of wrapper function which reads from file, creates and sorts the
   * linked list.
   */
  TS_Node *p_list_head = list_readinsort(f_read, args.usort);
  
  /* Successful reading/allocation? */
  if (p_list_head == NULL) {

    if (errno == EILSEQ)
      fwprintf(stderr, L"%s: %s: %s\n", argv[0], args.p_fname_r,
                                        strerror(errno));
    else
      fwprintf(stderr, L"%s: %s\n", argv[0], strerror(errno));

    ret_val = EXIT_FAILURE;
  }

  /* Printing of content of the list and list freeing. */
  list_wprint_free(&p_list_head, f_write);


  int ret_val1 = fclose(f_read);
  int ret_val2 = fclose(f_write);

  /* Test of closing of the read file. */
  if (ret_val1 == EOF)
    fwprintf(stderr, L"%s%ls%s\n", argv[0], err_str[FILE_CLOSE],
                                   args.p_fname_r);
  
  /* Test of closing of the write file. */
  if (ret_val2 == EOF)
    fwprintf(stderr, L"%s%ls\n%s%ls\n", argv[0], err_str[FILE_CLOSE],
                                        argv[0], err_str[DATA_CORRUPT]);

  /*
   * Return of proper value corresponding to setlocale(), readinsort() and files
   * closing.
   */
  return (ret_val || ret_val1 || ret_val2) ? EXIT_FAILURE : EXIT_SUCCESS;
}}}

/******************************************************************************
 ***[ END OF PROJ4_MAIN.C ]****************************************************
 ******************************************************************************/
 
