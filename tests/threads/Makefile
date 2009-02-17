HASHDIR = ../../src
PROGS = test1

# Thread support requires compiler-specific options
# ----------------------------------------------------------------------------
# GNU 
CFLAGS = -I$(HASHDIR) -g -pthread
# Solaris (Studio 11 on Sparc Ultra3) 
#CFLAGS = -I$(TPLSRC) -g -mt
# ----------------------------------------------------------------------------

all: $(PROGS) run_tests

$(PROGS) : $(HASHDIR)/uthash.h
	$(CC) $(CFLAGS) -o $@ $(@).c 

run_tests: $(PROGS)
	perl ../do_tests

.PHONY: clean

clean:	
	rm -f $(PROGS) test*.out 
	rm -rf test*.dSYM
