#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

void ascii_to_byte(char *buf, uint8_t *byte)
{
    *byte = 0;
    char *ptr = buf;
    int pos =0;
    while(*ptr){
        pos = 7 - (ptr - buf);
        *byte |= (((*ptr++) - '0') << pos);
    }
    printf("byte = 0x%x\n", *byte);
}

void ascii_to_bytes(char *buf, uint8_t *bytes, int size)
{
    int len = strlen(buf) >> 3;
    if(len > size) {
        printf("bytes size is too small, abort\n");
        return;
    }
    char *bufp = buf;
    uint8_t *bytesp = bytes;

    for(int i=0;i<len;i++){
        ascii_to_byte(bufp, bytesp);
        bufp += 8;
        bytesp += 8;
    }
}

void byte_to_ascii(char *buf, uint8_t byte)
{
    int i;
    for(i=0;i<8;i++){
        int val = (byte >> (7 - i)) & 0x1;
        buf[i] = val + '0';
    }

    printf("buf = %s\n", buf);
}

void bytes_to_ascii(char *buf, int buf_size, uint8_t *bytes, int byte_size)
{
    int len = (byte_size << 2);
    if(len > buf_size){
        printf("Error: bytes size = %d and buf size = %d, abort\n", byte_size, buf_size);
        return;
    }

    for(int i=0;i<byte_size;i++){
        byte_to_ascii(&buf[i << 3], *bytes++);
    }
}

void test_byte_to_ascii()
{
    char buf[8];
    uint8_t byte = 0xab;
    printf("byte to be encoded = 0x%x\n", byte);
    byte_to_ascii(buf, byte);
}

void test_bytes_to_ascii()
{
    uint8_t bytes[4] = {0x11, 0x22, 0x33, 0x44};
    printf("bytes to be decoded:\n");
    for(int i=0;i<4;i++){
        printf("0x%x\t", bytes[i]);
    }
    printf("\r\n");

    char buf[33];
    memset(buf,0,33);
    bytes_to_ascii(buf, 33, bytes, 4);
}

void test_ascii_to_byte()
{
    //0xab
    char buf[] = "10101011";
    uint8_t byte;
    ascii_to_byte(buf, &byte);
}

void test_ascii_to_bytes()
{
    //0xab0f8844
    char buf[] = "10101011000011111000100001000100";
    uint8_t bytes[4];
    ascii_to_bytes(buf, bytes, 4);
}



int main(int argc, char *argv[])
{
    test_bytes_to_ascii();
}