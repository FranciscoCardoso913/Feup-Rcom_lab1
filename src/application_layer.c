// Application layer protocol implementation

#include "application_layer.h"

int frameSent = 0;
long fileSize = 0;
double timeSpentReadingFrames = 0;
LinkLayer l;
int totalAlarmsCount = 0;
int totalRejCount = 0;
time_t startOfProgram;
time_t endOfProgram;

void applicationLayer(const char *serialPort, const char *role, int baudRate,
                      int nTries, int timeout, const char *filename)
{

    // time variables
    time_t start;
    time_t end;

    startOfProgram = time(NULL);
    // Configuring link layer
    l.baudRate = baudRate;
    l.nRetransmissions = nTries;
    if (role[0] == 't')
        l.role = LlTx;
    else
        l.role = LlRx;
    strcpy(l.serialPort, serialPort);
    l.timeout = timeout;

    // Connecting to serial port
    if (llopen(l))
    {
        printf("Time exeded!\nFailed to connect\n");
        return;
    }
    printf("Connected\n");
    start = time(NULL);

    // Receiving File
    if (l.role == LlRx)
    {

        // Creating file to write. If it already exists, it will be overwritten.
        FILE *file = fopen(filename, "wb");
        if (file == NULL)
        {
            printf("Error: Unable to create or open the file %s for writing.\n", filename);
            return;
        }

        long bytes_to_read = 0;
        unsigned char packet[MAX_PAYLOAD_SIZE + 5];

        // Read until the first start control packet is received
        do
        {
            llread(packet);
        } while (packet[0] != C_START);
        if (packet[1] == 0x00)
        {
            bytes_to_read = 0;
            unsigned char n_bytes = packet[2];
            int c = 0;
            for (int i = n_bytes; i > 0; i--)
            {
                bytes_to_read += pow_int(256, i - 1) * (int)packet[c + 3];
                c++;
            }
            fileSize = bytes_to_read;
        }
        else
        {
            printf("Error in the start control packet\n");
            exit(1);
        }

        // Read until the end control packet is received
        int size = -1;

        while (packet[0] != C_END)
        {

            // Read until a data packet is received
            while (size == -1)
            {

                size = llread(packet);
            }

            // Check if the packet is the end control packet
            if (packet[0] == C_END)
                break;

            // Write the data to the file
            if (size > 0)
            {
                size_t bytesWrittenNow = fwrite(packet + 3, 1, size - 3, file);
                if (bytesWrittenNow == (long)0)
                    break;
                bytes_to_read -= bytesWrittenNow;
            }
            size = -1;
        }

        // Check if all the bytes were read and written
        if (bytes_to_read > (long)0)
        {
            printf("Error: Data was lost! %ld bytes were lost\n", bytes_to_read);
            exit(1);
        }
        printf("All bytes were read and written\n");
        end = time(NULL);
        timeSpentReadingFrames += difftime(end, start);

        fclose(file);
    }

    // Sending File
    else
    {

        // Opening file to read.
        FILE *file = fopen(filename, "rb");
        if (file == NULL)
        {
            printf("Error: Unable to open the file %s for reading.\n", filename);
            return;
        }

        // Get file size
        fseek(file, 0, SEEK_END);
        long fileLength = ftell(file);
        fseek(file, 0, SEEK_SET);

        unsigned char buffer[MAX_PAYLOAD_SIZE];
        size_t bytes_to_Read = MAX_PAYLOAD_SIZE;

        // Send start control packet
        fileSize = fileLength;
        vector *v_open = write_control(C_START, filename, fileSize);
        if (llwrite(v_open->data, v_open->size))
        {
            printf("Time exeded\n");
            return;
        }

        // Send data packets until all the bytes are sent
        while (fileLength > 0)
        {

            // Read the file to check if it is the last packet
            int bytesRead = fread(buffer, 1, (fileLength >= bytes_to_Read) ? bytes_to_Read : fileLength, file);
            if (bytesRead == 0)
            {
                printf("Error reading the file.\n");
                break;
            }

            fileLength -= bytesRead;
            vector *v = write_data(buffer, bytesRead);
            if (llwrite(v->data, v->size))
            {
                printf("timed exeded\n");
                return;
            }
        }

        // Send end control packet
        vector *v_close = write_control(C_END, filename, fileSize);
        llwrite(v_close->data, v_close->size);

        printf("All bytes were written\n");

        end = time(NULL);
        timeSpentReadingFrames += difftime(end, start);

        fclose(file);
    }
    endOfProgram = time(NULL);

    llclose(SHOW_STATISTICS, l);
}
