// Read from serial port in non-canonical mode
//
// Modified by: Eduardo Nuno Almeida [enalmeida@fe.up.pt]

#include "headers.h"

int check_state(unsigned char read_char,unsigned char wanted_char, int new_state, int *current_state){
    if(read_char == wanted_char){
        *current_state = new_state;
        return TRUE;
    }
    return FALSE;
}

//State machine for reading set messages.
void read_SET(int fd){
    //small buffer for reading from serial port
    unsigned char buf[2];

    int state = START;
    while(state != STOP){
        int flag = 0;
        int bytes = read(fd, buf, 1);
        unsigned char read_char = buf[0];
        if(bytes != 0){
            switch(state){
                case START:
                    flag = check_state(read_char,F,FLAG_RCV,&state);
                    if(flag == FALSE)
                        state = START;
                    break;
                case FLAG_RCV:
                    flag = check_state(read_char,A_W,A_RCV,&state) + check_state(read_char,F,FLAG_RCV,&state);
                    if(flag == FALSE)
                        state = START;
                    break;
                case A_RCV:
                    flag = check_state(read_char,SET,C_RCV,&state) + check_state(read_char,F,FLAG_RCV,&state);
                    if(flag == FALSE)
                        state = START;
                    break;
                case C_RCV:
                    flag = check_state(read_char,BCC1_SET,BCC_OK,&state) + check_state(read_char,F,FLAG_RCV,&state);
                    if(flag == FALSE)
                        state = START;
                    break;
                case BCC_OK:
                    flag = check_state(read_char,F,STOP,&state);
                    if(flag == FALSE)
                        state = START;
                    break;
                default:
                    break;
            }
        } 
    }
}

void send_UA(int fd){
    write(fd, ua, CONTROL_FRAME_SIZE);
    printf("Sent UA to writer\n");
    // Wait until all bytes have been written to the serial port
    sleep(1);
}

int main(int argc, char *argv[])
{
    // Program usage: Uses either COM1 or COM2
    const char *serialPortName = argv[1];

    if (argc < 2)
    {
        printf("Incorrect program usage\n"
               "Usage: %s <SerialPort>\n"
               "Example: %s /dev/ttyS1\n",
               argv[0],
               argv[0]);
        exit(1);
    }

    // Open serial port device for reading and writing and not as controlling tty
    // because we don't want to get killed if linenoise sends CTRL-C.
    int fd = open(serialPortName, O_RDWR | O_NOCTTY);
    if (fd < 0)
    {
        perror(serialPortName);
        exit(-1);
    }

    struct termios oldtio;
    struct termios newtio;

    // Save current port settings
    if (tcgetattr(fd, &oldtio) == -1)
    {
        perror("tcgetattr");
        exit(-1);
    }

    // Clear struct for new port settings
    memset(&newtio, 0, sizeof(newtio));

    newtio.c_cflag = BAUDRATE | CS8 | CLOCAL | CREAD;
    newtio.c_iflag = IGNPAR;
    newtio.c_oflag = 0;

    // Set input mode (non-canonical, no echo,...)
    newtio.c_lflag = 0;
    newtio.c_cc[VTIME] = 0; // Inter-character timer unused
    newtio.c_cc[VMIN] = 5;  // Blocking read until 5 chars received

    // VTIME e VMIN should be changed in order to protect with a
    // timeout the reception of the following character(s)

    // Now clean the line and activate the settings for the port
    // tcflush() discards data written to the object referred to
    // by fd but not transmitted, or data received but not read,
    // depending on the value of queue_selector:
    //   TCIFLUSH - flushes data received but not read.
    tcflush(fd, TCIOFLUSH);

    // Set new port settings
    if (tcsetattr(fd, TCSANOW, &newtio) == -1)
    {
        perror("tcsetattr");
        exit(-1);
    }

    printf("New termios structure set\n");


    //<-----------Serial Port Settings over----------->



    read_SET(fd);
    printf("Established Connection with writer\n");

    send_UA(fd);


    // Restore the old port settings
    if (tcsetattr(fd, TCSANOW, &oldtio) == -1)
    {
        perror("tcsetattr");
        exit(-1);
    }

    close(fd);

    return 0;
}
