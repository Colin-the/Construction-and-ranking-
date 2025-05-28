CC = clang
CFLAGS = -Wall -std=c11 -g -fPIC

all: main
construct.o: construct.c constructAndRank.h
	$(CC) $(CFLAGS) -c construct.c -o construct.o

rank.o: rank.c constructAndRank.h
	$(CC) $(CFLAGS) -c rank.c -o rank.o

main.o: main.c constructAndRank.h
	$(CC) $(CFLAGS) -c main.c -o main.o

main: main.o rank.o construct.o
	$(CC) main.o rank.o construct.o -o run

clean:
	rm -f run *.o