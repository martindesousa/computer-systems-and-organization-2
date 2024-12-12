CC = gcc
CFLAGS = -Og -Wall -g -Wno-unused-result

all: gettimings

gettimings: gettimings.o
	$(CC) $(CFLAGS) -o gettimings gettimings.c

gettimings.o: gettimings.c
	$(CC) $(CFLAGS) -c gettimings.c 

clean:
	rm --force gettimings.o gettimings

.PHONY: all clean 