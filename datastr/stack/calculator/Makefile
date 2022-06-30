CC = gcc

all: test

test : Test_Calculator.o Calculator.o LinkedListStack.o
	$(CC) Test_Calculator.o Calculator.o LinkedListStack.o -o test
Test_Calculator.o: Test_Calculator.c Calculator.h LinkedListStack.h
	$(CC) -g -c Test_Calculator.c
Calculator.o: Calculator.c Calculator.h
	$(CC) -g -c Calculator.c
LinkedListStack.o : LinkedListStack.c LinkedListStack.h
	$(CC) -g -c LinkedListStack.c

clean:
	rm *.o
	rm test
