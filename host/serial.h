#ifndef __SERIAL_H__
#define __SERIAL_H__

int serial_open();

void serial_configure(int serial_port);

int serial_write(int serial_port, const char *msg, int size);

int serial_read(int serial_port, char *buf, int size);

void serial_close(int serial_port);

#endif /* __SERIAL_H__ */

