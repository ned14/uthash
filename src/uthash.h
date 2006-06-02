/*
Copyright (c) 2003-2006, Troy Hanson     http://uthash.sourceforge.net
All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:

    * Redistributions of source code must retain the above copyright
      notice, this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright
      notice, this list of conditions and the following disclaimer in
      the documentation and/or other materials provided with the
      distribution.
    * Neither the name of the copyright holder nor the names of its
      contributors may be used to endorse or promote products derived
      from this software without specific prior written permission.

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

#include <string.h> /* memcmp,strlen */

#ifndef UTHASH_H
#define UTHASH_H 

#define uthash_fatal(msg) exit(-1)        /* fatal error (out of memory,etc) */
#define uthash_bkt_malloc(sz) malloc(sz)  /* malloc fcn for UT_hash_bucket's */
#define uthash_bkt_free(ptr) free(ptr)    /* free fcn for UT_hash_bucket's   */
#define uthash_tbl_malloc(sz) malloc(sz)  /* malloc fcn for UT_hash_table    */
#define uthash_tbl_free(ptr) free(ptr)    /* free fcn for UT_hash_table      */

#define uthash_noexpand_fyi(tbl)          /* can be defined to log noexpand  */
#define uthash_expand_fyi(tbl)            /* can be defined to log expands   */

/* initial number of buckets */
#define UT_HASH_INITIAL_NUM_BUCKETS 32  /* initial number of buckets        */
#define UT_HASH_BKT_CAPACITY_THRESH 10  /* expand when bucket count reaches */

#define HASH_FIND(hh,head,out,field,keyptr,keylen_in)                    \
do {                                                                     \
  out=head;                                                              \
  if (head) {                                                            \
     (head)->hh.tbl->key = (char*)(keyptr);                              \
     (head)->hh.tbl->keylen = keylen_in;                                 \
     UT_HASH((head)->hh.tbl->key,(head)->hh.tbl->keylen,                 \
             (head)->hh.tbl->num_buckets,(head)->hh.tbl->bkt);           \
     HASH_FIND_IN_BKT(hh, (head)->hh.tbl->buckets[ (head)->hh.tbl->bkt], \
             out,field,keyptr,keylen_in);                                \
  }                                                                      \
} while (0)

#define HASH_ADD(hh,head,fieldname,fieldlen,add)                         \
do {                                                                     \
 add->hh.next = NULL;                                                    \
 add->hh.key = &add->fieldname;                                          \
 add->hh.keylen = fieldlen;                                              \
 add->hh.elmt = add;                                                     \
 if (!(head)) {                                                          \
    head = add;                                                          \
    (head)->hh.prev = NULL;                                              \
    (head)->hh.tbl = (UT_hash_table*)uthash_tbl_malloc(                  \
                    sizeof(UT_hash_table));                              \
    if (!((head)->hh.tbl))  { uthash_fatal( "out of memory"); }          \
    (head)->hh.tbl->name = #head;                                        \
    (head)->hh.tbl->tail = &(add->hh);                                   \
    (head)->hh.tbl->noexpand = 0;                                        \
    (head)->hh.tbl->hash_q = 1;                                          \
    (head)->hh.tbl->num_buckets = UT_HASH_INITIAL_NUM_BUCKETS;           \
    (head)->hh.tbl->num_items = 0;                                       \
    (head)->hh.tbl->hho = ((long)(&add->hh) - (long)(add));              \
    (head)->hh.tbl->buckets = (UT_hash_bucket*)uthash_bkt_malloc(        \
            UT_HASH_INITIAL_NUM_BUCKETS*sizeof(struct UT_hash_bucket));  \
    if (! (head)->hh.tbl->buckets) { uthash_fatal( "out of memory"); }   \
    memset((head)->hh.tbl->buckets, 0,                                   \
            UT_HASH_INITIAL_NUM_BUCKETS*sizeof(struct UT_hash_bucket));  \
 } else {                                                                \
    (head)->hh.tbl->tail->next = add;                                    \
    add->hh.prev = (head)->hh.tbl->tail->elmt;                           \
    (head)->hh.tbl->tail = &(add->hh);                                   \
 }                                                                       \
 (head)->hh.tbl->num_items++;                                            \
 add->hh.tbl = (head)->hh.tbl;                                           \
 (head)->hh.tbl->key = (char*)&add->fieldname;                           \
 (head)->hh.tbl->keylen = fieldlen;                                      \
 UT_HASH((head)->hh.tbl->key,(head)->hh.tbl->keylen,                     \
         (head)->hh.tbl->num_buckets,(head)->hh.tbl->bkt);               \
 HASH_ADD_TO_BKT(hh,(head)->hh.tbl->buckets[(head)->hh.tbl->bkt],add);   \
 HASH_FSCK(head);                                                        \
} while(0)

#define HASH_DELETE(hh,head,delptr)                                      \
do {                                                                     \
    if ( ((delptr)->hh.prev == NULL) && ((delptr)->hh.next == NULL) )  { \
        uthash_bkt_free((head)->hh.tbl->buckets );                       \
        uthash_tbl_free((head)->hh.tbl);                                 \
        head = NULL;                                                     \
    } else {                                                             \
        if ((delptr) == (head)->hh.tbl->tail->elmt) {                    \
            (head)->hh.tbl->tail = (void*)(((long)((delptr)->hh.prev)) + \
                                           (head)->hh.tbl->hho);         \
        }                                                                \
        if ((delptr)->hh.prev) {                                         \
            ((UT_hash_handle*)(((long)((delptr)->hh.prev)) +             \
                    (head)->hh.tbl->hho))->next = (delptr)->hh.next;     \
        } else {                                                         \
            head = (delptr)->hh.next;                                    \
        }                                                                \
        if ((delptr)->hh.next) {                                         \
            ((UT_hash_handle*)(((long)((delptr)->hh.next)) +             \
                    (head)->hh.tbl->hho))->prev = (delptr)->hh.prev;     \
        }                                                                \
        (head)->hh.tbl->key = (char*)((delptr)->hh.key);                 \
        (head)->hh.tbl->keylen = (delptr)->hh.keylen;                    \
        UT_HASH((head)->hh.tbl->key,(head)->hh.tbl->keylen,              \
                (head)->hh.tbl->num_buckets,(head)->hh.tbl->bkt);        \
        HASH_DEL_IN_BKT(hh,(head)->hh.tbl->buckets[(head)->hh.tbl->bkt], \
                delptr);                                                 \
        (head)->hh.tbl->num_items--;                                     \
    }                                                                    \
    HASH_FSCK(head);                                                     \
} while (0)


/* convenience forms of HASH_FIND/HASH_ADD/HASH_DEL */
#define HASH_FIND_STR(head,out,field,findstr)                           \
    HASH_FIND(hh,head,out,field,findstr,strlen(findstr))
#define HASH_ADD_STR(head,strfield,add)                                 \
    HASH_ADD(hh,head,strfield,strlen(add->strfield),add)
#define HASH_FIND_INT(head,out,field,findint)                           \
    HASH_FIND(hh,head,out,field,findint,sizeof(int))
#define HASH_ADD_INT(head,intfield,add)                                 \
    HASH_ADD(hh,head,intfield,sizeof(int),add)
#define HASH_DEL(head,delptr)                                           \
    HASH_DELETE(hh,head,delptr)

/* HASH_FSCK checks hash integrity on every add/delete when HASH_DEBUG is defined.
 * This is for uthash developer only; it compiles away if HASH_DEBUG isn't defined.
 * This function misuses fields in UT_hash_table for its bookkeeping variables.
 */
#ifdef HASH_DEBUG
#define HASH_OOPS(...) do { fprintf(stderr,__VA_ARGS__); exit(-1); } while (0)
#define HASH_FSCK(head)                                                  \
do {                                                                     \
    if (head) {                                                          \
        (head)->hh.tbl->keylen = 0;   /* item counter */                 \
        for(    (head)->hh.tbl->bkt_i = 0;                               \
                (head)->hh.tbl->bkt_i < (head)->hh.tbl->num_buckets;     \
                (head)->hh.tbl->bkt_i++)                                 \
        {                                                                \
            (head)->hh.tbl->bkt_ideal = 0; /* bkt item counter */        \
            (head)->hh.tbl->hh =                                         \
            (head)->hh.tbl->buckets[(head)->hh.tbl->bkt_i].hh_head;      \
            (head)->hh.tbl->key = NULL;  /* hh_prev */                   \
            while ((head)->hh.tbl->hh) {                                 \
               if ((head)->hh.tbl->key !=                                \
                   (char*)((head)->hh.tbl->hh->hh_prev)) {               \
                   HASH_OOPS("invalid hh_prev %x, actual %x\n",          \
                    (head)->hh.tbl->hh->hh_prev,                         \
                    (head)->hh.tbl->key );                               \
               }                                                         \
               (head)->hh.tbl->bkt_ideal++;                              \
               (head)->hh.tbl->key = (char*)((head)->hh.tbl->hh);        \
               (head)->hh.tbl->hh = (head)->hh.tbl->hh->hh_next;         \
            }                                                            \
            (head)->hh.tbl->keylen +=  (head)->hh.tbl->bkt_ideal;        \
            if ((head)->hh.tbl->buckets[(head)->hh.tbl->bkt_i].count     \
               !=  (head)->hh.tbl->bkt_ideal) {                          \
               HASH_OOPS("invalid bucket count %d, actual %d\n",         \
                (head)->hh.tbl->buckets[(head)->hh.tbl->bkt_i].count,    \
                (head)->hh.tbl->bkt_ideal);                              \
            }                                                            \
        }                                                                \
        if ((head)->hh.tbl->keylen != (head)->hh.tbl->num_items) {       \
            HASH_OOPS("invalid hh item count %d, actual %d\n",           \
                (head)->hh.tbl->num_items, (head)->hh.tbl->keylen );     \
        }                                                                \
        /* traverse hh in app order; check next/prev integrity, count */ \
        (head)->hh.tbl->keylen = 0;   /* item counter */                 \
        (head)->hh.tbl->key = NULL;  /* app prev */                      \
        (head)->hh.tbl->hh =  &(head)->hh;                               \
        while ((head)->hh.tbl->hh) {                                     \
           (head)->hh.tbl->keylen++;                                     \
           if ((head)->hh.tbl->key !=(char*)((head)->hh.tbl->hh->prev)) {\
              HASH_OOPS("invalid prev %x, actual %x\n",                  \
                    (head)->hh.tbl->hh->prev,                            \
                    (head)->hh.tbl->key );                               \
           }                                                             \
           (head)->hh.tbl->key = (head)->hh.tbl->hh->elmt;               \
           (head)->hh.tbl->hh = ( (head)->hh.tbl->hh->next ?             \
             (UT_hash_handle*)((long)((head)->hh.tbl->hh->next) +        \
                               (head)->hh.tbl->hho)                      \
                                 : NULL );                               \
        }                                                                \
        if ((head)->hh.tbl->keylen != (head)->hh.tbl->num_items) {       \
            HASH_OOPS("invalid app item count %d, actual %d\n",          \
                (head)->hh.tbl->num_items, (head)->hh.tbl->keylen );     \
        }                                                                \
    }                                                                    \
} while (0)
#else
#define HASH_FSCK(head) 
#endif


/* The Bernstein hash function, used in Perl prior to v5.6 */
#define UT_HASH(key,keylen,num_bkts,bkt)          \
  bkt = 0;                                        \
  while (keylen--)  bkt = (bkt * 33) + *key++;    \
  bkt &= (num_bkts-1);          

/* key comparison function; return 0 if keys equal */
#define HASH_KEYCMP(a,b,len) memcmp(a,b,len) 

/* iterate over items in a known bucket to find desired item */
#define HASH_FIND_IN_BKT(hh,head,out,field,keyptr,keylen_in)         \
out = (head.hh_head) ? (head.hh_head->elmt) : NULL;                  \
while (out) {                                                        \
    if (out->hh.keylen == keylen_in) {                               \
        if ((HASH_KEYCMP(&out->field,keyptr,keylen_in)) == 0) break; \
    }                                                                \
    out= (out->hh.hh_next) ? (out->hh.hh_next->elmt) : NULL;         \
}

/* add an item to a bucket  */
#define HASH_ADD_TO_BKT(hh,head,add)                                 \
 head.count++;                                                       \
 add->hh.hh_next = head.hh_head;                                     \
 add->hh.hh_prev = NULL;                                             \
 if (head.hh_head) head.hh_head->hh_prev = &add->hh;                 \
 head.hh_head=&add->hh;                                              \
 if (head.count >= UT_HASH_BKT_CAPACITY_THRESH &&                    \
     add->hh.tbl->noexpand != 1) {                                   \
       HASH_EXPAND_BUCKETS(add->hh.tbl)                              \
 }

/* remove an item from a given bucket */
#define HASH_DEL_IN_BKT(hh,head,delptr)                              \
    (head).count--;                                                  \
    if ((head).hh_head->elmt == delptr) {                            \
      (head).hh_head = delptr->hh.hh_next;                           \
    }                                                                \
    if (delptr->hh.hh_prev) {                                        \
        delptr->hh.hh_prev->hh_next = delptr->hh.hh_next;            \
    }                                                                \
    if (delptr->hh.hh_next) {                                        \
        delptr->hh.hh_next->hh_prev = delptr->hh.hh_prev;            \
    }                                                                

#define HASH_EXPAND_BUCKETS(tbl)                                     \
    tbl->new_buckets = (UT_hash_bucket*)uthash_bkt_malloc(           \
             2 * tbl->num_buckets * sizeof(struct UT_hash_bucket));  \
    if (!tbl->new_buckets) { uthash_fatal( "out of memory"); }       \
    memset(tbl->new_buckets, 0,                                      \
            2 * tbl->num_buckets * sizeof(struct UT_hash_bucket));   \
    tbl->bkt_ideal= (tbl->num_items /  tbl->num_buckets*2) +         \
                   ((tbl->num_items % (tbl->num_buckets*2)) ? 1 : 0);\
    tbl->sum_of_deltas = 0;                                          \
    for(tbl->bkt_i = 0; tbl->bkt_i < tbl->num_buckets; tbl->bkt_i++) \
    {                                                                \
        tbl->hh = tbl->buckets[ tbl->bkt_i ].hh_head;                \
        while (tbl->hh) {                                            \
           tbl->hh_nxt = tbl->hh->hh_next;                           \
           tbl->key = tbl->hh->key;                                  \
           tbl->keylen = tbl->hh->keylen;                            \
           UT_HASH(tbl->key,tbl->keylen,tbl->num_buckets*2,tbl->bkt);\
           tbl->newbkt = &(tbl->new_buckets[ tbl->bkt ]);            \
           if (++(tbl->newbkt->count) > tbl->bkt_ideal) {            \
             tbl->sum_of_deltas++;                                   \
           }                                                         \
           tbl->hh->hh_prev = NULL;                                  \
           tbl->hh->hh_next = tbl->newbkt->hh_head;                  \
           if (tbl->newbkt->hh_head) tbl->newbkt->hh_head->hh_prev = \
                tbl->hh;                                             \
           tbl->newbkt->hh_head = tbl->hh;                           \
           tbl->hh = tbl->hh_nxt;                                    \
        }                                                            \
    }                                                                \
    tbl->num_buckets *= 2;                                           \
    uthash_bkt_free( tbl->buckets );                                 \
    tbl->buckets = tbl->new_buckets;                                 \
    tbl->new_hash_q = 1-(tbl->sum_of_deltas * 1.0 / tbl->num_items); \
    if (tbl->hash_q < 0.5 && tbl->new_hash_q < 0.5) {                \
        tbl->noexpand=1;                                             \
        uthash_noexpand_fyi(tbl);                                    \
    }                                                                \
    tbl->hash_q = tbl->new_hash_q;                                   \
    uthash_expand_fyi(tbl);                                         


typedef struct UT_hash_bucket {
   struct UT_hash_handle *hh_head;
   unsigned count;  
} UT_hash_bucket;

typedef struct UT_hash_table {
   UT_hash_bucket *buckets;
   unsigned num_buckets;
   unsigned num_items;
   int noexpand;  /* when set, inhibits expansion of buckets for this hash  */
   double hash_q; /* measures the evenness of the items among buckets (0-1) */
   struct UT_hash_handle *tail; /* tail hh in app order, for fast append    */
   char *name;    /* macro-stringified name of list head, used by libut     */
   int hho;
   /* scratch */
   unsigned bkt;
   char *key;
   int keylen;
   /* scratch for bucket expansion */
   UT_hash_bucket *new_buckets, *newbkt;
   struct UT_hash_handle *hh, *hh_nxt;
   unsigned bkt_i, bkt_ideal, sum_of_deltas;
   double new_hash_q;
   
} UT_hash_table;


typedef struct UT_hash_handle {
   struct UT_hash_table *tbl;
   void *elmt;                       /* ptr to enclosing element       */
   void *prev;                       /* prev element in app order      */
   void *next;                       /* next element in app order      */
   struct UT_hash_handle *hh_prev;   /* previous hh in bucket order    */
   struct UT_hash_handle *hh_next;   /* next hh in bucket order        */
   void *key;                        /* ptr to enclosing struct's key  */
   int keylen;                       /* enclosing struct's key len     */
} UT_hash_handle;

#endif /* UTHASH_H */
