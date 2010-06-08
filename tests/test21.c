#include <stdlib.h>
#include <stdio.h>
#include "uthash.h"

typedef struct {
  char a;
  int b;
} record_key_t;

typedef struct {
    record_key_t key;
    /* ... other data ... */
    UT_hash_handle hh;
} record_t;

int main(int argc, char *argv[]) {
    record_t l, *p, *r, *records = NULL;

    r = (record_t*)malloc( sizeof(record_t) );
    memset(r, 0, sizeof(record_t));
    r->key.a = 'a';
    r->key.b = 1;
    HASH_ADD(hh, records, key, sizeof(record_key_t), r);

    memset(&l, 0, sizeof(record_t));
    l.key.a = 'a';
    l.key.b = 1;
    HASH_FIND(hh, records, &l.key, sizeof(record_key_t), p);

    if (p) printf("found %c %d\n", p->key.a, p->key.b);
    return 0;
}

