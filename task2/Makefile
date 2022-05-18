all: a.out
a.out: main.c
	gcc -std=c11 -DCACHELINE_SIZE=$$(getconf LEVEL1_DCACHE_LINESIZE) -pthread -Wall main.c -lm
clean:
	rm a.out
