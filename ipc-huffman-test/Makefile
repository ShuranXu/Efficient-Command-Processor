CC=gcc
CFLAGS= -g -I.
DEPS = huffman_table.h
WRIOBJ = writer.o huffman.o ascii_binary_conv.o
RDOBJ = reader.o huffman.o ascii_binary_conv.o
objects = writer reader huffman.o ascii_binary_conv.o

all: $(objects)

%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

reader: $(RDOBJ)
	$(CC) -o $@ $^ $(CFLAGS) 

writer: $(WRIOBJ)
	$(CC) -o $@ $^ $(CFLAGS) 

clean:
	rm writer reader *.o 