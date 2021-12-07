CC=gcc
CFLAGS= -g -I.
DEPS = huffman_table.h
OBJ = serial.o huffman.o terminal.o ascii_binary_conv.o

%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

terminal: $(OBJ)
	$(CC) -o $@ $^ $(CFLAGS) 

clean:
	rm terminal *.o 