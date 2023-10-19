#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <termios.h>
#include <unistd.h>
#include <signal.h>

#define BAUDRATE B38400
#define _POSIX_SOURCE 1

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
#define TRANSMITTER 0
#define RECIVER 1