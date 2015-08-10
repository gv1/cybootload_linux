#include <string.h>
// #include "StringImage.h" 
#include "communication_api.h" 
#include "cybtldr_api.h"
#include "cybtldr_command.h"
#include "cybtldr_parse.h" 
#include "cybtldr_utils.h"
#include <errno.h>
#include <stdlib.h>

CyBtldr_CommunicationsData comm1 ;
typedef     unsigned short uint16;
uint16 BootloadStringImage(const char *bootloadImagePtr[],unsigned int lineCount );

uint16 readCyacd(const char * fn, int * lines, char *** ret) 
{
  FILE * fin = fopen(fn,"r");
  if ( fin == NULL ) {
    printf("Error opening %s, %s\n",fn,strerror(errno));
    return(CYRET_ERR_FILE) ;
  }
  ssize_t read =  0;
  size_t len = 0;
  char * line = NULL;
  char *buf[1000];
  *lines = 0;
  while ( !feof(fin) ) {
    read = getline(&line,&len,fin);
    line[strlen(line)-2]='\0'; // replace newline
    buf[*lines]=strdup(line);
    if (( read == -1 ) && !feof(fin)) {
      printf("Error %s\n",strerror(errno));
      return CYRET_ERR_FILE;
    } 
    // printf("Read line %d %zu : %p\n",*lines,read,line);
    // printf("%s",buf[*lines]);
    (*lines)++;
    // printf("Read %zu : %s\n",read,line);
  }
  (*lines)--;
  *ret = (char **)malloc(sizeof(char*)**lines);
  if (*ret == NULL) {
    printf("Error allocating memory\n");
    printf("%s\n",strerror(errno));
    return(CYRET_ABORT);    
  }
  int i;
  for(i=0; i<*lines; i++)  {
    	(*ret)[i]=buf[i];
	// printf("%s",(*ret)[i]);
  }
  free(line);
}   

// see cybtldr_utils.h
// host error
void error_info_host(uint16 error)
{
  switch(error) 
    {
    case CYRET_ERR_FILE:
      printf("File is not accessable [0x%X]\n",error);
      break;
    case CYRET_ERR_EOF:
      printf("Reached the end of the file [0x%X]\n",error);
      break;	
    case CYRET_ERR_LENGTH:
      printf("The amount of data available is outside the expected range [0x%X]\n",error);
      break;
    case CYRET_ERR_DATA:
      printf("The data is not of the proper form [0x%X]\n",error);
      break;
    case CYRET_ERR_CMD:
      printf("The command is not recognized [0x%X]\n",error);
      break;
    case CYRET_ERR_DEVICE:
      printf("The expected device does not match the detected device [0x%X]\n",error);
      break;
    case CYRET_ERR_VERSION:
      printf("The bootloader version detected is not supported [0x%X]\n",error);
      break;
    case CYRET_ERR_CHECKSUM:
      printf("The checksum does not match the expected value [0x%X]\n",error);
      break;
    case CYRET_ERR_ARRAY:
      printf("The flash array is not valid [0x%X]\n",error);
      break;
    case CYRET_ERR_ROW:
      printf("The flash row is not valid [0x%X]\n",error);
      break;
    case CYRET_ERR_BTLDR:
      printf("The bootloader is not ready to process data [0x%X]\n",error);
      break;
    case CYRET_ERR_ACTIVE:
      printf("The application is currently marked as active [0x%X]\n",error);
      break;
    case CYRET_ERR_UNK:
      printf("The operation was aborted [0x%X]\n",error);
      break;
    case CYRET_ABORT:
      printf("The operation was aborted [0x%X]\n",error);
      break;
    case CYRET_ERR_COMM_MASK:
      printf("The communications object reported an error [0x%X]\n",error);
      break;
    case CYRET_ERR_BTLDR_MASK:
      printf("The bootloader reported an error [0x%X]\n",error);
      break;
    default:
      printf("An unknown error occured [0x%X]\n",error);
      break;
    }
}

// bootloader error
void error_info_bootldr(uint16 error)
{
  switch(error)
    {
    case CYBTLDR_STAT_SUCCESS:
      printf("Completed successfully [0x%X]\n",error);
      break;
    case CYBTLDR_STAT_ERR_KEY:
      printf("The provided key does not match the expected value [0x%X]\n",error);
      break;
    case CYBTLDR_STAT_ERR_VERIFY:
      printf("The verification of flash failed [0x%X]\n",error);
      break;
    case CYBTLDR_STAT_ERR_LENGTH:
      printf("The amount of data available is outside the expected range [0x%X]\n",error);
      break;
    case CYBTLDR_STAT_ERR_DATA:
      printf("The data is not of the proper form [0x%X]\n",error);
      break;
    case CYBTLDR_STAT_ERR_CMD:
      printf("The command is not recognized [0x%X]\n",error);
      break;
    case CYBTLDR_STAT_ERR_DEVICE:
      printf("The expected device does not match the detected device [0x%X]\n",error);
      break;
    case CYBTLDR_STAT_ERR_VERSION:
      printf("The bootloader version detected is not supported [0x%X]\n",error);
      break;
    case CYBTLDR_STAT_ERR_CHECKSUM:
      printf("The checksum does not match the expected value [0x%X]\n",error);
      break;
    case CYBTLDR_STAT_ERR_ARRAY:
      printf("The flash array is not valid [0x%X]\n",error);
      break;
    case CYBTLDR_STAT_ERR_ROW:
      printf("The flash row is not valid [0x%X]\n",error);
      break;
    case CYBTLDR_STAT_ERR_PROTECT:
      printf("The flash row is protected and can not be programmed [0x%X]\n",error);
      break;
    case CYBTLDR_STAT_ERR_APP:
      printf("The application is not valid and cannot be set as active [0x%X]\n",error);
      break;
    case CYBTLDR_STAT_ERR_ACTIVE:
      printf("The application is currently marked as active [0x%X]\n",error);
      break;
    case CYBTLDR_STAT_ERR_UNK:
    default:
      printf("An unknown error occured [0x%X]\n",error);
      break;
    }
}
int main()
{
  uint16 error = 0;
  printf("Bootloading\n");

  int lines;
  char ** stringImage;
  readCyacd("./Bootloadable Blinking LED.cyacd",&lines,&stringImage);
  int i;
  printf("Read Total lines = %d from .cyacd file\n",lines);

  comm1.OpenConnection = &OpenConnection;
  comm1.CloseConnection = &CloseConnection;
  comm1.ReadData = &ReadData;
  comm1.WriteData =&WriteData;
  comm1.MaxTransferSize =64;
  /* Bootloadable Blinking LED.cyacd */
  // error = BootloadStringImage(stringImage,LINE_CNT);
  error = BootloadStringImage((const char **)stringImage,lines);
  // error = BootloadStringImage(stringImage_6,LINE_CNT_6);
  if(error == CYRET_SUCCESS)
    {	
      printf("Bootloaded-Green\n");
      printf("P6.1 to BL Blue \n");
    } else {
    if(error & CYRET_ERR_COMM_MASK) /* Check for comm error*/
      {
	printf("Communicatn Err \n");
      } else {
      printf("Bootload Error: ");
      error_info_bootldr(error);	
    }
  }
}

/****************************************************************************************************
 * Function Name: BootloadStringImage
 *****************************************************************************************************
 *
 * Summary:
 *  Bootloads the .cyacd file contents which is stored as string array
 *
 * Parameters:  
 * bootloadImagePtr - Pointer to the string array
 * lineCount - No. of lines in the .cyacd file(No: of rows in the string array)
 *
 * Return: 
 *  Returns a flag to indicate whether the bootload operation was successful or not
 *
 *
 ****************************************************************************************************/
uint16 BootloadStringImage(const char *bootloadImagePtr[],unsigned int lineCount )
{
  uint16 err=0;
  unsigned char arrayId; 
  unsigned short rowNum;
  unsigned short rowSize; 
  unsigned char checksum ;
  unsigned char checksum2;
  unsigned long blVer=0;
  /* rowData buffer size should be equal to the length of data to be sent for
   *  each flash row. It equals 288 , if ECC  is disabled in the bootloadable project,
   *  else 255 (in the case of PSoC 4 the flash row size is 128) */
  unsigned char rowData[288];
  unsigned int lineLen;
  unsigned long  siliconID;
  unsigned char siliconRev;
  unsigned char packetChkSumType;
  unsigned int lineCntr ;
	
  /* Initialize line counter */
  lineCntr = 0;
  
  /* Get length of the first line in cyacd file*/
  lineLen = strlen(bootloadImagePtr[lineCntr]);
  
  /* Parse the first line(header) of cyacd file to extract 
     siliconID, siliconRev and packetChkSumType */
  err = CyBtldr_ParseHeader(lineLen ,(unsigned char *)bootloadImagePtr[lineCntr] , &siliconID , &siliconRev ,&packetChkSumType);
  
  /* Set the packet checksum type for communicating with bootloader. 
     The packet checksum type to be used is determined from the 
     cyacd file header information */
  CyBtldr_SetCheckSumType((CyBtldr_ChecksumType)packetChkSumType);
  
  if(err==CYRET_SUCCESS)
    {
      /* Start Bootloader operation */
      err = CyBtldr_StartBootloadOperation(&comm1 ,siliconID, siliconRev ,&blVer);
      lineCntr++ ;
      while((err == CYRET_SUCCESS)&& ( lineCntr <  lineCount ))
	{
	  
	  /* Get the string length for the line*/
	  lineLen =  strlen(bootloadImagePtr[lineCntr]);
	  
	  /*Parse row data*/
	  err = CyBtldr_ParseRowData((unsigned int)lineLen,(unsigned char *)bootloadImagePtr[lineCntr], &arrayId, &rowNum, rowData, &rowSize, &checksum);
          
	  
	  if (CYRET_SUCCESS == err)
            {
	      /* Program Row */
	      err = CyBtldr_ProgramRow(arrayId, rowNum, rowData, rowSize);
	      
	      if (CYRET_SUCCESS == err)
		{
		  /* Verify Row . Check whether the checksum received from bootloader matches
		   * the expected row checksum stored in cyacd file*/
		  checksum2 = (unsigned char)(checksum + arrayId + rowNum + (rowNum >> 8) + rowSize + (rowSize >> 8));
		  err = CyBtldr_VerifyRow(arrayId, rowNum, checksum2);
		}
            }
	  /* Increment the linCntr */
	  lineCntr++;
	}
      /* End Bootloader Operation */
      CyBtldr_EndBootloadOperation();
    }
  return(err); 
}

