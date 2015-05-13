CC=g++

CFLAGS = -Wall -g -std=c++11
CPPFLAGS = -Wall -g -std=c++11

all: test

clean:
	rm -f test1.o blockchain

test1.o: ../blockchain.h ../hash.h test1.cpp
	$(CC) $(CPPFLAGS) -c test1.cpp
	
test2.o: ../blockchain.h ../hash.h test2.cpp
	$(CC) $(CPPFLAGS) -c test2.cpp

test1: libblockchain.a test1.o
	$(CC) $(CPPFLAGS) test1.o -lcrypto -lpthread -L. -lhash -lblockchain -o blockchain
	./blockchain
	
test2: libblockchain.a test2.o
	$(CC) $(CPPFLAGS) test2.o -lcrypto -lpthread -L. -lhash -lblockchain -o blockchain
	./blockchain

test: test1 test2

.PHONY: test test1 test2

val: 
	valgrind --tool=memcheck --leak-check=full ./blockchain 
