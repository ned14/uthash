#include <stdio.h>
#include "utlist.h"

typedef struct el {
    int id;
    struct el *next, *prev;
} el;

el *head = NULL;

int main(int argc, char *argv[]) {
    int i;
    el els[10], *e, *f;
    for(i=0;i<10;i++) els[i].id='a'+i;

    /* test CDL macros */
    printf("CDL macros\n");
    CDL_ADD(head,&els[0]);
    CDL_ADD(head,&els[1]);
    CDL_ADD(head,&els[2]);
    CDL_FOREACH(head,e) 
        printf("%c ", e->id); 
    printf("\n");

    /* point head to head->next */
    printf("advancing head pointer\n");
    head = head->next;
    CDL_FOREACH(head,e) 
        printf("%c ", e->id); 
    printf("\n");

    /* follow circular loop a few times */
    for(i=0,e=head;e && i<10;i++,e=e->next) 
        printf("%c ", e->id); 
    printf("\n");

    /* follow circular loop backwards a few times */
    for(i=0,e=head;e && i<10;i++,e=e->prev) 
        printf("%c ", e->id); 
    printf("\n");

    printf("deleting b\n");
    CDL_DEL(head,&els[1]);
    CDL_FOREACH(head,e) printf("%c ", e->id); 
    printf("\n");
    printf("deleting head (a)\n");
    CDL_DEL(head,&els[0]);
    CDL_FOREACH(head,e) 
        printf("%c ", e->id); 
    printf("\n");
    printf("deleting new head (c)\n");
    CDL_DEL(head,&els[2]);
    CDL_FOREACH(head,e) 
        printf("%c ", e->id); 
    printf("\n");

    /* test DL macros */
    printf("DL macros\n");
    DL_ADD(head,&els[0]);
    DL_ADD(head,&els[1]);
    DL_ADD(head,&els[2]);
    DL_FOREACH(head,e) 
        printf("%c ", e->id); 
    printf("\n");

    printf("deleting tail c\n");
    DL_DEL(head,&els[2]);
    DL_FOREACH(head,e) 
        printf("%c ", e->id); 
    printf("\n");

    printf("deleting head a\n");
    DL_DEL(head,&els[0]);
    DL_FOREACH(head,e) 
        printf("%c ", e->id); 
    printf("\n");

    printf("deleting head b\n");
    DL_DEL(head,&els[1]);
    DL_FOREACH(head,e) 
        printf("%c ", e->id); 
    printf("\n");

    /* test LL macros */
    printf("LL macros\n");
    LL_ADD(head,f,&els[0]);
    LL_ADD(head,f,&els[1]);
    LL_ADD(head,f,&els[2]);
    LL_FOREACH(head,e) 
        printf("%c ", e->id); 
    printf("\n");

    printf("deleting tail c\n");
    LL_DEL(head,f,&els[2]);
    LL_FOREACH(head,e) 
        printf("%c ", e->id); 
    printf("\n");

    printf("deleting head a\n");
    LL_DEL(head,f,&els[0]);
    LL_FOREACH(head,e) 
        printf("%c ", e->id); 
    printf("\n");

    printf("deleting head b\n");
    LL_DEL(head,f,&els[1]);
    LL_FOREACH(head,e) 
        printf("%c ", e->id); 
    printf("\n");

    return 0;
}
