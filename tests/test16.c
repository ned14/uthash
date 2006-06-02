#include <sys/time.h>  /* gettimeofday */
#include <stdlib.h>    /* malloc       */
#include <stddef.h>    /* offsetof     */
#include <stdio.h>     /* printf       */
#include <string.h>    /* memset       */
#include <time.h>      /* ctime        */
#include "uthash.h"

struct my_event {
    struct timeval tv;         /* key is aggregate of this field */ 
    char event_code;           /* and this field.                */    
    int user_id;
    UT_hash_handle hh;         /* makes this structure hashable */
};


int main(int argc, char *argv[]) {
    struct my_event *e, ev, *events = NULL;
    int i, keylen;

    /* key length = (last field offset + its length) - first field offset */
    keylen =   offsetof(struct my_event, event_code) + sizeof(char)                         
             - offsetof(struct my_event, tv);

    for(i = 0; i < 10; i++) {
        e = malloc(sizeof(struct my_event));
        memset(e,0,sizeof(struct my_event));
        e->tv.tv_sec = i * (60*60*24*365);          /* i years (sec)*/
        e->tv.tv_usec = 0;
        e->event_code = 'a'+(i%2);                   /* meaningless */
        e->user_id = i;

        /* args: UT_hash_handle name, head, first key field, key len, item */
        HASH_ADD( hh, events, tv, keylen, e);
    }

    /* iterate over events, printing them out */
    for(e=events; e != NULL; e=e->hh.next) {
        printf("user %d caused event %c on %s", 
                e->user_id, 
                e->event_code,
                ctime(&e->tv.tv_sec));
    }

    /* look for one specific event */
    memset(&ev,0,sizeof(struct my_event));
    ev.tv.tv_sec = 5 * (60*60*24*365);          
    ev.tv.tv_usec = 0;
    ev.event_code = 'b';
    HASH_FIND( hh, events, e, tv, &ev.tv, keylen );
    if (e) printf("event found: user %d\n", e->user_id);
    else printf("lookup failed\n");
}
