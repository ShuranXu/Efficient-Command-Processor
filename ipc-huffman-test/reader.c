// C program to implement one side of FIFO
// This side reads first, then reads
// Link: https://www.geeksforgeeks.org/named-pipe-fifo-example-c-program/

#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include "huffman.h"
#include "ascii_binary_conv.h"

int main()
{
    int fd1;

    // FIFO file path
    char * myfifo = "/tmp/myfifo";

    // Creating the named file(FIFO)
    // mkfifo(<pathname>,<permission>)
    mkfifo(myfifo, 0666);

    import_huffman_table();

    char str1[32];
    uint8_t rbuf[128];
    uint8_t data[64];

    memset(str1, 0, sizeof(str1));
    memset(rbuf, 0, sizeof(rbuf));
    memset(data, 0, sizeof(data));

    while (1)
    {
        // First open in read only and read
        fd1 = open(myfifo,O_RDONLY);
        int read_len = read(fd1, str1, 80);
        // printf("%d bytes read\n", read_len);

        /* convert raw bytes to ASCII stream */
        bytes_to_ascii(rbuf, sizeof(rbuf), str1, read_len);
        // printf("rbuf = %s\n", rbuf);
        /* decode the ASCII stream */
        decode_message(rbuf, data);
        // Print the read string and close
        printf("User1: %s\n", data);
        close(fd1);

        //check if need to exit
        if(strstr(data,"quit")){
            printf("Exit program\n");
            return 0;
        }    
        // clear data
        memset(data, 0, sizeof(data));
        memset(str1, 0, sizeof(str1));    
    }
    return 0;
}
