#ifndef _HEADER_H_
#define _HEADER_H_
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <termios.h>
#include <unistd.h>
#include <signal.h>
#include "read.h"
#include "write.h"

enum State
    {
        START = 0,
        FLAG_RCV,
        A_RCV,
        C_RCV,
        BCC1_OK,
        D,
        BCC2_OK,
        STOP_
    };

#define BAUDRATE B38400
#define N_TRIES 3
#define TIMEOUT 4
#define _POSIX_SOURCE 1

#define TRANSMITTER 0
#define RECIVER 1;

#define FALSE 0
#define TRUE 1
#define BUF_SIZE 256
#define FLAG (0x7E)
#define ESCAPE (0x7D)
#define ADRESS_TRANSMITER (0x03)
#define ADRESS_RECIVER (0x01)
#define CONTROL_UA (0x07)
#define CONTROL_SET (0x03)
#define CONTROL_RR0 (0x05)
#define CONTROL_RR1 (0x85)
#define CONTROL_REJ0 (0x01)
#define CONTROL_REJ1 (0x81)
#define CONTROL_DISC (0x0B)
#define I0 (0x00)
#define I1 (0x40)
#define RR0 (0x05)
#define RR1 (0x85)
#define REJ0 (0x01)
#define REJ1 (0x81)
#define STUFFED_FLAG (0x5e)
#define STUFFED_ESCAPE (0x5d)
#define C_DATA (0x01)
#define C_START (0x02)
#define C_END (0x03)
#define T_SIZE (0x00)
#define T_NAME (0x01)

#endif 
