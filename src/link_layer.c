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
    alarmCount=0;
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

    unsigned char buf_[BUF_SIZE + 1] = {0}; // +1: Save space for the final '\0' char
    
  
    enum State state = START;
    
    
    while (state != STOP_ && alarmCount < N_TRIES)
    {

        if (alarmEnabled == FALSE && connectionParameters.role == LlTx)
        {
            
            write_s_u_d(fd,CONTROL_SET);
            alarm(TIMEOUT);
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
    if(alarmCount>=N_TRIES) return 1;
    if (connectionParameters.role == LlRx)
    {
       write_s_u_d(fd,CONTROL_UA);
    }
    printf("Stoped \n");

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
    printf("I : %X\n",information_frame);
    alarmEnabled = FALSE;
    alarmCount = 0;
    
    int res=0;
    while(alarmCount<N_TRIES){

        if (alarmEnabled == FALSE || res==-1) {
            write_i_frame(fd, buf, bufSize, information_frame); 
            alarm(TIMEOUT);
            alarmEnabled = TRUE;
        }    
        res  = read_s_u_frame(fd, information_frame);
        if(res==0){
            printf("Accepetd\n");
            if(information_frame==I0) information_frame=I1;
            else information_frame=I0;
            return 0;
        }


    }
    

    return 1;
}

////////////////////////////////////////////////
// LLREAD
////////////////////////////////////////////////
int llread(unsigned char *packet)
{
    printf("I : %X\n",information_frame);
    int size= read_package(fd, information_frame,packet);
    unsigned char bcc2=0;
    size= size-1;
    for(int i=0; i<size;i++){
        bcc2 ^= packet[i];
    }
    printf("Size %d \n", size);
    if(bcc2== packet[size ]){
        printf("Accepetd\n");
        if(size==-1){
            if(information_frame==I0)write_rr(fd, I0);
            else write_rr(fd, I1);
            return -1;
        }else{
            write_rr(fd, information_frame);
            if (information_frame==I0) information_frame=I1;
            else information_frame=I0;
            return size;
        }
    }else{
         printf("packet %X\n", packet[size+1]);
         printf("packet %X\n", packet[size]);
         printf("packet %X\n", bcc2);

        sleep(3);
        
        printf("Rejected\n");
        write_rej(fd, information_frame);
        return -1;
    }

}

////////////////////////////////////////////////
// LLCLOSE
////////////////////////////////////////////////
int llclose(int showStatistics, LinkLayer connectionParameters)
{
    
    if (connectionParameters.role == LlTx) {
        alarmEnabled = FALSE;
        alarmCount = 0;

        while(alarmCount<N_TRIES){

            if (alarmEnabled == FALSE) {
                write_s_u_d(fd, CONTROL_DISC);
                alarmEnabled = TRUE;
                alarm(TIMEOUT);
            }     
 
            if(!read_disc(fd)){
                write_s_u_d(fd, CONTROL_UA);
                printf("Closed successfuly\n");
                return 0;
            }
        }

    }

    else {

        while(read_disc(fd));
        do {
            printf("write disc\n");
            write_s_u_d(fd,CONTROL_DISC);
        } while(read_UA(fd) ) ;
        printf("Closed successfuly\n");
        return 0;
    
    }


    return 1;
}
