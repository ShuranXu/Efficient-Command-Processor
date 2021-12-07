#include <stdio.h>
#include <string.h>
#include <time.h>
#include "huffman.h"
#include "serial.h"


#define CMD_MAX_LIMIT               (16)
#define CMD_PROC_DELAY_MS           (500)

void delay(int milliseconds)
{
    long pause;
    clock_t now,then;

    pause = milliseconds*(CLOCKS_PER_SEC/1000);
    now = then = clock();
    while( (now-then) < pause )
        now = clock();
}

void basic_io_test(int serial_port)
{
    char str[] = "Help\r\n";
    char wbuf[1024];
    char rbuf[1024];
    int n;

    while(1) {
        serial_write(serial_port, &str, sizeof(str));
        
        n = serial_read(serial_port,rbuf, sizeof(rbuf));
        
        if(n > 0){
            if(n == 1){
                printf("%c",rbuf[0]); 
            }
            else{
                printf("%s",rbuf);
            }
        }
        memset(rbuf,0,sizeof(rbuf));
        delay(5);
    }
}

void get_user_cmd(char *cmd)
{
    memset(cmd,0,CMD_MAX_LIMIT);
    printf("\r\nEnter command:\r\n");
    char c;
    char *cmdp = cmd;
    do{
        c = fgetc(stdin);
        *cmdp = c;
        cmdp++;
    }while(((cmdp - cmd) < CMD_MAX_LIMIT) && (c != '\n'));
    cmdp--;
    *cmdp = '\r';
}

int main(int argc, char *argv[])
{
    int serial_port = serial_open();
    char rbuf[128];
    int i;
    int n;
    unsigned char cmd[CMD_MAX_LIMIT];

    serial_configure(serial_port);
    
    while(1) {
        get_user_cmd(cmd);

        for(i=0;i<strlen(cmd);i++){
            n = serial_write(serial_port, &cmd[i], 1);
            if(n <= 0){
                printf("Error writing %c, %d bytes written\n", n, cmd[i]);
                continue;
            }
            delay(10);
        }
        do{
            n = serial_read(serial_port,rbuf, sizeof(rbuf));
            if(n == 1){
                printf("%c",rbuf[0]);
            }
            else{
                printf("%s",rbuf);
            }
            memset(rbuf,0,sizeof(rbuf));
        }while(n > 0);

        delay(CMD_PROC_DELAY_MS);
    }
}