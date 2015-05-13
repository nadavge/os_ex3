CC=g++
FLAGS=-Wall -std=c++11 

TAR=tar
TARFLAGS=cvf
TARNAME=ex3.tar
TARSRCS=Makefile README Block.* blockchain.*

all: libblockchain.a

libblockchain.a: blockchain.o Block.o
	ar rcs $@ $^

#======== OBJECTS ==========

%.o: %.cpp
	$(CC) $(FLAGS) -c $<

#======== SOURCE ===========

blockchain.cpp: blockchain.h Block.h hash.h

Block.cpp: Block.h


#======== MISC =============

tar: $(TARSRCS)
	$(TAR) $(TARFLAGS) $(TARNAME) $(TARSRCS)

.PHONY: all clean tar remake

clean:
	rm -f *.o libblockchain.a

remake: clean all
