#include "uthash.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

/* This function authored by James Read */
void create_translation_model_hash_table ( int *source_text, int *target_text )
{
 struct translation
 {
   int target_word;
   int source_word;
   double probability;
   double count;
   UT_hash_handle hh;
 };
 struct
 {
   int target_word;
   int source_word;
 } lookup_key;
 struct translation *translations = NULL;
 int key_length;
 int number_of_source_words = source_text[0];
 double initial_probability = 1.0 / number_of_source_words;
 int number_of_keys = 0;
 int source_count, tmp_target_count, target_count, tmp_source_count;
 source_count = tmp_source_count = target_count = tmp_target_count = 0;
 source_count = 1;
 target_count = 1;
 while ( source_count < number_of_source_words )
 {
   while ( source_text[source_count] != 0 )
   {
     tmp_target_count = target_count;
     while ( target_text[tmp_target_count] != 0 )
     {
       //printf ("%i %i \n", source_text[source_count], target_text[tmp_target_count]);
       struct translation *search_result;
       lookup_key.target_word = target_text[tmp_target_count];
       lookup_key.source_word = source_text[source_count];
       key_length = offsetof(struct translation, source_word)
                  + sizeof (int)
                  - offsetof (struct translation, target_word);
       HASH_FIND ( hh, translations, &lookup_key.target_word, key_length, search_result );
       if ( search_result )
       {
         //printf ("Key %i %i already exists\n", target_text[tmp_target_count], source_text[source_count] );
       }
       else
       {
         struct translation *this_translation;
         this_translation = malloc ( sizeof ( struct translation ) );
         memset ( this_translation, 0, sizeof ( this_translation ) );
         this_translation->target_word = target_text[tmp_target_count];
         this_translation->source_word = source_text[source_count];
         this_translation->probability = initial_probability;
         this_translation->count = 0;
         key_length = offsetof(struct translation, source_word)
                    + sizeof (int)
                    - offsetof (struct translation, target_word);
         HASH_ADD( hh, translations, target_word, key_length, this_translation );
         //printf ("Adding key %i %i\n", target_text[tmp_target_count], source_text[source_count] );
         number_of_keys++;
         if ( number_of_keys % 100000 == 0 )
         {
           printf ( "%i keys in hash table\n", number_of_keys );
         }
       }
       tmp_target_count++;
     }
     tmp_target_count++;
     source_count++;
   }
   target_count = tmp_target_count++;
   source_count++;
 }
}

#define NUM_SEQS 1000000
#define SEQ_MAXLEN 20
int main() {
  int i, j, alen, blen, *a, *b;
  int *src=NULL, srclen=1, *tgt=NULL, tgtlen=1;
  for(i=0; i<NUM_SEQS; i++) {
    alen = (rand() % SEQ_MAXLEN) + 1;
    blen = (rand() % SEQ_MAXLEN) + 1;
    a = malloc(sizeof(int)*alen);
    b = malloc(sizeof(int)*blen);
    for(j=0; j<alen; j++) a[j]=rand();
    for(j=0; j<blen; j++) b[j]=rand();
    /* printf("seq %d alen %u blen %u\n",i,alen,blen); */

    src = realloc(src, (srclen*sizeof(int)) + (alen*sizeof(int)) + 1);
    tgt = realloc(tgt, (tgtlen*sizeof(int)) + (blen*sizeof(int)) + 1);
    if (src == NULL || tgt == NULL) {fprintf(stderr,"oom\n"); exit(-1);}
    if (i==0) src[i]=NUM_SEQS;
    memcpy(&src[srclen], a, alen*sizeof(int));
    memcpy(&tgt[tgtlen], b, blen*sizeof(int));
    src[srclen+alen]=0;
    tgt[tgtlen+blen]=0;
    srclen += alen + 1;
    tgtlen += blen + 1;
    free(a); free(b);
  }
  printf("srclen %u tgtlen %u\n", srclen, tgtlen);
  printf("creating the hash table\n");
  create_translation_model_hash_table(src,tgt);
  free(src); free(tgt);
  return 0;
}
