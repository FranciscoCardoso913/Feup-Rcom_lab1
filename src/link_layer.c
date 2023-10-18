// Link layer protocol implementation

#include "link_layer.h"
#include "utils.h"
// MISC
#define _POSIX_SOURCE 1 // POSIX compliant source

extern int alarmEnabled;
extern int alarmCount;
volatile int STOP = FALSE;
int fd;

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
////////////////////////////////////////////////
// LLOPEN
////////////////////////////////////////////////
int llopen(LinkLayer connectionParameters)
{
    
    fd = open(connectionParameters.serialPort, O_RDWR | O_NOCTTY);
    if (fd < 0)
    {
        perror(connectionParameters.serialPort);
        return -1;
    }

    llconfig(fd);
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
            alarm(3); // Set alarm to be triggered in 3s
            alarmEnabled = TRUE;
        }
        int adress = 0;
        int control = 0;

        if (connectionParameters.role == LlTx)
        {
            adress = ADRESS_RECIVER;
            control = CONTROL_UA;
        }
        else
        {
            adress = ADRESS_TRANSMITER;
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
            if (buf_[0] == adress)
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
            if (buf_[0] == (adress ^ control))
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
        printf("Received\n");
        buf[0] = FLAG;
        buf[1] = ADRESS_RECIVER;
        buf[2] = CONTROL_UA;
        buf[3] = (ADRESS_RECIVER ^ CONTROL_UA);
        buf[4] = FLAG;

        int bytes = write(fd, buf, 5);
        printf("%d bytes written\n", bytes);
    }
    printf("Stoped");

    // Wait until all bytes have been written to the serial port
    sleep(1);

    return 0;
}

////////////////////////////////////////////////
// LLWRITE
////////////////////////////////////////////////
int llwrite(const unsigned char *buf, int bufSize)
{
    if(bufSize<=0) return 1;

    int stop=0;
    alarmCount = 0;
    
    while(!stop && alarmCount<4){
        
    }
    

    return 0;
}

////////////////////////////////////////////////
// LLREAD
////////////////////////////////////////////////
int llread(unsigned char *packet)
{
    int size=0;
    unsigned char bcc2=0;
    int debuf= FALSE;
    int information_frame=I0;
    enum State state = START;
    unsigned char buf[BUF_SIZE + 1] = {0};
    while(state != STOP_){
        // Returns after 5 chars have been input
        read(fd, packet, 1);
        if(debuf){
            if(buf[0]== 0x5e) packet[size]=FLAG;
            else if(buf == 0x5d) packet[size]=ESCAPE;
            else continue;
            size++;
            debuf=FALSE;
        }else{
            switch (state)
            {
            case START:
                if (buf[0] == FLAG)
                    state = FLAG_RCV;
                    
                break;
            case FLAG_RCV:
                if (buf[0] == ADRESS_RECIVER)
                    state = A_RCV;
                else if (buf[0] == FLAG)
                    break;
                else
                    state = START;
                break;

            case A_RCV:
                if (buf[0] == information_frame)
                    state = C_RCV;
                else if (buf[0] == FLAG)
                    state = FLAG_RCV;
                else
                    state = START;
                break;
            case C_RCV:
                if (buf[0] == (ADRESS_RECIVER ^ information_frame))
                    state = BCC1_OK;
                else if (buf[0] == FLAG)
                    state = FLAG_RCV;
                else
                    state = START;
                break;
            case BCC1_OK:
                if (buf[0] == FLAG)state= STOP_;
                else
                    if(buf[0]==ESCAPE){
                        debuf=TRUE;
                    }else{
                        packet[size]=buf[0];
                        size++;
                    }
                break;
            case STOP_:
                STOP = TRUE;
                break;
            default:
                break;
            }
    }
    }
    size= size-1;
    for(int i=0; i<size;i++){
        if(i==0) bcc2= packet[i];
        else{
            bcc2 = bcc2 ^packet[i];
        }
    }
    unsigned char buf_[ 5] = {0};
    buf_[0]= FLAG;
    buf_[1]=ADRESS_RECIVER;
    buf_[4] =FLAG; 
    if(bcc2== packet[size +1]){
        if(information_frame==I0){
            buf_[2]=0x01;
        }else{
            buf_[2]=0x81;
        }
        buf_[3]= buf_[2] ^buf_[1];
        return -1;
    }else{
        if(information_frame==I0){
            buf_[2]=0x85;
            information_frame= I1;
        }else{
            buf_[2]=0x05;
            information_frame=I0;
        }
        buf_[3]= buf_[2] ^buf_[1];
        return size;

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
