CC = gcc

CFLAGS = -IcuTest -I../../notrap/publicHeaders -I../publicHeaders/ -ggdb -O1 -Wall -Werror
LDFLAGS = 

COMM_SRC = ../../notrap/src/notrap_posix_sockets.c ../../notrap/src/notrap_posix_threads.c

CSRC = $(wildcard *.c) cuTest/CuTest.c $(wildcard ../src/common/*.c) $(wildcard ../src/server/*.c) $(wildcard ../src/client/*.c) $(COMM_SRC)
HDRS = $(wildcard *.h) cuTest/CuTest.h $(wildcard ../src/common/*.h)

run: commTests
	./commTests

commTests: $(CSRC) $(HDRS)
	$(CC) -o commTests $(CFLAGS) $(CSRC) $(LDFLAGS)

clean:
	rm -fr commTests commTests.dSYM

.PHONY: run clean
