#ifndef _WRITE_H_
#define _WRITE_H_
#include "header.h"
#include "vector.h"

int write_package(int fd,unsigned char * package);
int write_UA(int fd);
vector* write_i_frame(int fd, const unsigned char *buf, int bufSize, unsigned char information_frame);
int write_rej(int fd,unsigned char information_frame);
int write_rr(int fd, unsigned char information_frame);
vector* write_disc(int fd);

#endif 