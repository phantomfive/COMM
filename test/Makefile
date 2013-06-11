CC = gcc

CFLAGS = -IcuTest -ggdb -O1 -Wall -Werror
LDFLAGS = 

CSRC = $(wildcard *.c) cuTest/CuTest.c
HDRS = $(wildcard *.h) cuTest/CuTest.h

run: commTests
	./commTests

commTests: $(CSRC) $(HDRS)
	$(CC) -o commTests $(CFLAGS) $(CSRC) $(LDFLAGS)

clean:
	rm -fr commTest commTests.dSYM

.PHONY: run clean
