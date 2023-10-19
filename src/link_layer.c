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
    enum State
    {
        START = 0,
        FLAG_RCV,
        A_RCV,
        C_RCV,
        BCC_OK,
        STOP_
    };
    
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
                state = BCC_OK;
            else if (buf_[0] == FLAG)
                state = FLAG_RCV;
            else
                state = START;
            break;
        case BCC_OK:
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
int llwrite(const unsigned char *buf, int bufSize, int I)
{
    if(bufSize<=0) return 1;

    alarmCount = 0;
    
    while(alarmCount<4){
        
        

        vector *v;
        vector_init(v);
        vector_set_size(v, bufSize+6);
        vector_push(v, FLAG,0);
        vector_push(v, ADRESS_TRANSMITER,1);
        if (I == 0)
            vector_push(v,I0,2);
        else
            vector_push(v,I1,2);
        vector_push(v,(ADRESS_TRANSMITER ^ I),3);
        
        int packetsize = 4;
        char BCC2 = 0;

        while(bufSize>0){
            BCC2 ^= *buf;
            
            if (*buf == FLAG){
                vector_push(v,ESCAPE,packetsize++);
                vector_push(v, 0x5e, packetsize);
                vector_set_size(v, v->size+1);
            }
            else if(*buf == ESCAPE){
                vector_push(v, ESCAPE, packetsize++);
                vector_push(v, 0x5d, packetsize);
                vector_set_size(v, v->size+1);
            }
            else
                vector_push(v, *buf,packetsize);
            buf++;
            bufSize--;
            packetsize++;

        }

        vector_push(v, BCC2,packetsize);
        vector_push(v,FLAG,packetsize);
        int bytes = write(fd, v->data, v->size);
        printf("%d bytes written\n", bytes);
        alarm(3); // Set alarm to be triggered in 3s
        alarmEnabled = TRUE;

        write(fd, v->data, v->size);

        sleep(2); //wait for response 

        //Get response from Receiver 

        

    }
    

    return 0;
}

////////////////////////////////////////////////
// LLREAD
////////////////////////////////////////////////
int llread(unsigned char *packet)
{
    
    

    return 0;
}

////////////////////////////////////////////////
// LLCLOSE
////////////////////////////////////////////////
int llclose(int showStatistics)
{
    // TODO

    return 1;
}
