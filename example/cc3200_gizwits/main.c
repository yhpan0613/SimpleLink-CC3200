//*****************************************************************************
//
//  Copyright (C) 2014 Texas Instruments Incorporated - http://www.ti.com/ 
// 
// 
//  Redistribution and use in source and binary forms, with or without 
//  modification, are permitted provided that the following conditions 
//  are met:
//
//  Redistributions of source code must retain the above copyright 
//  notice, this list of conditions and the following disclaimer.
//
//  Redistributions in binary form must reproduce the above copyright
//  notice, this list of conditions and the following disclaimer in the 
//  documentation and/or other materials provided with the   
//  distribution.
//
//  Neither the name of Texas Instruments Incorporated nor the names of
//  its contributors may be used to endorse or promote products derived
//  from this software without specific prior written permission.
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
// Application Name     -   Getting started with WLAN STATION
// Application Overview -   This is a sample application demonstrating how to
//                          start CC3200 in WLAN-Station mode and connect to a
//                          Wi-Fi access-point. The application connects to an
//                          access-point and ping the gateway. It also checks
//                          for an internet connectivity by pinging "www.ti.com"
// Application Details  -
// http://processors.wiki.ti.com/index.php/CC32xx_Getting_Started_with_WLAN_Station
// or
// doc\examples\CC32xx_Getting_Started_with_WLAN_Station.pdf
//
//*****************************************************************************


//****************************************************************************
//
//! \addtogroup getting_started_sta
//! @{
//
//****************************************************************************

// Standard includes
#include <stdlib.h>
#include <string.h>

// Simplelink includes
#include "simplelink.h"

//Driverlib includes
#include "hw_types.h"
#include "hw_ints.h"
#include "rom.h"
#include "rom_map.h"
#include "interrupt.h"
#include "hw_memmap.h"
#include "timer.h"

#include "prcm.h"
#include "utils.h"

//Free_rtos/ti-rtos includes
#include "osi.h"

//Common interface includes
#include "gpio_if.h"
#ifndef NOTERM
#include "uart_if.h"
#endif
#include "common.h"
#include "pinmux.h"

#include "systick.h"
#include "timer_if.h"
#include "button_if.h"


#include "gagent.h"

// Common interface includes
#include "udma.h"
#include "udma_if.h"
#include "hw_uart.h"
#include "uart.h"


#define APPLICATION_NAME        "WLAN STATION"
#define APPLICATION_VERSION     "1.1.1"

#define HOST_NAME               "www.ti.com"

#define SYSTICK_RELOAD_VALUE    0x0000C3500
#define SYSTICKS_PER_SECOND     100


#define OSI_STACK_SIZE      8192////2048 //Nik.chen 
#define MAX_MSG_LENGTH   32

// Application specific status/error codes
typedef enum{
    // Choosing -0x7D0 to avoid overlap w/ host-driver's error codes
    LAN_CONNECTION_FAILED = -0x7D0,       
    INTERNET_CONNECTION_FAILED = LAN_CONNECTION_FAILED - 1,
    DEVICE_NOT_IN_STATION_MODE = INTERNET_CONNECTION_FAILED - 1,

    STATUS_CODE_MAX = -0xBB8
}e_AppStatusCodes;


//*****************************************************************************
//                 GLOBAL VARIABLES -- Start
//*****************************************************************************
unsigned long  g_ulStatus = 0;//SimpleLink Status
unsigned long  g_ulGatewayIP = 0; //Network Gateway IP address
unsigned long  g_ulStaIp = 0;
unsigned char  g_ucConnectionSSID[SSID_LEN_MAX+1]; //Connection SSID
unsigned char  g_ucConnectionBSSID[BSSID_LEN_MAX]; //Connection BSSID

unsigned long g_ulSeconds = 0;
static volatile unsigned long g_ulBase;
unsigned long g_ulTimerInts;

extern  uint16 *halWiFiStatus;

extern unsigned int switchSmartlink;
//static unsigned char    testValue[] = {40,31,32,33,34,35,36,37,38,39,40,21,22,23,24,25,26,27,28,29,30,11,12,13,14,15,16,17,18,19,20,1,2,3,4,5,6,7,8,9,10};


static unsigned char dnsflag =0;
OsiTaskHandle dnsTaskHandle = NULL;

#if defined(gcc)
extern void (* const g_pfnVectors[])(void);
#endif
#if defined(ewarm)
extern uVectorEntry __vector_table;
#endif
//*****************************************************************************
//                 GLOBAL VARIABLES -- End
//*****************************************************************************



//****************************************************************************
//                      LOCAL FUNCTION PROTOTYPES
//****************************************************************************
static long WlanConnect();
void WlanStationMode( void *pvParameters );
static void InitializeAppVariables();
 long ConfigureSimpleLinkToDefaultState();
 void DnsParsingTick( void *pvParameters);
 void setDnsFlag(unsigned char flag);
unsigned char getDnsFlag(void);

//*****************************************************************************
//                 GLOBAL VARIABLES -- End
//*****************************************************************************

void
UART1IntHandler(void)
{
    unsigned long ulStatus;
    unsigned long ulMode;
	unsigned char UARTData;
     int jj=0, ii=0, kk=0;

	//g_ucRxBufA[0] = '\0';		
	//g_ucRxBufB[0] = '\0';
    //
    // Read the interrupt status of the UART.
    //
    ulStatus = MAP_UARTIntStatus(UARTA1_BASE, 1);
    //
    // Clear any pending status, even though there should be none since no UART
    // interrupts were enabled.  
    //
    MAP_UARTIntClear(UARTA1_BASE, ulStatus);

    //
    // Check the DMA control table to see if the ping-pong "A" transfer is
    // complete.  The "A" transfer uses receive buffer "A", and the primary
    // control structure.
    //
    ulMode = MAP_uDMAChannelModeGet(UDMA_CH10_UARTA1_RX | UDMA_PRI_SELECT);

    //
    // If the primary control structure indicates stop, that means the "A"
    // receive buffer is done.  The uDMA controller should still be receiving
    // data into the "B" buffer.
    //
     if(MAP_UARTCharsAvail(UARTA1_BASE))
     {
		UARTData = (unsigned char)MAP_UARTCharGetNonBlocking(UARTA1_BASE);
             //UARTData = (unsigned char)UARTCharGet (UARTA1_BASE);

		//MAP_UARTCharPut(CONSOLE,UARTData);
		
		
        UART_PRINT("%02x ", UARTData);
      
     }

     MAP_UARTIntClear(UARTA1_BASE, ulStatus);
	
#if 0
    //
    // Check the DMA control table to see if the ping-pong "B" transfer is
    // complete.  The "B" transfer uses receive buffer "B", and the alternate
    // control structure.
    //
    ulMode = MAP_uDMAChannelModeGet(UDMA_CH10_UARTA1_RX | UDMA_ALT_SELECT);

    //
    // If the alternate control structure indicates stop, that means the "B"
    // receive buffer is done.  The uDMA controller should still be receiving
    // data into the "A" buffer.
    //
    if(ulMode == UDMA_MODE_STOP)
    {
        //
        // Increment a counter to indicate data was received into buffer A. 
        //
        g_ulRxBufBCount++;

        //
        // Set up the next transfer for the "B" buffer, using the alternate
        // control structure.  When the ongoing receive into the "A" buffer is
        // done, the uDMA controller will switch back to this one. 
        //
        UDMASetupTransfer(UDMA_CH10_UARTA1_RX | UDMA_ALT_SELECT,
                          UDMA_MODE_PINGPONG, 16 /*sizeof(g_ucRxBufB)*/,UDMA_SIZE_8,
                          UDMA_ARB_4,(void *)(UARTA1_BASE + UART_O_DR), 
                          UDMA_SRC_INC_NONE, g_ucRxBufB, UDMA_DST_INC_8);

		//g_ucRxBufB[UART_RXBUF_SIZE-1] = '\0';

		UART_PRINT("B:%s\r\n", g_ucRxBufB);
    }
#endif
#if 0

    //
    // If the UART0 DMA TX channel is disabled, that means the TX DMA transfer
    // is done.
    //
    if(!MAP_uDMAChannelIsEnabled(UDMA_CH10_UARTA1_RX))
    {
        g_ulTxCount++;
        //
        // Start another DMA transfer to UART0 TX.
        //
        UDMASetupTransfer(UDMA_CH10_UARTA1_RX| UDMA_PRI_SELECT, UDMA_MODE_BASIC,
           sizeof(g_ucTxBuf),UDMA_SIZE_8, UDMA_ARB_4,g_ucTxBuf, UDMA_SRC_INC_8,
                          (void *)(UARTA1_BASE + UART_O_DR), UDMA_DST_INC_NONE);
        //
        // The uDMA TX channel must be re-enabled.
        //
        MAP_uDMAChannelEnable(UDMA_CH10_UARTA1_RX);
    }
    else
    {
        UARTDone=1;
        MAP_UARTIntUnregister(UARTA1_BASE);
    }
#endif

}

//*****************************************************************************
//
//! Initializes the UART0 peripheral and sets up the TX and RX uDMA channels.
//! The UART is configured for loopback mode so that any data sent on TX will be
//! received on RX.  The uDMA channels are configured so that the TX channel
//! will copy data from a buffer to the UART TX output.  And the uDMA RX channel
//! will receive any incoming data into a pair of buffers in ping-pong mode.
//!
//! \param None
//!
//! \return None
//!
//*****************************************************************************
void
InitUART1Transfer(void)
{
    GAgent_Printf( GAGENT_CRITICAL,"line=%d !!!\r\n", __LINE__);
    MAP_PRCMPeripheralReset(PRCM_UARTA1);
    MAP_PRCMPeripheralClkEnable(PRCM_UARTA1,PRCM_RUN_MODE_CLK);
    GAgent_Printf( GAGENT_CRITICAL,"line=%d !!!\r\n", __LINE__);

    MAP_UARTConfigSetExpClk(UARTA1_BASE,SYSCLK, COMMU_BAUD_RATE,
                            (UART_CONFIG_WLEN_8 | UART_CONFIG_STOP_ONE |
                                        UART_CONFIG_PAR_NONE));
    GAgent_Printf( GAGENT_CRITICAL,"line=%d !!!\r\n", __LINE__);

    MAP_UARTFlowControlSet(UARTA1_BASE, UART_FLOWCONTROL_NONE);
    GAgent_Printf( GAGENT_CRITICAL,"line=%d !!!\r\n", __LINE__);
	
    MAP_uDMAChannelAssign(UDMA_CH10_UARTA1_RX);
    MAP_uDMAChannelAssign(UDMA_CH11_UARTA1_TX);
    GAgent_Printf( GAGENT_CRITICAL,"line=%d !!!\r\n", __LINE__);
    MAP_UARTIntRegister(UARTA1_BASE,UART1IntHandler);
    GAgent_Printf( GAGENT_CRITICAL,"line=%d !!!\r\n", __LINE__);
    
    //
    // Set both the TX and RX trigger thresholds to 4.  This will be used by
    // the uDMA controller to signal when more data should be transferred.  The
    // uDMA TX and RX channels will be configured so that it can transfer 4
    // bytes in a burst when the UART is ready to transfer more data.
    //
    MAP_UARTFIFOLevelSet(UARTA1_BASE, UART_FIFO_TX4_8, UART_FIFO_RX4_8);
    GAgent_Printf( GAGENT_CRITICAL,"line=%d !!!\r\n", __LINE__);

    //
    // Enable the UART for operation, and enable the uDMA interface for both TX
    // and RX channels.
    //
    MAP_UARTEnable(UARTA1_BASE);

    //
    // Enable the UART peripheral interrupts. uDMA controller will cause an 
    // interrupt on the UART interrupt signal when a uDMA transfer is complete.
    //
   
    //MAP_UARTIntEnable(UARTA1_BASE,UART_INT_DMATX);
    //MAP_UARTIntEnable(UARTA1_BASE,UART_INT_DMARX); //Nik.chen open will stop/error

#if 0
    //
    // Configure the control parameters for the UART TX.  The uDMA UART TX
    // channel is used to transfer a block of data from a buffer to the UART.
    // The data size is 8 bits.  The source address increment is 8-bit bytes
    // since the data is coming from a buffer.  The destination increment is
    // none since the data is to be written to the UART data register.  The
    // arbitration size is set to 4, which matches the UART TX FIFO trigger
    // threshold.
    //
    UDMASetupTransfer(UDMA_CH10_UARTA1_RX | UDMA_PRI_SELECT, UDMA_MODE_PINGPONG,
            8 /*sizeof(g_ucRxBufA)*/,UDMA_SIZE_8, UDMA_ARB_4,
            (void *)(UARTA1_BASE + UART_O_DR), UDMA_SRC_INC_NONE,
                                            g_ucRxBufA, UDMA_DST_INC_8);

    UDMASetupTransfer(UDMA_CH10_UARTA1_RX | UDMA_ALT_SELECT, UDMA_MODE_PINGPONG,
            8 /*sizeof(g_ucRxBufB)*/,UDMA_SIZE_8, UDMA_ARB_4,
              (void *)(UARTA1_BASE + UART_O_DR), UDMA_SRC_INC_NONE,
                                            g_ucRxBufB, UDMA_DST_INC_8);
#endif	
    
    MAP_UARTDMAEnable(UARTA1_BASE, UART_DMA_RX | UART_DMA_TX);
}


//*****************************************************************************
//!
//! The interrupt handler for the SysTick timer.  This handler will increment a
//! seconds counter whenever the appropriate number of ticks has occurred. 
//!
//! \param None
//! 
//! \return None
//!
//*****************************************************************************
void
SysTickHandler(void)
{
    static unsigned long ulTickCount = 0;
    
    //
    // Increment the tick counter.
    //
    ulTickCount++;
    
    //
    // If the number of ticks per second has occurred, then increment the
    // seconds counter.
    //
    if(!(ulTickCount % SYSTICKS_PER_SECOND))
    {
        g_ulSeconds++;
    }

}

//*****************************************************************************
//
//! The interrupt handler for the first timer interrupt.
//!
//! \param  None
//!
//! \return none
//
//*****************************************************************************
void
TimerBaseIntHandler(void)
{
    //
    // Clear the timer interrupt.
    //
    Timer_IF_InterruptClear(g_ulBase);

    g_ulTimerInts ++;
	g_ulSeconds ++;

	Timer_IF_Stop(g_ulBase,TIMERA0_BASE);

	Timer_IF_Start(g_ulBase, TIMER_A, 1000);
}

//*****************************************************************************
//
//! Mandatory Configuration to put the PM into safe state before entering hibernate
//!
//! \param  None
//!
//! \return None
//
//*****************************************************************************
static inline void HIBEntrePreamble()
{
    HWREG(0x400F70B8) = 1;
    UtilsDelay(800000/5);
    HWREG(0x400F70B0) = 1;
    UtilsDelay(800000/5);

    HWREG(0x4402E16C) |= 0x2;
    UtilsDelay(800);
    HWREG(0x4402F024) &= 0xF7FFFFFF;
}

#ifdef USE_FREERTOS
//*****************************************************************************
// FreeRTOS User Hook Functions enabled in FreeRTOSConfig.h
//*****************************************************************************

//*****************************************************************************
//
//! \brief Application defined hook (or callback) function - assert
//!
//! \param[in]  pcFile - Pointer to the File Name
//! \param[in]  ulLine - Line Number
//! 
//! \return none
//!
//*****************************************************************************
void
vAssertCalled( const char *pcFile, unsigned long ulLine )
{
    //Handle Assert here
    while(1)
    {
    }
}

//*****************************************************************************
//
//! \brief Application defined idle task hook
//! 
//! \param  none
//! 
//! \return none
//!
//*****************************************************************************
void
vApplicationIdleHook( void)
{
    //Handle Idle Hook for Profiling, Power Management etc
}

//*****************************************************************************
//
//! \brief Application defined malloc failed hook
//! 
//! \param  none
//! 
//! \return none
//!
//*****************************************************************************
void vApplicationMallocFailedHook()
{
    //Handle Memory Allocation Errors
    while(1)
    {
    }
}

//*****************************************************************************
//
//! \brief Application defined stack overflow hook
//! 
//! \param  none
//! 
//! \return none
//!
//*****************************************************************************
void vApplicationStackOverflowHook( OsiTaskHandle *pxTask,
                                   signed char *pcTaskName)
{
    //Handle FreeRTOS Stack Overflow
    while(1)
    {
    }
}
#endif //USE_FREERTOS


//*****************************************************************************
// SimpleLink Asynchronous Event Handlers -- Start
//*****************************************************************************


//*****************************************************************************
//
//! \brief The Function Handles WLAN Events
//!
//! \param[in]  pWlanEvent - Pointer to WLAN Event Info
//!
//! \return None
//!
//*****************************************************************************
void SimpleLinkWlanEventHandler(SlWlanEvent_t *pWlanEvent)
{
    switch(pWlanEvent->Event)
    {
        case SL_WLAN_CONNECT_EVENT:
        {
            SET_STATUS_BIT(g_ulStatus, STATUS_BIT_CONNECTION);
            
            //
            // Information about the connected AP (like name, MAC etc) will be
            // available in 'slWlanConnectAsyncResponse_t'-Applications
            // can use it if required
            //
            //  slWlanConnectAsyncResponse_t *pEventData = NULL;
            // pEventData = &pWlanEvent->EventData.STAandP2PModeWlanConnected;
            //
            
            // Copy new connection SSID and BSSID to global parameters
            memset(g_ucConnectionSSID,0,sizeof(g_ucConnectionSSID));
            memset(g_ucConnectionBSSID,0,sizeof(g_ucConnectionBSSID));
            
            memcpy(g_ucConnectionSSID,pWlanEvent->EventData.
                   STAandP2PModeWlanConnected.ssid_name,
                   pWlanEvent->EventData.STAandP2PModeWlanConnected.ssid_len);
            memcpy(g_ucConnectionBSSID,
                   pWlanEvent->EventData.STAandP2PModeWlanConnected.bssid,
                   SL_BSSID_LENGTH);

            UART_PRINT("Nik.chen:[WLAN EVENT] STA Connected to the AP: %s ,"
                        "BSSID: %x:%x:%x:%x:%x:%x\n\r",
                      g_ucConnectionSSID,g_ucConnectionBSSID[0],
                      g_ucConnectionBSSID[1],g_ucConnectionBSSID[2],
                      g_ucConnectionBSSID[3],g_ucConnectionBSSID[4],
                      g_ucConnectionBSSID[5]);
        }

         GAgent_DevCheckWifiStatus( WIFI_STATION_CONNECTED,1);
         GAgent_DevCheckWifiStatus( WIFI_MODE_STATION,1);
         GAgent_DevCheckWifiStatus( WIFI_MODE_AP,0 );
         GAgent_AirlinkResult( pgContextData );
        GAgent_Printf(GAGENT_INFO,"SimpleLinkWlanEventHandler STA UP! ");
        break;

        case SL_WLAN_DISCONNECT_EVENT:
        {
            slWlanConnectAsyncResponse_t*  pEventData = NULL;

            CLR_STATUS_BIT(g_ulStatus, STATUS_BIT_CONNECTION);
            CLR_STATUS_BIT(g_ulStatus, STATUS_BIT_IP_AQUIRED);

            pEventData = &pWlanEvent->EventData.STAandP2PModeDisconnected;

            // If the user has initiated 'Disconnect' request, 
            //'reason_code' is SL_USER_INITIATED_DISCONNECTION 
            if(SL_USER_INITIATED_DISCONNECTION == pEventData->reason_code)
            {
                UART_PRINT("Nik.chen:[WLAN EVENT]Device disconnected from the AP: %s,"
                "BSSID: %x:%x:%x:%x:%x:%x on application's request \n\r",
                           g_ucConnectionSSID,g_ucConnectionBSSID[0],
                           g_ucConnectionBSSID[1],g_ucConnectionBSSID[2],
                           g_ucConnectionBSSID[3],g_ucConnectionBSSID[4],
                           g_ucConnectionBSSID[5]);
            }
            else
            {
                UART_PRINT("[WLAN ERROR]Device disconnected from the  AP: %s,"
                "BSSID: %x:%x:%x:%x:%x:%x on an ERROR..!! \n\r",
                           g_ucConnectionSSID,g_ucConnectionBSSID[0],
                           g_ucConnectionBSSID[1],g_ucConnectionBSSID[2],
                           g_ucConnectionBSSID[3],g_ucConnectionBSSID[4],
                           g_ucConnectionBSSID[5]);
            }
           
        }

        GAgent_DevCheckWifiStatus( WIFI_STATION_CONNECTED,0 );
        GAgent_DevCheckWifiStatus( WIFI_CLOUD_CONNECTED,0 );
        if(1 == getconnFlag()&&(0x55 == pgContextData->gc.airkiss_value))
        {
            GAgent_Printf(GAGENT_INFO,"before is softAP mode!! ");
            setconnFlag(2); 
        }else if(1 == getconnFlag()&&(0x53 == pgContextData->gc.airkiss_value))
        {
            GAgent_Printf(GAGENT_INFO,"before is airlink mode!! ");
            setconnFlag(3); 
        }
         GAgent_Printf(GAGENT_INFO,"SimpleLinkWlanEventHandler STA DOWN! ");
        break;

        case SL_WLAN_STA_CONNECTED_EVENT:
        {
            // when device is in AP mode and any client connects to device cc3xxx
            SET_STATUS_BIT(g_ulStatus, STATUS_BIT_CONNECTION);

            //
            // Information about the connected client (like SSID, MAC etc) will be
            // available in 'slPeerInfoAsyncResponse_t' - Applications
            // can use it if required
            //
            // slPeerInfoAsyncResponse_t *pEventData = NULL;
            // pEventData = &pSlWlanEvent->EventData.APModeStaConnected;
            //
      GAgent_DevCheckWifiStatus( WIFI_MODE_AP,1 ); //Nik.chen error
      GAgent_DevCheckWifiStatus( WIFI_MODE_STATION,0 );
             GAgent_Printf(GAGENT_INFO,"SimpleLinkWlanEventHandler AP mode UP! ");

        }
        break;

        case SL_WLAN_STA_DISCONNECTED_EVENT:
        {
            // when client disconnects from device (AP)
            CLR_STATUS_BIT(g_ulStatus, STATUS_BIT_CONNECTION);
            CLR_STATUS_BIT(g_ulStatus, STATUS_BIT_IP_LEASED);

            //
            // Information about the connected client (like SSID, MAC etc) will
            // be available in 'slPeerInfoAsyncResponse_t' - Applications
            // can use it if required
            //
            // slPeerInfoAsyncResponse_t *pEventData = NULL;
            // pEventData = &pSlWlanEvent->EventData.APModestaDisconnected;
            //            
        }

     // GAgent_DevCheckWifiStatus( WIFI_MODE_AP,0 );
      GAgent_Printf(GAGENT_INFO,"SimpleLinkWlanEventHandler AP mode DOWN! ");
        break;

        default:
        {
            UART_PRINT("[WLAN EVENT] Unexpected event [0x%x]\n\r",
                       pWlanEvent->Event);
        }
        break;
    }
}

//*****************************************************************************
//
//! \brief This function handles network events such as IP acquisition, IP
//!           leased, IP released etc.
//!
//! \param[in]  pNetAppEvent - Pointer to NetApp Event Info 
//!
//! \return None
//!
//*****************************************************************************
void SimpleLinkNetAppEventHandler(SlNetAppEvent_t *pNetAppEvent)
{
    switch(pNetAppEvent->Event)
    {
        case SL_NETAPP_IPV4_IPACQUIRED_EVENT:
        case SL_NETAPP_IPV6_IPACQUIRED_EVENT:
        {
            SlIpV4AcquiredAsync_t *pEventData = NULL;

            SET_STATUS_BIT(g_ulStatus, STATUS_BIT_IP_AQUIRED);
            
            //Ip Acquired Event Data
            pEventData = &pNetAppEvent->EventData.ipAcquiredV4;
            
            //Gateway IP address
            g_ulGatewayIP = pEventData->gateway;
            
            UART_PRINT("[NETAPP EVENT] IP Acquired: IP=%d.%d.%d.%d , "
            "Gateway=%d.%d.%d.%d\n\r", 
            SL_IPV4_BYTE(pNetAppEvent->EventData.ipAcquiredV4.ip,3),
            SL_IPV4_BYTE(pNetAppEvent->EventData.ipAcquiredV4.ip,2),
            SL_IPV4_BYTE(pNetAppEvent->EventData.ipAcquiredV4.ip,1),
            SL_IPV4_BYTE(pNetAppEvent->EventData.ipAcquiredV4.ip,0),
            SL_IPV4_BYTE(pNetAppEvent->EventData.ipAcquiredV4.gateway,3),
            SL_IPV4_BYTE(pNetAppEvent->EventData.ipAcquiredV4.gateway,2),
            SL_IPV4_BYTE(pNetAppEvent->EventData.ipAcquiredV4.gateway,1),
            SL_IPV4_BYTE(pNetAppEvent->EventData.ipAcquiredV4.gateway,0));
        }
        break;
        
        case SL_NETAPP_IP_LEASED_EVENT:
        {
            SET_STATUS_BIT(g_ulStatus, STATUS_BIT_IP_LEASED);
        
            g_ulStaIp = (pNetAppEvent)->EventData.ipLeased.ip_address;
            
            UART_PRINT("[NETAPP EVENT] IP Leased to Client: IP=%d.%d.%d.%d , ",
                        SL_IPV4_BYTE(g_ulStaIp,3), SL_IPV4_BYTE(g_ulStaIp,2),
                        SL_IPV4_BYTE(g_ulStaIp,1), SL_IPV4_BYTE(g_ulStaIp,0));
        }
        break;
        
        case SL_NETAPP_IP_RELEASED_EVENT:
        {
            CLR_STATUS_BIT(g_ulStatus, STATUS_BIT_IP_LEASED);

            UART_PRINT("[NETAPP EVENT] IP Released for Client: IP=%d.%d.%d.%d , ",
                        SL_IPV4_BYTE(g_ulStaIp,3), SL_IPV4_BYTE(g_ulStaIp,2),
                        SL_IPV4_BYTE(g_ulStaIp,1), SL_IPV4_BYTE(g_ulStaIp,0));

        }
        break;

        default:
        {
            UART_PRINT("[NETAPP EVENT] Unexpected event [0x%x] \n\r",
                       pNetAppEvent->Event);
        }
        break;
    }
}


//*****************************************************************************
//
//! \brief This function handles HTTP server events
//!
//! \param[in]  pServerEvent - Contains the relevant event information
//! \param[in]    pServerResponse - Should be filled by the user with the
//!                                      relevant response information
//!
//! \return None
//!
//****************************************************************************
void SimpleLinkHttpServerCallback(SlHttpServerEvent_t *pHttpEvent,
                                  SlHttpServerResponse_t *pHttpResponse)
{
    // Unused in this application
}

//*****************************************************************************
//
//! \brief This function handles General Events
//!
//! \param[in]     pDevEvent - Pointer to General Event Info 
//!
//! \return None
//!
//*****************************************************************************
void SimpleLinkGeneralEventHandler(SlDeviceEvent_t *pDevEvent)
{
    //
    // Most of the general errors are not FATAL are are to be handled
    // appropriately by the application
    //
    UART_PRINT("[GENERAL EVENT] - ID=[%d] Sender=[%d]\n\n",
               pDevEvent->EventData.deviceEvent.status, 
               pDevEvent->EventData.deviceEvent.sender);
}


//*****************************************************************************
//
//! This function handles socket events indication
//!
//! \param[in]      pSock - Pointer to Socket Event Info
//!
//! \return None
//!
//*****************************************************************************
void SimpleLinkSockEventHandler(SlSockEvent_t *pSock)
{
    //
    // This application doesn't work w/ socket - Events are not expected
    //
    switch( pSock->Event )
    {
        case SL_SOCKET_TX_FAILED_EVENT:
            switch( pSock->socketAsyncEvent.SockTxFailData.status)
            {
                case SL_ECLOSE: 
                    UART_PRINT("[SOCK ERROR] - close socket (%d) operation "
                                "failed to transmit all queued packets\n\n", 
                                    pSock->socketAsyncEvent.SockTxFailData.sd);
                    break;
                default: 
                    UART_PRINT("[SOCK ERROR] - TX FAILED  :  socket %d , reason "
                                "(%d) \n\n",
                                pSock->socketAsyncEvent.SockTxFailData.sd, pSock->socketAsyncEvent.SockTxFailData.status);
                  break;
            }
            break;

        default:
        	UART_PRINT("[SOCK EVENT] - Unexpected Event [%x0x]\n\n",pSock->Event);
          break;
    }

}

//*****************************************************************************
// SimpleLink Asynchronous Event Handlers -- End
//*****************************************************************************



//*****************************************************************************
//
//! \brief This function initializes the application variables
//!
//! \param    None
//!
//! \return None
//!
//*****************************************************************************
static void InitializeAppVariables()
{
    g_ulStatus = 0;
    g_ulGatewayIP = 0;
    g_ulStaIp = 0;
    memset(g_ucConnectionSSID,0,sizeof(g_ucConnectionSSID));
    memset(g_ucConnectionBSSID,0,sizeof(g_ucConnectionBSSID));
}


//*****************************************************************************
//! \brief This function puts the device in its default state. It:
//!           - Set the mode to STATION
//!           - Configures connection policy to Auto and AutoSmartConfig
//!           - Deletes all the stored profiles
//!           - Enables DHCP
//!           - Disables Scan policy
//!           - Sets Tx power to maximum
//!           - Sets power policy to normal
//!           - Unregister mDNS services
//!           - Remove all filters
//!
//! \param   none
//! \return  On success, zero is returned. On error, negative is returned
//*****************************************************************************

 long ConfigureSimpleLinkToDefaultState2()
{
    SlVersionFull   ver = {0};
    _WlanRxFilterOperationCommandBuff_t  RxFilterIdMask = {0};

    unsigned char ucVal = 1;
    unsigned char ucConfigOpt = 0;
    unsigned char ucConfigLen = 0;
    unsigned char ucPower = 0;

    long lRetVal = -1;
    long lMode = -1;

    lMode = sl_Start(0, 0, 0);
    ASSERT_ON_ERROR(lMode);

    // If the device is not in station-mode, try configuring it in station-mode 
    if (ROLE_STA != lMode)
    {
        if (ROLE_AP == lMode)
        {
            // If the device is in AP mode, we need to wait for this event 
            // before doing anything 
            while(!IS_IP_ACQUIRED(g_ulStatus))
            {
#ifndef SL_PLATFORM_MULTI_THREADED
              _SlNonOsMainLoopTask(); 
#endif
            }
        }

        // Switch to STA role and restart 
        lRetVal = sl_WlanSetMode(ROLE_STA);
        ASSERT_ON_ERROR(lRetVal);

        lRetVal = sl_Stop(0xFF);
        ASSERT_ON_ERROR(lRetVal);

        lRetVal = sl_Start(0, 0, 0);
        ASSERT_ON_ERROR(lRetVal);

        // Check if the device is in station again 
        if (ROLE_STA != lRetVal)
        {
            // We don't want to proceed if the device is not coming up in STA-mode 
            ASSERT_ON_ERROR(DEVICE_NOT_IN_STATION_MODE);
        }
    }
    
    // Get the device's version-information
    ucConfigOpt = SL_DEVICE_GENERAL_VERSION;
    ucConfigLen = sizeof(ver);
    lRetVal = sl_DevGet(SL_DEVICE_GENERAL_CONFIGURATION, &ucConfigOpt, 
                                &ucConfigLen, (unsigned char *)(&ver));
    ASSERT_ON_ERROR(lRetVal);
    
    UART_PRINT("Host Driver Version: %s\n\r",SL_DRIVER_VERSION);
    UART_PRINT("Build Version %d.%d.%d.%d.31.%d.%d.%d.%d.%d.%d.%d.%d\n\r",
    ver.NwpVersion[0],ver.NwpVersion[1],ver.NwpVersion[2],ver.NwpVersion[3],
    ver.ChipFwAndPhyVersion.FwVersion[0],ver.ChipFwAndPhyVersion.FwVersion[1],
    ver.ChipFwAndPhyVersion.FwVersion[2],ver.ChipFwAndPhyVersion.FwVersion[3],
    ver.ChipFwAndPhyVersion.PhyVersion[0],ver.ChipFwAndPhyVersion.PhyVersion[1],
    ver.ChipFwAndPhyVersion.PhyVersion[2],ver.ChipFwAndPhyVersion.PhyVersion[3]);

    // Set connection policy to Auto 
    lRetVal = sl_WlanPolicySet(SL_POLICY_CONNECTION, 
                                SL_CONNECTION_POLICY(1, 0, 0, 0, 0), NULL, 0);
    ASSERT_ON_ERROR(lRetVal);

    //
    // Device in station-mode. Disconnect previous connection if any
    // The function returns 0 if 'Disconnected done', negative number if already
    // disconnected Wait for 'disconnection' event if 0 is returned, Ignore 
    // other return-codes
    //
    lRetVal = sl_WlanDisconnect();
    if(0 == lRetVal)
    {
        // Wait
        while(IS_CONNECTED(g_ulStatus))
        {
#ifndef SL_PLATFORM_MULTI_THREADED
              _SlNonOsMainLoopTask(); 
#endif
        }
    }

    // Enable DHCP client
    lRetVal = sl_NetCfgSet(SL_IPV4_STA_P2P_CL_DHCP_ENABLE,1,1,&ucVal);
    ASSERT_ON_ERROR(lRetVal);

    // Disable scan
    ucConfigOpt = SL_SCAN_POLICY(0);
    lRetVal = sl_WlanPolicySet(SL_POLICY_SCAN , ucConfigOpt, NULL, 0);
    ASSERT_ON_ERROR(lRetVal);

    // Set Tx power level for station mode
    // Number between 0-15, as dB offset from max power - 0 will set max power
    ucPower = 0;
    lRetVal = sl_WlanSet(SL_WLAN_CFG_GENERAL_PARAM_ID, 
            WLAN_GENERAL_PARAM_OPT_STA_TX_POWER, 1, (unsigned char *)&ucPower);
    ASSERT_ON_ERROR(lRetVal);

    // Set PM policy to normal
    lRetVal = sl_WlanPolicySet(SL_POLICY_PM , SL_NORMAL_POLICY, NULL, 0);
    ASSERT_ON_ERROR(lRetVal);

    // Unregister mDNS services
    lRetVal = sl_NetAppMDNSUnRegisterService(0, 0);
    ASSERT_ON_ERROR(lRetVal);

    // Remove  all 64 filters (8*8)
    memset(RxFilterIdMask.FilterIdMask, 0xFF, 8);
    lRetVal = sl_WlanRxFilterSet(SL_REMOVE_RX_FILTER, (_u8 *)&RxFilterIdMask,
                       sizeof(_WlanRxFilterOperationCommandBuff_t));
    ASSERT_ON_ERROR(lRetVal);

   // lRetVal = sl_Stop(SL_STOP_TIMEOUT);
   // ASSERT_ON_ERROR(lRetVal);

   // InitializeAppVariables();
    
    return lRetVal; // Success
}

//*****************************************************************************
//! \brief This function puts the device in its default state. It:
//!           - Set the mode to STATION
//!           - Configures connection policy to Auto and AutoSmartConfig
//!           - Deletes all the stored profiles
//!           - Enables DHCP
//!           - Disables Scan policy
//!           - Sets Tx power to maximum
//!           - Sets power policy to normal
//!           - Unregister mDNS services
//!           - Remove all filters
//!
//! \param   none
//! \return  On success, zero is returned. On error, negative is returned
//*****************************************************************************

 long ConfigureSimpleLinkToDefaultState()
{
    SlVersionFull   ver = {0};
    _WlanRxFilterOperationCommandBuff_t  RxFilterIdMask = {0};

    unsigned char ucVal = 1;
    unsigned char ucConfigOpt = 0;
    unsigned char ucConfigLen = 0;
    unsigned char ucPower = 0;

    long lRetVal = -1;
    long lMode = -1;

    lMode = sl_Start(0, 0, 0);
    ASSERT_ON_ERROR(lMode);

    // If the device is not in station-mode, try configuring it in station-mode 
    if (ROLE_STA != lMode)
    {
        if (ROLE_AP == lMode)
        {
            // If the device is in AP mode, we need to wait for this event 
            // before doing anything 
            while(!IS_IP_ACQUIRED(g_ulStatus))
            {
#ifndef SL_PLATFORM_MULTI_THREADED
              _SlNonOsMainLoopTask(); 
#endif
            }
        }

        // Switch to STA role and restart 
        lRetVal = sl_WlanSetMode(ROLE_STA);
        ASSERT_ON_ERROR(lRetVal);

        lRetVal = sl_Stop(0xFF);
        ASSERT_ON_ERROR(lRetVal);

        lRetVal = sl_Start(0, 0, 0);
        ASSERT_ON_ERROR(lRetVal);

        // Check if the device is in station again 
        if (ROLE_STA != lRetVal)
        {
            // We don't want to proceed if the device is not coming up in STA-mode 
            ASSERT_ON_ERROR(DEVICE_NOT_IN_STATION_MODE);
        }
    }
    
    // Get the device's version-information
    ucConfigOpt = SL_DEVICE_GENERAL_VERSION;
    ucConfigLen = sizeof(ver);
    lRetVal = sl_DevGet(SL_DEVICE_GENERAL_CONFIGURATION, &ucConfigOpt, 
                                &ucConfigLen, (unsigned char *)(&ver));
    ASSERT_ON_ERROR(lRetVal);
    
    UART_PRINT("Host Driver Version: %s\n\r",SL_DRIVER_VERSION);
    UART_PRINT("Build Version %d.%d.%d.%d.31.%d.%d.%d.%d.%d.%d.%d.%d\n\r",
    ver.NwpVersion[0],ver.NwpVersion[1],ver.NwpVersion[2],ver.NwpVersion[3],
    ver.ChipFwAndPhyVersion.FwVersion[0],ver.ChipFwAndPhyVersion.FwVersion[1],
    ver.ChipFwAndPhyVersion.FwVersion[2],ver.ChipFwAndPhyVersion.FwVersion[3],
    ver.ChipFwAndPhyVersion.PhyVersion[0],ver.ChipFwAndPhyVersion.PhyVersion[1],
    ver.ChipFwAndPhyVersion.PhyVersion[2],ver.ChipFwAndPhyVersion.PhyVersion[3]);

    // Set connection policy to Auto + SmartConfig 
    //      (Device's default connection policy)
    lRetVal = sl_WlanPolicySet(SL_POLICY_CONNECTION, 
                                SL_CONNECTION_POLICY(1, 0, 0, 0, 1), NULL, 0);
    ASSERT_ON_ERROR(lRetVal);

    // Remove all profiles
    lRetVal = sl_WlanProfileDel(0xFF); //Nik.chen mark
    ASSERT_ON_ERROR(lRetVal);

    

    //
    // Device in station-mode. Disconnect previous connection if any
    // The function returns 0 if 'Disconnected done', negative number if already
    // disconnected Wait for 'disconnection' event if 0 is returned, Ignore 
    // other return-codes
    //
    lRetVal = sl_WlanDisconnect();
    if(0 == lRetVal)
    {
        // Wait
        while(IS_CONNECTED(g_ulStatus))
        {
#ifndef SL_PLATFORM_MULTI_THREADED
              _SlNonOsMainLoopTask(); 
#endif
        }
    }

    // Enable DHCP client
    lRetVal = sl_NetCfgSet(SL_IPV4_STA_P2P_CL_DHCP_ENABLE,1,1,&ucVal);
    ASSERT_ON_ERROR(lRetVal);

    // Disable scan
    ucConfigOpt = SL_SCAN_POLICY(0);
    lRetVal = sl_WlanPolicySet(SL_POLICY_SCAN , ucConfigOpt, NULL, 0);
    ASSERT_ON_ERROR(lRetVal);

    // Set Tx power level for station mode
    // Number between 0-15, as dB offset from max power - 0 will set max power
    ucPower = 0;
    lRetVal = sl_WlanSet(SL_WLAN_CFG_GENERAL_PARAM_ID, 
            WLAN_GENERAL_PARAM_OPT_STA_TX_POWER, 1, (unsigned char *)&ucPower);
    ASSERT_ON_ERROR(lRetVal);

    // Set PM policy to normal
    lRetVal = sl_WlanPolicySet(SL_POLICY_PM , SL_NORMAL_POLICY, NULL, 0);
    ASSERT_ON_ERROR(lRetVal);

    // Unregister mDNS services
    lRetVal = sl_NetAppMDNSUnRegisterService(0, 0);
    ASSERT_ON_ERROR(lRetVal);

    // Remove  all 64 filters (8*8)
    memset(RxFilterIdMask.FilterIdMask, 0xFF, 8);
    lRetVal = sl_WlanRxFilterSet(SL_REMOVE_RX_FILTER, (_u8 *)&RxFilterIdMask,
                       sizeof(_WlanRxFilterOperationCommandBuff_t));
    ASSERT_ON_ERROR(lRetVal);

   // lRetVal = sl_Stop(SL_STOP_TIMEOUT);
   // ASSERT_ON_ERROR(lRetVal);

   // InitializeAppVariables();
    
    return lRetVal; // Success
}

//****************************************************************************
//
//! \brief Connecting to a WLAN Accesspoint
//!
//!  This function connects to the required AP (SSID_NAME) with Security
//!  parameters specified in te form of macros at the top of this file
//!
//! \param  None
//!
//! \return  None
//!
//! \warning    If the WLAN connection fails or we don't aquire an IP 
//!            address, It will be stuck in this function forever.
//
//****************************************************************************
static long WlanConnect()
{
    SlSecParams_t secParams = {0};
    long lRetVal = 0;

    secParams.Key = (signed char*)SECURITY_KEY;
    secParams.KeyLen = strlen(SECURITY_KEY);
    secParams.Type = SECURITY_TYPE;

    lRetVal = sl_WlanConnect((signed char*)SSID_NAME, strlen(SSID_NAME), 0, &secParams, 0);
    ASSERT_ON_ERROR(lRetVal);

    // Wait for WLAN Event
    while((!IS_CONNECTED(g_ulStatus)) || (!IS_IP_ACQUIRED(g_ulStatus))) 
    { 
        // Toggle LEDs to Indicate Connection Progress
        GPIO_IF_LedOff(MCU_IP_ALLOC_IND);
        MAP_UtilsDelay(800000);
        GPIO_IF_LedOn(MCU_IP_ALLOC_IND);
        MAP_UtilsDelay(800000);
    }

    return SUCCESS;
   
}


void NetStop()
{
    unsigned long lRetVal;

    //
    // Disconnect from the AP
    //
    lRetVal = sl_WlanDisconnect();
    if(0 == lRetVal)
    {
        // Wait
        while(IS_CONNECTED(g_ulStatus))
        {
#ifndef SL_PLATFORM_MULTI_THREADED
              _SlNonOsMainLoopTask();
#endif
        }
    }

    //
    // Stop the simplelink host
    //
    sl_Stop(SL_STOP_TIMEOUT);
}


int gizwits_main()
{
    GAgent_Init( &pgContextData );
    GAgent_dumpInfo( pgContextData );
    while(1)
    {
        GAgent_Tick( pgContextData );
        GAgent_SelectFd( pgContextData,1,0 ); //200000
        GAgent_Lan_Handle( pgContextData, pgContextData->rtinfo.Rxbuf , GAGENT_BUF_LEN );
        GAgent_Local_Handle( pgContextData, pgContextData->rtinfo.Rxbuf, GAGENT_BUF_LEN ); //Nik.chen mark for test
        GAgent_Cloud_Handle( pgContextData, pgContextData->rtinfo.Rxbuf, GAGENT_BUF_LEN );
    }
}

//****************************************************************************
//
//! \brief Start simplelink, connect to the ap and run the ping test
//!
//! This function starts the simplelink, connect to the ap and start the ping
//! test on the default gateway for the ap
//!
//! \param[in]  pvParameters - Pointer to the list of parameters that 
//!             can bepassed to the task while creating it
//!
//! \return  None
//
//****************************************************************************
void WlanStationMode( void *pvParameters )
{

    long lRetVal = -1;
    InitializeAppVariables();

    //
    // Following function configure the device to default state by cleaning
    // the persistent settings stored in NVMEM (viz. connection profiles &
    // policies, power policy etc)
    //
    // Applications may choose to skip this step if the developer is sure
    // that the device is in its default state at start of applicaton
    //
    // Note that all profiles and persistent settings that were done on the
    // device will be lost
    //
      #if 0
    lRetVal = ConfigureSimpleLinkToDefaultState();
    if(lRetVal < 0)
    {
        if (DEVICE_NOT_IN_STATION_MODE == lRetVal)
        {
            UART_PRINT("Failed to configure the device in its default state\n\r");
        }

        LOOP_FOREVER();
    }
    #endif
    #if 1
    UART_PRINT("Device is configured in default state \n\r");

    //
    // Assumption is that the device is configured in station mode already
    // and it is in its default state
    //
   #if 1
    lRetVal = sl_Start(0, 0, 0); //
    if (lRetVal < 0 ||( ROLE_STA != lRetVal&& ROLE_AP != lRetVal)) 
    {
        UART_PRINT("Failed to start the device \n\r");
        LOOP_FOREVER();
    }

    //setMode(lRetVal);
    UART_PRINT("Device started as:%d \n\r", lRetVal);
    if(ROLE_AP == lRetVal)
    {
        lRetVal = sl_WlanSetMode(ROLE_STA);
        if(lRetVal < 0)
        {
            UART_PRINT("sl_WlanSetMode failed!!! \n\r");
            return; 
        }
         /* Restart Network processor */
       // lRetVal = sl_Stop(SL_STOP_TIMEOUT);

        // reset status bits
        CLR_STATUS_BIT_ALL(g_ulStatus);

        lRetVal = sl_Start(NULL,NULL,NULL);
        if(lRetVal < 0)
        {
           UART_PRINT("sl_Start failed!!! \n\r");
           return; 
        } 
        setMode(lRetVal);

	 setAirlinkTimes(0);

        //wlanscan();
        UART_PRINT("boot and switch to STA success!!! \n\r");
    }
   #endif
    //
    //Connecting to WLAN AP
    //
    #if 0 //Nik.chen reconnect ap
    lRetVal = WlanConnect();
    if(lRetVal < 0)
    {
        UART_PRINT("Failed to establish connection w/ an AP \n\r");
        LOOP_FOREVER();
    }

    UART_PRINT("Connection established w/ AP and IP is aquired \n\r");
    #endif
    #endif

	gizwits_main();
    
}
//*****************************************************************************
//
//! Application startup display on UART
//!
//! \param  none
//!
//! \return none
//!
//*****************************************************************************
static void
DisplayBanner(char * AppName)
{

    UART_PRINT("\n\n\n\r");
    UART_PRINT("\t\t *************************************************\n\r");
    UART_PRINT("\t\t           CC3200 %s Application       \n\r", AppName);
    UART_PRINT("\t\t *************************************************\n\r");
    UART_PRINT("\n\n\n\r");
}
//*****************************************************************************
//
//! \brief  Board Initialization & Configuration
//!
//! \param  None
//!
//! \return None
//
//*****************************************************************************
static void
BoardInit(void)
{
// In case of TI-RTOS vector table is initialize by OS itself
#ifndef USE_TIRTOS
    //
    // Set vector table base
    //
#if defined(ccs) || defined(gcc)
    MAP_IntVTableBaseSet((unsigned long)&g_pfnVectors[0]);
#endif
#if defined(ewarm)
    MAP_IntVTableBaseSet((unsigned long)&__vector_table);
#endif
#endif //USE_TIRTOS

    //
    // Enable Processor
    //
    MAP_IntMasterEnable();
    MAP_IntEnable(FAULT_SYSTICK);

    PRCMCC3200MCUInit();
}


//*****************************************************************************
//                            MAIN FUNCTION
//*****************************************************************************
void main()
{
    long lRetVal = -1;
    unsigned long ulResetCause;

    //
    // Board Initialization
    //
    BoardInit();
    
    UDMAInit(); 
    
    //
    // configure the GPIO pins for LEDs,UART
    //
    PinMuxConfig();

    //
    // Configure the UART
    //
#ifndef NOTERM
    InitTerm();
#endif  //NOTERM

#if 1
    WDT_IF_Init(NULL,80000000 * 40);

  //
    // Get the reset cause
    //
    ulResetCause = PRCMSysResetCauseGet();

   //
    // If watchdog triggered reset request hibernate
    // to clean boot the system
    //
    if( ulResetCause == PRCM_WDT_RESET )
    {
        HIBEntrePreamble();
        MAP_PRCMOCRRegisterWrite(0,1);
        MAP_PRCMHibernateWakeupSourceEnable(PRCM_HIB_SLOW_CLK_CTR);
        MAP_PRCMHibernateIntervalSet(330);
        MAP_PRCMHibernateEnter();
    }

    if( ulResetCause == PRCM_HIB_EXIT &&  (MAP_PRCMOCRRegisterRead(0) & 1) == 1 )
    {
        UART_PRINT("Reset Cause        : Watchdog Reset\n\r");
    }
    else
    {
        UART_PRINT("Reset Cause        : Power On\n\r");
    }

#endif
    //
    // Display Application Banner
    //
    DisplayBanner(APPLICATION_NAME);
    
    //
    // Configure all 3 LEDs
    //
    GPIO_IF_LedConfigure(LED1|LED2|LED3);
    

    // switch off all LEDs
    GPIO_IF_LedOff(MCU_ALL_LED_IND);

	//
    // SysTick Enabling
    //
    //SysTickIntRegister(SysTickHandler);
    ///SysTickPeriodSet(SYSTICK_RELOAD_VALUE);
    //SysTickEnable();

    //
    // Base address for first timer
    //
    g_ulBase = TIMERA0_BASE;
    //
    // Configuring the timers
    //
    Timer_IF_Init(PRCM_TIMERA0, g_ulBase, TIMER_CFG_PERIODIC, TIMER_A, 0);

    //
    // Setup the interrupts for the timer timeouts.
    //
    Timer_IF_IntSetup(g_ulBase, TIMER_A, TimerBaseIntHandler);

    //
    // Turn on the timers feeding values in mSec
    //
    Timer_IF_Start(g_ulBase, TIMER_A, 1000);

    Button_IF_Init(SW2InterruptHandler,SW3InterruptHandler);
    Button_IF_EnableInterrupt(SW2);
    Button_IF_EnableInterrupt(SW3);
     switchSmartlink = 0;
     setDnsFlag(0);
     //setselBlockFlag(FALSE);
	
    //
    // Start the SimpleLink Host
    //
    lRetVal = VStartSimpleLinkSpawnTask(SPAWN_TASK_PRIORITY);
    if(lRetVal < 0)
    {
        ERR_PRINT(lRetVal);
        LOOP_FOREVER();
    }

    //
    // Start the WlanStationMode task
    //
    lRetVal = osi_TaskCreate( WlanStationMode, \
                                (const signed char*)"Wlan Station Task", \
                                OSI_STACK_SIZE, NULL, 1, NULL );
    if(lRetVal < 0)
    {
        ERR_PRINT(lRetVal);
        LOOP_FOREVER();
    }

     //
    // Start the dns parsing task
    //
    #if 0
    lRetVal = osi_TaskCreate( DnsParsingTick, \
                            (const signed char*)"dns parsing task", \
                            1024, NULL, 3, dnsTaskHandle );
    if(lRetVal < 0)
    {
        ERR_PRINT(lRetVal);
        LOOP_FOREVER();
    }	
   #endif

    //
    // Start the task scheduler
    //
    osi_start();
  }

//######################
void DnsParsingTick( void *pvParameters)
{
    int8 tmpip[32] = {0},failed=0,ret=0;
    uint32 preTime;
    static uint32 nowTime = 0;
    uint32 dTime;
	
    int8 taskData[DID_LEN+PASSCODE_LEN+1];
    int8* pTaskData = NULL;
    int8*   pName = NULL;
    int8*   pPasscode = NULL;
	
    memset(taskData, 0, sizeof(taskData));
    pName = &taskData[0];
    pPasscode = &taskData[DID_LEN];

    if(NULL != pvParameters)
    {
    pTaskData = (int8*)pvParameters; 
    memcpy(pName, (pTaskData), DID_LEN-2); 
    memcpy(pPasscode, (pTaskData+DID_LEN), PASSCODE_LEN); 
    pTaskData[DID_LEN]= '0';
    pTaskData[DID_LEN+PASSCODE_LEN]= '0';
    GAgent_Printf( GAGENT_CRITICAL,"pTaskData=%s, pName=%s,nameLen=%d, pPasscode=%s, passcodeLen=%d !!!\r\n", pTaskData, pName, strlen( pName ), pPasscode, strlen( pPasscode ));
    }
  #if 1
     while(1)
    {

	 if(4 == getDnsFlag())
        {
          if( (0==memcmp(pName, RECONNHTTPGLAG1, strlen(RECONNHTTPGLAG1))) && (0==memcmp(pPasscode, RECONNHTTPGLAG2, strlen(RECONNHTTPGLAG2))))
          	{
          		GAgent_Printf( GAGENT_INFO," Cloud_ReqProvision!!! ");
			Cloud_ReqProvision(pgContextData);
		}
         else
         	{
           	GAgent_Printf( GAGENT_INFO," Cloud_ReqConnect!!! ");
            	Cloud_ReqConnect( pgContextData,pName, pPasscode);
         	}
	      osi_Sleep(300);
            continue;
        }
  
        if(3 == getDnsFlag())
        {
           GAgent_Printf( GAGENT_INFO," DnsFlag =3, don't need to parsing!!! ");
            osi_Sleep(200);
            continue;
        }

	 if(1 != getDnsFlag())
	 {
        ret = GAgent_GetHostByName(HTTP_SERVER, tmpip);
        if( ret!=0 )
        {
            GAgent_Printf( GAGENT_ERROR,"HTTP_SERVER parsing error!line %d",__LINE__ );
            failed=1;
        }
        else
        {
            GAgent_Printf( GAGENT_DEBUG," %s : %s, gc.Gserverip=%s \n",HTTP_SERVER,tmpip, (pgContextData->gc.GServer_ip));
	      GAgent_Printf( GAGENT_DEBUG,"taskData[0]=%s, taskData[1]=%s", (taskData[0]), (taskData[1]));
            if(strcmp( pgContextData->gc.GServer_ip, tmpip) != 0)
            {
                 GAgent_Printf( GAGENT_DEBUG,"Save GService ip into flash!");
                strcpy(pgContextData->gc.GServer_ip, tmpip );
                GAgent_DevSaveConfigData( &(pgContextData->gc) );
                
            }
            failed=0;
        }
	}

       setDnsFlag(1);

      if(strlen(pgContextData->minfo.m2m_SERVER) == 0)
        {
        #if 1  
           GAgent_Printf(GAGENT_DEBUG," m2m server is NULL!");
            //not yet m2m host
            failed = 1;
            //osi_Sleep(1000); //Nik.chen for test
            sleep(1);
        #else
        #define tmpiptest "m2m1.iotsdk.com"
        strcpy( pgContextData->gc.m2m_ip,tmpiptest );
        GAgent_DevSaveConfigData( &(pgContextData->gc) );
        GAgent_DevGetConfigData( &(pgContextData->gc) );
        failed = 0;
        #endif
        }
        else
        {
            ret = GAgent_GetHostByName( pgContextData->minfo.m2m_SERVER,tmpip );
            if( ret!=0)
            {
                GAgent_Printf( GAGENT_DEBUG,"line %d",__LINE__ );
                failed = 1;
            }
            else
            {
                GAgent_Printf( GAGENT_DEBUG,"got %s : %s",pgContextData->minfo.m2m_SERVER,tmpip);
                if( 0!=strcmp( pgContextData->gc.m2m_ip,tmpip) )
                {
                    strcpy( pgContextData->gc.m2m_ip,tmpip );
                    GAgent_DevSaveConfigData( &(pgContextData->gc) );
                    GAgent_DevGetConfigData( &(pgContextData->gc) );
                }
               failed=0;
            }
            
        }

        #if 1
        if( 1==failed )
        {
            if( ((pgContextData->rtinfo.GAgentStatus)&WIFI_MODE_TEST) == WIFI_MODE_TEST )
            {
                pgContextData->rtinfo.waninfo.RefreshIPTime =  1*10;
            }
            else
            {

                pgContextData->rtinfo.waninfo.RefreshIPTime = 1; 
            }
        }
        else
        {
            pgContextData->rtinfo.waninfo.RefreshIPTime = ONE_HOUR; //Nik.chen 1 -> ONE_HOUR
            GAgent_Printf( GAGENT_DEBUG,"RefreshIPTime=%ld, line %d", (pgContextData->rtinfo.waninfo.RefreshIPTime),__LINE__ );
            setDnsFlag(2);
             sleep(1);
            // break;
        }
        #endif

        GAgent_Printf( GAGENT_DEBUG,"setDnsFlag, flag=%d, line %d",getDnsFlag(), __LINE__ );
        //setDnsFlag(FALSE);
     }
     // sleep(1);
    #endif
}

void setDnsFlag(unsigned char flag)
{
     dnsflag = flag;
}
unsigned char getDnsFlag(void)
{
     //GAgent_Printf( GAGENT_DEBUG,"getDnsFlag, enter" );
    return dnsflag;
}

void CC3200_DNSTaskDelete(void)
{

    if(dnsTaskHandle)
    {
        osi_TaskDelete(&dnsTaskHandle);
        dnsTaskHandle = NULL;
        GAgent_Printf( GAGENT_DEBUG,"osi_TaskDelete enter!!! " );
    }
}

unsigned char CC3200_CreateDnsTask(void* para)
{
    long lRetVal = -1;

    if(dnsTaskHandle)
        return 0;

    lRetVal = osi_TaskCreate( DnsParsingTick, \
                            (const signed char*)"dns parsing task", \
                            1024, para, 3, &dnsTaskHandle );
    if(lRetVal < 0)
    {
        GAgent_Printf( GAGENT_DEBUG,"CC3200_CreateDnsTask failed!!! " );
        return lRetVal;
    }
    return 0;
}

unsigned char setStaMode(void)
{
    SlVersionFull   ver = {0};
    _WlanRxFilterOperationCommandBuff_t  RxFilterIdMask = {0};

    unsigned char ucVal = 1;
    unsigned char ucConfigOpt = 0;
    unsigned char ucConfigLen = 0;
    unsigned char ucPower = 0;

    long lRetVal = -1;
    long lMode = -1;

    lMode = sl_Start(0, 0, 0);
    ASSERT_ON_ERROR(lMode);

    // If the device is not in station-mode, try configuring it in station-mode 
    if (ROLE_STA != lMode)
    {
        if (ROLE_AP == lMode)
        {
            // If the device is in AP mode, we need to wait for this event 
            // before doing anything 
            while(!IS_IP_ACQUIRED(g_ulStatus))
            {
#ifndef SL_PLATFORM_MULTI_THREADED
              _SlNonOsMainLoopTask(); 
#endif
            }
        }

        // Switch to STA role and restart 
        lRetVal = sl_WlanSetMode(ROLE_STA);
        ASSERT_ON_ERROR(lRetVal);

        lRetVal = sl_Stop(0xFF);
        ASSERT_ON_ERROR(lRetVal);

        lRetVal = sl_Start(0, 0, 0);
        ASSERT_ON_ERROR(lRetVal);

        // Check if the device is in station again 
        if (ROLE_STA != lRetVal)
        {
            // We don't want to proceed if the device is not coming up in STA-mode 
            ASSERT_ON_ERROR(DEVICE_NOT_IN_STATION_MODE);
        }
    }
    
    // Get the device's version-information
    ucConfigOpt = SL_DEVICE_GENERAL_VERSION;
    ucConfigLen = sizeof(ver);
    lRetVal = sl_DevGet(SL_DEVICE_GENERAL_CONFIGURATION, &ucConfigOpt, 
                                &ucConfigLen, (unsigned char *)(&ver));
    ASSERT_ON_ERROR(lRetVal);
    
    UART_PRINT("Host Driver Version: %s\n\r",SL_DRIVER_VERSION);
    UART_PRINT("Build Version %d.%d.%d.%d.31.%d.%d.%d.%d.%d.%d.%d.%d\n\r",
    ver.NwpVersion[0],ver.NwpVersion[1],ver.NwpVersion[2],ver.NwpVersion[3],
    ver.ChipFwAndPhyVersion.FwVersion[0],ver.ChipFwAndPhyVersion.FwVersion[1],
    ver.ChipFwAndPhyVersion.FwVersion[2],ver.ChipFwAndPhyVersion.FwVersion[3],
    ver.ChipFwAndPhyVersion.PhyVersion[0],ver.ChipFwAndPhyVersion.PhyVersion[1],
    ver.ChipFwAndPhyVersion.PhyVersion[2],ver.ChipFwAndPhyVersion.PhyVersion[3]);

     lRetVal = sl_WlanDisconnect();
    if(0 == lRetVal)
    {
        // Wait
        while(IS_CONNECTED(g_ulStatus))
        {
       #ifndef SL_PLATFORM_MULTI_THREADED
               _SlNonOsMainLoopTask(); 
        #endif
        }
    }

    CLR_STATUS_BIT_ALL(g_ulStatus);

      // Set connection policy to Auto 
    //      (Device's default connection policy)
    lRetVal = sl_WlanPolicySet(SL_POLICY_CONNECTION, 
                                SL_CONNECTION_POLICY(1, 0, 0, 0, 0), NULL, 0);
    ASSERT_ON_ERROR(lRetVal);
    
        lRetVal = sl_Stop(0xFF);
        ASSERT_ON_ERROR(lRetVal);

        lRetVal = sl_Start(0, 0, 0);
        ASSERT_ON_ERROR(lRetVal);

        // Check if the device is in station again 
        if (ROLE_STA != lRetVal)
        {
            // We don't want to proceed if the device is not coming up in STA-mode 
            ASSERT_ON_ERROR(DEVICE_NOT_IN_STATION_MODE);
        }
#if 0
    // Enable DHCP client
    lRetVal = sl_NetCfgSet(SL_IPV4_STA_P2P_CL_DHCP_ENABLE,1,1,&ucVal);
    ASSERT_ON_ERROR(lRetVal);

    // Disable scan
    ucConfigOpt = SL_SCAN_POLICY(0);
    lRetVal = sl_WlanPolicySet(SL_POLICY_SCAN , ucConfigOpt, NULL, 0);
    ASSERT_ON_ERROR(lRetVal);

    // Set Tx power level for station mode
    // Number between 0-15, as dB offset from max power - 0 will set max power
    ucPower = 0;
    lRetVal = sl_WlanSet(SL_WLAN_CFG_GENERAL_PARAM_ID, 
            WLAN_GENERAL_PARAM_OPT_STA_TX_POWER, 1, (unsigned char *)&ucPower);
    ASSERT_ON_ERROR(lRetVal);

    // Set PM policy to normal
    lRetVal = sl_WlanPolicySet(SL_POLICY_PM , SL_NORMAL_POLICY, NULL, 0);
    ASSERT_ON_ERROR(lRetVal);

    // Unregister mDNS services
    lRetVal = sl_NetAppMDNSUnRegisterService(0, 0);
    ASSERT_ON_ERROR(lRetVal);

    // Remove  all 64 filters (8*8)
    memset(RxFilterIdMask.FilterIdMask, 0xFF, 8);
    lRetVal = sl_WlanRxFilterSet(SL_REMOVE_RX_FILTER, (_u8 *)&RxFilterIdMask,
                       sizeof(_WlanRxFilterOperationCommandBuff_t));
    ASSERT_ON_ERROR(lRetVal);

    lRetVal = sl_Start(0, 0, 0);
    ASSERT_ON_ERROR(lRetVal);

    // Check if the device is in station again 
    if (ROLE_STA != lRetVal)
    {
        // We don't want to proceed if the device is not coming up in STA-mode 
        ASSERT_ON_ERROR(DEVICE_NOT_IN_STATION_MODE);
    }
    
    return lRetVal; // Success
#endif
        
    return 0;
}
//######################
