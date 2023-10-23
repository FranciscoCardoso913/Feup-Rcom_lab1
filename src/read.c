#include "read.h"

extern int alarmEnabled;

// Reads a package
int read_package(int fd, int information_frame, unsigned char *packet)
{
    // information frame that is received in the package
    unsigned char read_i;

    // size of the package
    int size = 0;

    // flag that signals if a defuf is needed
    int deStuff = FALSE;

    // Current state of the state machine
    enum State state = START;
    unsigned char buf[BUF_SIZE + 1] = {0};
    while (state != STOP_)
    {

        // Returns after 30 seconds without input
        read(fd, buf, 1);

        // Code to defuf a byte
        if (deStuff)
        {
            if (buf[0] == STUFFED_FLAG)
                packet[size] = FLAG;
            else if (buf[0] == STUFFED_ESCAPE)
                packet[size] = ESCAPE;
            else
                continue;
            size++;
            deStuff = FALSE;
        }
        else
        {
            // State machine
            switch (state)
            {
            case START:
                // First state
                if (buf[0] == FLAG)
                    state = FLAG_RCV;

                break;
            case FLAG_RCV:
                // State after receiving a flag
                if (buf[0] == ADRESS_TRANSMITER)
                    state = A_RCV;
                else if (buf[0] == FLAG)
                    // if flag is received should not do nothing
                    break;
                else
                    // if value read is neither the Adress nor the flag it should start over again
                    state = START;
                break;

            case A_RCV:
                // State after receiving the adress
                if (buf[0] == I0 || buf[0] == I1)
                {
                    state = C_RCV;
                    // information frame can be either one, we will store it for now and deal with it later
                    read_i = buf[0];
                }
                else if (buf[0] == FLAG)
                    // if a flag is receive it should go back to the flag state
                    state = FLAG_RCV;
                else
                    // Any other value should make the machine go back to the begining
                    state = START;
                break;
            case C_RCV:
                // State after receiving the control (information frame)
                if (buf[0] == (ADRESS_TRANSMITER ^ read_i))
                    // BCC1 is correct
                    state = BCC1_OK;
                else if (buf[0] == FLAG)
                    // if a flag is receive it should go back to the flag state
                    state = FLAG_RCV;
                else
                    // Any other value should make the machine go back to the begining
                    state = START;
                break;
            case BCC1_OK:
                // State after the BCC1, we will start reading the packet now
                if (buf[0] == FLAG)
                {
                    // When receiving the flag the all the information has been sent
                    state = STOP_;
                    break;
                }
                else
                {
                    // Reading the packet
                    if (buf[0] == ESCAPE)
                    {
                        // If a escape appears that means a destuff is needed
                        deStuff = TRUE;
                    }
                    else
                    {
                        // Otherwise we can just insert the data in the packet
                        packet[size] = buf[0];
                        size++;
                    }
                }
                break;
            default:
                break;
            }
        }
    }
    // if current information frame is different from the one read that means that the package is repeated
    // we will signal that to the link layer
    if (information_frame != read_i)
        return -1;

    // Return the size of the package
    return size;
}

// read a response to the i frame sent
int read_res_i_frame(int fd, int information_frame)
{

    // response
    unsigned char res = 0;

    // Current state of the machine state
    enum State state = START;
    unsigned char buf[BUF_SIZE + 1] = {0};

    while (state != STOP_ && alarmEnabled)
    {
        // Returns after 30 seconds without input
        read(fd, buf, 1);

        // state machine
        switch (state)
        {

        case START:
            // first state

            if (buf[0] == FLAG)
                state = FLAG_RCV;

            break;
        case FLAG_RCV:
            // state after receiving a flag

            if (buf[0] == ADRESS_TRANSMITER)
                state = A_RCV;
            else if (buf[0] == FLAG)
                // if a flag is received we should stay in this state
                break;
            else
                // if a invalid byte is read we should go back to the start
                state = START;
            break;

        case A_RCV:

            // State after receiving the Adress
            if (buf[0] == RR1 || buf[0] == RR0 || buf[0] == REJ0 || buf[0] == REJ1)
            {
                // A valid response was read, we store it and deal with it later
                res = buf[0];
                state = C_RCV;
            }
            else if (buf[0] == FLAG)
            {

                state = FLAG_RCV;
            }
            else
            {
                // invalid data we go back to the begining
                state = START;
            }
            break;
        case C_RCV:
            // State after receiving a control (response)

            if (buf[0] == (ADRESS_TRANSMITER ^ res))
                state = BCC1_OK;
            else if (buf[0] == FLAG)
                state = FLAG_RCV;
            else
                state = START;
            break;
        case BCC1_OK:
            // state after verefing if the BCC1 is OK

            if (buf[0] == FLAG)
            {
                // disabled alarm
                alarm(0);
                // A flag signal the end of the package sent

                // the response was a reject, we signal an error to the link layer
                if (res == REJ1 || res == REJ0)
                    return -1;
                // the receiver is waiting for the current information frame, an error has occur
                if ((res == RR0 && information_frame == I0) || (res == RR1 && information_frame == I1))
                    return -1;
                // Else we send a positive response
                return 0;
            }
            else
            {
                state = START;
            }
            break;
        default:
            break;
        }
    }
    return 1;
}

int read_disc(int fd)
{
    alarmEnabled = TRUE;
    enum State state = START;
    unsigned char buf[BUF_SIZE + 1] = {0};
    while (state != STOP_ && alarmEnabled)
    {
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

            if (buf[0] == FLAG)
            {
                alarm(0);
                return 0;
            }
            else
            {
                state = START;
            }
            break;
        default:
            break;
        }
    }
    return 1;
}

int read_UA(int fd)
{
    enum State state = START;
    unsigned char buf[BUF_SIZE + 1] = {0};
    while (state != STOP_)
    {
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

            if (buf[0] == FLAG)
            {
                return 0;
            }
            else
            {
                state = START;
            }
            break;
        default:
            break;
        }
    }
    return 1;
}