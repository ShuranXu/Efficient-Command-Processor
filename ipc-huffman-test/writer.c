// C program to implement one side of FIFO
// This side writes first, then reads
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
    int fd;

    // FIFO file path
    char * myfifo = "/tmp/myfifo";

    import_huffman_table();

    // Creating the named file(FIFO)
    // mkfifo(<pathname>, <permission>)
    mkfifo(myfifo, 0666);

    char arr[32];
    uint8_t wbuf[128];
    uint8_t data[64];
    int data_written;

    memset(arr, 0, sizeof(arr));
    memset(wbuf, 0, sizeof(wbuf));
    memset(data, 0, sizeof(data));

    const char *str = "h\r";

    while (1)
    {

        // Open FIFO for write only
        fd = open(myfifo, O_WRONLY);

        // Take an input arr2ing from user.
        // 80 is maximum length
        printf("\r\nUser Input: ");
        fgets(arr, 80, stdin);
        printf("user input is %s\n", arr);

        /* encode the command using the huffman coding */
        encode_message((const char *)arr, wbuf); 
        // printf("wbuf = %s\n", wbuf);
        /* convert the ascii code to raw bytes */
        data_written = ascii_to_bytes(wbuf, strlen(wbuf), \
        data, sizeof(data));

        // Write the input arr on FIFO
        // and close it
        write(fd, data, data_written);
        close(fd);
        //check if need to exit
        if(strstr(arr,"quit")){
            printf("Exit program\n");
            return 0;
        }
      
        // clear data
        memset(data, 0, sizeof(data));
        memset(arr, 0, sizeof(arr));
    }
    return 0;
}
