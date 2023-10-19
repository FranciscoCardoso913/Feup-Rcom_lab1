#include "read.h"

int read_package(int fd, int information_frame,unsigned char * packet){
    int size=0;
    int debuf= FALSE;
    enum State state = START;
    unsigned char buf[BUF_SIZE + 1] = {0};
    while(state != STOP_){
        // Returns after 5 chars have been input
        read(fd, buf, 1);
        if(debuf){
            if(buf[0]== STUFED_FLAG) packet[size]=FLAG;
            else if(buf[0] == STUFED_ESCAPE) packet[size]=ESCAPE;
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
                if (buf[0] == information_frame)
                    state = C_RCV;
                else if (buf[0] == FLAG)
                    state = FLAG_RCV;
                else
                    state = START;
                break;
            case C_RCV:
                if (buf[0] == (ADRESS_TRANSMITER ^ information_frame))
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
            default:
                break;
            }
    }
    }

    printf("STOPED\n");
    return size;
}