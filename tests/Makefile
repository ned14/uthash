HASHDIR = ../src
FUNCS = BER SAX FNV OAT JEN 
UTILS = emit_keys
PROGS = test1 test2 test3 test4 test5 test6 test7 test8 test9   \
		    test10 test11 test12 test13 test14 test15 test16 test17 \
        test18 test19 test20 test21 test22 test23
CFLAGS = -I$(HASHDIR) -O3

ifeq ($(HASH_DEBUG),1)
CFLAGS += -DHASH_DEBUG=1
endif

TEST_TARGET=run_tests
TESTS=./do_tests

# detect Cygwin
ifneq ($(strip $(shell $(CC) -v 2>&1 |grep "cygwin")),)
  TESTS=./do_tests.cygwin
endif

# detect MinGW
ifneq ($(strip $(shell $(CC) -v 2>&1 |grep "mingw")),)
  TEST_TARGET=run_tests_mingw
  TESTS=./do_tests.mingw
endif


all: $(PROGS) $(UTILS) $(FUNCS) $(TEST_TARGET)

debug:
	$(MAKE) all HASH_DEBUG=1

example: example.c $(HASHDIR)/uthash.h
	$(CC) $(CFLAGS) -o $@ $(@).c 

$(PROGS) $(UTILS) : $(HASHDIR)/uthash.h
	$(CC) $(CFLAGS) -o $@ $(@).c 

$(FUNCS) : $(HASHDIR)/uthash.h
	$(CC) $(CFLAGS) -DHASH_FUNCTION=HASH_$@ -o keystat.$@ keystat.c 

run_tests: $(PROGS)
	perl $(TESTS)

run_tests_mingw: $(PROGS)
	@echo ""
	@echo "MinGW test script not implemented, manual test only, sorry"
	@echo ""
	@exit 1

.PHONY: clean

clean:	
	rm -f $(UTILS) $(PROGS) test*.out keystat.??? example *.exe
	rm -rf *.dSYM
