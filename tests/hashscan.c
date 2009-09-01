#include <string.h>
#include <errno.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <inttypes.h>
#include <sys/ptrace.h>
#include <unistd.h>
#include <sys/types.h>
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

void found(off_t at) {
  fprintf(stderr,"found at %p\n", (void*)at);
}

void iscan(int fd, off_t start, off_t end, char *pat, size_t len) {
  int rlen, w, rw;
  off_t at=0;
  size_t buflen = end-start;
  char *acc = calloc(2,len), *a = &acc[len];
  if (!acc) { fprintf(stderr,"malloc failed\n"); exit(-1); } 

  if (lseek(fd, start, SEEK_SET) == (off_t)-1) {
    fprintf(stderr, "lseek failed: %s\n", strerror(errno));
    goto done;
  }

  /* whenever we read 'len' (or fewer bytes-- tolerate short reads),
   * see if those bytes match the target pattern, otherwise slide the
   * window backward (up to len-1) bytes to see if the previous bytes,
   * plus part of the new buffer, constitute a match. So there are a
   * total of 'len' possible match windows. */
  while ( at<buflen && ((rlen = read(fd,a,len)) > 0)) {
    fprintf(stderr,"%p\n",a);
    for(w=0,rw=rlen; w<len && at>=w; w++) {
      if (w + rw > len) rw--;
      if (w + rw == len) {
        if (!memcmp(a-w,pat,w+rw)) found(at-w);
      }
    }
    memmove(acc, a-(len-rlen), len); /* shift */
    at += rlen;
  }
  if (rlen == -1) {
    fprintf(stderr,"read failed: %s\n", strerror(errno));
    exit(-1);
  }
 done:
  free(acc);
  return;
}

int main(int argc, char *argv[]) {
  pid_t pid;
  FILE *mapf;
  char mapfile[30], memfile[30], line[100];
  vma_t *vmas=NULL, vma;
  unsigned i, num_vmas = 0;
  int memfd;
  void *pstart, *pend, *unused;

  if (argc < 2) {
    fprintf(stderr,"usage: %s <pid>\n", argv[0]);
    exit(-1);
  }

  /* attach to the target process and wait for it to suspend */
  pid = atoi(argv[1]);
  if (ptrace(PTRACE_ATTACH,pid,NULL,NULL) == -1) {
    fprintf(stderr,"failed to attach to %u: %s\n", (unsigned)pid, strerror(errno));
    exit(-1);
  }
  if (waitpid(pid,NULL,0) != pid) {
    fprintf(stderr,"failed to wait for pid %u: %s\n",(unsigned)pid, strerror(errno));
    goto die;
  }

  /* get ready to open its memory map. this gives us its valid memory areas */
  snprintf(mapfile,sizeof(mapfile),"/proc/%u/maps",(unsigned)pid);
  snprintf(memfile,sizeof(memfile),"/proc/%u/mem", (unsigned)pid);
  if ( (mapf = fopen(mapfile,"r")) == NULL) {
    fprintf(stderr,"failed to open %s: %s\n", mapfile, strerror(errno));
    goto die;
  }
  while(fgets(line,sizeof(line),mapf)) {
    assert(sizeof(off_t) == sizeof(void*));
    if (sscanf(line, "%p-%p %4c %p %5c", &pstart,&pend,vma.perms,&unused,vma.device) == 5) {
      vma.start = (off_t)pstart;
      vma.end = (off_t)pend;
      if (vma.perms[0] != 'r') continue;          /* only readable vma's */
      if (memcmp(vma.device,"fd",2)==0) continue; /* skip mapped files */
      vmas = realloc(vmas, (num_vmas+1) * sizeof(vma_t));
      vmas[num_vmas++] = vma;
    }
  }
  fclose(mapf);

  /* ok, open up its memory and start looking around in there */
  if ( (memfd=open(memfile,O_RDONLY)) == -1) {
    fprintf(stderr,"failed to open %s: %s\n", memfile, strerror(errno));
    goto die;
  }
  /* look for the hash signature */
  for(i=0;i<num_vmas;i++) {
    vma = vmas[i];
    pstart = (void*)vma.start;
    pend = (void*)vma.end;
    fprintf(stderr,"scanning %p-%p %.4s %.5s\n", pstart, pend, vma.perms, vma.device);
    iscan(memfd, vma.start, vma.end, (char*)&sig, sizeof(sig));
  }

  /* done. close memory and detach. this resumes the target process */
  close(memfd);

 die:
  if (ptrace(PTRACE_DETACH, pid, NULL, NULL) == -1) {
    fprintf(stderr,"failed to detach from %u: %s\n", (unsigned)pid, strerror(errno));
  }
  return 0;
}
