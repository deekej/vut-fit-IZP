/**
 * File:          proj4_wcio.h
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
 * Description:   This header file of proj4_wciolib.c module contains one
 *                structure used for initializing wide characters buffer and
 *                functional prototypes of global functions.
 *
 * More info @:
 *      https://www.fit.vutbr.cz/study/courses/IZP/private/projekty/proj4.html
 *
 * File encoding: en_US.utf8 (United States)
 * Compiler used: gcc 4.5.2 (Ubuntu/Linaro 4.5.2-8ubuntu4)
 */

/******************************************************************************
 ***[ START OF PROJ4_WCIO.H ]**************************************************
 ******************************************************************************/

/* Safety mechanism against multi-including of this header file. */
#ifndef PROJ4_WCIO_H
#define PROJ4_WCIO_H


/******************************************************************************
 ~~~[ GLOBAL DATA TYPES DECLARATIONS ]~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 ******************************************************************************/

/*
 * Structure containing informations about buffer for wide characters.
 */
typedef struct wbuffer {
  unsigned size;            /* Actual buffer size. */
  unsigned used;            /* Number of used characters from buffer. */

  wchar_t *p_data;          /* Pointer to memory of buffer. */
} TS_Wbuffer;


/******************************************************************************
 ~~~[ FUNCTIONAL PROTOTYPES OF GLOBAL FUNCTIONS ]~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 ******************************************************************************/

int get_wline(TS_Wbuffer *p_wbuff, FILE *f_stream);

int wbuff_init(TS_Wbuffer *p_wbuff);
void wbuff_free(TS_Wbuffer *p_wbuff);

#endif

/******************************************************************************
 ***[ END OF PROJ4_WCIO.H ]****************************************************
 ******************************************************************************/
 
