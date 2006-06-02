#include <string.h>  /* strcpy */
#include <stdlib.h>  /* malloc */
#include <stdio.h>   /* printf */
#include "uthash.h"

struct my_struct {
    char name[10];             /* key */
    int id;                    
    UT_hash_handle hh;         /* makes this structure hashable */
};


int main(int argc, char *argv[]) {
    char **n, *names[] = { "joe", "bob", "betty", NULL };
    struct my_struct *s, *users = NULL;
    int i=0;

    for (n = names; *n != NULL; n++) {
        s = malloc(sizeof(struct my_struct));
        strcpy(s->name, *n);
        s->id = i++;
        HASH_ADD_STR( users, name, s );  /* hash, key field name, item */
    }

    /* args are hash, output pointer, key field name, key sought */
    HASH_FIND_STR( users, s, name, "betty");
    if (s) {
        printf("betty's id is %d\n", s->id);
    }
}
