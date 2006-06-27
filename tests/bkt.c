#include "uthash.h"
#include "stdio.h"
#include "stdlib.h"

/* Print the hash bucket number for string(s) */

void show_usage(void) {
    printf("usage: bkt [-b #buckets] [-f] [-n] string1 ...\n");
    printf("       -f filter: read stdin, write stdout\n");
    printf("       -n numeric: interpret strings as integers\n");
    exit(0);
}

int main(int argc, char *argv[]) {
    int i=1, j, num_bkts=32, keylen, bkt, n_mode=0, f_mode=0;
    char line[100], *eol, *kp; 
    int x,y,z;

    if (argc < 2) show_usage();

    while (i < argc && argv[i][0] == '-') {
        for(j=1;argv[i][j] != '\0'; j++) {
            switch (argv[i][j]) {
                case 'b':
                    if (i+1 < argc) num_bkts = atoi(argv[i+1]);
                    else show_usage();
                    break;
                case 'f':
                    f_mode=1;
                    break;
                case 'n':
                    n_mode=1;
                    break;
                default:
                    show_usage();
            }
        }
        i++;
    }

    /* filter mode: read stdin, write stdout */
    if (f_mode) {
        while (fgets(line,100,stdin)) {
            if (*line != '\0') {
                eol = &line[strlen(line)-1];
                if (*eol == '\n') *eol = '\0'; /* chomp */
                if (n_mode) {
                    keylen = sizeof(int);
                    j=atoi(line);
                    kp = (char*)&j;
                    HASH_FCN(kp, keylen, num_bkts, bkt,x,y,z);
                } else {
                    keylen = strlen(line);
                    kp = line;
                    HASH_FCN(kp, keylen, num_bkts, bkt,x,y,z);
                }
                printf("%d\n", bkt);
            }
        }
        exit(0);
    }

    /* non-f mode: read inputs from argv */
    while(i < argc) {
        /* printf("%s: ", argv[i]); */
        if (n_mode) {
            j = atoi(argv[i]);
            kp = (char*)&j;
            keylen = sizeof(int);
            HASH_FCN(kp, keylen, num_bkts, bkt,x,y,z);
        } else {
            keylen = strlen(argv[i]);
            HASH_FCN(argv[i], keylen, num_bkts, bkt,x,y,z);
        }
        printf("%d\n", bkt);
        i++;
    }
}

