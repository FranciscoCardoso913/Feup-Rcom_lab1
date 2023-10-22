#include "write.h"

int write_rej(int fd,unsigned char information_frame){

    vector v2;
    vector *v = &v2;
    vector_init(v);
    vector_push(v, FLAG,0);
    vector_push(v, ADRESS_TRANSMITER,1);
    if(information_frame==I0){
        vector_push(v,REJ0,2);
    }else{
        vector_push(v,REJ1,2);
    }
    vector_push(v, (ADRESS_TRANSMITER ^ v->data[2]),3);
    vector_push(v, FLAG,4);

    int bytes = write(fd, v->data, v->size);
    printf("Writting REJ! %d bytes written\n", bytes);

    return 1;
       
}

int write_rr(int fd, unsigned char information_frame){

    vector v2;
    vector *v = &v2;
    vector_init(v);
    vector_push(v, FLAG,0);
    vector_push(v, ADRESS_TRANSMITER,1);
    if(information_frame==I0){
        vector_push(v,RR1,2);
    }else{
        vector_push(v,RR0,2);
    }
    vector_push(v, (ADRESS_TRANSMITER ^ v->data[2]),3);
    vector_push(v, FLAG,4);

    int bytes = write(fd, v->data, v->size);
    printf("Writting RR! %d bytes written\n", bytes);

    return 1;

}

int write_s_u_d(int fd, unsigned char control) {

    vector v2;
    vector *v = &v2;

    vector_init(v);
    vector_push(v, FLAG,0);
    vector_push(v, ADRESS_TRANSMITER,1);
    vector_push(v, control,2);
    vector_push(v, (ADRESS_TRANSMITER ^ control),3);
    vector_push(v, FLAG,4);

    int bytes = write(fd, v->data, v->size);
    printf("%d bytes written\n", bytes);

    return 1;

}

int write_i_frame(int fd, const unsigned char *buf, int bufSize, unsigned char information_frame) {

    vector v2;
    vector *v = &v2;
    vector_init(v);
    vector_push(v, FLAG,0);
    vector_push(v, ADRESS_TRANSMITER,1);
    if (information_frame == 0)
        vector_push(v,I0,2);
    else
        vector_push(v,I1,2);
    vector_push(v,(ADRESS_TRANSMITER ^ v->data[2]),3);

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
    
    int bytes = write(fd, v->data, v->size);
    printf("%d bytes written\n", bytes);

    return 1;

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
    vector_push(v,C_DATA,0);
    vector_push(v,l2,1);
    vector_push(v,l1,2);
    for(int i=0; i< bufSize ; i++){
        vector_push(v, buf[i], v->size);
    }
    return v;
}