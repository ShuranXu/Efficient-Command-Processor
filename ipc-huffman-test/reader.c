// C program to implement one side of FIFO
// This side reads first, then writes
// Reference Link: https://www.geeksforgeeks.org/named-pipe-fifo-example-c-program/

#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include "huffman.h"
#include "ascii_binary_conv.h"

/*

original: ?
encoded: 111001 --> 0xe4
length of raw bytes = 6

original: .
encoded: 100001 -->0x84
length of raw bytes = 6

*/

#define QUESTION_MARK_BYTE      (0xe4)
#define PERIOD_MARK_BYTE        (0x84)

/* The following main code receives one byte at a time  
 * and only decode and parse the accumulated data once
 * a particular bitstream is received.
 */
int main()
{
    int fd;

    // FIFO file path
    char *myfifo = "/tmp/myfifo";

    // Creating the named file(FIFO)
    // mkfifo(<pathname>,<permission>)
    mkfifo(myfifo, 0666);

    uint8_t c;
    uint8_t cmd[64];
    uint8_t rbuf[512];
    uint8_t data[128];
    uint8_t reply[128];
    uint8_t wbuf[512];

    int read_len;
    int idx = 0;
    int data_written;

    memset(rbuf, 0, sizeof(rbuf));
    memset(data, 0, sizeof(data));
    memset(cmd,0, sizeof(cmd));

    while (1)
    {
        // First open in read only and read
        fd = open(myfifo,O_RDONLY);

        /* check if the ending byte is received */
        do{
            read_len = read(fd, &c, 1);
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

        printf("rbuf = %s, %ld bytes\n", rbuf, strlen(rbuf));

        /* decode the ASCII stream */
        decode_message(rbuf, data);
        // Print the read string 
        printf("User1 command: %s\n", data);

        close(fd);
        
        //check if need to exit
        if(strstr(data,"quit")){
            printf("Exit program\n");
            return 0;
        }    

        // Open FIFO for write only
        fd = open(myfifo, O_WRONLY);
    
        //clear data
        memset(data, 0, sizeof(data));

        // Take an reply message from user.
        // 32 is maximum length
        printf("\r\nUser Reply: ");
        fgets(reply, 32, stdin);
        printf("%ld bytes are provided by the user as the reply\n", strlen(reply));

        /* encode the command using the huffman coding */
        encode_message((const char *)reply, wbuf, sizeof(wbuf)); 
        printf("wbuf = %s, %ld bytes\n", wbuf, strlen(wbuf));
        /* convert the ascii code to raw bytes */
        data_written = ascii_to_bytes(wbuf, strlen(wbuf), \
        data, sizeof(data));
        // append PERIOD_MARK_BYTE to data
        data[++data_written] = PERIOD_MARK_BYTE;
        //increment data_written
        data_written++;

        printf("%d bytes are written to data\n",data_written);

        // Write the input cmd on FIFO
        for(int i=0;i<data_written;i++){
            printf("data[%d] = 0x%x\n",i,data[i]);
            write(fd, &data[i], 1);
        }

        close(fd);

        // clear data
        idx = 0;
        memset(data, 0, sizeof(data));
        memset(wbuf, 0, sizeof(wbuf));
        memset(cmd, 0, sizeof(cmd));
        memset(reply, 0, sizeof(reply));
        memset(rbuf, 0, sizeof(rbuf));
    }

    return 0;
}
