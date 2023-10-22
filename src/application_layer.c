// Application layer protocol implementation

#include "application_layer.h"


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
    
    
    if(llopen(l)){
        printf("Time exeded!\n");
        return ;
    }
    sleep(1);

    if(l.role==LlRx) {
        FILE *file = fopen(filename, "wb");
        if (file == NULL) {
            printf("Error: Unable to create or open the file %s for writing.\n", filename);
            return;
        }
        
        long t=0;

        unsigned char packet[MAX_PAYLOAD_SIZE+5];
        do{
            llread(packet);
        }while(packet[0]!= C_START);
        if(packet[1]==0x00){
            t=0;
            unsigned char n_bytes = packet[2];
            int c=0;
            for(int i= n_bytes; i>0;i--){
                t+= pow_int(256,i-1)*(int)packet[c+3];
                c++;
            }

        }
        int size=-1;
        while(packet[0]!=C_END){
            while(size==-1){
                size = llread(packet);
                printf("%d\n ", size);
            }
            if(size>0){
                size_t bytesWrittenNow = fwrite(packet+3, 1, size-3, file);
                if(bytesWrittenNow==(size_t)0) break;
                t -= bytesWrittenNow;
            }
            size=-1;
            printf("Bytes left %ld\n",t);
            if(t==(size_t)0) break;
        
        }
        llread(packet);
        if(t>(long)0){ 
            printf("Error: Data was lost! %ld bytes were lost\n", t);
            exit(1);
            }
        printf("Finished\n");
        fclose(file);
    }

    else {

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
        
        long size = fileLength;
        vector *v_open = write_control( C_START, filename, size);
        if(llwrite(v_open->data, v_open->size)){
            printf("Time exeded\n");
            return ;
        }

        while(fileLength>0){
            printf("Writting again\n");
            int bytesRead = fread(buffer, 1, (fileLength >= bytes_to_Read) ? bytes_to_Read : fileLength, file);
            if (bytesRead == 0) {
                printf("Error reading the file.\n");
                break;
            }
           
            fileLength -= bytesRead;
            vector * v = write_data(buffer,bytesRead);
            printf("BytesREad: %d \n", bytesRead);
            if(llwrite(v->data, v->size)){
                printf("timed exeded\n");
                return ;
            }
        }

        vector *v_close = write_control( C_END, filename, size);
        llwrite(v_close->data, v_close->size);

        printf("Finished\n");
        
        fclose(file);

    }
    sleep(1);
    printf("awake\n");
    llclose(0, l);
}
