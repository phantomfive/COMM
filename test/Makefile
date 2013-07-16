CC = gcc

CFLAGS = -IcuTest -I../../notrap/publicHeaders -ggdb -O1 -Wall -Werror
LDFLAGS = 

CSRC = $(wildcard *.c) cuTest/CuTest.c $(wildcard ../src/common/*.c)
HDRS = $(wildcard *.h) cuTest/CuTest.h $(wildcard ../src/common/*.h)

run: commTests
	./commTests

commTests: $(CSRC) $(HDRS)
	$(CC) -o commTests $(CFLAGS) $(CSRC) $(LDFLAGS)

clean:
	rm -fr commTests commTests.dSYM

.PHONY: run clean
