
CC = gcc
CFLAGS = -Wall 
LDFLAGS = -static

all: libmlpt.a

libmlpt.a: pagetables.o
	ar rcs libmlpt.a pagetables.o

pagetables.o: pagetables.c mlpt.h config.h
	$(CC) $(CFLAGS) $(LDFLAGS) -c pagetables.c -o pagetables.o

clean:
	rm -f pagetables.o libmlpt.a

.PHONY: all clean

