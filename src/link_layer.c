// Link layer protocol implementation

#include "link_layer.h"
#include "utils.h"
#include "vector.h"

// MISC
#define _POSIX_SOURCE 1 // POSIX compliant source

extern int alarmEnabled;
extern int alarmCount;
volatile int STOP = FALSE;
int fd;
int information_frame=I0;


////////////////////////////////////////////////
// LLOPEN
////////////////////////////////////////////////
int llopen(LinkLayer connectionParameters)
{

    (void)signal(SIGALRM, alarmHandler);
    if(alarmCount==0){
        fd = open(connectionParameters.serialPort, O_RDWR | O_NOCTTY);
        if (fd < 0)
        {
            perror(connectionParameters.serialPort);
            return -1;
        }
    
        llconfig(fd);
    }
    unsigned char buf[5] = {0};

    unsigned char buf_[BUF_SIZE + 1] = {0}; // +1: Save space for the final '\0' char
    
  
    enum State state = START;
    
    
    while (state != STOP_ && alarmCount < 4)
    {

        if (alarmEnabled == FALSE && connectionParameters.role == LlTx)
        {
            buf[0] = FLAG;
            buf[1] = ADRESS_TRANSMITER;
            buf[2] = CONTROL_SET;
            buf[3] = (ADRESS_TRANSMITER ^ CONTROL_SET);
            buf[4] = FLAG;
            int bytes = write(fd, buf, 5);
            printf("%d bytes written\n", bytes);
            alarm(3);
            alarmEnabled = TRUE;
        }
        int control = 0;

        if (connectionParameters.role == LlTx)
        {
            control = CONTROL_UA;
        }
        else
        {
            control = CONTROL_SET;
        }

        // Returns after 5 chars have been input
        read(fd, buf_, 1);
        switch (state)
        {
        case START:
            if (buf_[0] == FLAG)
                state = FLAG_RCV;
                
            break;
        case FLAG_RCV:
            if (buf_[0] == ADRESS_TRANSMITER)
                state = A_RCV;
            else if (buf_[0] == FLAG)
                break;
            else
                state = START;
            break;

        case A_RCV:
            if (buf_[0] == control)
                state = C_RCV;
            else if (buf_[0] == FLAG)
                state = FLAG_RCV;
            else
                state = START;
            break;
        case C_RCV:
            if (buf_[0] == (ADRESS_TRANSMITER ^ control))
                state = BCC1_OK;
            else if (buf_[0] == FLAG)
                state = FLAG_RCV;
            else
                state = START;
            break;
        case BCC1_OK:
            if (buf_[0] == FLAG)
            {
                state = STOP_;
                alarm(0);
            }
            else
                state = START;
            break;
        case STOP_:
            STOP = TRUE;
            break;
        default:
            break;
        }
    }
    if (connectionParameters.role == LlRx)
    {
       write_UA(fd);
    }
    printf("Stoped \n");

    // Wait until all bytes have been written to the serial port
    sleep(1);

    return 0;
}

////////////////////////////////////////////////
// LLWRITE
////////////////////////////////////////////////
int llwrite(const unsigned char *buf, int bufSize, int I)
{
    if(bufSize<=0) return 1;

    alarmEnabled = FALSE;
    alarmCount = 0;
    
    vector v2;
    vector *v = &v2;
    vector_init(v);
    vector_push(v, FLAG,0);
    vector_push(v, ADRESS_TRANSMITER,1);
    if (I == 0)
        vector_push(v,I0,2);
    else
        vector_push(v,I1,2);
    vector_push(v,(ADRESS_TRANSMITER ^ I),3);
    
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
    while(alarmCount<4){

        if (alarmEnabled == FALSE) {
            int bytes = write(fd, v->data, v->size);
            
            printf("%d bytes written\n", bytes);
            alarm(3); 
            alarmEnabled = TRUE;
        }      


        //Get response from Receiver 


    }
    

    return 0;
}

////////////////////////////////////////////////
// LLREAD
////////////////////////////////////////////////
int llread(unsigned char *packet)
{

    int size= read_package(fd, information_frame,packet);
    if(size<0) return -1;
    unsigned char bcc2=0;
    size= size-1;
    for(int i=0; i<size;i++){
        bcc2 ^= packet[i];
    }

    if(bcc2== packet[size ]){
        write_rr(fd, information_frame);
        if (information_frame==I0) information_frame=I1;
        else information_frame=I0;
        return size;
    }else{
        write_rej(fd, information_frame);
        return -1;
    }

}

////////////////////////////////////////////////
// LLCLOSE
////////////////////////////////////////////////
int llclose(int showStatistics)
{
    // TODO

    return 1;
}
