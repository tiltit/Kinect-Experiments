PRG = permutate-pgm-endianness

CXX = g++
CC = g++

all: $(PRG)

split.o: split.cpp

main.o: main.cpp split.h

$(PRG): main.o split.o
	$(CC) -o permutate-pgm-endianness main.o split.o

install: $(PRG)
	cp ./permutate-pgm-endianness /usr/local/bin/

uninstall:
	rm /usr/local/bin/permutate-pgm-endianness

clean:
	rm -f *.o $(PRG)