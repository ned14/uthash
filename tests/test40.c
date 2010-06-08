#include <string.h>  /* strcpy */
#include <stdlib.h>  /* malloc */
#include <stdio.h>   /* printf */
#include "uthash.h"

struct my_struct {
    const char *name;          /* key */
    int id;                    
    UT_hash_handle hh;         /* makes this structure hashable */
};


int main(int argc, char *argv[]) {
    const char **n, *names[] = { "joe", "bob", "betty", NULL };
    struct my_struct *s, *users = NULL;
    int i=0;

    for (n = names; *n != NULL; n++) {
        s = (struct my_struct*)malloc(sizeof(struct my_struct));
        s->name = *n;
        s->id = i++;
        HASH_ADD_KEYPTR( hh, users, s->name, strlen(s->name), s );  
    }

    HASH_FIND_STR( users, "betty", s);
    if (s) printf("betty's id is %d\n", s->id);
   return 0;
}
