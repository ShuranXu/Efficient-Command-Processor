// C program to implement one side of FIFO
// This side writes first, then reads
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
encoded: 111011 --> 0xec --> no longer used

original: .
encoded: 100001
length of raw bytes = 6

*/

// #define BYTE_MODE
// #define STREAM_MODE
#define LOOKUP_MODE


#ifdef LOOKUP_MODE

#define QUESTION_MARK_BYTE      (0xe4)
#define PERIOD_MARK_BYTE        (0x84)

/* The following main code sends one byte at a time. */
int main()
{
    int fd;

    // FIFO file path
    char *myfifo = "/tmp/myfifo";

    // Creating the named file(FIFO)
    // mkfifo(<pathname>, <permission>)
    mkfifo(myfifo, 0666);

    uint8_t cmd[32];
    uint8_t reply[32];
    uint8_t wbuf[128];
    uint8_t rbuf[128];
    uint8_t data[32];
    int data_written;
    uint8_t c;
    int read_len;
    int idx = 0;

    memset(cmd, 0, sizeof(cmd));
    memset(wbuf, 0, sizeof(wbuf));
    memset(data, 0, sizeof(data));
    memset(rbuf, 0, sizeof(rbuf));
    memset(reply, 0, sizeof(reply));

    while (1)
    {
        // Open FIFO for write only
        fd = open(myfifo, O_WRONLY);

        // Take an input cmd2ing from user.
        // 32 is maximum length
        printf("\r\nUser Input: ");
        fgets(cmd, 32, stdin);
        printf("%d bytes are provided by the user\n", strlen(cmd));

        /* encode the command using the huffman coding */
        encode_message((const char *)cmd, wbuf); 
        printf("wbuf = %s, %d bytes\n", wbuf, strlen(wbuf));
        /* convert the ascii code to raw bytes */
        data_written = ascii_to_bytes(wbuf, strlen(wbuf), \
        data, sizeof(data));
        // append QUESTION_MARK_BYTE to data
        data[++data_written] = QUESTION_MARK_BYTE;
        //increment data_written
        data_written++;

        printf("%d bytes are written to data\n",data_written);

        // Write the input cmd on FIFO
        for(int i=0;i<data_written;i++){
            printf("data[%d] = 0x%x\n",i,data[i]);
            write(fd, &data[i], 1);
        }

        close(fd);

         //check if need to exit
        if(strstr(cmd,"quit")){
            printf("Exit program\n");
            
            return 0;
        }

        // Open FIFO for read only
        fd = open(myfifo,O_RDONLY);

        /* now read the response from the receiver */
        /* check if the ending byte is received */
        do{
            read_len = read(fd, &c, 1);
            if(c == PERIOD_MARK_BYTE)
                break;

            reply[idx++] = c;

            if(idx >= sizeof(reply)){
                printf("Failed attempts so far, reset the buffer\n");
                memset(reply,0,sizeof(reply));
                idx = 0;
            }
        }while(1);

        //print reply
        for(int i=0;i< idx;i++){
            printf("reply[%d] = 0x%x\n", i, reply[i]);
        }

        /* convert raw bytes to ASCII stream */
        bytes_to_ascii(rbuf, sizeof(rbuf), reply, idx);

        printf("rbuf = %s, %d bytes\n", rbuf, strlen(rbuf));

        /* decode the ASCII stream */
        decode_message(rbuf, data);
        // Print the read string and close
        printf("User2 Reply: %s\n", data);

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

#endif 

#ifdef BYTE_MODE

#define QUESTION_MARK_BYTE      (0xec)
#define PERIOD_MARK_BYTE        (0x84)

/* The following main code sends one byte at a time. */
int main()
{
    int fd;

    // FIFO file path
    char *myfifo = "/tmp/myfifo";

    import_huffman_table();

    // Creating the named file(FIFO)
    // mkfifo(<pathname>, <permission>)
    mkfifo(myfifo, 0666);

    uint8_t cmd[32];
    uint8_t reply[32];
    uint8_t wbuf[128];
    uint8_t rbuf[128];
    uint8_t data[32];
    int data_written;
    uint8_t c;
    int read_len;
    int idx = 0;

    memset(cmd, 0, sizeof(cmd));
    memset(wbuf, 0, sizeof(wbuf));
    memset(data, 0, sizeof(data));
    memset(rbuf, 0, sizeof(rbuf));
    memset(reply, 0, sizeof(reply));

    while (1)
    {
        // Open FIFO for write only
        fd = open(myfifo, O_WRONLY);

        // Take an input cmd2ing from user.
        // 32 is maximum length
        printf("\r\nUser Input: ");
        fgets(cmd, 32, stdin);
        printf("%d bytes are provided by the user\n", strlen(cmd));

        /* encode the command using the huffman coding */
        encode_message((const char *)cmd, wbuf); 
        printf("wbuf = %s, %d bytes\n", wbuf, strlen(wbuf));
        /* convert the ascii code to raw bytes */
        data_written = ascii_to_bytes(wbuf, strlen(wbuf), \
        data, sizeof(data));
        // append QUESTION_MARK_BYTE to data
        data[++data_written] = QUESTION_MARK_BYTE;
        //increment data_written
        data_written++;

        printf("%d bytes are written to data\n",data_written);

        // Write the input cmd on FIFO
        for(int i=0;i<data_written;i++){
            printf("data[%d] = 0x%x\n",i,data[i]);
            write(fd, &data[i], 1);
        }

        close(fd);

         //check if need to exit
        if(strstr(cmd,"quit")){
            printf("Exit program\n");
            
            return 0;
        }

        // Open FIFO for read only
        fd = open(myfifo,O_RDONLY);

        /* now read the response from the receiver */
        /* check if the ending byte is received */
        do{
            read_len = read(fd, &c, 1);
            if(c == PERIOD_MARK_BYTE)
                break;

            reply[idx++] = c;

            if(idx >= sizeof(reply)){
                printf("Failed attempts so far, reset the buffer\n");
                memset(reply,0,sizeof(reply));
                idx = 0;
            }
        }while(1);

        //print reply
        for(int i=0;i< idx;i++){
            printf("reply[%d] = 0x%x\n", i, reply[i]);
        }

        /* convert raw bytes to ASCII stream */
        bytes_to_ascii(rbuf, sizeof(rbuf), reply, idx);

        printf("rbuf = %s, %d bytes\n", rbuf, strlen(rbuf));

        /* decode the ASCII stream */
        decode_message(rbuf, data);
        // Print the read string and close
        printf("User2 Reply: %s\n", data);

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

#endif 

#ifdef STREAM_MODE

/* The following main code encodes and 
 * sends the entire data at once.
 */

int main()
{
    int fd;

    // FIFO file path
    char * myfifo = "/tmp/myfifo";

    import_huffman_table();

    // Creating the named file(FIFO)
    // mkfifo(<pathname>, <permission>)
    mkfifo(myfifo, 0666);

    char cmd[32];
    uint8_t wbuf[128];
    uint8_t data[64];
    int data_written;

    memset(cmd, 0, sizeof(cmd));
    memset(wbuf, 0, sizeof(wbuf));
    memset(data, 0, sizeof(data));

    const char *str = "h\r";

    while (1)
    {
        // Open FIFO for write only
        fd = open(myfifo, O_WRONLY);

        // Take an input cmd2ing from user.
        // 80 is maximum length
        printf("\r\nUser Input: ");
        fgets(cmd, 80, stdin);
        printf("user input is %s\n", cmd);

        /* encode the command using the huffman coding */
        encode_message((const char *)cmd, wbuf); 
        // printf("wbuf = %s\n", wbuf);
        /* convert the ascii code to raw bytes */
        data_written = ascii_to_bytes(wbuf, strlen(wbuf), \
        data, sizeof(data));

        // Write the input cmd on FIFO
        // and close it
        write(fd, data, data_written);
        close(fd);
        //check if need to exit
        if(strstr(cmd,"quit")){
            printf("Exit program\n");
            return 0;
        }
      
        // clear data
        memset(data, 0, sizeof(data));
        memset(cmd, 0, sizeof(cmd));
    }
    return 0;
}

#endif

