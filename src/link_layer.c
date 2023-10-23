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
int information_frame = I0;
extern int frameSent;
extern long fileSize;
extern double timeSpentReadingFrames;
extern time_t startOfProgram;
extern time_t endOfProgram;
extern int totalAlarmsCount;
extern int totalRejCount;

////////////////////////////////////////////////
// LLOPEN
////////////////////////////////////////////////
int llopen(LinkLayer connectionParameters)
{

    // Activate alarm handler
    alarmCount = 0;
    (void)signal(SIGALRM, alarmHandler);
    if (alarmCount == 0)
    {
        // Open serial port device for reading and writing and not as controlling tty
        fd = open(connectionParameters.serialPort, O_RDWR | O_NOCTTY);

        if (fd < 0)
        {
            perror(connectionParameters.serialPort);
            return -1;
        }

        // Save current serial port settings
        llconfig(fd);
    }

    unsigned char buf_[BUF_SIZE + 1] = {0}; // +1: Save space for the final '\0' char

    // Set state machine initial state
    enum State state = START;

    // Loop until it reaches the stop state or the alarm tries exceeded the maximum
    while (state != STOP_ && alarmCount < N_TRIES)
    {

        // The transmitter sends a SET frame if it is the first time it is sending a frame
        if (alarmEnabled == FALSE && connectionParameters.role == LlTx)
        {
            write_s_u_d(fd, CONTROL_SET);
            alarm(TIMEOUT);
            alarmEnabled = TRUE;
        }
        int control = 0;

        // What tipe of frame is the receiver expecting
        if (connectionParameters.role == LlTx)
        {
            control = CONTROL_UA;
        }
        else
        {
            control = CONTROL_SET;
        }

        // Read one byte from the serial port
        read(fd, buf_, 1);

        // Check the state machine
        switch (state)
        {

        // Confirm that the first byte is a FLAG
        case START:
            if (buf_[0] == FLAG)
                state = FLAG_RCV;

            break;

        // Confirm that the second byte is the adress of the transmitter. If not, go back to the start state
        case FLAG_RCV:
            if (buf_[0] == ADRESS_TRANSMITER)
                state = A_RCV;
            else if (buf_[0] == FLAG)
                break;
            else
                state = START;
            break;

        // Confirm that the third byte is the control byte that the receiver is expecting. If not, go back to the start state.
        case A_RCV:
            if (buf_[0] == control)
                state = C_RCV;
            else if (buf_[0] == FLAG)
                state = FLAG_RCV;
            else
                state = START;
            break;

        // Confirm that the fourth byte is the XOR of the adress and the control bytes to confirm that the frame is valid. If not, go back to the first state.
        case C_RCV:
            if (buf_[0] == (ADRESS_TRANSMITER ^ control))
                state = BCC1_OK;
            else if (buf_[0] == FLAG)
                state = FLAG_RCV;
            else
                state = START;
            break;

        // Confirm that the fifth byte is a flag. If it it, then we set an alarm and we quit from the while. If not, go back to the start state.
        case BCC1_OK:
            if (buf_[0] == FLAG)
            {
                state = STOP_;
                alarm(0);
            }
            else
                state = START;
            break;
        default:
            break;
        }
    }

    // If the alarm tries exceeded the maximum, return 1
    if (alarmCount >= N_TRIES)
        return 1;

    // If it's the receiver, send a UA frame
    if (connectionParameters.role == LlRx)
    {
        write_s_u_d(fd, CONTROL_UA);
    }

    // Wait until all bytes have been written to the serial port
    sleep(1);

    return 0;
}

////////////////////////////////////////////////
// LLWRITE
////////////////////////////////////////////////
int llwrite(const unsigned char *buf, int bufSize)
{

    if (bufSize <= 0)
        return 1;
    alarmEnabled = FALSE;
    alarmCount = 0;

    int res = 0;

    // While the alarm tries don't exceed the maximum, send the frame and wait for a response
    while (alarmCount < N_TRIES)
    {

        // If the alarm is not enabled yet, and the frame received was not a RR, send the frame
        if (alarmEnabled == FALSE || res == -1)
        {
            write_i_frame(fd, buf, bufSize, information_frame);
            frameSent++;
            alarm(TIMEOUT);
            alarmEnabled = TRUE;
        }

        // Read the response
        res = read_res_i_frame(fd, information_frame);

        // If the response was a RR, then we can send the next frame. For that, we change the information frame
        if (res == 0)
        {
            if (information_frame == I0)
                information_frame = I1;
            else
                information_frame = I0;
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

    // Read the frame
    int size = read_package(fd, information_frame, packet);

    frameSent++;

    unsigned char bcc2 = 0;
    size = size - 1;

    // Check if the frame is valid, and send a RR for the next frame
    if (size == -1)
    {

        printf("Repeted information! Resending response!\n");
        if (information_frame == I0)
            write_rr(fd, I0);
        else
            write_rr(fd, I1);
        totalRejCount++;
        return -1;
    }

    // Calculate the BCC2
    for (int i = 0; i < size; i++)
    {
        bcc2 ^= packet[i];
    }

    // Check if the BCC2 is equal to the one received
    if (bcc2 == packet[size])
    {

        // Write the RR for the next frame
        write_rr(fd, information_frame);
        // Change the information frame
        if (information_frame == I0)
            information_frame = I1;
        else
            information_frame = I0;
        return size;
    }
    else // If the BCC2 is not equal, send a REJ
    {
        printf("Rejected\n");
        write_rej(fd, information_frame);
        totalRejCount++;
        return -1;
    }
}

////////////////////////////////////////////////
// LLCLOSE
//////////////////////////////////////////////////
int llclose(int showStatistics, LinkLayer connectionParameters)
{

    // Check if the user is the transmitter or the receiver
    if (connectionParameters.role == LlTx)
    {
        alarmEnabled = FALSE;
        alarmCount = 0;

        // Try to close the connection until the alarm tries exceed the maximum
        while (alarmCount < N_TRIES)
        {

            // If the alarm is not enabled yet, send a DISC frame
            if (alarmEnabled == FALSE)
            {
                write_s_u_d(fd, CONTROL_DISC);
                alarmEnabled = TRUE;
                alarm(TIMEOUT);
            }

            // Wait for a DISC frame to send a UA frame
            if (!read_disc(fd))
            {
                write_s_u_d(fd, CONTROL_UA);
                printf("Closed successfuly\n");
                break;
            }
        }
        if (alarmCount >= N_TRIES)
        {
            printf("Timed exeded!\n");
            return 1;
        }
    }

    // If the user is the receiver
    else
    {

        // Wait for a DISC frame to send a DISC frame, and wait for a UA frame
        while (read_disc(fd))
            ;
        do
        {
            write_s_u_d(fd, CONTROL_DISC);
        } while (read_UA(fd));
        printf("Closed successfuly\n");
    }

    // If the user wants to see the statistics, print them
    if (showStatistics)
    {
        printf("\n\n\nStatistics:\n");

        if (connectionParameters.role == LlRx)
            printf("User: Receiver \n");
        else
            printf("User: Transmiter \n");

        printf("File size: %ld\n", fileSize);

        if (connectionParameters.role == LlTx)
        {
            printf("Frames sent: %d\n", frameSent);
            printf("Total number of alarms: %d\n", totalAlarmsCount);
        }
        else
        {
            printf("Frames read: %d\n", frameSent);
            printf("Number of rejection/ repeted information %d\n", totalRejCount);
        }

        printf("Time spent: %g seconds\n", timeSpentReadingFrames);
        printf("Total time: %g seconds\n", difftime(endOfProgram, startOfProgram));
    }

    return 0;
}
