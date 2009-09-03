#include <string.h>
#include <errno.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <inttypes.h>
#include <sys/ptrace.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/wait.h>
#include <assert.h>
#include "uthash.h"

typedef struct {
  off_t start;
  off_t end;
  char perms[4];   /* rwxp */
  char device[5];  /* fd:01 or 00:00 */
} vma_t;

const uint32_t sig = HASH_SIGNATURE;
int verbose=0;

#define vv(...)  do {if (verbose>0) printf(__VA_ARGS__);} while(0)
#define vvv(...) do {if (verbose>1) printf(__VA_ARGS__);} while(0)

/* read peer's memory from addr for len bytes, store into our dst */
int read_mem(void *dst, int fd, off_t start, size_t len) {
  int rc;
  size_t bytes_read=0;
  if (lseek(fd, start, SEEK_SET) == (off_t)-1) {
    fprintf(stderr, "lseek failed: %s\n", strerror(errno));
    return -1;
  }
  while ( len && ((rc=read(fd, (char*)dst+bytes_read, len)) > 0)) {
    len -= rc;
    bytes_read += rc;
  }
  return (rc == -1) ? -1 : 0;
}

/* later compensate for possible presence of bloom filter */
char *tbl_from_sig_addr(char *sig) {
  return (sig - offsetof(UT_hash_table,signature));
}

void found(int fd, char* peer_sig) {
  UT_hash_table *tbl;
  char *peer_tbl;
  vv("found signature at peer %p\n", peer_sig);
  peer_tbl = tbl_from_sig_addr(peer_sig);
  vv("expect table at peer %p\n", peer_tbl);

  if ( (tbl = (UT_hash_table*)malloc(sizeof(UT_hash_table))) == NULL) {
    fprintf(stderr, "out of memory\n");
    exit(-1);
  }
  if (read_mem(tbl, fd, (off_t)peer_tbl, sizeof(UT_hash_table)) != 0) {
    fprintf(stderr, "failed to read peer memory at %p\n", peer_tbl);
    free(tbl);
    return;
  }

  dump_hash(tbl);
  if (want_keys) dump_keys(fd,tbl);
  printf("read peer tbl:\n");
  printf("num_buckets: %u\n", tbl->num_buckets); /* TODO safely */
  printf("num_items: %u\n", tbl->num_items); 
  printf("nonideal_items: %u (%.2f%%)\n", tbl->nonideal_items, 
    tbl->nonideal_items*100.0/tbl->num_items); 
  printf("expand: %s\n", tbl->noexpand ? "inhibited": "normal"); 

  /* sanity checks on hash */
  /* if requested scan down the bucket chains to emit keys */

 finished:
  free(tbl);
}

void sigscan(int fd, off_t start, off_t end, uint32_t sig) {
  int rlen;
  uint32_t u;
  off_t at=0;

  if (lseek(fd, start, SEEK_SET) == (off_t)-1) {
    fprintf(stderr, "lseek failed: %s\n", strerror(errno));
    return;
  }

  while ( (rlen = read(fd,&u,sizeof(u))) == sizeof(u)) {
     if (!memcmp(&u,&sig,sizeof(u))) found(fd, (char*)(start+at));
     at += sizeof(u);
     if (at + sizeof(u) > end-start) break;
  }

  if (rlen == -1) {
    //fprintf(stderr,"read failed: %s\n", strerror(errno));
    //exit(-1);
  }
}

void usage(const char *prog) {
  fprintf(stderr,"usage: %s [-v] <pid>\n", prog);
  exit(-1);
}

/* return 1 if region is in one of the vma's, so ok to try to read */
int region_in_vma(char *start, size_t len, vma_t *vmas, unsigned num_vmas) {
  int i;
  for(i=0; i<num_vmas; i++) {
    if (((off_t)start     >= vmas[i].start) && 
        ((off_t)start+len <= vmas[i].end)) return 1;
  }
  return 0;
}


int main(int argc, char *argv[]) {
  pid_t pid;
  FILE *mapf;
  char mapfile[30], memfile[30], line[100];
  vma_t *vmas=NULL, vma;
  unsigned i, num_vmas = 0;
  int memfd,opt;
  void *pstart, *pend, *unused;

  while ( (opt = getopt(argc, argv, "v")) != -1) {
    switch (opt) {
      case 'v':
        verbose++;
        break;
      default:
        usage(argv[0]);
        break;
    }
  }
 
  if (optind < argc) pid=atoi(argv[optind++]);
  else usage(argv[0]);

  /* attach to the target process and wait for it to suspend */
  vv("attaching to peer\n");
  if (ptrace(PTRACE_ATTACH,pid,NULL,NULL) == -1) {
    fprintf(stderr,"failed to attach to %u: %s\n", (unsigned)pid, strerror(errno));
    exit(-1);
  }
  vv("waiting for peer to suspend temporarily\n");
  if (waitpid(pid,NULL,0) != pid) {
    fprintf(stderr,"failed to wait for pid %u: %s\n",(unsigned)pid, strerror(errno));
    goto die;
  }

  /* get ready to open its memory map. this gives us its valid memory areas */
  snprintf(mapfile,sizeof(mapfile),"/proc/%u/maps",(unsigned)pid);
  snprintf(memfile,sizeof(memfile),"/proc/%u/mem", (unsigned)pid);
  vv("opening peer memory map [%s]\n", mapfile);
  if ( (mapf = fopen(mapfile,"r")) == NULL) {
    fprintf(stderr,"failed to open %s: %s\n", mapfile, strerror(errno));
    goto die;
  }
  vv("listing peer virtual memory areas\n");
  while(fgets(line,sizeof(line),mapf)) {
    if (sscanf(line, "%p-%p %4c %p %5c", &pstart, &pend, vma.perms,
         &unused, vma.device) == 5) {
      vma.start = (off_t)pstart;
      vma.end = (off_t)pend;
      if (vma.perms[0] != 'r') continue;          /* only readable vma's */
      if (memcmp(vma.device,"fd",2)==0) continue; /* skip mapped files */
      vmas = realloc(vmas, (num_vmas+1) * sizeof(vma_t));
      vmas[num_vmas++] = vma;
    }
  }
  vv("peer has %u virtual memory areas\n",num_vmas);
  fclose(mapf);

  /* ok, open up its memory and start looking around in there */
  vv("opening peer memory\n");
  if ( (memfd=open(memfile,O_RDONLY)) == -1) {
    fprintf(stderr,"failed to open %s: %s\n", memfile, strerror(errno));
    goto die;
  }
  /* look for the hash signature */
  vv("scanning peer memory for hash table signatures\n");
  for(i=0;i<num_vmas;i++) {
    vma = vmas[i];
    pstart = (void*)vma.start;
    pend = (void*)vma.end;
    /*fprintf(stderr,"scanning %p-%p %.4s %.5s\n", pstart, pend, 
              vma.perms, vma.device);*/
    sigscan(memfd, vma.start, vma.end, sig);
  }

  /* done. close memory and detach. this resumes the target process */
  close(memfd);

 die:
  vv("detaching and resuming peer\n");
  if (ptrace(PTRACE_DETACH, pid, NULL, NULL) == -1) {
    fprintf(stderr,"failed to detach from %u: %s\n", (unsigned)pid, strerror(errno));
  }
  return 0;
}
