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
// Application Name     -   AirKissDemo Application
// Application Overview -   
//
//*****************************************************************************


//*****************************************************************************
//
//! \addtogroup AirKissDemo
//! @{
//
//*****************************************************************************

#include <stdio.h>

// Simplelink includes
#include "simplelink.h"

#include "rom_map.h"
#include "utils.h"

#include "common.h"

#ifndef NOTERM
#include "uart_if.h"
#endif

#define MAX_RECV_BUF_SIZE   1536
 
#define PACKET_NUMBER_FOR_EACH_CHANNEL 200

/*************************************** source code for full scan start*************************************/

typedef struct
{
    _u8     ssid[MAXIMAL_SSID_LENGTH]   ;
    _u8     ssid_len                    ;
    _u8     ExtendedSecurityType        ;
    _u8     ExtendedSecurityType_Wpa    ;
    _u8     ExtendedSecurityType_Wpa2   ;
    _u8     bssid[SL_BSSID_LENGTH]      ;
    _i8     rssi                        ;
    _i16     channel                     ;
	_i8     is_ess                      ;

} ExtendedScanResult_t ;

static _u8              gThisChipMacAddress[SL_MAC_ADDR_LEN] ;

#define RSSI_THRESHOLD_FOR_SCANS           -127
/*---------------------------------------------------------------------------*/
#define WLAN_MSG_TYPE_CONTROL               0x00
#define WLAN_MSG_SUBTYPE_PROB_RESPONSE      0x50
#define WLAN_MSG_SUBTYPE_BEACON             0x80
/*************************************** source code for full scan end*************************************/

static _u8              gThisChipMacAddress[SL_MAC_ADDR_LEN] ;

//*****************************************************************************
//                 GLOBAL VARIABLES -- Start
//*****************************************************************************

//*****************************************************************************
//                 GLOBAL VARIABLES -- End
//*****************************************************************************


//****************************************************************************
//                      LOCAL FUNCTION PROTOTYPES
//****************************************************************************

/*************************************** source code for full scan start*************************************/
static  int   AddBeaconFilter (SlrxFilterIdMask_t   FiltersIdMask)
{
	SlrxFilterRuleType_t   RuleType      ;
	SlrxFilterID_t         FilterId = 0  ;
	SlrxFilterFlags_t      FilterFlags   ;
	SlrxFilterRule_t       Rule          ;
	SlrxFilterTrigger_t    Trigger       ;
	SlrxFilterAction_t     Action        ;
	char                   RetVal = -1   ;	
	_u8                    FrameType     ;
	_u8                    FrameTypeMask ;
    int                    i             ;


    // check if filter already added
    for (i = 0  ;  i < sizeof(FiltersIdMask)  ;  i++)
    {
        if (0 != FiltersIdMask[i])
        {
            /***************************/
            /* Filter already created  */
            /***************************/
            return (0) ;
        }
    }

	/*
	 * First Filter is:
	 * if FrameType != Management --> Action == Drop
	 */

	/* Build filter parameters */
	RuleType = HEADER;
	FilterFlags.IntRepresentation = RX_FILTER_BINARY;
	FrameType     = 0; //0=Management
	FrameTypeMask = 0xFF;
	Rule.HeaderType.RuleHeaderfield = FRAME_TYPE_FIELD;	
	memcpy(Rule.HeaderType.RuleHeaderArgsAndMask.RuleHeaderArgs.RxFilterDB1BytesRuleArgs[0],&FrameType, 1); // Filter packets with destination IP address that ends with 255, (i.e. x.x.x.255)
	memcpy(Rule.HeaderType.RuleHeaderArgsAndMask.RuleHeaderArgsMask,&FrameTypeMask, 1);
	Rule.HeaderType.RuleCompareFunc = COMPARE_FUNC_NOT_EQUAL_TO;
	Trigger.ParentFilterID = 0;
	Trigger.Trigger = NO_TRIGGER;
	Trigger.TriggerArgConnectionState.IntRepresentation = RX_FILTER_CONNECTION_STATE_STA_NOT_CONNECTED;
	Trigger.TriggerArgRoleStatus.IntRepresentation = RX_FILTER_ROLE_PROMISCUOUS;
	Action.ActionType.IntRepresentation = RX_FILTER_ACTION_DROP;


	/* Add first Filter */
	RetVal = (char)sl_WlanRxFilterAdd(RuleType,FilterFlags,&Rule,&Trigger,&Action,&FilterId);
	if(RetVal != 0)
	{
		return -1;
	}

	SETBIT8(FiltersIdMask,FilterId);

	/*
	 * Second Filter is:
	 * if FrameType == Management --> Action == Continue
	 */
	Rule.HeaderType.RuleCompareFunc = COMPARE_FUNC_EQUAL;
	Action.ActionType.IntRepresentation = RX_FILTER_ACTION_NULL;

	/* Add second Filter */
	RetVal = (char)sl_WlanRxFilterAdd(RuleType,FilterFlags,&Rule,&Trigger,&Action,&FilterId);
	if(RetVal != 0)
	{
		return -1;
	}

	SETBIT8(FiltersIdMask,FilterId);

	/*
	 * Third Filter is:
	 * if Frame SubType != Beacon --> Action == Continue
	 */
	Trigger.ParentFilterID = FilterId;//The parent Id
	Rule.HeaderType.RuleCompareFunc = COMPARE_FUNC_NOT_EQUAL_TO;
	Action.ActionType.IntRepresentation = RX_FILTER_ACTION_NULL;
	FrameType = 0x80;//Beacon Frame Type
	Rule.HeaderType.RuleHeaderfield = FRAME_SUBTYPE_FIELD;
	memcpy(Rule.HeaderType.RuleHeaderArgsAndMask.RuleHeaderArgs.RxFilterDB1BytesRuleArgs[0],&FrameType, 1); // Filter packets with destination IP address that ends with 255, (i.e. x.x.x.255)

	/* Add third Filter */
	RetVal = (char)sl_WlanRxFilterAdd(RuleType,FilterFlags,&Rule,&Trigger,&Action,&FilterId);
	if(RetVal != 0)
	{
		return -1;
	}

	SETBIT8(FiltersIdMask,FilterId);


	/*
	 * Fourth Filter is:
	 * if Frame SubType != Probe Response --> Action == Drop
	 */
	Trigger.ParentFilterID = FilterId;//The parent Id
	Rule.HeaderType.RuleCompareFunc = COMPARE_FUNC_NOT_EQUAL_TO;
	Action.ActionType.IntRepresentation = RX_FILTER_ACTION_DROP;
	FrameType = 0x50;//Probe Response Frame Type
	Rule.HeaderType.RuleHeaderfield = FRAME_SUBTYPE_FIELD;
	memcpy(Rule.HeaderType.RuleHeaderArgsAndMask.RuleHeaderArgs.RxFilterDB1BytesRuleArgs[0],&FrameType, 1); // Filter packets with destination IP address that ends with 255, (i.e. x.x.x.255)

	/* Add third Filter */
	RetVal = (char)sl_WlanRxFilterAdd(RuleType,FilterFlags,&Rule,&Trigger,&Action,&FilterId);
	if(RetVal != 0)
	{
		return -1;
	}

	SETBIT8(FiltersIdMask,FilterId);

	return 1;
}

static int    EnableBeaconFilter (void)
{
    static SlrxFilterIdMask_t           BeaconFiltersIdMask = {0} ;
	_WlanRxFilterOperationCommandBuff_t RxFilterIdMask            ;	
    int                                 sl_rc                     ;


	if (-1 == AddBeaconFilter (BeaconFiltersIdMask))
	{
		UART_PRINT ("EnableBeaconFilter() failed to add.\n") ;

        return (-1) ;
	}

	memcpy (RxFilterIdMask.FilterIdMask , BeaconFiltersIdMask , sizeof (BeaconFiltersIdMask)) ;
	
	sl_rc = sl_WlanRxFilterSet (SL_ENABLE_DISABLE_RX_FILTER , (unsigned char *)&RxFilterIdMask , sizeof(RxFilterIdMask)) ;

    if (sl_rc < 0)
    {
		UART_PRINT ("EnableBeaconFilter() failed to set - error %d.\n" , sl_rc) ;

        return (-1) ;
    }

    return (0) ;
}

static void    DisableBeaconFilter (void)
{
	_WlanRxFilterOperationCommandBuff_t RxFilterIdMask;	


	//Disable the Beacon filters
	memset(RxFilterIdMask.FilterIdMask, 0, sizeof(RxFilterIdMask.FilterIdMask)) ;
	sl_WlanRxFilterSet (SL_ENABLE_DISABLE_RX_FILTER , (unsigned char *)&RxFilterIdMask , sizeof(RxFilterIdMask)) ;
}

static int   SendProbReqest ( _i16 sd)
{
    static const char   WLAN_PROB_REQ_SUPPORTED_RATES_ELEMENT[] = {0x01 , 0x05 , 0x82 , 0x84 , 0x8B , 0x96 , 0x2C }; // 1 , 2 , 5.5 , 11 , 22
    static const char   WLAN_PROB_REQ_EXT_SUPPORTED_RATES_ELEMENT[] = {0x32 , 0x08 , 0x0C , 0x12 , 0x18 , 0x24 , 0x30 , 0x48 , 0x60 , 0x6C }; // 6 , 7 , 12 , 18 , 24 , 36 , 48 , 54
    static char         LastSeq = 1                        ;
    char Prob[100] = {0};
    int Size;

    // MAC Header
    Prob[0] = 0x40;                     // Version = 0 , Type = 0 (Managment) , Subtype = 4 (Prob Req)
    Prob[2] = 0x30;                     // Duration
    Prob[3] = 0x2;                     // Duration
    memset(&Prob[4],0xff,6);             // Destination Address
    memcpy(&Prob[10],&gThisChipMacAddress,6);  // Source Address
    memset(&Prob[16],0xff,6);            // BSSID
    Prob[23] = LastSeq++;

    // Prob Request
    Size = sizeof(WLAN_PROB_REQ_SUPPORTED_RATES_ELEMENT);
    memcpy(&Prob[26],WLAN_PROB_REQ_SUPPORTED_RATES_ELEMENT,Size);
    memcpy(&Prob[26 + Size],WLAN_PROB_REQ_EXT_SUPPORTED_RATES_ELEMENT,sizeof(WLAN_PROB_REQ_EXT_SUPPORTED_RATES_ELEMENT));

    Size += 26 + sizeof(WLAN_PROB_REQ_EXT_SUPPORTED_RATES_ELEMENT);

    Size = sl_Send(sd,Prob,Size,SL_RAW_RF_TX_PARAMS(0,1,0,0));

    if (Size > 0)
    {
        return 0;
    }
    else
    {
        return Size;
    }
}

static void   ParseAndAddResult (unsigned char *    pBuf                   ,
                                 int                Len                    ,
                                 const char   scan_channel_order[], int channels_count, int gApScanRssi[])
{
    SlTransceiverRxOverHead_t * pRxFrame       = (SlTransceiverRxOverHead_t *)pBuf          ;
    const unsigned char *       beacon         = pBuf + sizeof(SlTransceiverRxOverHead_t)   ;
    ExtendedScanResult_t        scan_result    = {{0}}                  ;
    const unsigned char *       pBssid         = &beacon[16]            ;
    unsigned char               MsgType        =  beacon[ 0]  &  0x0C   ;
    unsigned char               MsgSubType     =  beacon[ 0]  &  0xF0   ;
    int                         i                                       ;


    if ((MsgType != WLAN_MSG_TYPE_CONTROL) ||
        ((MsgSubType != WLAN_MSG_SUBTYPE_PROB_RESPONSE) && (MsgSubType != WLAN_MSG_SUBTYPE_BEACON)))
    {
        /*******************************/
        /* Ignore non-beacon messages  */
        /*******************************/
        return ;
    }

    if (pRxFrame->rssi < RSSI_THRESHOLD_FOR_SCANS)
    {
        /********************************************/
        /* Ignore APs that are too weakly received. */
        /********************************************/
        return ;
    }

    memcpy (scan_result.bssid , pBssid , 6);

    scan_result.rssi    = pRxFrame->rssi;
    scan_result.channel = pRxFrame->channel ;
    
    //UART_PRINT("Channel:%d and RSSI is:%d\r\n", scan_result.channel, scan_result.rssi);
    
    for(i = 0; i < channels_count; i ++)
    {
        //UART_PRINT("XXXChannel:%d and RSSI is:%d\r\n", scan_channel_order[i], gApScanRssi[i]);
        if(scan_result.channel == scan_channel_order[i] && scan_result.rssi > gApScanRssi[i])
        {
            gApScanRssi[i] = scan_result.rssi;
        }
    }
}


static int    PerformExtendedScan (const char       scan_channel_order[]   ,
                                   int              channels_count, int ApScanRssi[])

{
	#define BUFF_SIZE					(2048)

	unsigned char       buffer[BUFF_SIZE] = {0} ;
	_i16                SocketId                ; 	
	int                 Count                   ;
	_i16                Status                  ;
    SlSockNonblocking_t NonBlocking             ;
    int                 ChannelIndex            ;
    int                 Repeat                  ;
    _u8                 macAddressLen = SL_MAC_ADDR_LEN ;

    sl_NetCfgGet (SL_MAC_ADDRESS_GET , NULL , &macAddressLen , gThisChipMacAddress) ;
	
	for (ChannelIndex = 0  ;  ChannelIndex < channels_count  ; ChannelIndex++)
	{
		Count = 0;

		SocketId = sl_Socket (SL_AF_RF , SL_SOCK_RAW , scan_channel_order[ChannelIndex]) ;

		if (SocketId < 0)
		{
			UART_PRINT ("RAW Socket Open Error with status=%d\n" , SocketId) ;
                        
                        MAP_UtilsDelay(20000);
                        
                        SocketId = sl_Socket (SL_AF_RF , SL_SOCK_RAW , scan_channel_order[ChannelIndex]) ;
                        
                        if (SocketId < 0)
                              return (SocketId) ;
		}

		//Configure the socket to non blocking mode
        NonBlocking.NonblockingEnabled = 1 ;

		sl_SetSockOpt (SocketId , SL_SOL_SOCKET , SL_SO_NONBLOCKING , &NonBlocking , sizeof(NonBlocking)) ;
		
        for (Repeat = 0  ;  Repeat < 3  ;  Repeat++)
        {
            SendProbReqest (SocketId) ;

            MAP_UtilsDelay(8000);

		    while (Count < 200)
		    {
			    Status = sl_Recv (SocketId , buffer , BUFF_SIZE , 0) ;

			    if (Status > 0)
			    {
                    ParseAndAddResult (buffer                            ,
                                       Status                            ,
                                       scan_channel_order, channels_count, ApScanRssi) ;

			    }
		
			    Count++;
		    }
        }

		sl_Close (SocketId) ; //Close the RAW socket in order to change the to radio tune to next channel
        MAP_UtilsDelay(25000);
	}

        return 0;
}

void SortArray(int ApScanRssi[], int AirKissChannelOrder[], int          channels_count)
{
     int temp;
     int i, j;

     for(i = 0; i < channels_count; i ++)
     {
          for(j = i + 1; j < channels_count; j++)
          {
                if(ApScanRssi[j] > ApScanRssi[i])
                {
                    temp = ApScanRssi[j];
                    ApScanRssi[j] = ApScanRssi[i];
                    ApScanRssi[i] = temp;
                    
                    temp = AirKissChannelOrder[j];
                    AirKissChannelOrder[j] = AirKissChannelOrder[i];
                    AirKissChannelOrder[i] = temp;
                }
          }
     }	 
}

int     SimpleLinkFullScan (const char   scan_channel_order[]   ,
                                          int          channels_count, int ApScanRssi[], int AirKissChannelOrder[])

{
	EnableBeaconFilter ();

	PerformExtendedScan (scan_channel_order , channels_count, ApScanRssi) ;

	DisableBeaconFilter () ;

	SortArray(ApScanRssi, AirKissChannelOrder, channels_count);
      
    return 0;

}
/*************************************** source code for full scan end*************************************/

//*****************************************************************************
//
// Close the Doxygen group.
//! @}
//
//*****************************************************************************
