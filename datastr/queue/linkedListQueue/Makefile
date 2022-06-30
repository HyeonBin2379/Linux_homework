CC = gcc

all: test

test : Test_LinkedListQueue.o LinkedListQueue.o
	$(CC) Test_LinkedListQueue.o LinkedListQueue.o -o test
Test_LinkedListQueue.o: Test_LinkedListQueue.c LinkedListQueue.h
	$(CC) -c Test_LinkedListQueue.c
LinkedListQueue.o : LinkedListQueue.c LinkedListQueue.h
	$(CC) -c LinkedListQueue.c

clean:
	rm *.o
	rm test
