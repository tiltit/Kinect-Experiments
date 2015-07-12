PRG = krec

CC = gcc
LD = gcc

OBJECTS = record.o

all: $(PRG)

LIBS = -L/usr/local/lib64 -lm -lfreenect
CFLAGS = -g -Wall -I/usr/include/libusb-1.0

%.o: %.c
	$(CC) $(CFLAGS) -c $<

$(PRG): $(OBJECTS)
	$(LD) -o $(PRG) $(OBJECTS) $(LIBS)

clean:
	rm -f *.o $(PRG)