#Worked with Malik Kurtz (kra8ku) and Jad Beidas (afu4pv)

CC = gcc
CFLAGS = -Wall
LDFLAGS = -Wall

all: guesser

guesser.o: guesser.c say.h
	$(CC) $(CFLAGS) -c guesser.c -o $@

cheer.o: cheer.c say.h
	$(CC) $(CFLAGS) -c cheer.c -o $@

grunt.o: grunt.c say.h
	$(CC) $(CFLAGS) -c grunt.c -o $@

libsay.a: cheer.o grunt.o
	ar rcs $@ $^

guesser: guesser.o libsay.a
	$(CC) $(LDFLAGS) -o $@ $^

clean:
	rm --force *.o libsay.a guesser 

.PHONY: all clean
