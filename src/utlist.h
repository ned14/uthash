/*
Copyright (c) 2007-2009, Troy D. Hanson
All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:

    * Redistributions of source code must retain the above copyright
      notice, this list of conditions and the following disclaimer.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS
IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED
TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A
PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER
OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#ifndef UTLIST_H
#define UTLIST_H

/* 
 * This file contains macros to manipulate singly and doubly-linked lists.
 *
 * 1. LL_ macros:  Non-circular, singly-linked lists.
 * 2. DL_ macros:  Non-circular, doubly-linked lists.
 * 3. CDL_ macros: Circular, doubly-linked lists.
 *
 * The lists are comprised of structs which have "prev" and "next" 
 * members. Singly-linked structures need only have the "next" member.
 * The pointer to the list head must be initialized to NULL.
 * 
 * ----------------.EXAMPLE -------------------------
 * struct my_item {
 *      int id;
 *      struct my_item *prev, *next;
 * }
 *
 * struct my_item *my_list = NULL:
 *
 * int main() {
 *      struct my_item *item;
 *      ... allocate and populate item ...
 *      DL_ADD(my_list, item);
 * }
 * --------------------------------------------------
 *
 * The ADD/DEL macros for doubly-linked lists are constant-time, 
 * but the macros for singly-linked lists are O(n).
 */

/******************************************************************************
 * singly linked list macros (non-circular)                                   *
 *****************************************************************************/
#define LL_ADD(head,tmp,add)                                    \
    do {                                                        \
        (tmp) = (head);                                         \
        if (tmp) {                                              \
            while ((tmp)->next)                                 \
                 (tmp)=(tmp)->next;                             \
            (tmp)->next=(add);                                  \
            (add)->next=NULL;                                   \
        } else {                                                \
            (head)=(add);                                       \
            (add)->next=NULL;                                   \
        }                                                       \
    } while (0);

#define LL_DEL(head,tmp,del)                                    \
    do {                                                        \
        (tmp) = (head);                                         \
        if (tmp == del) {                                       \
            (head)=(tmp)->next;                                 \
        } else {                                                \
            while ((tmp)->next && (tmp)->next != del)           \
                (tmp)=(tmp)->next;                              \
            if ((tmp)->next)                                    \
                (tmp)->next=(del)->next;                        \
        }                                                       \
    } while (0);

#define LL_FOREACH(head,el) \
    for(el=head;el;el=el->next)

/******************************************************************************
 * doubly linked list macros (non-circular)                                   *
 *****************************************************************************/
#define DL_ADD(head,add)                                        \
    do {                                                        \
        if (head) {                                             \
            (add)->prev = (head)->prev;                         \
            (head)->prev->next = (add);                         \
            (head)->prev = (add);                               \
            (add)->next = NULL;                                 \
        } else {                                                \
            (head)=(add);                                       \
            (head)->prev = (head);                              \
            (head)->next = NULL;                                \
        }                                                       \
    } while (0);

#define DL_DEL(head,del)                                        \
    do {                                                        \
        if ((del)->prev == (del)) {                             \
            (head)=NULL;                                        \
        } else if ((del)==(head)) {                             \
            (del)->next->prev = (del)->prev;                    \
            (head) = (del)->next;                               \
        } else {                                                \
            (del)->prev->next = (del)->next;                    \
            if ((del)->next) {                                  \
                (del)->next->prev = (del)->prev;                \
            } else {                                            \
                (head)->prev = (del)->prev;                     \
            }                                                   \
        }                                                       \
    } while (0);


#define DL_FOREACH(head,el) \
    for(el=head;el;el=el->next)

/******************************************************************************
 * circular doubly linked list macros                                         *
 *****************************************************************************/
#define CDL_ADD(head,add)                                       \
do {                                                            \
    if (head) {                                                 \
      (add)->next = (head)->next;                               \
      (add)->prev = (head);                                     \
      (head)->next->prev = (add);                               \
      (head)->next = (add);                                     \
    } else {                                                    \
      (head)=(add);                                             \
      (add)->prev = (add);                                      \
      (add)->next = (add);                                      \
    }                                                           \
 } while (0);

#define CDL_DEL(head,del)                                       \
do {                                                            \
    if ( ((head)==(del)) && ((head)->next == (head))) {         \
        (head) = 0L;                                            \
    } else {                                                    \
       (del)->next->prev = (del)->prev;                         \
       (del)->prev->next = (del)->next;                         \
       if ((del) == (head)) (head)=(del)->next;                 \
    }                                                           \
} while (0);

#define CDL_FOREACH(head,el) \
    for(el=head;el;el= (el->next==head ? 0L : el->next)) 

/* This is an adaptation of Simon Tatham's O(n log(n)) mergesort */
#define FIELD_OFFSET(ptr,field) ((char*)&((ptr)->field) - (char*)(ptr))
#define LL_SORT(l,cmp) LISTSORT(l,0,0,FIELD_OFFSET(l,next),cmp)
#define DL_SORT(l,cmp) LISTSORT(l,0,FIELD_OFFSET(l,prev),FIELD_OFFSET(l,next),cmp)
#define CDL_SORT(l,cmp) LISTSORT(l,1,FIELD_OFFSET(l,prev),FIELD_OFFSET(l,next),cmp)
#define NEXT(e,no) (*(char**)(((char*)e) + no))
#define PREV(e,po) (*(char**)(((char*)e) + po))
#define LISTSORT(list, is_circular, po, no, cmp) \
do { \
    void *p, *q, *e, *tail, *oldhead; \
    int insize, nmerges, psize, qsize, i, looping;  \
    int is_double = (po==0) ? 0 : 1; \
  \
    if (list) { \
  \
      insize = 1; \
      looping = 1;  \
  \
      while (looping) { \
          p = list; \
          oldhead = list;                \
          list = NULL;  \
          tail = NULL;  \
  \
          nmerges = 0;  \
  \
          while (p) { \
              nmerges++;  \
              \
              q = p;  \
              psize = 0;  \
              for (i = 0; i < insize; i++) {  \
                  psize++;  \
                  if (is_circular)  { \
                      q = ((NEXT(q,no) == oldhead) ? NULL : NEXT(q,no));  \
                  } else  { \
                      q = NEXT(q,no);  \
                  } \
                  if (!q) break;  \
              } \
  \
              \
              qsize = insize; \
  \
              \
              while (psize > 0 || (qsize > 0 && q)) { \
  \
                  \
                  if (psize == 0) { \
                      \
                      e = q; q = NEXT(q,no); qsize--;  \
                      if (is_circular && q == oldhead) { q = NULL; }  \
                  } else if (qsize == 0 || !q) {  \
                      \
                      e = p; p = NEXT(p,no); psize--;  \
                      if (is_circular && p == oldhead) { p = NULL; } \
                  } else if (cmp(p,q) <= 0) { \
                      \
                      \
                      e = p; p = NEXT(p,no); psize--;  \
                      if (is_circular && p == oldhead) { p = NULL; } \
                  } else {  \
                      \
                      e = q; q = NEXT(q,no); qsize--;  \
                      if (is_circular && q == oldhead) { q = NULL; } \
                  } \
  \
                  \
                  if (tail) { \
                      NEXT(tail,no) = e; \
                  } else {  \
                      list = e; \
                  } \
                  if (is_double) {  \
                      \
                      PREV(e,po) = tail; \
                  } \
                  tail = e; \
              } \
  \
              \
              p = q;  \
          } \
          if (is_circular) {  \
              NEXT(tail,no) = (char*)list;  \
              if (is_double) { \
                  PREV(list,po) = tail;  \
              } \
          } else  { \
              NEXT(tail,no) = NULL;  \
          } \
  \
          \
          if (nmerges <= 1) { \
              looping=0;  \
              \
          } \
  \
          insize *= 2;  \
      } \
    } \
} while (0)

#endif /* UTLIST_H */

