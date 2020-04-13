/**
 * File:          proj1timeconv.h
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
 * Description:   This header file contains functional prototypes of PRIMARY
 *                functions of proj1timeconv.c file and also data structures,
 *                that can be used alongside them.
 * More info @:
 *      https://www.fit.vutbr.cz/study/courses/IZP/private/projekty/proj1.html
 *
 * Encoding:      en_US.utf8 (United States)
 * Compiler used: gcc 4.5.2 (Ubuntu/Linaro 4.5.2-8ubuntu4) 
 */


/******************************************************************************
 ***[ START OF PROJ1TIMECONV.H]************************************************
 ******************************************************************************/

/*
 * Safety mechanism against multi-including of of this .h file.
 */
#ifndef PROJ1TIMECONV_H

#define PROJ1TIMECONV_H


/******************************************************************************
 ~~~[ GLOBAL CONSTANTS DEFINITIONS ]~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 ******************************************************************************/

#define ESUCCESS 0                  /* Value representing errno == 0. */


/******************************************************************************
 ~~~[ GLOBAL DATA TYPES DECLARATIONS ]~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 ******************************************************************************/

/*
 * Enumerate representing maximum time unit to be displayed.
 */
typedef enum max_unit {
  NOT_DEF = -1,   /* No argument defined. */

  HELP    = 0,    /* Help page. */
  SECONDS = 1,
  MINUTES,
  HOURS,
  DAYS,
  WEEKS,
} TE_Max_unit;


/*
 * Structure containing one time value sorted from weeks to seconds.
 */
typedef struct time {
  unsigned long secs;
  unsigned long mins;
  unsigned long hours;
  unsigned long days;
  unsigned long weeks;
} TS_Time;


/******************************************************************************
 ~~~[ FUNCTIONAL PROTOTYPES OF PRIMARY FUNCTIONS ]~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 ******************************************************************************/

int readtoul(unsigned long *p_num);

TS_Time process_time_value(unsigned long seconds, TE_Max_unit max_unit);

void print_time_value(TS_Time *p_time);

#endif

/******************************************************************************
 ***[END OF PROJ1TIMECONV.H]***************************************************
 ******************************************************************************/
 
