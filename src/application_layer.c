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

    while(llopen(l))printf("ola\n");;

    if(l.role==LlRx) {

        //PREPARING PACKETS

    }

    else {

        //PREPARING PACKETS

    }
  
    llclose(0);
}
