// Application layer protocol implementation

#include "application_layer.h"

int tramas_sent=0;
long fileSize=0;
LinkLayer l;
void applicationLayer(const char *serialPort, const char *role, int baudRate,
                      int nTries, int timeout, const char *filename)
{
    //Configuring link layer
    
    l.baudRate=baudRate;
    l.nRetransmissions=nTries;
    if(role[0]=='t') l.role=LlTx;
    else l.role=LlRx;
    strcpy(l.serialPort, serialPort);
    l.timeout=timeout;
    
    
    if(llopen(l)){
        printf("Time exeded!\nFailed to connect\n");
        return ;
    }
    printf("Connected\n");


    if(l.role==LlRx) {
        FILE *file = fopen(filename, "wb");
        if (file == NULL) {
            printf("Error: Unable to create or open the file %s for writing.\n", filename);
            return;
        }
        
        long bytes_to_read=0;

        unsigned char packet[MAX_PAYLOAD_SIZE+5];
        do{
            llread(packet);
        }while(packet[0]!= C_START);
        if(packet[1]==0x00){
            bytes_to_read=0;
            unsigned char n_bytes = packet[2];
            int c=0;
            for(int i= n_bytes; i>0;i--){
                bytes_to_read+= pow_int(256,i-1)*(int)packet[c+3];
                c++;
            }

        }
        int size=-1;
        while(packet[0]!=C_END){
            while(size==-1){
                size = llread(packet);
                printf("%d\n ", size);
            }
            if(packet[0]==C_END) break;
            if(size>0){
                size_t bytesWrittenNow = fwrite(packet+3, 1, size-3, file);
                if(bytesWrittenNow==(long)0) break;
                bytes_to_read -= bytesWrittenNow;
            }
            size=-1;
        
        }

        if(bytes_to_read>(long)0){ 
            printf("Error: Data was lost! %ld bytes were lost\n", bytes_to_read);
            exit(1);
            }
        printf("All bytes where read and written\n");
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

        unsigned char buffer[MAX_PAYLOAD_SIZE];
        size_t bytes_to_Read=MAX_PAYLOAD_SIZE;
        
        fileSize = fileLength;
        vector *v_open = write_control( C_START, filename, fileSize);
        if(llwrite(v_open->data, v_open->size)){
            printf("Time exeded\n");
            return ;
        }

        while(fileLength>0){
            int bytesRead = fread(buffer, 1, (fileLength >= bytes_to_Read) ? bytes_to_Read : fileLength, file);
            if (bytesRead == 0) {
                printf("Error reading the file.\n");
                break;
            }
           
            fileLength -= bytesRead;
            vector * v = write_data(buffer,bytesRead);
            if(llwrite(v->data, v->size)){
                printf("timed exeded\n");
                return ;
            }
        }

        vector *v_close = write_control( C_END, filename, fileSize);
        llwrite(v_close->data, v_close->size);

        printf("All bytes where written\n");
        
        fclose(file);

    }
    llclose(0, l);
}
