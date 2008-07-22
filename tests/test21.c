#include <stdlib.h>
#include <stdio.h>
#include "uthash.h"

typedef struct {
    double flux; 
    /* ... other data ... */
    UT_hash_handle hh;
} flux_t;

int main(int argc, char *argv[]) {
    flux_t *f, *f2, *flux_hash_table = NULL;
    double x = 1/3.0;

    f = malloc( sizeof(*f) );
    f->flux = x;
    HASH_ADD(hh, flux_hash_table, flux, sizeof(double), f);
    HASH_FIND(hh, flux_hash_table, &x, sizeof(double), f2 );

    if (f2) printf("found (%.2f)\n", f2->flux);
}
