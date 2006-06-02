#include "uthash.h"
#include "stdio.h"
#include "stdlib.h"

/* Print the Bernstein hash bucket number for string(s)
 * usage: bkt [-b #buckets] string1 ... 
 */
int main(int argc, char *argv[]) {
    int i=1, num_bkts=32, keylen, bkt;
    if (argc < 2) {
        printf("usage: bkt [-b #buckets] string1 ...\n");
    }
    if (argc >= 3 && !strcmp(argv[1],"-b")) {
        num_bkts = atoi(argv[2]);
        i = 3;
    }
    while(i < argc) {
        printf("%s: ", argv[i]);
        keylen = strlen(argv[i]);
        UT_HASH(argv[i], keylen, num_bkts, bkt);
        printf("%d\n", bkt);
        i++;
    }
}

