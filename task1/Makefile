all: a.out

test_coverage: test.out
	./test.out > /dev/null 2> /dev/null
	gcov HashTable.c

a.out: main.o HashTable.o
	$(CC) -o a.out $^

test.out: main.c HashTable.c
	$(CC) $(CFLAGS) -DFAULT_INJ -fprofile-arcs -ftest-coverage -o test.out $^

%.o: %.c
	$(CC) $(CFLAGS) -c -o $@ $<

HashTable.c: HashTable.h
main.c: HashTable.h

clean:
	rm -rf *.o a.out test.out HashTable.gcda HashTable.gcno main.gcda main.gcno

CFLAGS = -Wall
CC = gcc
# $^ means all prerequizites
# $< means first prerequizite
# $@ means target
