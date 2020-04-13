/**
 * File:          proj4_wcscoll_cz.h
 * Version:       0.9 beta
 * Date:          17-12-2011
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
 * Description:   This header file contains functional prototype for the
 *                wcscoll_cz function for sorting by Czech CSN_97_6030.
 *
 * More info @:
 *      https://www.fit.vutbr.cz/study/courses/IZP/private/projekty/proj4.html
 *
 * File encoding: en_US.utf8 (United States)
 * Compiler used: gcc 4.5.2 (Ubuntu/Linaro 4.5.2-8ubuntu4)
 */


/******************************************************************************
 ***[ START OF PROJ4_WCSCOLL_CZ.H ]********************************************
 ******************************************************************************/

/* Safety mechanism against multi-including of this header file. */
#ifndef PROJ4_WCSCOLL_CZ_H
#define PROJ4_WCSCOLL_CZ_H


/******************************************************************************
 ~~~[ GLOBAL DATA TYPES DECLARATIONS ]~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 ******************************************************************************/


/******************************************************************************
 ~~~[ FUNCTIONAL PROTOTYPES OF GLOBAL FUNCTIONS ]~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 ******************************************************************************/

int wcscoll_cz(const wchar_t *ws1, const wchar_t *ws2);

#endif

/******************************************************************************
 ***[ END OF PROJ4_WCSCOLL_CZ.H ]**********************************************
 ******************************************************************************/
 

