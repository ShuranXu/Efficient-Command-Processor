CC=gcc
CFLAGS= -g -I.
DEPS = huffman_table.h
OBJ = test.o huffman.o ascii_binary_conv.o
LDFLAG = -lcunit

%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

test: $(OBJ)
	$(CC) -o $@ $^ $(CFLAGS) $(LDFLAG)

clean:
	rm test *.o 