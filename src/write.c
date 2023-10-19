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
    buf[1]=ADRESS_TRANSMITER;
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

vector* write_i_frame(int fd, const unsigned char *buf, int bufSize, unsigned char information_frame) {

    vector v2;
    vector *v = &v2;
    vector_init(v);
    vector_push(v, FLAG,0);
    vector_push(v, ADRESS_TRANSMITER,1);
    if (information_frame == 0)
        vector_push(v,I0,2);
    else
        vector_push(v,I1,2);
    vector_push(v,(ADRESS_TRANSMITER ^ information_frame),3);

    int packetsize = 4;
    unsigned char BCC2 = 0;

    while(bufSize>0){

        BCC2 ^= *buf;
        vector_push(v, *buf,packetsize);
        buf++;
        bufSize--;
        packetsize++;

    }

    vector_push(v, BCC2,packetsize);
    packetsize++;
    vector_stuff(v);
    vector_push(v, FLAG,v->size);
    
    return v;
}