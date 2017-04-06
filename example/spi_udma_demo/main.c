//*****************************************************************************
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

//*****************************************************************************
//
// Application Name     - SPI Demo
// Application Overview - The demo application focuses on showing the required 
//                        initialization sequence to enable the CC3200 SPI 
//                        module in full duplex 4-wire master and slave mode(s).
// Application Details  -
// http://processors.wiki.ti.com/index.php/CC32xx_SPI_Demo
// or
// docs\examples\CC32xx_SPI_Demo.pdf
//
//*****************************************************************************


//*****************************************************************************
//
//! \addtogroup SPI_Demo
//! @{
//
//*****************************************************************************

// Standard includes
#include <string.h>

// Driverlib includes
#include "hw_types.h"
#include "hw_memmap.h"
#include "hw_common_reg.h"
#include "hw_ints.h"
#include "hw_mcspi.h"
#include "hw_udma.h"
#include "spi.h"
#include "rom.h"
#include "rom_map.h"
#include "utils.h"
#include "prcm.h"
#include "uart.h"
#include "udma.h"
#include "interrupt.h"
#include "timer.h"
#include "timer_if.h"

// Common interface includes
#include "uart_if.h"
#include "udma_if.h"
#include "pinmux.h"

#define MASTER_MODE      0


#define APPLICATION_VERSION     "1.1.0"
//*****************************************************************************
//
// Application Master/Slave mode selector macro
//
// MASTER_MODE = 1 : Application in master mode
// MASTER_MODE = 0 : Application in slave mode
//
//*****************************************************************************

#define SPI_IF_BIT_RATE  10000000  //10M
#define TR_BUFF_SIZE     1024

#define MASTER_MSG       "This is CC3200 SPI Master Application\n\r"
#define SLAVE_MSG        "This is CC3200 SPI Slave Application\n\r"

//*****************************************************************************
//                 GLOBAL VARIABLES -- Start
//*****************************************************************************
unsigned char g_ucTxBuff[TR_BUFF_SIZE] = {0x63, 0x64, 0x31, 0x30 };
unsigned char g_ucRxBuff[TR_BUFF_SIZE];
//static unsigned char g_ucDummyBuff[10];
static unsigned char ucTxBuffNdx;
static unsigned char ucRxBuffNdx;

unsigned char g_ucDMAEnabled = 0;

volatile static unsigned char g_cDummy;

#if defined(ccs)
extern void (* const g_pfnVectors[])(void);
#endif
#if defined(ewarm)
extern uVectorEntry __vector_table;
#endif
//*****************************************************************************
//                 GLOBAL VARIABLES -- End
//*****************************************************************************

//*****************************************************************************
//
//! SPI Slave Interrupt handler
//!
//! This function is invoked when SPI slave has its receive register full or
//! transmit register empty.
//!
//! \return None.
//
//*****************************************************************************
static void SpiIntHandler()
{
    unsigned long ulRecvData;
    unsigned long ulStatus;

    ulStatus = MAP_SPIIntStatus(GSPI_BASE,true);
    
    MAP_SPIIntClear(GSPI_BASE,SPI_INT_EOW);
    MAP_SPICSDisable(GSPI_BASE);

    if(ulStatus & SPI_INT_TX_EMPTY)
    {
        Report("TX EMPTY\r\n");
        ucTxBuffNdx++;
    }

    if(ulStatus & SPI_INT_RX_FULL)
    {
        Report("RX FULL\r\n");
        ucRxBuffNdx++;
    }
    
    if(ulStatus & SPI_INT_DMARX)
    {
      Report("DMA RX\r\n");
      ucTxBuffNdx++;
    }
    
    if(ulStatus & SPI_INT_DMATX)
    {
      Report("DMA TX\r\n");
      ucTxBuffNdx++;
    }
    if(ulStatus & SPI_INT_EOW)
    {
      //Report("EOWisr\r\n");
      g_cDummy = 1;
    }
}

void Slave_Receive(unsigned char *rxBuff, unsigned char *txBuff, int len)
{
  int i = 0;
  
  MAP_SPIWordCountSet(GSPI_BASE,len);

  UDMASetupTransfer(UDMA_CH30_GSPI_RX,UDMA_MODE_BASIC,len,
                UDMA_SIZE_8,UDMA_ARB_1,
                (void *)(GSPI_BASE + MCSPI_O_RX0),UDMA_SRC_INC_NONE,
                (void *)rxBuff,UDMA_DST_INC_8); 

  UDMASetupTransfer(UDMA_CH31_GSPI_TX,UDMA_MODE_BASIC,len,
                UDMA_SIZE_8,UDMA_ARB_1,
                (void *)txBuff,UDMA_SRC_INC_8,(void *)(GSPI_BASE + MCSPI_O_TX0),
                UDMA_DST_INC_NONE);      
  
  MAP_SPIEnable(GSPI_BASE);
  
#if MASTER_MODE
#else
  while( !(MAP_SPIIntStatus(GSPI_BASE,false) & SPI_INT_EOW) ); 
#if DEBUG_FULL  
  for(i = 1; i < 1024; i ++)
  {
    Report("%02x ", rxBuff[i-1]);
    if(i%64 == 0)
      Report("\r\n");
  }
  Report("\r\n===============================================================\r\n");
#endif  
  MAP_SPIIntClear(GSPI_BASE,SPI_INT_EOW);
  Report("%02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x\r\n", 
            rxBuff[0], rxBuff[1], rxBuff[2], rxBuff[3],
            rxBuff[4], rxBuff[5], rxBuff[6], rxBuff[7],
            rxBuff[8], rxBuff[9], rxBuff[10], rxBuff[11]);
#endif
  
  MAP_SPIDisable(GSPI_BASE);
}

void Master_Send(unsigned char *rxBuff, unsigned char *txBuff, int len)
{  
  MAP_SPIWordCountSet(GSPI_BASE,len);
  
  UDMASetupTransfer(UDMA_CH31_GSPI_TX,UDMA_MODE_BASIC,len,
                UDMA_SIZE_8,UDMA_ARB_1,
                (void *)txBuff,UDMA_SRC_INC_8,(void *)(GSPI_BASE + MCSPI_O_TX0),
                UDMA_DST_INC_NONE); 
  
  UDMASetupTransfer(UDMA_CH30_GSPI_RX,UDMA_MODE_BASIC,len,
                UDMA_SIZE_8,UDMA_ARB_1,
                (void *)(GSPI_BASE + MCSPI_O_RX0),UDMA_SRC_INC_NONE,
                (void *)rxBuff,UDMA_DST_INC_8);  
  
  MAP_SPIEnable(GSPI_BASE);
}

//*****************************************************************************
//
//! SPI Slave mode main loop
//!
//! This function configures SPI modelue as slave and enables the channel for
//! communication
//!
//! \return None.
//
//*****************************************************************************
void Spi_Init()
{
        unsigned long ulBase;
        unsigned char i, j;
            
        //NWP master interface
        ulBase = GSPI_BASE;

        //Enable MCSPIA2
        MAP_PRCMPeripheralClkEnable(PRCM_GSPI,PRCM_RUN_MODE_CLK);

        //Disable Chip Select
        MAP_SPICSDisable(ulBase);

        //Disable SPI Channel
        MAP_SPIDisable(ulBase);

        // Reset SPI
        MAP_SPIReset(ulBase);

      //
      // Configure SPI interface
      //
#if MASTER_MODE
      MAP_SPIConfigSetExpClk(ulBase,PRCMPeripheralClockGet(PRCM_GSPI),
                         SPI_IF_BIT_RATE,SPI_MODE_MASTER,SPI_SUB_MODE_0,
                         (SPI_SW_CTRL_CS |
                         SPI_4PIN_MODE |
                         SPI_TURBO_OFF |
                         SPI_CS_ACTIVELOW |
                         SPI_WL_8));
        for (j = 0; j < 4; j++)
                    for(i = 0; i < 255; i++)
                            g_ucTxBuff[i + j*255] = i;
                    
            g_ucTxBuff[0] = 0x63;
            g_ucTxBuff[1] = 0x64;
            g_ucTxBuff[2] = 0x31;
            g_ucTxBuff[3] = 0x30;
#else
      MAP_SPIConfigSetExpClk(ulBase,PRCMPeripheralClockGet(PRCM_GSPI),
                         SPI_IF_BIT_RATE,SPI_MODE_SLAVE,SPI_SUB_MODE_0,
                         (SPI_HW_CTRL_CS |
                         SPI_4PIN_MODE |
                         SPI_TURBO_OFF |
                         SPI_CS_ACTIVELOW |
                         SPI_WL_8));

#endif
      //
      // Initialize UDMA
      //
      UDMAInit();
      
    if(MAP_PRCMPeripheralStatusGet(PRCM_UDMA))
    {
      g_ucDMAEnabled = (HWREG(UDMA_BASE + UDMA_O_CTLBASE) != 0x0) ? 1 : 0;
    }
    else
    {
            g_ucDMAEnabled = 0;
    }

    if(g_ucDMAEnabled)
    {
        // Set DMA channel
        MAP_uDMAChannelAssign(UDMA_CH30_GSPI_RX);
        MAP_uDMAChannelAttributeDisable(UDMA_CH30_GSPI_RX,UDMA_ATTR_ALTSELECT);
        MAP_uDMAChannelAssign(UDMA_CH31_GSPI_TX);
        MAP_uDMAChannelAttributeDisable(UDMA_CH31_GSPI_TX,UDMA_ATTR_ALTSELECT);

        MAP_SPIFIFOLevelSet(ulBase,1,1);
        MAP_SPIFIFOEnable(ulBase,SPI_RX_FIFO|SPI_TX_FIFO);
        MAP_SPIDmaEnable(ulBase,SPI_RX_DMA|SPI_TX_DMA);

        MAP_IntRegister(INT_GSPI,SpiIntHandler);
#if MASTER_MODE
        //interrupt setting 
        MAP_SPIIntEnable(GSPI_BASE, SPI_INT_EOW);
        MAP_IntEnable(INT_GSPI); 
#endif          
        MAP_SPICSEnable(GSPI_BASE);
    }
}

//*****************************************************************************
//
//! Board Initialization & Configuration
//!
//! \param  None
//!
//! \return None
//
//*****************************************************************************
static void
BoardInit(void)
{
/* In case of TI-RTOS vector table is initialize by OS itself */
#ifndef USE_TIRTOS
  //
  // Set vector table base
  //
#if defined(ccs)
    MAP_IntVTableBaseSet((unsigned long)&g_pfnVectors[0]);
#endif
#if defined(ewarm)
    MAP_IntVTableBaseSet((unsigned long)&__vector_table);
#endif
#endif
    //
    // Enable Processor
    //
    MAP_IntMasterEnable();
    MAP_IntEnable(FAULT_SYSTICK);

    PRCMCC3200MCUInit();
}

//*****************************************************************************
//
//! Main function for spi demo application
//!
//! \param none
//!
//! \return None.
//
//*****************************************************************************
void main()
{
    //
    // Initialize Board configurations
    //
    BoardInit();

    //
    // Muxing UART and SPI lines.
    //
    PinMuxConfig();

    //
    // Enable the SPI module clock
    //
    MAP_PRCMPeripheralClkEnable(PRCM_GSPI,PRCM_RUN_MODE_CLK);

    //
    // Initialising the Terminal.
    //
    InitTerm();

    //
    // Clearing the Terminal.
    //
    ClearTerm();

    //
    // Display the Banner
    //
    Message("\n\n\n\r");
    Message("\t\t   ********************************************\n\r");
    Message("\t\t        CC3200 SPI Demo Application  \n\r");
#if MASTER_MODE
    Message(MASTER_MSG);
    Message("\n\r");
#else
    Message(SLAVE_MSG);
    Message("\n\r");
#endif    
    Message("\t\t   ********************************************\n\r");
    Message("\n\n\n\r");

    g_cDummy = 0;
    
    //
    // Reset the peripheral
    //
    MAP_PRCMPeripheralReset(PRCM_GSPI);
    
    Spi_Init();
#if MASTER_MODE
    Master_Send(g_ucRxBuff, g_ucTxBuff, 1024);
#endif
    
    while(1)
    {

#if MASTER_MODE      
	if(g_cDummy == 1)
#endif          
        {
#if MASTER_MODE     
          MAP_UtilsDelay(40000);
        
          Spi_Init();
          Master_Send(g_ucRxBuff, g_ucTxBuff, 1024);
          g_cDummy = 0;
          MAP_SPIIntEnable(GSPI_BASE, SPI_INT_EOW);
          MAP_IntEnable(INT_GSPI);           
#else       
          memset(g_ucRxBuff, 0x0, sizeof(g_ucRxBuff));
          memset(g_ucTxBuff, 0x0, sizeof(g_ucTxBuff));
          Slave_Receive(g_ucRxBuff, g_ucTxBuff, 1024);
#endif            
            
        }
    }
}

