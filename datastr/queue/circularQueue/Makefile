CC = gcc

all: test

test : Test_CircularQueue.o CircularQueue.o
	$(CC) Test_CircularQueue.o CircularQueue.o -o test
Test_CircularQueue.o: Test_CircularQueue.c CircularQueue.h
	$(CC) -c Test_CircularQueue.c
CircularQueue.o : CircularQueue.c CircularQueue.h
	$(CC) -c CircularQueue.c

clean:
	rm *.o
	rm test
