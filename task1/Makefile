all: a.out

test_coverage: main.c HashTable.c
	$(CC) $(CFLAGS) -DFAULT_INJ -fprofile-arcs -ftest-coverage -o a.out $^
	./a.out > /dev/null 2> /dev/null
	gcov HashTable.c

a.out: main.o HashTable.o
	$(CC) -o a.out $^

%.o: %.c
	$(CC) $(CFLAGS) -c -o $@ $<

HashTable.c: HashTable.h
main.c: HashTable.h

clean:
	rm -rf *.o a.out HashTable.gcda HashTable.gcno main.gcda main.gcno 

CFLAGS = -Wall

# $^ means all prerequizites
# $< means first prerequizite
# $@ means target
