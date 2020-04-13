/**
 * File:          proj3_memlib.c
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
 * Description:   Module for working with pseudo-stack on the heap. For now
 *                there support only for int values, but this code can be easily
 *                transformed.
 *
 * More info @:
 *      https://www.fit.vutbr.cz/study/courses/IZP/private/projekty/proj3.html
 *
 * Encoding:      en_US.utf8 (United States)
 * Compiler used: gcc 4.5.2 (Ubuntu/Linaro 4.5.2-8ubuntu4)
 */

/******************************************************************************
 ***[ START OF PROJ3_MEMLIB.C ]***********************************************
 ******************************************************************************/

/******************************************************************************
 ~~~[ HEADER FILES ]~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 ******************************************************************************/

#include <errno.h>
#include <stdarg.h>
#include <stdlib.h>

#include "proj3_memlib.h"


/******************************************************************************
 ~~~[ LOCAL CONSTANTS DEFINITIONS  ]~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 ******************************************************************************/

/* Default value for multiplying default size of new stack. */
static const unsigned long MULT_DEFAULT = 16;


/******************************************************************************
 ~~~[ FUNCTIONAL PROTOTYPES OF LOCAL AUXILIARY FUNCTIONS ]~~~~~~~~~~~~~~~~~~~~~
 ******************************************************************************/

static int stack_ext(TS_Stack_info *p_st);


/******************************************************************************
 ~~~[ GLOBAL STACK FUNCTIONS ]~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 ******************************************************************************/

/**
 * Initialize the given pseudo-stack structure. Also allocates the memory for
 * the stack, then sets stack pointers. Returns EXIT_FAILURE upon error and sets
 * errno to ENOMEM, otherwise returns EXIT_SUCCESS.
 */
int stack_init(TS_Stack_info *p_st)
{
  /* Stack values initialization. */
  p_st->size_bef = 1;
  p_st->size_act = 1;
  p_st->size_mult = MULT_DEFAULT;
  p_st->arg_count = 1;

  /* Stack pointers initialization. */
  p_st->stack_beg = NULL;
  p_st->stack_end = NULL;
  p_st->stack_top = NULL;

  unsigned long stack_size;
  
  stack_size = p_st->size_act * p_st->size_mult;
  p_st->stack_beg = (int *) malloc(stack_size * sizeof(int));

  if (p_st->stack_beg == NULL) {
    errno = ENOMEM;
    return EXIT_FAILURE;
  }

  /* Pointer assignment to stack top and end. */
  p_st->stack_top = p_st->stack_beg;
  p_st->stack_end = p_st->stack_beg + stack_size;

  return EXIT_SUCCESS;
}


/**
 * Frees the stack memory from TS_Stack_info structure and resets its values.
 */
void stack_free(TS_Stack_info *p_st)
{
  free((void *) p_st->stack_beg);

  p_st->stack_beg = NULL;
  p_st->stack_end = NULL;
  p_st->stack_top = NULL;

  p_st->size_bef = 1;
  p_st->size_act = 1;
  p_st->arg_count = 1;
}


/**
 * Stores int values into allocated memory for the stack. The number of stored
 * values depends on value of arg_count member of TS_Stack_info. If the stack
 * reaches the end of allocated memory, then function calls stack_ext() function
 * for stack extending. If the extending failed, then returns EXIT_FAILURE and
 * keeps current state of stack unchanged, so the calling function can try to
 * recover from data loss or can try it again.
 * WARNING: The behaviour of the function is not defined if the arg_count does
 * not match the optional int arguments!!!
 */
int stack_push(TS_Stack_info *p_st, ...)
{
  va_list ap;                   /* Pointer to optional argument. */

  va_start(ap, p_st);           /* Initialization for 1st optional argument. */

  /* Stores the given integers for amount set in arg_count. */
  for (unsigned i = 0; i < p_st->arg_count; i++) {

    /* End of stack? */
    if (p_st->stack_top == p_st->stack_end
        && stack_ext(p_st) == EXIT_FAILURE) {
      va_end(ap);               /* End of work with optional arguments. */
      return EXIT_FAILURE;
    }
    
    /* Gets the value and stores it in the stack. */
    *p_st->stack_top = (int) va_arg(ap, int);
    p_st->stack_top++;          /* New stack top. */
  }

  va_end(ap);                   /* End of work with optional arguments. */
  return EXIT_SUCCESS;
}


/**
 * Function restores int values from stack to given addresses. If the stack is
 * empty, then the given addresses remain unchanged. The values popping should
 * be in reverse order as the values were stored to the stack.
 */
void stack_pop(TS_Stack_info *p_st, ...)
{
  /* Nothing in stack? */
  if (p_st->stack_top != p_st->stack_beg) {
    int *p_dest;                /* Address for int value restoration. */
    va_list ap;                 /* Pointer to optional argument. */

    va_start(ap, p_st);         /* Initialization for the 1st argument. */

    /* Restores values into given integers for amount set in arg_count. */
    for (unsigned i = 0; i < p_st->arg_count; i++) {
      p_st->stack_top--;                    /* Accessing last stored value. */
      p_dest = (int *) va_arg(ap, int *);   /* Gets destination address. */
      *p_dest = *p_st->stack_top;           /* Restore the value. */
    }

    va_end(ap);                 /* End of work with optional arguments. */
  }

  return;
}


/******************************************************************************
 ~~~[ LOCAL AUXILIARY FUNCTIONS ]~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 ******************************************************************************/

/**
 * Extends the size of stack by reallocating the stacks' memory. Upon failure
 * returns EXIT_FAILURE and sets errno to ENOMEM, the original stack remains
 * unchanged. Otherwise returns EXIT_SUCCESS.
 */
static int stack_ext(TS_Stack_info *p_st)
{
  int *p_stack_beg_cp;              /* Backup pointer of stack begin. */
  unsigned long stack_size;         /* For new stack size value. */
  
  p_stack_beg_cp = p_st->stack_beg;
  stack_size = (p_st->size_bef + p_st->size_act) * p_st->size_mult;

  /* Stack extending. */
  p_st->stack_beg = realloc(p_st->stack_beg, stack_size * sizeof(int));

  if (p_st->stack_beg == NULL) {
    /* Restoring original pointer to prevent memory leaks etc. */
    p_st->stack_beg = p_stack_beg_cp;
    errno = ENOMEM;
    return EXIT_FAILURE;
  }

  /* Reassigning pointers because of possible new memory alignment. */
  p_st->stack_end = p_st->stack_beg + stack_size;
  p_st->stack_top = p_st->stack_beg + (p_st->stack_top - p_stack_beg_cp);

  /* Updating the actual and before values. */
  p_st->size_act += p_st->size_bef;
  p_st->size_bef = p_st->size_act - p_st->size_bef;

  return EXIT_SUCCESS;
}


/******************************************************************************
 ***[ END OF PROJ3_MEMLIB.C ]**************************************************
 ******************************************************************************/
 
