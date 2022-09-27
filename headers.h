#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <termios.h>
#include <unistd.h>

//<----------SERIAL PORT CONFIG---------->

// Baudrate settings are defined in <asm/termbits.h>, which is
// included by <termios.h>
#define BAUDRATE B38400
#define _POSIX_SOURCE 1 // POSIX compliant source

#define FALSE 0
#define TRUE 1

volatile int STOP = FALSE;

//<----------SERIAL PORT CONFIG END---------->

//<----------FRAMES---------->

#define CONTROL_FRAME_SIZE 5

//END & START FLAG
#define F 0x7E

//ADRESS FIELD
// --A_W is for commands sent by the writer and responses sent by the Reader.
#define A_W 0x03
// --A_R is for commands sent by the Reader and responses sent by the Writer.
#define A_R 0x01

//CONTROL FIELD
#define SET 0x03
#define DISC 0x0B
#define UA 0x07

#define BCC1_W A_W^SET
#define BCC1_R A_R^SET

//<----------FRAMES END---------->

//<----------OTHER---------->

#define BUF_SIZE 256

//<----------OTHER END---------->