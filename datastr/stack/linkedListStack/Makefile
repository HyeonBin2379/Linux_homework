CC = gcc

all: test

test : Test_LinkedListStack.o LinkedListStack.o
	$(CC) Test_LinkedListStack.o LinkedListStack.o -o test
Test_ArrayStack.o: Test_LinkedListStack.c LinkedListStack.h
	$(CC) -c Test_LinkedListStack.c
LinkedListStack.o : LinkedListStack.c LinkedListStack.h
	$(CC) -c LinkedListStack.c

clean:
	rm *.o
	rm test
