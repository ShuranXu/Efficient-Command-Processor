// C library headers
#include <stdio.h>
#include <string.h>

// Linux headers
#include <fcntl.h> // Contains file controls like O_RDWR
#include <errno.h> // Error integer and strerror() function
#include <termios.h> // Contains POSIX terminal control definitions
#include <unistd.h> // write(), read(), close()

#define BUFSIZE     32

typedef enum {
    NORMAL = 0,
    NO_PORT = -1,
    ERROR_CLOSE = -2
}output_type_t;

static output_type_t get_serial_port(char *dstbuf) {

    char *cmd = "ls /dev | grep ttyACM";    
    char buf[BUFSIZE];
    FILE *fp;

    if ((fp = popen(cmd, "r")) == NULL) {
        return NO_PORT;
    }

    while (fgets(buf, BUFSIZE, fp) != NULL) {
        strcat(dstbuf,buf);
    }

    //remove newline character in the output buffer
    dstbuf[strcspn(dstbuf, "\n")] = 0;

    if(pclose(fp))  {
        
        return ERROR_CLOSE;
    }

    return 0;
}

int serial_open()
{
    char serial_port_addr[32];
    const char *prefix = "/dev/";
    memcpy(serial_port_addr, prefix, strlen(prefix));
    output_type_t status = get_serial_port(serial_port_addr);

    if(status ==  NO_PORT){
        printf("No port found, program exits\n");
        return -1;
    }

    if(status == ERROR_CLOSE){
        printf("Exited command with error status\n");
        return -1;
    }

    int serial_port = open(serial_port_addr, O_RDWR);

    // Check for errors
    if (serial_port < 0) {
        printf("Error %i from open: %s\n", errno, strerror(errno));
        return -1;
    }

    return serial_port;
}

void serial_configure(int serial_port)
{
    // Create new termios struct, we call it 'tty' for convention
    // No need for "= {0}" at the end as we'll immediately write the existing
    // config to this struct
    struct termios tty;

    // Read in existing settings, and handle any error
    // NOTE: This is important! POSIX states that the struct passed to tcsetattr()
    // must have been initialized with a call to tcgetattr() overwise behaviour
    // is undefined
    if(tcgetattr(serial_port, &tty) != 0) {
        printf("Error %i from tcgetattr: %s\n", errno, strerror(errno));
    }

    // Clear parity bit, disabling parity 
    tty.c_cflag &= ~PARENB; 
    // Set stop field, two stop bits used in communication
    tty.c_cflag |= CSTOPB;  
    // Set one-bit stop bit 
    // tty.c_cflag &= ~CSTOPB; 
    // Clear all the size bits
    tty.c_cflag &= ~CSIZE; 
    // 8 bits per byte
    tty.c_cflag |= CS8; 
    // Disable RTS/CTS hardware flow control
    tty.c_cflag &= ~CRTSCTS; 
    // Turn on READ & ignore ctrl lines (CLOCAL = 1)
    tty.c_cflag |= CREAD | CLOCAL; 
    // Configure VMIN and VTIME 
    tty.c_cc[VTIME] = 1;   
    tty.c_cc[VMIN] = 0;
    // Set in/out baud rate to be 38400
    cfsetispeed(&tty, B115200);
    cfsetospeed(&tty, B115200);

    // Save tty settings, also checking for error
    if (tcsetattr(serial_port, TCSANOW, &tty) != 0) {
        printf("Error %i from tcsetattr: %s\n", errno, strerror(errno));
    }

    return;
}

int serial_write(int serial_port, const char *msg, int size)
{
    return write(serial_port, msg, size);
}

int serial_read(int serial_port, char *buf, int size)
{
    // Read bytes. The behaviour of read() (e.g. does it block?,
    // how long does it block for?) depends on the configuration
    // settings above, specifically VMIN and VTIME
    return read(serial_port, buf, size);
}

void serial_close(int serial_port)
{
    close(serial_port);
}