#ifndef _UTILS_H_
#define _UTILS_H_

// Alarm handler that will be called when the alarm goes off
void alarmHandler(int signal);

// Function to configure the port
int llconfig(int fd);

// Math function for power using integers
long pow_int(int x, int y);

#endif 