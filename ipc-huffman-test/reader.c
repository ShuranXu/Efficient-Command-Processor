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

#define BYTE_MODE
// #define STREAM_MODE

/*

original: ?
encoded: 111011 --> 0xec
length of raw bytes = 6

original: .
encoded: 100001 -->0x84
length of raw bytes = 6

*/

#ifdef BYTE_MODE

#define QUESTION_MARK_BYTE      (0xec)
#define PERIOD_MARK_BYTE        (0x84)

/* The following main code receives one byte at a time  
 * and only decode and parse the accumulated data once
 * a particular bitstream is received.
 */
int main()
{
    int fd1;

    // FIFO file path
    char *myfifo = "/tmp/myfifo";

    // Creating the named file(FIFO)
    // mkfifo(<pathname>,<permission>)
    mkfifo(myfifo, 0666);

    import_huffman_table();

    uint8_t c;
    uint8_t cmd[32];
    uint8_t rbuf[128];
    uint8_t data[32];

    int read_len;
    int idx = 0;

    memset(rbuf, 0, sizeof(rbuf));
    memset(data, 0, sizeof(data));
    memset(cmd,0, sizeof(cmd));

    while (1)
    {
        // First open in read only and read
        fd1 = open(myfifo,O_RDONLY);

        /* check if the ending byte is received */
        do{
            read_len = read(fd1, &c, 1);
            if(c == QUESTION_MARK_BYTE)
                break;

            cmd[idx++] = c;

            if(idx >= sizeof(cmd)){
                printf("Failed attempts so far, reset the buffer\n");
                memset(cmd,0,sizeof(cmd));
                idx = 0;
            }
        }while(1);

        printf("%d bytes received\n", idx);

        //print cmd
        for(int i=0;i< idx;i++){
            printf("cmd[%d] = 0x%x\n", i, cmd[i]);
        }

        /* convert raw bytes to ASCII stream */
        bytes_to_ascii(rbuf, sizeof(rbuf), cmd, idx);

        printf("rbuf = %s, %d bytes\n", rbuf, strlen(rbuf));

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
        memset(cmd,0, sizeof(cmd));
        memset(rbuf,0, sizeof(rbuf));
        idx = 0;
    }

    return 0;
}

#endif 


#ifdef STREAM_MODE

/* The following main code receives the entire data and 
 * decode and parse at once.
 */
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

#endif 

