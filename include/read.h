#ifndef _READ_H_
#define _READ_H_
#include "header.h"
int read_package(int fd,int information_frame, unsigned char * package);
int read_s_u_frame(int fd, int information_frame);

#endif 