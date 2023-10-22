#ifndef _WRITE_H_
#define _WRITE_H_
#include "header.h"
#include "vector.h"

int write_rej(int fd,unsigned char information_frame);
int write_rr(int fd, unsigned char information_frame);
int write_s_u_d(int fd, unsigned char control);
int write_i_frame(int fd, const unsigned char *buf, int bufSize, unsigned char information_frame);
vector* write_control(unsigned char control, const char *filename, long filezise);
vector* write_data(unsigned char *buf, int bufSize);



#endif 