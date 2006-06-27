#include <sys/types.h>  /* for 'open' */
#include <sys/stat.h>   /* for 'open' */
#include <fcntl.h>      /* for 'open' */
#include <stdlib.h>     /* for 'malloc' */
#include <stdio.h>      /* for 'printf' */
#include <unistd.h>     /* for 'read' */
#include <errno.h>      /* for 'sterror' */
#include <sys/time.h>   /* for 'gettimeofday' */
#include "uthash.h"

#ifndef timersub
#define timersub(a, b, result)                                                \
  do {                                                                        \
    (result)->tv_sec = (a)->tv_sec - (b)->tv_sec;                             \
    (result)->tv_usec = (a)->tv_usec - (b)->tv_usec;                          \
    if ((result)->tv_usec < 0) {                                              \
      --(result)->tv_sec;                                                     \
      (result)->tv_usec += 1000000;                                           \
    }                                                                         \
  } while (0)
#endif

typedef struct stat_key {
    char *key;
    int len;
    UT_hash_handle hh, hh2;
} stat_key;

int main(int argc, char *argv[]) {
    int dups=0, rc, fd, done=0, err=0, keylen;
    char *key, *filename = "/dev/stdin"; 
    stat_key *keyt, *keytmp, *keys=NULL, *keys2=NULL;
    struct timeval start_tm, end_tm, elapsed_tm, elapsed_tm2;

    if (argc > 1) filename=argv[1];
    fd=open(filename,O_RDONLY);

    if ( fd == -1 ) {
        fprintf(stderr,"open failed %s: %s\n", filename, strerror(errno));
        return -1;
    }

    while (!done) {
          rc = read(fd,&keylen,sizeof(int));
          if (rc != sizeof(int)) {
              if (rc == 0) done=1;
              else err=1;
              if (rc == -1) fprintf(stderr,"read failed: %s\n", strerror(errno));
              else if (rc > 0) fprintf(stderr,"incomplete file\n");
          }
          if (done || err) break;
  
          if ( (keyt = (stat_key*)malloc(sizeof(stat_key))) == NULL) {
              fprintf(stderr,"out of memory\n");
              exit(-1);
          }
  
          /* read key */
          if ( (keyt->key = (char*)malloc(keylen)) == NULL) {
              fprintf(stderr,"out of memory\n");
              exit(-1);
          }
          keyt->len = keylen;
  
          rc = read(fd,keyt->key,keylen);
          if (rc != keylen) {
              if (rc == -1) fprintf(stderr,"read failed: %s\n", strerror(errno));
              else if (rc >= 0) fprintf(stderr,"incomplete file\n");
              err=1;
          }
          if (err) break;
  
          /* eliminate dups */
          HASH_FIND(hh,keys,keyt->key,keylen,keytmp);
          if (keytmp) {
              dups++;
              free(keyt->key);
            free(keyt);
          } else {
            HASH_ADD_KEYPTR(hh,keys,keyt->key,keylen,keyt);
          }
    }

    /* add all keys to a new hash, so we can measure add time w/o malloc */
    gettimeofday(&start_tm,NULL);
    for(keyt = keys; keyt != NULL; keyt=keyt->hh.next) {
        HASH_ADD_KEYPTR(hh2,keys2,keyt->key,keyt->len,keyt);
    }
    gettimeofday(&end_tm,NULL);
    timersub(&end_tm, &start_tm, &elapsed_tm);

    /* now look up all keys in the new hash, again measuring elapsed time */
    gettimeofday(&start_tm,NULL);
    for(keyt = keys; keyt != NULL; keyt=keyt->hh.next) {
        HASH_FIND(hh2,keys2,keyt->key,keyt->len,keytmp);
        if (!keytmp) fprintf(stderr,"internal error, key not found\n");
    }
    gettimeofday(&end_tm,NULL);
    timersub(&end_tm, &start_tm, &elapsed_tm2);


    if (!err) {
        printf("%f,%d,%d,%d,%s,%ld,%ld\n",
        keys->hh.tbl->hash_q, 
        keys->hh.tbl->num_items, 
        keys->hh.tbl->num_buckets, 
        dups,
        (keys->hh.tbl->noexpand ? "noexpand" : "ok"),
        (elapsed_tm.tv_sec * 1000000) + elapsed_tm.tv_usec,
        (elapsed_tm2.tv_sec * 1000000) + elapsed_tm2.tv_usec );
    }
}

