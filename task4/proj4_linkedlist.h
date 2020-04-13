/**
 * File:          proj4_linkedlist.h
 * Version:       1.1 final
 * Date:          13-12-2011
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
 * Description:   This header file contains functional prototypes and global
 *                data declarations for the proj4_linkedlist.c module.
 *
 * More info @:
 *      https://www.fit.vutbr.cz/study/courses/IZP/private/projekty/proj4.html
 *
 * File encoding: en_US.utf8 (United States)
 * Compiler used: gcc 4.5.2 (Ubuntu/Linaro 4.5.2-8ubuntu4)
 */


/******************************************************************************
 ***[ START OF PROJ4_LINKEDLIST.H ]********************************************
 ******************************************************************************/

/* Safety mechanism against multi-including of this header file. */
#ifndef PROJ4_LINKEDLIST_H
#define PROJ4_LINKEDLIST_H


/******************************************************************************
 ~~~[ GLOBAL DATA TYPES DECLARATIONS ]~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 ******************************************************************************/

/*
 * Structure used as a node for the one-way linked list.
 */
typedef struct node {
  struct node *p_next;        /* Pointer to next node of the list. */

  wchar_t p_line[];           /* Flexible array member containing read line.  */
} TS_Node;


/******************************************************************************
 ~~~[ FUNCTIONAL PROTOTYPES OF GLOBAL FUNCTIONS ]~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 ******************************************************************************/


TS_Node *list_readinsort(FILE *f_read, bool usort);
void list_wprint_free(TS_Node **pp_list_head, FILE *f_write);


void list_ins_node_beg(TS_Node **pp_list_head, TS_Node *p_node);
void list_ins_node(TS_Node *p_node_prev, TS_Node *p_node);
TS_Node *node_make_fill(wchar_t *p_buff, unsigned wchar_count);

#endif

/******************************************************************************
 ***[ END OF PROJ4_LINKEDLIST.H ]**********************************************
 ******************************************************************************/
 
