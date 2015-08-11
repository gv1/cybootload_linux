/*******************************************************************************
 * File Name: communication_api.h  
 * Version 1.0
 *
 * Description:
 * This is the header file for the communication module created by the author.  
 * It contains function prototypes and constants for the users' convenience. 
 ********************************************************************************/

#include <stdint.h>
#include "cybtldr_utils.h"

typedef     unsigned char uint8;
typedef     unsigned short uint16;

#define MODEMDEV "/dev/ttyACM0"
#define COMSPEED B115200

/* Function declarations */
int OpenConnection(void);
int CloseConnection(void);
int ReadData(uint8* rdData, int byteCnt);
int WriteData(uint8* wrData, int byteCnt);

//[] END OF FILE

