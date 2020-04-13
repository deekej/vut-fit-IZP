/**
 * File:          proj4_wcio.c
 * Version:       1.2 final
 * Date:          12-12-2011
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
 * Description:   This module contains functions for wide character buffer
 *                initializing, extending, freeing. Also contains function for
 *                reading line of wide characters.
 *
 * More info @:
 *      https://www.fit.vutbr.cz/study/courses/IZP/private/projekty/proj4.html
 *
 * File encoding: en_US.utf8 (United States)
 * Compiler used: gcc 4.5.2 (Ubuntu/Linaro 4.5.2-8ubuntu4)
 */


/******************************************************************************
 ***[ START OF PROJ4_WCIO.C ]**************************************************
 ******************************************************************************/


/******************************************************************************
 ~~~[ HEADER FILES ]~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 ******************************************************************************/

#include <errno.h>
#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <wchar.h>

#include "proj4_wcio.h"


/******************************************************************************
 ~~~[ LOCAL DATA TYPES DECLARATIONS ]~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 ******************************************************************************/

static const unsigned BUFF_SIZE_DEF = 82;


/******************************************************************************
 ~~~[ FUNCTIONAL PROTOTYPES OF LOCAL FUNCTIONS ]~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 ******************************************************************************/

int wbuff_ext(TS_Wbuffer *p_wbuff);


/******************************************************************************
 ~~~[ PRIMARY FUNCTIONS ]~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 ******************************************************************************/

/**
 * Reads entire line of wide characters from f_stream and stores it into buffer.
 * The length of line is not limited by size, only by available memory. The
 * buffer is null-terminated and includes the newline character, if one was
 * found. Function sets errno upon error to ENOMEM, if memory reallocation have
 * have failed or EILSEQ, if the reading from stream have failed. The given
 * buffer must be initialized, otherwise the function will not do anything.
 */
int get_wline(TS_Wbuffer *p_wbuff, FILE *f_stream)
{{{
  errno = 0;                  /* Reseting errno. */

  /* Is there any place to store the line? */
  if (p_wbuff->p_data == NULL)
    return EXIT_FAILURE;

  p_wbuff->used = 0;          /* Reseting number of used chars. in buffer. */

  wint_t wch;                 /* Wide char variable. */

  do {
    wch = fgetwc(f_stream);   /* Same as fgetc(). */

    /* Extend buffer? */
    if (p_wbuff->used == p_wbuff->size && wbuff_ext(p_wbuff) == EXIT_FAILURE)
      break;                  /* Buffer extending failed. */
    else
      p_wbuff->p_data[p_wbuff->used++] = wch;   /* Store and increment. */
  
  } while (wch != WEOF && wch != L'\n');

  /* Extend buffer? One more character to store is needed... */
  if (p_wbuff->used == p_wbuff->size && wbuff_ext(p_wbuff) == EXIT_FAILURE)
    return EXIT_FAILURE;

  p_wbuff->p_data[p_wbuff->used++] = L'\0';     /* String terminating. */

  /* Return of appropriate value to determine what happened. */
  if (errno != 0)
    return EXIT_FAILURE;      /* ENOMEM or EILSEQ. */
  else if (wch == WEOF)
    return WEOF;              /* Nothing to read. */
  else
    return EXIT_SUCCESS;      /* Line correctly read. */
}}}


/**
 * Initialization of wide character buffer of TS_Wbuffer structure. 
 */
int wbuff_init(TS_Wbuffer *p_wbuff)
{{{
  /* Buffer values initialization. */
  p_wbuff->size = 0;
  p_wbuff->used = 0;
  p_wbuff->p_data = NULL;

  /* Buffer memory allocation. */
  p_wbuff->p_data = (wchar_t *) malloc(BUFF_SIZE_DEF * sizeof(wchar_t));

  if (p_wbuff->p_data == NULL) {    /* Memory allocation failed? */
    errno = ENOMEM;
    return EXIT_FAILURE;
  }
  
  p_wbuff->size = BUFF_SIZE_DEF;    /* New size of buffer. */

  return EXIT_SUCCESS;    
}}}


/**
 * Dispose of wide character buffer of TS_Wbuffer structure.
 */
void wbuff_free(TS_Wbuffer *p_wbuff)
{{{
  free((void *) p_wbuff->p_data);

  /* Values reseting for safety reasons. */
  p_wbuff->size = 0;
  p_wbuff->used = 0;
  p_wbuff->p_data = NULL;

  return;
}}}


/******************************************************************************
 ~~~[ AUXILIARY FUNCTIONS ]~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 ******************************************************************************/

/**
 * Extends size of wide character buffer of TS_Wbuffer structure. Sets errno
 * upon reallocation failure to ENOMEM.
 */
int wbuff_ext(TS_Wbuffer *p_wbuff)
{{{
  wchar_t *p_data_cp;             /* Backup pointer in case realloc() fails. */

  p_data_cp = p_wbuff->p_data;
  
  /* Buffer reallocation. */
  p_wbuff->p_data = (wchar_t *) realloc(p_wbuff->p_data,
                                        p_wbuff->size * sizeof(wchar_t) * 2);

  /* Fail of reallocation? */
  if (p_wbuff->p_data == NULL) {

    /* Freeing of buffer. (TS_Wbuffer data pointer was altered.)*/
    free((void *) p_data_cp);

    /* Values reseting for safety reasons. */
    p_wbuff->p_data = NULL;
    p_wbuff->size = 0;
    p_wbuff->used = 0;

    return EXIT_FAILURE;
  }
  else
    return EXIT_SUCCESS;
}}}


/******************************************************************************
 ***[ END OF PROJ4_WCIO.C ]****************************************************
 ******************************************************************************/
 
