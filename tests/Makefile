HASHDIR = ../src
FUNCS = BER SAX FNV OAT JEN JSW 
UTILS = bkt 
PROGS = test1 test2 test3 test4 test5 test6 test7 test8 test9 \
		test10 test11 test12 test13 test14 test15 test16 
CFLAGS = -I$(HASHDIR) -g 

ifeq ($(HASH_DEBUG),1)
CFLAGS += -DHASH_DEBUG=1
endif

all: $(PROGS) $(UTILS) $(FUNCS) run_tests 

debug:
	$(MAKE) all HASH_DEBUG=1

$(PROGS) $(UTILS) : $(HASHDIR)/uthash.h
	$(CC) $(CFLAGS) -o $@ $(@).c 

$(FUNCS) : $(HASHDIR)/uthash.h
	$(CC) $(CFLAGS) -DHASH_FUNCTION=HASH_$@ -o keystat.$@ keystat.c 
#	$(CC) $(CFLAGS) -DHASH_FUNCTION=HASH_$@ -o bkt.$@ bkt.c 

run_tests: $(PROGS)
	perl ./do_tests

.PHONY: clean

clean:	
	rm -f $(UTILS) $(PROGS) test*.out keystat.???
