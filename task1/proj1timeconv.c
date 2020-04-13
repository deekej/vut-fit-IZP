/**
 * File:          proj1timeconv.c
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
 * Description:   This module contains primary and subsidiary functions used
 *                for time entries conversions. It is used mainly by proj1.c
 *                module, but it can be used by other modules also.
 * More info @:
 *      https://www.fit.vutbr.cz/study/courses/IZP/private/projekty/proj1.html
 *
 * Encoding:      en_US.utf8 (United States)
 * Compiler used: gcc 4.5.2 (Ubuntu/Linaro 4.5.2-8ubuntu4)
 */


/******************************************************************************
 ***[ START OF PROJ1TIMECONV.C ]***********************************************
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
 ~~~[ LOCAL CONSTANTS DEFINITIONS ]~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 ******************************************************************************/

/* Max. size of fields used below. */
#define MAX_ELEMENTS 5LU

/* Default word ending for given number. */
static const unsigned long DEFAULT_ENDING = 0;

/* Number of seconds of week. */
static const unsigned long WEEK_SECS = 604800;

/* Number of seconds of day. */
static const unsigned long DAY_SECS = 86400;

/* Number of seconds of hour. */
static const unsigned long HOUR_SECS = 3600;

/* Number of seconds of minute. */
static const unsigned long MIN_SECS = 60;

/*
 * Maximum characters of longest word which can be printed (including trailing
 * '\0'). The longest actual word is 'seconds' in Czech.
 */
static const int MAX_CHARS = 8;


/******************************************************************************
 ~~~[ LOCAL DATA TYPES DECLARATIONS ]~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 ******************************************************************************/

/*
 * Field containing endings for Czech words hours, minutes, seconds
 * corresponding to Czech language cases.
 */
static char * const hms_endings[MAX_ELEMENTS] = {
  "",
  "a",
  "y",
  "y", 
  "y",
};


/*
 * Field containing endings for Czech words weeks and days which have other
 * endings than words higher.
 */
static char * const wd_endings[MAX_ELEMENTS] = {
  "nu",
  "en",
  "ny",
  "ny",
  "ny",
};


/*
 * Field containing unit names. There's an empty string for indexing starting
 * from number 1. (See function prepare_time_value() for more information.)
 * Then there are roots of Czech words seconds, minutes, hours, days, weeks.
 */
static char * const units_names[] = {
  "",
  "sekund",
  "minut",
  "hodin",
  "d",
  "tyd",
};


/*
 * Same structure as standard ldiv_t, but instead of long int it uses unsigned
 * long int. This structure is used by uldiv() function below.
 */
typedef struct uldiv_t {
  unsigned long int quot;       /* Quotient. */
  unsigned long int rem;        /* Remainder. */
} TS_Uldiv_t;


/******************************************************************************
 ~~~[ FUNCTIONAL PROTOTYPES OF LOCAL SUBSIDIARY FUNCTIONS ]~~~~~~~~~~~~~~~~~~~~
 ******************************************************************************/

static void prepare_time_value(unsigned long value, char *p_unit_name,
                               TE_Max_unit unit);

static TS_Uldiv_t uldiv(unsigned long numer, unsigned long denom);

/*
 * NOTE: Declarations of other (PRIMARY) functions can be found in
 *       "proj1timeconv.h" header file.
 */


/******************************************************************************
 ~~~[ PRIMARY FUNCTIONS ]~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 ******************************************************************************/

/**
 * Function transforms sequence of digits from stdin to given unsigned long
 * value, character by character. Function expects only digits on the input
 * line. Returns EOF if there was no input, EXIT_SUCCESS if everything went fine
 * and EXIT_FAILURE if there was an error. (Invalid character or value cannot be
 * stored in given data type.) Function also sets errno so the calling function
 * can determine which error has occurred.
 */
int readtoul(unsigned long *p_num)
{
  /*
   * Helpful value for determining if the actual value can be multiplied by 10
   * without the value overflowing.
   */
  unsigned long max_val_quo = ULONG_MAX / 10;
  
  /*
   * If the actual value is even as max_val_quo, then only the remainder after
   * modulo dividing can be added after multiplying by 10, otherwise the value
   * would overflow. This is the helpful value for condition testing below.
   */
  int max_val_rem = (int) (ULONG_MAX % 10);

  int c = 0;                          /* Character to read. */

  int i = 0;                          /* Iteration counter. */
  
  *p_num = 0;                         /* Actual value of reading. */

  errno = ESUCCESS;                   /* Setting errno to 0 for use below. */

  
  while ((errno == ESUCCESS) && ((c = fgetc(stdin)) != EOF) && (c != '\n')) {

    if (isdigit(c)) {

      c = c - '0';          /* Actual value of the read digit. */

      /*
       * Actual value must be below than max_val quotient or if the actual value
       * is even to max_val quotient, then the last character read must be below
       * or equal to max_val remainder for no overflow.
       */
      if (*p_num < max_val_quo || (*p_num == max_val_quo && c <= max_val_rem)) {
        *p_num *= 10;       /* Shifting the numbers' set. */
        *p_num += c;        /* Adding the value of the last read character. */
        i++;                /* Transforming of the number was performed. */
      }
      else
        errno = EOVERFLOW;  /* Value too big to be stored in given type. */

    }
    else
      errno = EDOM;         /* Invalid character was read. */

  }

  if (errno != ESUCCESS)
    return EXIT_FAILURE;  /* Invalid character or overflow occurred. */
  else if (i == 0)
    return EOF;           /* No reading was performed. */
  else
    return EXIT_SUCCESS;  /* Reading proceed without error. */
}


/** 
 * Function reads unsigned long value representing number of seconds, then
 * converts it to seconds, minutes, hours, days and weeks and stores it in
 * TS_Time structure, which is then returned. However it converts only to max
 * time unit which is given in enumerate TE_Max_unit. Functions uses constants
 * representing seconds which each time unit (e.g. hour or day) has for quicker
 * computation.
 */
TS_Time process_time_value(unsigned long seconds, TE_Max_unit max_unit)
{
  /*
   * Definition & initialization of helpful TS_Time structure, which will be
   * returned.
   */
  TS_Time processed_time = {
    .secs = 0,
    .mins = 0,
    .hours = 0,
    .days = 0,
    .weeks = 0,
  };
  
  /*
   * Declaration of TS_Uldiv_t structure in which the quotient and remainder
   * are returned. This structure is used by uldiv() function.
   */   
  TS_Uldiv_t result;

  
  /*
   * Jumps over to wanted case given by max_unit. This means not wanted units
   * are skipped and every other is computed. If max_unit is wrong, then jumps
   * to default and sets errno to EINVAL and zeros are returned in TS_Time
   * structure.
   */
  switch (max_unit) {

    case WEEKS :
      result = uldiv(seconds, WEEK_SECS); /* Divide the number of seconds. */
      processed_time.weeks = result.quot; /* Number of weeks. */
      seconds = result.rem;               /* Remainder saved to seconds. */

    case DAYS :
      result = uldiv(seconds, DAY_SECS);  /* Divide the number of seconds. */
      processed_time.days = result.quot;  /* Number of days. */
      seconds = result.rem;               /* Remainder saved to seconds. */

    case HOURS :
      result = uldiv(seconds, HOUR_SECS); /* Divide the number of seconds. */
      processed_time.hours = result.quot; /* Number of hours. */
      seconds = result.rem;               /* Remainder saved to seconds. */

    case MINUTES :
      result = uldiv(seconds, MIN_SECS);  /* Divide the number of seconds. */
      processed_time.mins = result.quot;  /* Number of minutes. */
      seconds = result.rem;               /* Remainder saved to seconds. */

    case SECONDS :
      processed_time.secs = seconds;      /* Last remainder of seconds. */
      break; 

    default :                             /* Wrong value of max_unit. */
      errno = EINVAL;
      break;
    }

  return processed_time;  /* Return of structure containing processed time. */
}


/**
 * Function prints correctly content of given TS_Time structure corresponding
 * to Czech grammar to stdout. The 0 values in TS_Time structure are skipped,
 * others are printed. If every value in given structure is 0, then prints
 * default value (0 seconds).
 */
void print_time_value(TS_Time *p_time)
{
  char s_unit_name[MAX_CHARS];  /* Helpful string used for printing. */

  bool print_space = false;     /* Needed for skipping of first space char. */

  int printed_chars = 0;        /* The actual value of printed characters. */

  int *p_printed_chars = &printed_chars;  /* Pointer used by printf(). */

  unsigned long value = 0;      /* Actual value for printing. */

  TE_Max_unit unit;             /* Helpful enumerate unit for iterating. */

 
  /*
   * Goes through every element of TS_Time structure and if it's a nonzero 
   * value, then prints it properly in human readable form.
   */ 
  for (unit = WEEKS; unit >= SECONDS; unit--) {
    
    /*
     * Assigning correct value from given TS_Time structure.
     */
    switch (unit) {
    
      case WEEKS :
        value = p_time->weeks;
        break;

      case DAYS :
        value = p_time->days;
        break;

      case HOURS :
        value = p_time->hours;
        break;

      case MINUTES :
        value = p_time->mins;
        break;

      case SECONDS :
        value = p_time->secs;
        break;

      default :
        break;

    }
        
    if (value == 0)         /* Zero value -> nothing to print. */
      continue;
  
    /*
     * Preparation of Czech word and its' ending for printing. Result is stored
     * in helpful string s_unit_name.
     */
    prepare_time_value(value, s_unit_name, unit);

    if (print_space == true)
      fprintf(stdout, " ");
    
    /*
     * Prints the value of actual unit and name of the unit. Also stores the
     * number of printed characters in printed_chars variable.
     */
    fprintf(stdout, "%lu %s%n", value, s_unit_name, p_printed_chars);
    
    print_space = true;     /* 1st printing is done, print space next time. */
  }
  

  /*
   * Prints the default value if no printing was performed. (Zeros in whole
   * structure.)
   */
  if (printed_chars == 0)
    fprintf(stdout, "0 sekund");

  fprintf(stdout, "\n");
    
  return;
}


/******************************************************************************
 ~~~[ SUBSIDIARY FUNCTIONS ]~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 ******************************************************************************/

/**
 * Same function as ldiv(), but instead of long int it uses unsigned lont int.
 * Because the unsigned long can't be negative, there's no need for test on
 * negative results like in standard ldiv() function.
 */
static TS_Uldiv_t uldiv(unsigned long numer, unsigned long denom)
{
  TS_Uldiv_t result;    /* Helpful structure. */

  result.quot = numer / denom;
  result.rem = numer % denom;

  return result;
}


/**
 * Function prepares string for printing which contains Czech name of
 * corresponding unit and its' ending given by the value of the unit. The result
 * is stored in string by given pointer. Function uses static global fields of
 * this module.
 */
static void prepare_time_value(unsigned long value, char *p_unit_name,
                               TE_Max_unit unit)
{
  char *p_unit_ending = NULL;   /* Helpful pointer to corresponding ending. */

  /*
   * Prepares string for printing by determining which unit is being print and
   * assigning correct word ending. Then prints the results into given string.
   */
  switch (unit) {
    
    /* Weeks & days uses wd_endings field. The rest of code is similar. */
    case WEEKS : {
      if (value < MAX_ELEMENTS)
        p_unit_ending = wd_endings[value];
      else
        p_unit_ending = wd_endings[DEFAULT_ENDING];

      snprintf(p_unit_name, MAX_CHARS,
               "%s%s", units_names[WEEKS], p_unit_ending);
      break;
    }

    case DAYS : {
      if (value < MAX_ELEMENTS)
        p_unit_ending = wd_endings[value];
      else
        p_unit_ending = wd_endings[DEFAULT_ENDING];

      snprintf(p_unit_name, MAX_CHARS,
               "%s%s", units_names[DAYS], p_unit_ending);
      break;
    }
    
    /* Hours, minutes and seconds uses hms_endings field. */
    case HOURS : {
      if (value < MAX_ELEMENTS)
        p_unit_ending = hms_endings[value];
      else
        p_unit_ending = hms_endings[DEFAULT_ENDING];

      snprintf(p_unit_name, MAX_CHARS,
               "%s%s", units_names[HOURS], p_unit_ending);
      break;
    }

    case MINUTES : {
      if (value < MAX_ELEMENTS)
        p_unit_ending = hms_endings[value];
      else
        p_unit_ending = hms_endings[DEFAULT_ENDING];

      snprintf(p_unit_name, MAX_CHARS,
               "%s%s", units_names[MINUTES], p_unit_ending);
      break;
    }

    case SECONDS : {
      if (value < MAX_ELEMENTS)
        p_unit_ending = hms_endings[value];
      else
        p_unit_ending = hms_endings[DEFAULT_ENDING];

      snprintf(p_unit_name, MAX_CHARS,
               "%s%s", units_names[SECONDS], p_unit_ending);
      break;
    }

    /*
     * Default case for avoiding warnings, that NOT_DEF & HELP are not
     * processed in the switch.
     */
    default :
      break;     
  }

  return;
}


/******************************************************************************
 ***[END OF PROJ1TIMECONV.C]***************************************************
 ******************************************************************************/
 
