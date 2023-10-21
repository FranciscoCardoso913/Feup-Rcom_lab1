#include "write.h"

int write_rej(int fd,unsigned char information_frame){
    unsigned char buf[ 5] = {0};
    buf[0]= FLAG;
    buf[1]= ADRESS_TRANSMITER;
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
        //printf("%d bytes written\n", bytes);
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

vector* write_disc(int fd) {

    vector v2; 
    vector *v = &v2;

    vector_init(v);
    vector_push(v, FLAG,0);
    vector_push(v, ADRESS_TRANSMITER,1);
    vector_push(v, CONTROL_DISC,2);
    vector_push(v, (ADRESS_TRANSMITER ^ CONTROL_DISC),3);
    vector_push(v, FLAG,4);

    return v;

}

vector* write_control(unsigned char control, const char *filename, long filesize) {
    

    vector v2;
    vector *v = &v2;
    
    vector_init(v);
    vector_push(v, control, 0);


    unsigned char size = sizeof(filesize);

    vector_push(v, T_SIZE, 1);
    vector_push(v, size, 2);

    while (size > 0) {

        vector_push(v, (unsigned char) filesize % 256, 3);
        filesize /= 256;
        size--;

    }

    if (*filename != '\0') {

        vector_push(v, T_NAME, v->size);
        vector_push(v, strlen(filename), v->size);

        for (int i = 0; i < strlen(filename); i++) {
            vector_push(v, filename[i], v->size);
        }

    }

    return v;

}

vector* write_data(unsigned char *buf, int bufSize) {
    
    vector v2;
    vector *v = &v2;

    vector_init(v);

    unsigned char l2 = bufSize/ 256;
    unsigned char l1 = bufSize % 256;
    vector_push(v,C_DATA,0x00);
    vector_push(v,l2,1);
    vector_push(v,l1,2);
    for(int i=0; i< bufSize ; i++){
        vector_push(v, buf[i], v->size);
    }
    return v;
}