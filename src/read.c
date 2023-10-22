#include "read.h"
extern int alarmEnabled;
int read_package(int fd, int information_frame,unsigned char * packet){
    unsigned char read_i;
    int size=0;
    int debuf= FALSE;
    enum State state = START;
    unsigned char buf[BUF_SIZE + 1] = {0};
    while(state != STOP_){
        // Returns after 5 chars have been input
        read(fd, buf, 1);
        if(debuf){
            if(buf[0]== STUFFED_FLAG) packet[size]=FLAG;
            else if(buf[0] == STUFFED_ESCAPE) packet[size]=ESCAPE;
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
   
                if (buf[0] == ADRESS_TRANSMITER)
                    state = A_RCV;
                else if (buf[0] == FLAG)
                    break;
                else
                    state = START;
                break;

            case A_RCV:

                if (buf[0] == I0 || buf[0]==I1){
                    state = C_RCV;
                    read_i= buf[0];
                }
                else if (buf[0] == FLAG)
                    state = FLAG_RCV;
                else
                    state = START;
                break;
            case C_RCV:
   
                if (buf[0] == (ADRESS_TRANSMITER ^ read_i))
                    state = BCC1_OK;
                else if (buf[0] == FLAG)
                    state = FLAG_RCV;
                else
                    state = START;
                break;
            case BCC1_OK:
    
                if (buf[0] == FLAG){
                    printf("Stoped\n");
                    state=STOP_;
                    break;
                }
                else{
                    if(buf[0]==ESCAPE){
                        debuf=TRUE;
                    }else{
                        packet[size]=buf[0];
                        size++;
                    }
                }
                break;
            default:
                break;
            }
    }
    }

    printf("STOPED\n");
    if(information_frame!=read_i) return -1;
    return size;
}

int read_s_u_frame(int fd,int information_frame){
    alarm(3);
    unsigned char res=0;
    enum State state = START;
    unsigned char buf[BUF_SIZE + 1] = {0};
    while(state != STOP_ && alarmEnabled){
        // Returns after 5 chars have been input
        read(fd, buf, 1);
        switch (state)
        {
        case START:
            if (buf[0] == FLAG)
                state = FLAG_RCV;
                
            break;
        case FLAG_RCV:
      
            if (buf[0] == ADRESS_TRANSMITER)
                state = A_RCV;
            else if (buf[0] == FLAG)
                break;
            else
                state = START;
            break;

        case A_RCV:

            if(buf[0]==RR0){
                if(information_frame==I1){
                    state= C_RCV;
                    res=RR0;
                }
                else{
                    printf("ERROR\n");
                    return -1;
                }
            }else if( buf[0]==RR1){
                if(information_frame==I0){
                    state= C_RCV;
                    res=RR1;
                }
                else{
                    printf("ERROR\n");
                    return -1;
                }

            }else if(buf[0]==REJ0 || buf[0]==REJ1){
                return -1;
            }else{
                state=START;
            }
            break;
        case C_RCV:
   
            if (buf[0] == (ADRESS_TRANSMITER ^ res))
                state = BCC1_OK;
            else if (buf[0] == FLAG)
                state = FLAG_RCV;
            else
                state = START;
            break;
        case BCC1_OK:
   
            if (buf[0] == FLAG){
                return 0;
            }
            else{
               state=START;
            }
            break;
        default:
            break;
        }
    }
    return 1;

}

int read_disc(int fd){
    alarm(3);
    alarmEnabled=TRUE;
    enum State state = START;
    unsigned char buf[BUF_SIZE + 1] = {0};
    while(state != STOP_ && alarmEnabled){
        // Returns after 5 chars have been input
        read(fd, buf, 1);
        switch (state)
        {
        case START:
            if (buf[0] == FLAG)
                state = FLAG_RCV;
                
            break;
        case FLAG_RCV:
      
            if (buf[0] == ADRESS_TRANSMITER)
                state = A_RCV;
            else if (buf[0] == FLAG)
                break;
            else
                state = START;
            break;

        case A_RCV:
            if (buf[0] == (CONTROL_DISC))
                state = C_RCV;
            else if (buf[0] == FLAG)
                state = FLAG_RCV;
            else
                state = START;

            break;
        case C_RCV:
   
            if (buf[0] == (ADRESS_TRANSMITER ^ CONTROL_DISC))
                state = BCC1_OK;
            else if (buf[0] == FLAG)
                state = FLAG_RCV;
            else
                state = START;
            break;
        case BCC1_OK:
   
            if (buf[0] == FLAG){
                alarm(0);
                return 0;
            }
            else{
               state=START;
            }
            break;
        default:
            break;
        }
    }
    return 1;

}

int read_UA(int fd){
    enum State state = START;
    unsigned char buf[BUF_SIZE + 1] = {0};
    while(state != STOP_ ){
        // Returns after 5 chars have been input
        read(fd, buf, 1);
        switch (state)
        {
        case START:
            if (buf[0] == FLAG)
                state = FLAG_RCV;
                
            break;
        case FLAG_RCV:
      
            if (buf[0] == ADRESS_TRANSMITER)
                state = A_RCV;
            else if (buf[0] == FLAG)
                break;
            else
                state = START;
            break;

        case A_RCV:
            if (buf[0] == (CONTROL_UA))
                state = C_RCV;
            else if (buf[0] == FLAG)
                state = FLAG_RCV;
            else
                state = START;

            break;
        case C_RCV:
   
            if (buf[0] == (ADRESS_TRANSMITER ^ CONTROL_UA))
                state = BCC1_OK;
            else if (buf[0] == FLAG)
                state = FLAG_RCV;
            else
                state = START;
            break;
        case BCC1_OK:
   
            if (buf[0] == FLAG){
                return 0;
            }
            else{
               state=START;
            }
            break;
        default:
            break;
        }
    }
    return 1;
}