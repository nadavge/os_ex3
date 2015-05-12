CC=g++
FLAGS=-Wall -std=c++11

TAR=tar
TARFLAGS=cvf
TARNAME=ex3.tar
TARSRCS=Makefile README #TODO choose srcs

all: libblockchain.a

#======== OBJECTS ==========

%.o: %.cpp
	$(CC) $(FLAGS) -c 

stablePriorityQueue.o: stablePriorityQueue.cpp
	$(CC) $(FLAGS) -c stablePriorityQueue.cpp

#======== SOURCE ===========

uthreads.cpp: uthreads.h stablePriorityQueue.h thread.h utils.h

stablePriorityQueue.cpp: uthreads.h thread.h stablePriorityQueue.h

libuthreads.a: uthreads.o stablePriorityQueue.o
	ar rcs libuthreads.a uthreads.o stablePriorityQueue.o

#======== MISC =============

tar: $(TARSRCS)
	$(TAR) $(TARFLAGS) $(TARNAME) $(TARSRCS)

.PHONY: clean tar

clean:
	rm -f *.o libuthreads.a
