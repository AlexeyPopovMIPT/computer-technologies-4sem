all: a.out
test_coverage: main.c HashTable.c
	gcc -DFAULT_INJ -fprofile-arcs -ftest-coverage -o a.out $^
	./a.out > /dev/null
	gcov HashTable.c
a.out: main.o HashTable.o
	$(CC) -DFAULT_INJ -o a.out $^

%.o: %.c
	$(CC) $(CFLAGS) -o $@ $<

HashTable.c: HashTable.h
main.c: HashTable.h

clean:
	rm -rf *.o a.out

CFLAGS = -Wall -c

# $^ means all prerequizites
# $< means first prerequizite
# $@ means target
