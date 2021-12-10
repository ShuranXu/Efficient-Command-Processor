#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <errno.h>
#include <time.h>
#include "huffman.h"
#include "serial.h"
#include "ascii_binary_conv.h"


#define CMD_MAX_LIMIT               (32)
#define QUESTION_MARK_BYTE          (0xe4)
#define PERIOD_MARK_BYTE            (0x84)


void delay_ms(int milliseconds)
{
    long pause;
    clock_t now,then;

    pause = milliseconds*(CLOCKS_PER_SEC/1000);
    now = then = clock();
    while( (now-then) < pause )
        now = clock();
}

void get_user_cmd(char *cmd)
{
    memset(cmd,0,CMD_MAX_LIMIT);
    printf("\r\n\r\nEnter command:\r\n");
    char c;
    char *cmdp = cmd;
    do{
        c = fgetc(stdin);
        *cmdp = c;
        cmdp++;
    }while(((cmdp - cmd) < CMD_MAX_LIMIT) && (c != '\n'));
    cmdp--;
    *cmdp = '\0';
}

int main(int argc, char *argv[])
{
    int serial_port = serial_open();
    uint8_t wbuf[256];
    uint8_t rbuf[512];
    uint8_t wdata[512];
    uint8_t reply[256];
    uint8_t c;
    uint8_t result[128];
    int i;
    int n;
    int data_written;
    int idx = 0;
    unsigned char cmd[CMD_MAX_LIMIT];

    idx = 0;
    memset(wdata, 0, sizeof(wdata));
    memset(wbuf, 0, sizeof(wbuf));
    memset(cmd, 0, sizeof(cmd));
    memset(reply, 0, sizeof(reply));
    memset(rbuf, 0, sizeof(rbuf));
    memset(result,0,sizeof(result));

    serial_configure(serial_port);

    while(1) {
        /* obtain the user-supplied command */
        get_user_cmd(cmd);

        /* encode the command using the huffman coding */
        encode_message((const char *)cmd,wbuf, sizeof(wbuf)); 

        /* convert the ascii code to raw bytes */
        data_written = ascii_to_bytes(wbuf, strlen(wbuf), wdata, sizeof(wdata));

        // printf("data_written = %d\n", data_written);

        // append QUESTION_MARK_BYTE to data
        wdata[++data_written] = QUESTION_MARK_BYTE;
        //increment data_written
        data_written++;
        
        /* we have to only send one byte at a time */
        for(i=0;i<data_written;i++){
            // printf("Byte to send : %c\n", wdata[i]);
            n = serial_write(serial_port, &wdata[i], 1);
            if(n <= 0){
                printf("Error writing %c, %d bytes written\n", wdata[i], n);
                continue;
            }
        }
        
        while(1){
            /* Wait to receive response from the device */
            do{
                /* read one byte at a time */
                n = serial_read(serial_port,&c, 1);
                if(n > 0){

                    /* check if the ending byte is received */
                    if(c == PERIOD_MARK_BYTE){
                        break;
                    }

                    reply[idx++] = c;

                    if(idx >=  sizeof(reply)){
                        printf("receiver buffer overlow, clear the buffer\n");
                        memset(reply,0,sizeof(reply));
                        idx = 0;
                    }
                }
                if(n < 0){
                    printf("Error read: %s\n", strerror(errno));
                    idx = 0;
                }
                
            }while(1);

            /* convert raw bytes to ASCII stream */
            bytes_to_ascii(rbuf, sizeof(rbuf), reply, idx);

            /* decode the ASCII stream */
            decode_message(rbuf, result);

            /* clear buffers */
            idx = 0;
            memset(wdata, 0, sizeof(wdata));
            memset(wbuf, 0, sizeof(wbuf));
            memset(cmd, 0, sizeof(cmd));
            memset(reply, 0, sizeof(reply));
            memset(rbuf, 0, sizeof(rbuf));

            if(strstr(result,"END")){
                memset(result,0,sizeof(result));
                break;
            }

            /* print the characters on terminal */
            printf("%s",result);
            memset(result,0,sizeof(result));
        }
    }
}

