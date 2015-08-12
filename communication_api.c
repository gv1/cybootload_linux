/*******************************************************************************
 * File Name: communication_api.c  
 * Version 1.0
 *
 * Description:
 * This file is created by the author . This contains definitions of APIs 
 * used in structure 'CyBtldr_CommunicationsData' defined in cybtldr_api.h ,
 * using SPI commuincations component 
 ********************************************************************************/

#include "communication_api.h"

#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <termios.h>
#include <errno.h>


static struct termios tio;
static int tty_fd;
extern int errno;

/*******************************************************************************
 * Function Name: OpenConnection
 ********************************************************************************
 *
 * Summary:
 *  Initializes the communications component : In this case UART
 *
 * Parameters:  
 *  void
 *
 * Return: 
 *  Returns a flag to indicate whether the operation was successful or not
 *
 *
 *******************************************************************************/
extern char * serial_port;
extern int serial_speed;
int OpenConnection(void)
{
  /*
    UART_Start();
  */
  int speed;
  if (serial_port == NULL)
    {
      if ((serial_port = strdup(MODEMDEV)) == NULL) {
	printf("[ERROR] malloc failed\n");
	return(CYRET_ERR_FILE);
      }
    }
  // tty_fd=open(argv[1], O_RDWR | O_NONBLOCK);      
  tty_fd=open(serial_port, O_RDWR | O_NOCTTY | O_SYNC); 
  if ( tty_fd == -1) {
    printf("[ERROR] opening %s : %s\n",serial_port,strerror(errno));
    exit(1);
    // return (CYRET_ERR_FILE);
  }
  memset(&tio,0,sizeof(tio));
  tio.c_iflag=0;
  tio.c_oflag=0;
  tio.c_cflag=CS8|CREAD|CLOCAL;           // 8n1, see termios.h for more information
  tio.c_lflag=0;
  tio.c_cc[VMIN]=1;
  tio.c_cc[VTIME]=5;


  switch(serial_speed) {
  case 9600:
    speed = B9600;
    break;
  case 19200:
    speed = B19200;
    break;
  case 38400:
    speed = B38400;
    break;
  case 57600:
    speed = B57600;
    break;
  case 115200:
    speed = B115200;
    break;    
  default:
    speed = COMSPEED;
    printf("[INFO] unsupported speed %d, defaulting to %d\n",serial_speed,speed);
    break;
  }
  if ( cfsetospeed(&tio,speed) == -1) {            // 115200 baud
    printf("[ERROR] %s\n",strerror(errno));
    return (CYRET_ERR_FILE);    
  }
  if (cfsetispeed(&tio,speed) == -1) {            // 115200 baud
    printf("[ERROR] %s\n",strerror(errno));
    return (CYRET_ERR_FILE);     
  }
  if (tcsetattr(tty_fd,TCSANOW,&tio) == -1) {
    printf("[ERROR] %s\n",strerror(errno));
    return (CYRET_ERR_FILE);    
  }
  return(CYRET_SUCCESS);
}


/*******************************************************************************
 * Function Name: CloseConnection
 ********************************************************************************
 *
 * Summary:
 *  Clears the status and stops the communications component (UART).
 *
 * Parameters:  
 *  void
 *
 * Return: 
 *  Returns a flag to indicate whether the operation was successful or not
 *
 *
 *******************************************************************************/
int CloseConnection(void)
{
  /*
    UART_Stop();
  */
  printf("[INFO] Closing UART connection\n");
  close(tty_fd);
  return(CYRET_SUCCESS);
}

/*******************************************************************************
 * Function Name: WriteData
 ********************************************************************************
 *
 * Summary:
 *  Writes the specified number of bytes using the communications component (UART)
 *
 * Parameters:  
 *  wrData - Pointer to write data buffer
 *  byteCnt - No. of bytes to be written 
 *
 * Return: 
 *  Returns a flag to indicate whether the operation was successful or not
 *
 *
 *******************************************************************************/
int WriteData(uint8* wrData, int byteCnt)
{
  if (write(tty_fd,wrData,byteCnt) == -1){
    printf("[ERROR]: writing data to serial port: %s\n",strerror(errno));
    return(CYRET_ERR_COMM_MASK);
  }
  return(CYRET_SUCCESS);
}


/*******************************************************************************
 * Function Name: ReadData
 ********************************************************************************
 *
 * Summary:
 *  Reads the specified number of bytes usign the communications component (UART)
 *
 * Parameters:  
 *  rdData - Pointer to read data buffer
 *  byteCnt - No. of bytes to be read 
 *
 * Return: 
 *  Returns a flag to indicate whether the operation was successful or not
 *
 *
 *******************************************************************************/
int ReadData(uint8* rdData, int byteCnt)
{
  //  read(tty_fd,rdData,byteCnt);
  /* Read the data bytes */      
  uint16 timeOut =1;
  uint8 dataIndexCntr = 0;
  while(read(tty_fd,&rdData[dataIndexCntr],1) == -1 ) {
    timeOut++;
    if(timeOut == 0)
      {
	printf("[ERROR] Read from target fail\n");
	return(CYRET_ERR_COMM_MASK);
      }
  }
  dataIndexCntr++;
  byteCnt--;
  while (byteCnt>0)
    {
      if ( read(tty_fd,&rdData[dataIndexCntr++],1) == -1 ) {
	printf("[ERROR] Reading data from serial port: %s\n",strerror(errno));
	return(CYRET_ERR_COMM_MASK);
      }
      if(usleep(1)==-1){
	printf("[ERROR] usleep : %s\n",strerror(errno));
      }
      byteCnt--;
    }
  return(CYRET_SUCCESS);
}

/* [] END OF FILE */

