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
//#include <device.h>

#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <termios.h>
#include <errno.h>

typedef     unsigned short uint16;
#define MODEMDEV "/dev/ttyACM0"
static struct termios tio;
static struct termios stdio;
static struct termios old_stdio;
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
int OpenConnection(void)
{
  /*
    UART_Start();
  */

  // tty_fd=open(argv[1], O_RDWR | O_NONBLOCK);      
  tty_fd=open(MODEMDEV, O_RDWR | O_NONBLOCK); 
  if ( tty_fd == -1) {
    printf("ERROR opening %s : %s\n",MODEMDEV,strerror(errno));
    printf("Exit\n");
    exit(1);
    return (CYRET_ERR_FILE);
  }
  tcgetattr(STDOUT_FILENO,&old_stdio);
  memset(&stdio,0,sizeof(stdio));
  stdio.c_iflag=0;
  stdio.c_oflag=0;
  stdio.c_cflag=0;
  stdio.c_lflag=0;
  stdio.c_cc[VMIN]=1;
  stdio.c_cc[VTIME]=0;
  tcsetattr(STDOUT_FILENO,TCSANOW,&stdio);
  tcsetattr(STDOUT_FILENO,TCSAFLUSH,&stdio);
  fcntl(STDIN_FILENO, F_SETFL, O_NONBLOCK);       // make the reads non-blocking
  
  memset(&tio,0,sizeof(tio));
  tio.c_iflag=0;
  tio.c_oflag=0;
  tio.c_cflag=CS8|CREAD|CLOCAL;           // 8n1, see termios.h for more information
  tio.c_lflag=0;
  tio.c_cc[VMIN]=1;
  tio.c_cc[VTIME]=5;
  

  cfsetospeed(&tio,B115200);            // 115200 baud
  cfsetispeed(&tio,B115200);            // 115200 baud
  tcsetattr(tty_fd,TCSANOW,&tio);
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
  close(tty_fd);
  tcsetattr(STDOUT_FILENO,TCSANOW,&old_stdio);
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
  // write(tty_fd,wrData,byteCnt);
  
  uint16 timeOut =1;
		
  /* Clears TX and RX FIFOs and the status registers */
  // UART_ClearRxBuffer();
  // UART_ClearTxBuffer();
	
  /* Send the data*/   
  // UART_PutArray(wrData, byteCnt);
  int i;
  for (i=0; i< byteCnt; i++) {
    if ( write(tty_fd,&wrData[i],1) == -1 ) {
      printf("Error writing to serial port %s\n",strerror(errno));
      return (CYRET_ERR_COMM_MASK);
    }
  }
	
#if 0
  /* Wait till send operation is complete or timeout  */
  while(!(UART_ReadTxStatus() & UART_TX_STS_COMPLETE))
    {
      timeOut++;
      /* Check for timeout and if so exit with communication error code*/
      if(timeOut == 0)
	{
	  return(CYRET_ERR_COMM_MASK);
	}	
    }
				
#endif
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
  // read(tty_fd,rdData,byteCnt);
#if 0
  // uint16 timeOut =1;
  // uint8 dataIndexCntr = 0;
    
	
  /* Clears TX and RX FIFOs and the status registers */	
  // UART_ClearRxBuffer();
  UART_ClearTxBuffer();
	
  /* Wait until timeout  */
  while(UART_GetRxBufferSize() == 0u)
    {
      timeOut++;
      /* Check for timeout and if so exit with communication error code*/
      if(timeOut == 0)
	{			
	  return(CYRET_ERR_COMM_MASK);
	}	
    }
	
#endif    
  /* Read the data bytes */      
  uint8 dataIndexCntr = 0;
  while (byteCnt>0)
    {
      // if(UART_GetRxBufferSize() > 0u)
      //  {
      //    rdData[dataIndexCntr]=UART_GetChar();
      if ( read(tty_fd,&rdData[dataIndexCntr++],1) == -1 ) {
	printf("ERROR: reading data from serial port: %s\n",strerror(errno));
	return(CYRET_ERR_COMM_MASK);
      }
      // dataIndexCntr++;
      byteCnt--;
      //  }
    }	
  return(CYRET_SUCCESS);
}

/* [] END OF FILE */

