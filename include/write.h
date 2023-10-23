#ifndef _WRITE_H_
#define _WRITE_H_
#include "header.h"
#include "vector.h"

// Writes a REJ frame to the serial port
int write_rej(int fd,unsigned char information_frame);

// Writes a RR frame to the serial port
int write_rr(int fd, unsigned char information_frame);

// Writes a SET, UA or DISC frame to the serial port, depending on the control parameter 
int write_s_u_d(int fd, unsigned char control);

// Writes a I frame to the serial port
int write_i_frame(int fd, const unsigned char *buf, int bufSize, unsigned char information_frame);

// Prepares a control frame to be written to the serial port
vector* write_control(unsigned char control, const char *filename, long filezise);

// Prepares a data frame to be written to the serial port
vector* write_data(unsigned char *buf, int bufSize);

#endif 