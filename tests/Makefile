HASHDIR = ../src
PROGS = bkt test1 test2 test3 test4 test5 test6 test7 test8 test9 test10 \
		test11 test12 test13 test14 test15 test16
CFLAGS = -I$(HASHDIR) -g 

ifeq ($(HASH_DEBUG),1)
CFLAGS += -DHASH_DEBUG=1
endif

all: $(PROGS) run_tests 

debug:
	$(MAKE) all HASH_DEBUG=1

$(PROGS) : $(HASHDIR)/uthash.h
	$(CC) $(CFLAGS) -o $@ $(@).c 

run_tests: $(PROGS)
	perl ./do_tests

.PHONY: clean

clean:	
	rm -f $(PROGS) test*.out
