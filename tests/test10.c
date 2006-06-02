#include "uthash.h"
#include <stdlib.h>   /* malloc */
#include <stdio.h>    /* printf */

typedef struct example_user_t {
    int id;
    int cookie;
    UT_hash_handle hh;
    UT_hash_handle alth;
} example_user_t;

int main(int argc,char *argv[]) {
    int i;
    example_user_t *user, *tmp, *users=NULL, *altusers=NULL;

    /* create elements */
    for(i=0;i<1000;i++) {
        if ( (user = malloc(sizeof(example_user_t))) == NULL) exit(-1);
        user->id = i;
        user->cookie = i*i;
        if (i<10) HASH_ADD_INT(users,id,user);
        HASH_ADD(alth,altusers,id,sizeof(int),user);
    }

    printf("hh items: %d, alth items: %d\n", 
            users->hh.tbl->num_items, users->alth.tbl->num_items);
    printf("hh buckets: %d, alth buckets: %d\n", 
            users->hh.tbl->num_buckets, users->alth.tbl->num_buckets);
    printf("hh hash_q: %f, alth hash_q: %f\n", 
            users->hh.tbl->hash_q, users->alth.tbl->hash_q);

    i=9;
    HASH_FIND_INT(users,tmp,id,&i);
    printf("%d %s in hh\n", i, (tmp ? "found" : "not found"));
    HASH_FIND(alth,altusers,tmp,id,&i,sizeof(int));
    printf("%d %s in alth\n", i, (tmp ? "found" : "not found"));

    i=10;
    HASH_FIND_INT(users,tmp,id,&i);
    printf("%d %s in hh\n", i, (tmp ? "found" : "not found"));
    HASH_FIND(alth,altusers,tmp,id,&i,sizeof(int));
    printf("%d %s in alth\n", i, (tmp ? "found" : "not found"));

}
