## sample makefile for hw7 - Words
## a number of the dependencies are implicit.
## make figures out the right thing to do.

CC     = gcc
 CFLAGS = -g3 -std=gnu99 -pedantic -Wall
## using gnu99 library to get strdup() library function.
# CFLAGS = -std=c99 -pedantic -Wall
HWK    = /c/cs223/hw7


Words:  Words.o $(HWK)/hash.o
	${CC} ${CFLAGS} -o Words Words.o $(HWK)/hash.o

hashtest: hashtest.o $(HWK)/hash.o
	$(CC) $(CFLAGS) $^ -o $@

bighashtest: bighashtest.o $(HWK)/hash.o
	$(CC) $(CFLAGS) $^ -o $@

hashtest.o bighashtest.o $(HWK)/hash.o: $(HWK)/hash.h $(HWK)/hash.c

clean:
	rm -i *.o *~

