HASHDIR = ../../src
PROGS = test1 test2

# Thread support requires compiler-specific options
# ----------------------------------------------------------------------------
# GNU 
CFLAGS = -I$(HASHDIR) -g -pthread
# Solaris (Studio 11 on Sparc Ultra3) 
#CFLAGS = -I$(TPLSRC) -g -mt
# ----------------------------------------------------------------------------

ifeq ($(HASH_DEBUG),1)
CFLAGS += -DHASH_DEBUG=1
endif

all: $(PROGS) run_tests

$(PROGS) : $(HASHDIR)/uthash.h
	$(CC) $(CFLAGS) -o $@ $(@).c 

debug:
	$(MAKE) all HASH_DEBUG=1

run_tests: $(PROGS)
	perl ../do_tests

.PHONY: clean

clean:	
	rm -f $(PROGS) test*.out 
	rm -rf test*.dSYM
