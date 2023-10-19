#include "write.h"

int write_rej(int fd,unsigned char information_frame){
    unsigned char buf[ 5] = {0};
    buf[0]= FLAG;
    buf[1]=ADRESS_RECIVER;
    buf[4] =FLAG; 
    if(information_frame==I0){
        buf[2]=REJ0;
    }else{
        buf[2]=REJ1;
    }
    buf[3]= buf[2] ^buf[1];
    int bytes = write(fd, buf, 5);
    printf("%d bytes written\n", bytes);
    return 1;
       
}
int write_rr(int fd, unsigned char information_frame){
    unsigned char buf[ 5] = {0};
    buf[0]= FLAG;
    buf[1]=ADRESS_RECIVER;
    buf[4] =FLAG; 
    if(information_frame==I0){
        buf[2]=RR1;
    }else{
        buf[2]=RR0;
    }
    buf[3]= buf[2] ^buf[1];
    int bytes = write(fd, buf, 5);
    printf("%d bytes written\n", bytes);
    return 1;
}
int write_UA(int fd){
        printf("Received\n");
        unsigned char buf[ 5] = {0};
        buf[0] = FLAG;
        buf[1] = ADRESS_TRANSMITER;
        buf[2] = CONTROL_UA;
        buf[3] = (ADRESS_TRANSMITER ^ CONTROL_UA);
        buf[4] = FLAG;

        int bytes = write(fd, buf, 5);
        printf("%d bytes written\n", bytes);
        return 0;
}