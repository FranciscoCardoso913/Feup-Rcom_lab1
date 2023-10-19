// Application layer protocol implementation

#include "application_layer.h"
#include "link_layer.h"


void applicationLayer(const char *serialPort, const char *role, int baudRate,
                      int nTries, int timeout, const char *filename)
{
    //Configuring link layer

    LinkLayer l;
    l.baudRate=baudRate;
    l.nRetransmissions=nTries;

    if(role[0]=='t') l.role=LlTx;
    else l.role=LlRx;
    strcpy(l.serialPort, serialPort);
    l.timeout=timeout;

    while(llopen(l));

    if(l.role==LlRx) {
        unsigned char packet[MAX_PAYLOAD_SIZE];
        llread(packet);

    }

    else {

        //PREPARING PACKETS
        unsigned char buf[BUF_SIZE + 1] = {0}; // +1: Save space for the final '\0' char
        buf[0]=0x7e;
        buf[1]=0x7d;
        buf[2]=0xbb;
        buf[3]=0xaa;
        buf[4]=0x7f;
        buf[5]=0xac;
        buf[6]=0x45;
        buf[7]=0x05;

        //SENDING PACKETS

        int bytes = llwrite(buf, 8, 0);

        printf("%d bytes written\n", bytes);





    }
  
    llclose(0);
}
