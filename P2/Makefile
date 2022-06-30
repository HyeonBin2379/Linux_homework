CC = gcc
all: ssu_sdup fmd5 fsha1 help

ssu_sdup : ssu_sdup.o ssu_functions.o
	$(CC) ssu_sdup.o ssu_functions.o -o ssu_sdup
fmd5: ssu_find-md5.o ssu_functions.o
	$(CC) ssu_find-md5.o ssu_functions.o -O2 -lcrypto -o fmd5
fsha1: ssu_find-sha1.o ssu_functions.o
	$(CC) ssu_find-sha1.o ssu_functions.o -O2 -lcrypto -o fsha1
help: ssu_help.o
	$(CC) ssu_help.o -o help

ssu_sdup.o: ssu_sdup.c ssu_functions.h
	$(CC) -g -c ssu_sdup.c
ssu_functions.o: ssu_functions.c ssu_functions.h
	$(CC) -g -c ssu_functions.c
ssu_find-md5.o: ssu_find-md5.c ssu_functions.h
	$(CC) -g -c ssu_find-md5.c 
ssu_find-sha1.o: ssu_find-sha1.c ssu_functions.h
	$(CC) -g -c ssu_find-sha1.c
ssu_help.o: ssu_help.c
	$(CC) -g -c ssu_help.c

clean:
	rm *.o
	rm ssu_sdup fmd5 fsha1 help
