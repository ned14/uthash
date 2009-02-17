#include "uthash.h"
#include <stdlib.h>   /* malloc */
#include <stdio.h>    /* printf */

/* Set up macros for alternative malloc/free functions  */
#undef uthash_bkt_malloc
#undef uthash_bkt_free
#undef uthash_tbl_malloc
#undef uthash_tbl_free
#define uthash_bkt_malloc(sz) alt_bkt_malloc(sz)
#define uthash_bkt_free(ptr) alt_bkt_free(ptr)
#define uthash_tbl_malloc(sz) alt_tbl_malloc(sz)
#define uthash_tbl_free(ptr) alt_tbl_free(ptr)

#undef uthash_noexpand_fyi 
#undef uthash_expand_fyi 
#define uthash_noexpand_fyi(tbl) printf("noexpand set\n");
#define uthash_expand_fyi(tbl) printf("hash expanded\n");

typedef struct example_user_t {
    int id;
    int cookie;
    UT_hash_handle hh;
} example_user_t;

void *alt_bkt_malloc(size_t sz) {
    printf("%s\n", "alt_bkt_malloc");
    return malloc(sz);
}
void alt_bkt_free(void *ptr) {
    printf("%s\n", "alt_bkt_free");
    free(ptr);
}
void *alt_tbl_malloc(size_t sz) {
    printf("%s\n", "alt_tbl_malloc");
    return malloc(sz);
}
void alt_tbl_free(void *ptr) {
    printf("%s\n", "alt_tbl_free");
    free(ptr);
}

int main(int argc,char *argv[]) {
    int i;
    example_user_t *user, *tmp, *users=NULL;

    /* create elements */
    for(i=0;i<1000;i++) {
        if ( (user = (example_user_t*)malloc(sizeof(example_user_t))) == NULL) exit(-1);
        user->id = i;
        user->cookie = i*i;
        HASH_ADD_INT(users,id,user);
    }

    /* delete each ID */
    for(i=0;i<1000;i+=2) {
        HASH_FIND_INT(users,&i,tmp);
        if (tmp) {
            printf("user %d, cookie %d\n", tmp->id, tmp->cookie);
        } else printf("user id %d not found\n", i);
    }
   return 0;
}
