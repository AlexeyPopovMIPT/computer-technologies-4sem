CFLAGS = -std=c99 -Wall
all: server client

server: mutual.o server.c
	gcc $(CFLAGS) server.c -lm  mutual.o -o server

client: mutual.o integral.o client.c
	gcc $(CFLAGS) client.c integral.o -lpthread -lm mutual.o -o client

integral.o: integral.c
	gcc $(CFLAGS) -DCACHELINE_SIZE=$$(getconf LEVEL1_DCACHE_LINESIZE) -c integral.c -o integral.o

mutual.o: mutual.c
	gcc $(CFLAGS) -c mutual.c -o mutual.o

clean:
	rm server client integral.o mutual.o
