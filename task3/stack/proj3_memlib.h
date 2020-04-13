/**
 * File:          proj3_memlib.h
 * Version:       1.0 RC
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
 * Description:   This header file contains functional prototypes of
 *                proj3_memlib.c file and global structure for working with 
 *                functions defined in proj3_memlib.c.
 *
 * More info @:
 *      https://www.fit.vutbr.cz/study/courses/IZP/private/projekty/proj3.html
 *
 * Encoding:      en_US.utf8 (United States)
 * Compiler used: gcc 4.5.2 (Ubuntu/Linaro 4.5.2-8ubuntu4)
 */

/******************************************************************************
 ***[ START OF PROJ3_MEMLIB.H ]************************************************
 ******************************************************************************/

/* Safety mechanism against multi-including of this header file. */
#ifndef PROJ3_MEMLIB_H
#define PROJ3_MEMLIB_H

/******************************************************************************
 ~~~[ GLOBAL CONSTANTS DECLARATIONS ]~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 ******************************************************************************/

/******************************************************************************
 ~~~[ GLOBAL DATA TYPES DECLARATIONS ]~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 ******************************************************************************/

typedef struct stack_info {
  unsigned long size_bef;
  unsigned long size_act;
  unsigned long size_mult;
  unsigned arg_count;

  int *stack_beg;
  int *stack_end;
  int *stack_top;
} TS_Stack_info;

/******************************************************************************
 ~~~[ FUNCTIONAL PROTOTYPES OF GLOBAL FUNCTIONS ]~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 ******************************************************************************/

int stack_init(TS_Stack_info *p_st);
void stack_free(TS_Stack_info *p_st);
int stack_push(TS_Stack_info *p_st, ...);
void stack_pop(TS_Stack_info *p_st, ...);
#endif

/******************************************************************************
 ***[ END OF PROJ3_MEMLIB.H ]**************************************************
 ******************************************************************************/
 
