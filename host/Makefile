CC=gcc
CFLAGS=-I.
DEPS = huffman_table.h
OBJ = serial.o huffman.o terminal.o

%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

terminal: $(OBJ)
	$(CC) -o $@ $^ $(CFLAGS)