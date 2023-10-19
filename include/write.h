#ifndef _WRITE_H_
#define _WRITE_H_
#include "header.h"
int write_package(int fd,unsigned char * package);
int write_UA(int fd);
int write_i_frame(int fd, unsigned char information_frame);
int write_rej(int fd,unsigned char information_frame);
int write_rr(int fd, unsigned char information_frame);

#endif 