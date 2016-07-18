//*****************************************************************************
// uart_if.c
//
// uart interface file: Prototypes and Macros for UARTLogger
//
// Copyright (C) 2014 Texas Instruments Incorporated - http://www.ti.com/ 
// 
// 
//  Redistribution and use in source and binary forms, with or without 
//  modification, are permitted provided that the following conditions 
//  are met:
//
//    Redistributions of source code must retain the above copyright 
//    notice, this list of conditions and the following disclaimer.
//
//    Redistributions in binary form must reproduce the above copyright
//    notice, this list of conditions and the following disclaimer in the 
//    documentation and/or other materials provided with the   
//    distribution.
//
//    Neither the name of Texas Instruments Incorporated nor the names of
//    its contributors may be used to endorse or promote products derived
//    from this software without specific prior written permission.
//
//  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS 
//  "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT 
//  LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
//  A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT 
//  OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, 
//  SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT 
//  LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
//  DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
//  THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT 
//  (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE 
//  OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//
//*****************************************************************************

// Standard includes
#include <stdarg.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

// Driverlib includes
#include "hw_types.h"
#include "hw_memmap.h"
#include "prcm.h"
#include "pin.h"
#include "uart.h"
#include "rom.h"
#include "rom_map.h"

#if defined(USE_FREERTOS) || defined(USE_TI_RTOS)
#include "osi.h"
#endif

#include "giz_uart_if.h"
#include "simplelink.h"
//#include "gagent.h"
#include "udma.h"
#include "hw_uart.h"
#include "common.h"
#define UART_PRINT              Report

#define IS_SPACE(x)       (x == 32 ? 1 : 0)

//*****************************************************************************
// Global variable indicating command is present
//*****************************************************************************
static unsigned long __Errorlog;

//*****************************************************************************
// Global variable indicating input length
//*****************************************************************************
unsigned int ilen=1;


//UART1
unsigned char uartRxBuff[MAX_RX_BUFF] ={0};
static unsigned long uartRxLength = 0;
static unsigned char recvFlag = 0;
//unsigned char testValue[] = {10, 1,2,3,4,5,6,7,8,9,10};

static unsigned long intStatus;

unsigned char getRecvFlag(void)
{
     return recvFlag;
}

void setRecvFlag(unsigned char Flag)
{
    recvFlag = (unsigned char)Flag;
}

void printRecv(void)
{
      unsigned char ii=0;
      
      if(getRecvFlag())
    {
         for(ii=0;ii<uartRxLength;ii++)
         {
              UART_PRINT("%02x ", uartRxBuff[ii]); 
         }
         setRecvFlag(FALSE);
         uartRxLength =0;
         memset((char *)uartRxBuff, 0, sizeof(uartRxBuff));  
    }
}
#if 1

void Uart1IntHandler(void)
{
    //unsigned long intStatus;
    unsigned char ii = 0;

    //MAP_UARTIntClear(UARTA1_BASE, intStatus);

    intStatus = MAP_UARTIntStatus(UARTA1_BASE, UART_INT_RX); //  |uartRxBuff[uartRxLength] //UART_INT_RX
    //MAP_UARTIntClear(UARTA1_BASE, intStatus);
    while((intStatus & UART_INT_RX) && MAP_UARTCharsAvail(UARTA1_BASE) )
    {

	
       #if 0 //Nik.chen UART Mode NonBlocking
	 uartRxBuff[uartRxLength++] = (unsigned char)MAP_UARTCharGetNonBlocking(UARTA1_BASE);
       #else 
       uartRxBuff[uartRxLength++] = (unsigned char)UARTCharGet (UARTA1_BASE);
       #endif
       //  if(uartRxBuff[uartRxLength] == -1)
       if(uartRxLength >= 1023)
         {
             uartRxLength = 0;
             setRecvFlag(0);
             break;
         }
         else
         {
          // if(0 == ii++)
           // UART_PRINT("uartRxBuff: ");
           //UART_PRINT("%02x ", uartRxBuff[uartRxLength]); 
           //MAP_UARTCharPut(CONSOLE,uartRxBuff[uartRxLength]);
            setRecvFlag(1);
	     // MAP_UARTIntClear(UARTA1_BASE, intStatus);
         }
    }

    #if 0
     if(uartRxLength == jj)
     {
          setRecvFlag(TRUE);
         UART_PRINT(" uartRxLength=%d\n", uartRxLength);
         for(ii=0;ii<uartRxLength;ii++)
         {
              UART_PRINT("%02x ", uartRxBuff[ii]); 
         }

         uartRxLength = 0;
         jj = -1;
         memset((char *)uartRxBuff, 0, sizeof(uartRxBuff));  
     }
    #endif

    //MAP_UARTIntClear(UARTA1_BASE, intStatus);
}
#endif
#if 1
int SendTestUartData(unsigned char *buff, unsigned char length)
{
    unsigned char ii;
    for(ii = 0; ii < length; ii++)
        MAP_UARTCharPut(UARTA1_BASE, *(buff+ii));
    return SUCCESS;
}

//######################CC3200 UART API#############################################
int uart_write(int dev_fd, const char *buf, int count)
{
    unsigned char ii;
    const char *pch = buf;
    if (buf == NULL)
        return -1;
#if 0
      UART_PRINT("write buf:");
     for(ii = 0; ii < count; ii++)
    {
        UART_PRINT("%02x ", buf[ii]);
    }

       UART_PRINT("\n");
#endif    
    while(count--) {
        #if 0 //Nik.chen UART Mode NonBlocking
       // if (UARTCharsAvail(UARTA1_BASE)) {
            UARTCharPutNonBlocking(UARTA1_BASE, *pch++);
       // }
        #else
        UARTCharPut(UARTA1_BASE, *pch++);
        #endif
    }
    return 0;
}

int uart_read(int dev_fd, char *buf, int count)
{
   unsigned char ii;
   unsigned long retDataLen = 0;

    if (!count)
        return 0;
    if (buf == NULL)
        return -1;
    #if 0
    unsigned int numRead = 0;
    int retVal = 0;
    while (uartRxLength--) {
        if (UARTCharsAvail(UARTA1_BASE)) {
            retVal = UARTCharGetNonBlocking(UARTA1_BASE);
            // retVal = UARTCharGet(UARTA1_BASE);
            // if character available is EOF, return 0
            if (retVal == -1)
                return numRead; //Nik.chen 0 -> numRead
            *pch++ = retVal;
            numRead++;
             count--;
        } 
    }
     #else

    memcpy(buf, uartRxBuff, uartRxLength);
    #if 0
     UART_PRINT("uartRxLength=%d, read buf:", uartRxLength);
     UART_PRINT("\n"); 
     for(ii = 0; ii < uartRxLength; ii++)
    {
        UART_PRINT("[ii=%d, %02x] ", ii, buf[ii]);
    }

    UART_PRINT("\n"); 
    UART_PRINT("receive end \n");
   #endif
    retDataLen = uartRxLength;

    memset((char *)uartRxBuff, 0, uartRxLength);
    uartRxLength = 0;
    setRecvFlag(0);
    MAP_UARTIntClear(UARTA1_BASE, intStatus);

   #endif
    return retDataLen;
}
#endif
//*****************************************************************************
//
//! Initialization
//!
//! This function
//!        1. Configures the UART to be used.
//!
//! \return none
//
//*****************************************************************************
void 
InitTerm()
{
#ifndef NOTERM
  setRecvFlag(0);
  MAP_UARTConfigSetExpClk(CONSOLE,MAP_PRCMPeripheralClockGet(CONSOLE_PERIPH), 
                  UART_BAUD_RATE, (UART_CONFIG_WLEN_8 | UART_CONFIG_STOP_ONE |
                   UART_CONFIG_PAR_NONE));
   //Enabling UART 1

    MAP_UARTConfigSetExpClk(UARTA1_BASE,MAP_PRCMPeripheralClockGet(PRCM_UARTA1),
                COMMU_BAUD_RATE,
                (UART_CONFIG_WLEN_8 | UART_CONFIG_STOP_ONE |
                UART_CONFIG_PAR_NONE));

   
   //UARTFIFOEnable(UARTA1_BASE);
   //MAP_UARTDMAEnable(UARTA1_BASE,UART_DMA_RX);
   UARTEnable(UARTA1_BASE); 

    uartRxLength = 0;
    memset((char *)uartRxBuff, 0, sizeof(uartRxBuff));  
#if 1   
    //UARTFIFOLevelSet(UARTA1_BASE, UART_FIFO_TX1_8, UART_FIFO_RX1_8);
    //UARTTxIntModeSet(UARTA1_BASE, UART_TXINT_MODE_FIFO);
   // intStatus = MAP_UARTIntStatus(UARTA1_BASE, UART_INT_RX);
   // MAP_UARTIntClear(UARTA1_BASE, intStatus);
    MAP_UARTFlowControlSet(UARTA1_BASE, UART_FLOWCONTROL_NONE);
    MAP_UARTIntRegister(UARTA1_BASE, Uart1IntHandler);
    MAP_UARTIntEnable(UARTA1_BASE, UART_INT_RX); //Nik.chen UART_INT_RT
    UARTFIFODisable(UARTA1_BASE);
#endif

#endif
  __Errorlog = 0;
}

//*****************************************************************************
//
//!    Outputs a character string to the console
//!
//! \param str is the pointer to the string to be printed
//!
//! This function
//!        1. prints the input string character by character on to the console.
//!
//! \return none
//
//*****************************************************************************

void Message(const char *str)
{
#ifndef NOTERM
    if(str != NULL)
    {
        while(*str!='\0')
        {
            MAP_UARTCharPut(CONSOLE,*str++);
        }
    }
#endif
}

void GAgent_Message(char *buffer, int len)
{
    buffer[ len ]='\0';
#ifndef NOTERM
    if(buffer != NULL && (len!=0))
    {
        while(len--)
        {
            MAP_UARTCharPut(CONSOLE,*buffer++);
        }
    }
#endif

}

//*****************************************************************************
//
//!    Clear the console window
//!
//! This function
//!        1. clears the console window.
//!
//! \return none
//
//*****************************************************************************
void 
ClearTerm()
{
    Message("\33[2J\r");
}

//*****************************************************************************
//
//! Error Function
//!
//! \param 
//!
//! \return none
//! 
//*****************************************************************************
void 
Error(char *pcFormat, ...)
{
#ifndef NOTERM
    char  cBuf[256];
    va_list list;
    va_start(list,pcFormat);
    vsnprintf(cBuf,256,pcFormat,list);
    Message(cBuf);
#endif
    __Errorlog++;
}

//*****************************************************************************
//
//! Get the Command string from UART
//!
//! \param  pucBuffer is the command store to which command will be populated
//! \param  ucBufLen is the length of buffer store available
//!
//! \return Length of the bytes received. -1 if buffer length exceeded.
//! 
//*****************************************************************************
int
GetCmd(char *pcBuffer, unsigned int uiBufLen)
{
    char cChar;
    int iLen = 0;
    
    //
    // Wait to receive a character over UART
    //
    while(MAP_UARTCharsAvail(CONSOLE) == false)
    {
#if defined(USE_FREERTOS) || defined(USE_TI_RTOS)
    	osi_Sleep(1);
#endif
    }
    cChar = MAP_UARTCharGetNonBlocking(CONSOLE);
    
    //
    // Echo the received character
    //
    MAP_UARTCharPut(CONSOLE, cChar);
    iLen = 0;
    
    //
    // Checking the end of Command
    //
    while((cChar != '\r') && (cChar !='\n') )
    {
        //
        // Handling overflow of buffer
        //
        if(iLen >= uiBufLen)
        {
            return -1;
        }
        
        //
        // Copying Data from UART into a buffer
        //
        if(cChar != '\b')
        { 
            *(pcBuffer + iLen) = cChar;
            iLen++;
        }
        else
        {
            //
            // Deleting last character when you hit backspace 
            //
            if(iLen)
            {
                iLen--;
            }
        }
        //
        // Wait to receive a character over UART
        //
        while(MAP_UARTCharsAvail(CONSOLE) == false)
        {
#if defined(USE_FREERTOS) || defined(USE_TI_RTOS)
        	osi_Sleep(1);
#endif
        }
        cChar = MAP_UARTCharGetNonBlocking(CONSOLE);
        //
        // Echo the received character
        //
        MAP_UARTCharPut(CONSOLE, cChar);
    }

    *(pcBuffer + iLen) = '\0';

    Report("\n\r");

    return iLen;
}

//*****************************************************************************
//
//!    Trim the spaces from left and right end of given string
//!
//! \param  Input string on which trimming happens
//!
//! \return length of trimmed string
//
//*****************************************************************************
int TrimSpace(char * pcInput)
{
    size_t size;
    char *endStr, *strData = pcInput;
    char index = 0;
    size = strlen(strData);

    if (!size)
        return 0;

    endStr = strData + size - 1;
    while (endStr >= strData && IS_SPACE(*endStr))
        endStr--;
    *(endStr + 1) = '\0';

    while (*strData && IS_SPACE(*strData))
    {
        strData++;
        index++;
    }
    memmove(pcInput,strData,strlen(strData)+1);

    return strlen(pcInput);
}

//*****************************************************************************
//
//!    prints the formatted string on to the console
//!
//! \param format is a pointer to the character string specifying the format in
//!           the following arguments need to be interpreted.
//! \param [variable number of] arguments according to the format in the first
//!         parameters
//! This function
//!        1. prints the formatted error statement.
//!
//! \return count of characters printed
//
//*****************************************************************************
int Report(const char *pcFormat, ...)
{
 int iRet = 0;
#ifndef NOTERM

  char *pcBuff, *pcTemp;
  int iSize = 256;
 
  va_list list;
  pcBuff = (char*)malloc(iSize);
  if(pcBuff == NULL)
  {
      return -1;
  }
  while(1)
  {
      va_start(list,pcFormat);
      iRet = vsnprintf(pcBuff,iSize,pcFormat,list);
      va_end(list);
      if(iRet > -1 && iRet < iSize)
      {
          break;
      }
      else
      {
          iSize*=2;
          if((pcTemp=realloc(pcBuff,iSize))==NULL)
          { 
              Message("Could not reallocate memory\n\r");
              iRet = -1;
              break;
          }
          else
          {
              pcBuff=pcTemp;
          }
          
      }
  }
  Message(pcBuff);
  free(pcBuff);
  
#endif
  return iRet;
}
