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
    sleep(2);
    if(l.role==LlRx) {
        FILE *file = fopen(filename, "wb");
        if (file == NULL) {
            printf("Error: Unable to create or open the file %s for writing.\n", filename);
            return;
        }
        size_t t=10968;
        unsigned char packet[MAX_PAYLOAD_SIZE+1];
        int size=-1;
        while(t>(size_t)0){
            while(size==-1){
                printf("Reading again\n");
                size = llread(packet);
                /*for(int i = 0; i < size; i++) {
                    printf("%x\n ", packet[i]);
                }*/
                printf("%d\n ", size);
            }
            if(size>0){
                printf("Finished reading\n");
                size_t bytesWrittenNow = fwrite(packet, 1, size, file);
                if(bytesWrittenNow==(size_t)0) break;
                t= t- bytesWrittenNow;
            }
            size=-1;
            printf("Bytes left %ld\n",t);
            if(t==(size_t)0) break;
        
        }
        printf("Finished\n");
        fclose(file);
    }

    else {

        //PREPARING PACKETS
        /*unsigned char buf[BUF_SIZE + 1] = {0}; // +1: Save space for the final '\0' char
        buf[0]=0x7e;
        buf[1]=0x7d;
        buf[2]=0xbb;
        buf[3]=0xaa;
        buf[4]=0x7f;
        buf[5]=0xac;
        buf[6]=0x45;
        buf[7]=0x05;

        //SENDING PACKETS

        while(llwrite(buf, 8, 0));*/
        FILE *file = fopen(filename, "rb");
        if (file == NULL) {
            printf("Error: Unable to open the file %s for reading.\n", filename);
            return;
        }

        fseek(file, 0, SEEK_END);
        long fileLength = ftell(file);
        fseek(file, 0, SEEK_SET);
        printf("lenght: %ld\n",fileLength);
        unsigned char buffer[MAX_PAYLOAD_SIZE];
        size_t bytes_to_Read=MAX_PAYLOAD_SIZE;
        while(fileLength>0){
            printf("Writting again\n");
            int bytesRead = fread(buffer, 1, (fileLength >= bytes_to_Read) ? bytes_to_Read : fileLength, file);
            if (bytesRead == 0) {
                printf("Error reading the file.\n");
                break;
            }
            fileLength -= bytesRead;
            if(llwrite(buffer, bytesRead,0)){
                printf("timed exeded\n");
                return ;
            }
        }
        printf("Finished\n");
        
        fclose(file);




    }
  
    //llclose(0);
}
