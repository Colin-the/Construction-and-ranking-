CC = clang
CFLAGS = -Wall -std=c11 -g -fPIC

all: main

rank.o: rank.c
	$(CC) $(CFLAGS) -c rank.c -o rank.o

main.o: main.c
	$(CC) $(CFLAGS) -c main.c -o main.o

main: main.o rank.o
	$(CC) main.o rank.o -o run

clean:
	rm -f run *.o