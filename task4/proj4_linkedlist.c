/**
 * File:          proj4_linkedlist.c
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
 * Description:   This module contains functions for working with the one-way
 *                linked list. See the functions description for more info.
 *
 * More info @:
 *      https://www.fit.vutbr.cz/study/courses/IZP/private/projekty/proj4.html
 *
 * File encoding: en_US.utf8 (United States)
 * Compiler used: gcc 4.5.2 (Ubuntu/Linaro 4.5.2-8ubuntu4)
 */


/******************************************************************************
 ***[ START OF PROJ4_LINKEDLIST.C ]********************************************
 ******************************************************************************/


/******************************************************************************
 ~~~[ HEADER FILES ]~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 ******************************************************************************/

#include <errno.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <wchar.h>

#include "proj4_wcio.h"
#include "proj4_wcscoll_cz.h"
#include "proj4_linkedlist.h"


/******************************************************************************
 ~~~[ LOCAL DATA TYPES DECLARATIONS ]~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 ******************************************************************************/


/******************************************************************************
 ~~~[ FUNCTIONAL PROTOTYPES OF LOCAL FUNCTIONS ]~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 ******************************************************************************/


/******************************************************************************
 ~~~[ GLOBAL FUNCTIONS ]~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 ******************************************************************************/

/**
 * Reads from given stream using get_wline() function and every read line
 * inserts into the linked list using insert sort algorithm. If the usort is
 * required, then duplicate lines are skipped. Function returns either the
 * pointer to the head of the list or the NULL pointer in case of failure (errno
 * is set to ENOMEM or EILSEQ, depending on what error occurred).
 */
TS_Node *list_readinsort(FILE *f_read, bool usort)
{{{
  /* Auxiliary structures used below. */
  TS_Wbuffer wbuff;             /* Wbuffer structure. */
  TS_Node *p_list_head;         /* Pointer to head of linked list. */
  TS_Node *p_node_prev;         /* Pointer to previous node in list. */
  TS_Node *p_node_act;          /* Pointer to actual node in list. */
  TS_Node *p_node_new;          /* Pointer for new allocated node. */

  int coll_res;                 /* Result of strings collating. */
  int gtwln_res;                /* Return value of get_wline() function. */
  unsigned i;                   /* Auxiliary iterator. */

  errno = 0;                    /* Reseting errno. */

  /* List initialization with the first node. */
  if (wbuff_init(&wbuff) == EXIT_FAILURE
      || get_wline(&wbuff, f_read) == EXIT_FAILURE
      || (p_list_head = node_make_fill(wbuff.p_data, wbuff.used)) == NULL)
    return NULL;

  /*
   * Whole line reading and insert sorting cycle.
   */
  while ((gtwln_res = get_wline(&wbuff, f_read)) == EXIT_SUCCESS) {

    i = 0;                      /* Iterator reseting. */

    p_node_act = p_list_head;   /* Actual node pointer reseting. */
    p_node_prev = p_list_head;  /* Previous node pointer reseting. */

    /*
     * Finding the proper place in the list where to store the newly read line.
     */
    do {

      /* Comparing newly read line with one of actual node. */
      coll_res = wcscoll_cz(wbuff.p_data, p_node_act->p_line);

      /* The newly read line is supposed to be behind one of the actual node. */
      if (coll_res > 0) {
        p_node_prev = p_node_act;           /* Backup of actual node. */
        p_node_act = p_node_act->p_next;    /* Reassigning of actual node. */
      }
      
      i++;                      /* Number of iterations through the list. */

    /*
     * If the newly read line is supposed to be behind one of the actual node
     * and it's not the end of the list, then repeat.
     */
    } while (coll_res > 0 && p_node_act != NULL);


    /* Usort required? */
    if (usort == true && coll_res == 0)
      continue;                 /* Skip of the same line. */
    else {
      
      /* 
       * Allocating memory for the new node and filling it with the content of
       * the buffer.
       */
      p_node_new = node_make_fill(wbuff.p_data, wbuff.used);

      /* Only one iteration? If so, insert to the beginning of the list. */
      if (i == 1 && p_node_act != NULL)
        list_ins_node_beg(&p_list_head, p_node_new);
      else
        list_ins_node(p_node_prev, p_node_new);   /* Regular insertion. */
    }

  }

  wbuff_free(&wbuff);           /* Helpful buffer memory freeing. */

  return p_list_head;           /* Return of the pointer to list's head. */
}}}


/**
 * Creates node for the linked list and fills it with the content of the given
 * buffer. The node uses flexible array member, so the size of the node is also
 * flexible. The number of wide characters to fill to node from buffer is given
 * by wchar_count argument. In case of allocation failure the NULL pointer is
 * returned and errno is set to ENOMEM. If the pointer to buffer is not valid,
 * then returns NULL pointer and errno is set to 0.
 */
TS_Node *node_make_fill(wchar_t *p_buff, unsigned wchar_count)
{{{
  errno = 0;              /* Reseting errno. */

  if (p_buff == NULL)     /* Valid buffer pointer? */
    return NULL;
  
  /* Pre-counting of line size of new node. */
  unsigned line_size = wchar_count * sizeof(wchar_t);

  TS_Node *p_node = (TS_Node *) malloc(sizeof(TS_Node) + line_size);

  /* Successful allocation? */
  if (p_node == NULL)
    return NULL;
  
  /* Node initialization. */  
  p_node->p_next = NULL;
  memcpy(p_node->p_line, p_buff, line_size);

  return p_node;          /* Pointer to node. */
}}}


/**
 * Function inserts given node at the beginning of the linked list. It requires
 * pointer to pointer to the start of the list and pointer to node, which is to
 * be insert.
 */
void list_ins_node_beg(TS_Node **pp_list_head, TS_Node *p_node)
{{{
  p_node->p_next = *pp_list_head;
  *pp_list_head = p_node;

  return;
}}}


/**
 * Function inserts given node behind the given ("actual") node into the linked
 * list.
 */
void list_ins_node(TS_Node *p_node_act, TS_Node *p_node)
{{{
  p_node->p_next = p_node_act->p_next;
  p_node_act->p_next = p_node;
  
  return;
}}}


/**
 * Prints the node's lines of the given list to given FILE stream. The list is
 * freed after printing (each node is freed after the node's line is printed).
 */
void list_wprint_free(TS_Node **pp_list_head, FILE *f_write)
{{{
  TS_Node *p_node_2free;              /* Backup pointer to node to be freed. */
  TS_Node *p_node_act = *pp_list_head;

  while (p_node_act != NULL) {
    fputws(p_node_act->p_line, f_write);   /* Wide character line printing. */

    p_node_2free = p_node_act;             /* Backup of actual pointer. */
    p_node_act = p_node_act->p_next;    /* New actual pointer to list. */

    free((void *) p_node_2free);              /* Freeing of node before. */
  }

  *pp_list_head = NULL;

  return;
}}}


/******************************************************************************
 ~~~[ AUXILIARY FUNCTIONS ]~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 ******************************************************************************/


/******************************************************************************
 ***[ END OF PROJ4_LINKEDLIST.C ]**********************************************
 ******************************************************************************/
 
