#ifndef _READ_H_
#define _READ_H_
#include "header.h"

// Reads a I frame from the serial port, and returns the number of bytes read
int read_package(int fd,int information_frame, unsigned char * package);

// Reads a response frame from the serial port, and returns the number of bytes read
int read_res_i_frame(int fd, int information_frame);

// Reads a DISC frame from the serial port, and returns the number of bytes read
int read_disc(int fd);

// Reads a UA frame from the serial port, and returns the number of bytes read
int read_UA(int fd);
#endif 