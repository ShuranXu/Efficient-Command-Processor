CC=gcc
CFLAGS= -g -I.
DEPS = huffman_table.h
OBJ = serial.o huffman.o terminal.o ascii_binary_conv.o
FFTOBJ = cbfifo_test.o cbfifo.o 
objects = terminal cbfifo_test

all: $(objects)

%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

terminal: $(OBJ)
	$(CC) -o $@ $^ $(CFLAGS)

cbfifo_test: $(FFTOBJ)
	$(CC) -o $@ $^ $(CFLAGS) 

clean:
	rm terminal cbfifo_test *.o 