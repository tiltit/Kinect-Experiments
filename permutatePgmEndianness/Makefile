PRG = permutate-pgm-endianness

CXX = g++
CC = g++

all: $(PRG)

split.o: split.cpp

main.o: main.cpp split.h

$(PRG): main.o split.o
	$(CC) -o permutate-pgm-endianness main.o split.o

clean:
	rm -f *.o $(PRG)