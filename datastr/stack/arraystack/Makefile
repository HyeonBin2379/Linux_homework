CC = gcc

all: test

test : Test_ArrayStack.o ArrayStack.o
	$(CC) Test_ArrayStack.o ArrayStack.o -o test
Test_ArrayStack.o: Test_ArrayStack.c ArrayStack.h
	$(CC) -c Test_ArrayStack.c
ArrayStack.o : ArrayStack.c ArrayStack.h
	$(CC) -c ArrayStack.c

clean:
	rm *.o
	rm test
