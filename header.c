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
#define ADRESS_TRANSMITER (0x01)
#define ADRESS_RECIVER (0x01)
#define CONTROL_UA (0x07)
#define CONTROL_SET (0x03)
#define TRANSMITTER 0
#define RECIVER 1