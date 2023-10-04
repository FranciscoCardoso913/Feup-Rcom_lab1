// Write to serial port in non-canonical mode
//
// Modified by: Eduardo Nuno Almeida [enalmeida@fe.up.pt]


#include "header.c"
// Baudrate settings are defined in <asm/termbits.h>, which is
// included by <termios.h>
#define BAUDRATE B38400
#define _POSIX_SOURCE 1 // POSIX compliant source


int alarmEnabled = FALSE;
int alarmCount = 0;

volatile int STOP = FALSE;
void alarmHandler(int signal)
{
    alarmEnabled = FALSE;
    alarmCount++;

    printf("Alarm #%d\n", alarmCount);
}
int llopen(int port, int role){
        // Create string to send
    unsigned char buf[5] = {0};


    unsigned char buf_[BUF_SIZE + 1] = {0}; // +1: Save space for the final '\0' char
    enum State {START,FLAG_RCV, A_RCV, C_RCV,BCC_OK,STOP_};
    enum State state= START;
    while (state != STOP_ && alarmCount < 4)
    {
        
        if (alarmEnabled == FALSE && role == TRANSMITTER)
        {
            buf[0]= FLAG;
            buf[1]=ADRESS_TRANSMITER;
            buf[2]=CONTROL_SET;
            buf[3]=(ADRESS_TRANSMITER ^ CONTROL_SET);
            buf[4]=FLAG;
            int bytes = write(port, buf, 5);
            printf("%d bytes written\n", bytes);
            alarm(3); // Set alarm to be triggered in 3s
            alarmEnabled = TRUE;
        }
        int adress = 0;
        int control = 0;

        if(role == TRANSMITTER ){
            adress = ADRESS_RECIVER;
            control = CONTROL_UA;
        }
        else{
            adress = ADRESS_TRANSMITER;
            control = CONTROL_SET;
        }
    
        // Returns after 5 chars have been input
        int bytes = read(port, buf_, 1);
        
        switch (state)
        {
            case START:
                if(buf_[0] == FLAG ) state=FLAG_RCV;
                break;
            case FLAG_RCV:

                if(buf_[0]== adress ) state=A_RCV;
                else if(buf_[0]==FLAG) break;
                else state= START;
                break;

            case A_RCV:
                if(buf_[0]== control ) state=C_RCV;
                else if(buf_[0]==FLAG) state= FLAG_RCV;
                else state=START;
                break;
            case C_RCV:
                if(buf_[0]== (adress^control) ) state=BCC_OK;
                else if(buf_[0]==FLAG) state=FLAG_RCV;
                else state=START;
                break;
            case BCC_OK:
                if(buf_[0]==FLAG){ 
                    state=STOP_;
                    sleep(0);
                    }
                else state=START;
                break;
            case STOP_:
                STOP=TRUE;
                break;
            default:
                break;
        }
        


    }
    if(role == RECIVER){
        buf[0]=FLAG;
        buf[1]=ADRESS_RECIVER;
        buf[2]=CONTROL_UA;
        buf[3]=(ADRESS_RECIVER^CONTROL_UA);
        buf[4]=FLAG;

        int bytes = write(port, buf, 5);
        printf("%d bytes written\n", bytes);
    }
    printf("Stoped");

    // Wait until all bytes have been written to the serial port
    sleep(1);

};
