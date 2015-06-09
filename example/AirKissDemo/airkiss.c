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
#include "airkiss.h"

#ifndef NOTERM
#include "uart_if.h"
#endif

#define MAX_RECV_BUF_SIZE   1536
#define MAC_ADDR_LENGTH 6

#define PACKET_NUMBER_FOR_EACH_CHANNEL 200
#define FRAME_TYPE_MASK     0x0C
#define FRAME_SUBTYPE_MASK  0xF0

#define HEAD_BUFFER_SEQUENCE_MAX_SIZE 1000

#define HEAD_BUFFER_MAX_SIZE 100

#define SWAP_UINT32(val)    (((val>>24) & 0x000000FF) + ((val>>8) & 0x0000FF00) + \
                            ((val<<24) & 0xFF000000) + ((val<<8) & 0x00FF0000))
/*
typedef struct{
  _u8 ssid_crc;
  _u8 pswd_crc;
  _u8 total_len;
  _u8 ssid_len;
  _u8 pswd_len;
  _u8 precursor_flag;
  _u8 magic_flag;
  _u8 prefix_flag;
  _u8 ssid[33];
  _u8 pswd[33];
  _u8 rand_num;
  _u16 working_channel;
  _u8 bssid[6];
}_st_data_frame;
*/

static _u8 PacDeltaOffset;
static _u8 AirKissSrcMac[6];

//_st_data_frame data_frame;
_u8 buffer[MAX_RECV_BUF_SIZE] = {'\0'};
_u32  g_Status = 0;
_u8   g_Exit = 0;
_i16 sd = -1;

_u8 MAC[MAX_RECV_BUF_SIZE] = {'\0'};
_u8 sequence_matched[32] = {'\0'};

const airkiss_config_t akconf =
{
	(airkiss_memset_fn)&memset,
	(airkiss_memcpy_fn)&memcpy,
	(airkiss_memcmp_fn)&memcmp,
	0
};

airkiss_context_t akcontex;

airkiss_result_t result;

static _i32 DiscoverOnChannel(_i16 scan_channel);

/*!
    \brief This function creates filters based on rule specified by user

    \param[in]   Input : Filter chosen   
    \param[in]   FilterNumber : Chosen filter type ( Source MAC ID, Dst MAC ID,
                                BSS ID,  IP Address etc)
    \param[in]   Filterparams: parameters  of chosen filter type 
    \param[in]   Filter Rule:  Check for equal or Not
    \param[in]   Filter Rule : If Rule match, to drop packet or pass to Host
    \param[in]   parent Id : in case sub-filter of existing filer, id of the parent filter

    \return      Unique filter ID in long format for success, -ve otherwise

    \note

    \warning
*/
static _i32 RxFiltersExample(_i8 input, _i32 filterNumber,
                                const _u8 *filterParam, _i8 equalOrNot,
                                _i8 dropOrNot, _i8 parentId)
{
    SlrxFilterID_t          FilterId = 0;
    SlrxFilterRuleType_t    RuleType = 0;
    SlrxFilterFlags_t       FilterFlags = {0};
    SlrxFilterRule_t        Rule = {0};
    SlrxFilterTrigger_t     Trigger = {0};
    SlrxFilterAction_t      Action = {0};

    _u8 MacMAsk[6]      = {0xFF,0xFF,0xFF,0xFF,0xFF,0xFF};
    _u8 FrameMask[1]    = {0xFF};
    _u8 IPMask[4]       = {0xFF,0xFF,0xFF,0xFF};
    _u8 zeroMask[4]     = {0x00,0x00,0x00,0x00};

    _i32 retVal = -1;
    _u8 frameByte = 0;

    switch(input)
    {
        case '1': /* Create filter */
            /* Rule definition */
            RuleType = HEADER;
            FilterFlags.IntRepresentation = RX_FILTER_BINARY;
            /* When RX_FILTER_COUNTER7 is bigger than 0 */
            Trigger.Trigger = NO_TRIGGER;

            /* connection state and role */
            Trigger.TriggerArgConnectionState.IntRepresentation = RX_FILTER_CONNECTION_STATE_STA_NOT_CONNECTED;
            Trigger.TriggerArgRoleStatus.IntRepresentation = RX_FILTER_ROLE_PROMISCUOUS;

            switch (filterNumber)
            {
                case 1:
                    Rule.HeaderType.RuleHeaderfield = MAC_SRC_ADDRESS_FIELD;
                    memcpy(Rule.HeaderType.RuleHeaderArgsAndMask.RuleHeaderArgs.RxFilterDB6BytesRuleArgs[0],
                                                                       filterParam, 6);
                    memcpy(Rule.HeaderType.RuleHeaderArgsAndMask.RuleHeaderArgsMask,
                                                                           MacMAsk, 6);
                    break;
                case 2:
                    Rule.HeaderType.RuleHeaderfield = MAC_DST_ADDRESS_FIELD;
                    memcpy(Rule.HeaderType.RuleHeaderArgsAndMask.RuleHeaderArgs.RxFilterDB6BytesRuleArgs[0],
                                                                       filterParam, 6);
                    memcpy(Rule.HeaderType.RuleHeaderArgsAndMask.RuleHeaderArgsMask,
                                                                           MacMAsk, 6);
                    break;
                case 3:
                    Rule.HeaderType.RuleHeaderfield = BSSID_FIELD;
                    memcpy(Rule.HeaderType.RuleHeaderArgsAndMask.RuleHeaderArgs.RxFilterDB6BytesRuleArgs[0],
                                                                       filterParam, 6);
                    memcpy(Rule.HeaderType.RuleHeaderArgsAndMask.RuleHeaderArgsMask,
                                                                           MacMAsk, 6);
                    break;
                case 4:
                {
                    frameByte = (*filterParam & FRAME_TYPE_MASK);

                    Rule.HeaderType.RuleHeaderfield = FRAME_TYPE_FIELD;
                    memcpy(Rule.HeaderType.RuleHeaderArgsAndMask.RuleHeaderArgs.RxFilterDB1BytesRuleArgs[0],
                                                                       &frameByte, 1);
                    memcpy(Rule.HeaderType.RuleHeaderArgsAndMask.RuleHeaderArgsMask,
                                                                          FrameMask, 1);
                }
                break;
                case 5:
                {
                    if(parentId <=0 )
                    {
                        UART_PRINT("\n[Error] Enter a parent frame type filter id for frame subtype filter\r\n");
                        return -1;
                    }
                    
                    frameByte = (*filterParam & FRAME_SUBTYPE_MASK);

                    Rule.HeaderType.RuleHeaderfield = FRAME_SUBTYPE_FIELD;
                    memcpy(Rule.HeaderType.RuleHeaderArgsAndMask.RuleHeaderArgs.RxFilterDB1BytesRuleArgs[0],
                                                                       &frameByte, 1);
                    memcpy(Rule.HeaderType.RuleHeaderArgsAndMask.RuleHeaderArgsMask,
                                                                          FrameMask, 1);
                }
                    break;
                case 6:
                    Rule.HeaderType.RuleHeaderfield = IPV4_SRC_ADRRESS_FIELD;
                    memcpy(Rule.HeaderType.RuleHeaderArgsAndMask.RuleHeaderArgs.RxFilterDB4BytesRuleArgs[0],
                                                                       filterParam, 4);
                    memcpy(Rule.HeaderType.RuleHeaderArgsAndMask.RuleHeaderArgsMask,
                                                                            IPMask, 4);
                    break;
                case 7:
                    Rule.HeaderType.RuleHeaderfield = IPV4_DST_ADDRESS_FIELD;
                    memcpy(Rule.HeaderType.RuleHeaderArgsAndMask.RuleHeaderArgs.RxFilterDB4BytesRuleArgs[0],
                                                                       filterParam, 4);
                    memcpy(Rule.HeaderType.RuleHeaderArgsAndMask.RuleHeaderArgsMask,
                                                                            IPMask, 4);
                    break;
                case 8:
                    Rule.HeaderType.RuleHeaderfield = FRAME_LENGTH_FIELD;
                    memcpy(Rule.HeaderType.RuleHeaderArgsAndMask.RuleHeaderArgs.RxFilterDB4BytesRuleArgs[0],
                                                                          zeroMask, 4);
                    memcpy(Rule.HeaderType.RuleHeaderArgsAndMask.RuleHeaderArgsMask,
                                                                            IPMask, 4);
                    memcpy(Rule.HeaderType.RuleHeaderArgsAndMask.RuleHeaderArgs.RxFilterDB4BytesRuleArgs[1],
                                                                       filterParam, 4);
                    memcpy(Rule.HeaderType.RuleHeaderArgsAndMask.RuleHeaderArgsMask,
                                                                            IPMask, 4);
                    break;
            }

            switch(equalOrNot)
            {
                case 'y':
                    Rule.HeaderType.RuleCompareFunc = COMPARE_FUNC_EQUAL;
                    break;
                case 'h':
                    Rule.HeaderType.RuleCompareFunc = COMPARE_FUNC_NOT_IN_BETWEEN;
                    break;
                case 'l':
                    Rule.HeaderType.RuleCompareFunc = COMPARE_FUNC_IN_BETWEEN;
                    break;
                case 'n':
                    Rule.HeaderType.RuleCompareFunc = COMPARE_FUNC_NOT_EQUAL_TO;
                    break;
            }

            Trigger.ParentFilterID = parentId;

            /* Action */
            if(dropOrNot == 'y')
            {
                Action.ActionType.IntRepresentation = RX_FILTER_ACTION_DROP;
            }
            else
            {
                Action.ActionType.IntRepresentation = RX_FILTER_ACTION_NULL;
            }

            retVal = sl_WlanRxFilterAdd(RuleType,
                                            FilterFlags,
                                            &Rule,
                                            &Trigger,
                                            &Action,
                                            &FilterId);
            if( retVal < 0)
            {
                UART_PRINT("\nError creating the filter. Error number: %d.\n",retVal);
                ASSERT_ON_ERROR(retVal);
            }
            
            UART_PRINT("\r\nThe filter ID is %d\r\n",FilterId);
            break;

        case '2': /* remove filter */
        {
                _WlanRxFilterOperationCommandBuff_t     RxFilterIdMask ;
                memset(RxFilterIdMask.FilterIdMask, 0xFF , 8);
                retVal = sl_WlanRxFilterSet(SL_REMOVE_RX_FILTER, (_u8 *)&RxFilterIdMask,
                                    sizeof(_WlanRxFilterOperationCommandBuff_t));
                ASSERT_ON_ERROR(retVal);

        }
        break;

        case '3' : /* enable\disable filter */
        {
            _WlanRxFilterOperationCommandBuff_t     RxFilterIdMask ;
            memset(RxFilterIdMask.FilterIdMask, 0xFF , 8);
            retVal = sl_WlanRxFilterSet(SL_ENABLE_DISABLE_RX_FILTER, (_u8 *)&RxFilterIdMask,
                                sizeof(_WlanRxFilterOperationCommandBuff_t));
            ASSERT_ON_ERROR(retVal);
        }

        break;
    }

    return FilterId;
}


static int CreateFilters()
{
    _u8   filterData[MAC_ADDR_LENGTH] = {'\0'};
    _i32  retVal = -1;
    int i;

    filterData[0] = 0x8;
    retVal = RxFiltersExample('1', 4, filterData, 'n', 'y', 0);
    
    for(i = 0; i < MAC_ADDR_LENGTH; i ++)
		filterData[i] = 0xff;

    retVal = RxFiltersExample('1', 2, filterData, 'n', 'y', 0);

    retVal = RxFiltersExample('3',0,NULL,0,0,0);

    return retVal;
}

static _i32 DiscoverOnChannel(_i16 scan_channel)
{
    int pac_cnt = 0;
    int retVal, retVal2;
    int i = 0;
    int PacHeadBuffer[PACKET_NUMBER_FOR_EACH_CHANNEL];
    long lNonBlocking = 1;
	_u8 *buf_point;

#ifndef NOTERM    
    unsigned short tmp_num = 0;
#endif
    
    UART_PRINT("Listen on channel: %d\r\n", scan_channel);
	
    while(1)
    {
          sd = sl_Socket(SL_AF_RF, SL_SOCK_RAW, scan_channel);

          if(sd >= 0)
              break;
          else
          {
               UtilsDelay(2000);
          }
    }

    sl_SetSockOpt(sd, SL_SOL_SOCKET, SL_SO_NONBLOCKING, &lNonBlocking, sizeof(lNonBlocking));
    
    i = 0;
	
    for(pac_cnt = 0; pac_cnt < PACKET_NUMBER_FOR_EACH_CHANNEL; pac_cnt ++)
    {
         retVal = sl_Recv(sd,buffer,MAX_RECV_BUF_SIZE,0);

		//UART_PRINT("The length is:%d\r\n", retVal);
		
		if(retVal >= 12)
		{
			buf_point = buffer + 8;
			
			PacHeadBuffer[i] = retVal;	
#ifndef NOTERM  
			tmp_num = ((buffer[31] << 8)+buffer[30]) >> 4;
			//UART_PRINT("PRECURSOR; The length is:%d, i:%d, seq:%d, buf_point:%d\r\n", retVal, i, tmp_num, strlen(buf_point));
#endif                        

			retVal2 = airkiss_recv(&akcontex, buf_point, retVal - 8);
			
			if(retVal2 == AIRKISS_STATUS_CHANNEL_LOCKED)
			{
				PacDeltaOffset = PacHeadBuffer[i-3] - 1;

				memcpy(AirKissSrcMac,&buffer[24], 6);
					
				UART_PRINT("Got the precursor packets, SrcMac:%02x:%02x:%02x:%02x:%02x:%02x, i:%d, PacDeltaOffset:%d\r\n",
					AirKissSrcMac[0], AirKissSrcMac[1], AirKissSrcMac[2], AirKissSrcMac[3], 
					AirKissSrcMac[4], AirKissSrcMac[5], i, PacDeltaOffset);						

				return SUCCESS; 
			}

			i ++;
		}
	}

	return FAILURE;
	
}

static _i32 AirKissPrecursorProcess(int gAirKissChannelOrder[])
{
	int i;
	
	while(1)
	{
		for(i = 0; i < 5; i ++)
		{	
		
			if(gAirKissChannelOrder[i] == 0)
				continue;
			
			if(DiscoverOnChannel(gAirKissChannelOrder[i]) == SUCCESS)
			{
				UART_PRINT("Precursor is successfully done\r\n");
				return SUCCESS;
			}
			else 
				sl_Close(sd);
		}
		
	}
	
}

static _i32 AirKissMagicPrefixSequenceProcess(Sl_WlanNetworkEntry_t gNetEntries[])
{
	int retVal;
	int retVal2;
	_u8 *buf_point;

	int err;
		
	while(1) 
	{
		retVal = sl_Recv(sd,buffer,MAX_RECV_BUF_SIZE,0);

		if(retVal < PacDeltaOffset)
			continue;

		buf_point = buffer + 8;
		
		retVal2 = airkiss_recv(&akcontex, buf_point, retVal - 8);

		if(retVal2 == AIRKISS_STATUS_COMPLETE)
		{
			err = airkiss_get_result(&akcontex, &result);

			if (err == 0)
			{
					UART_PRINT("ssid = \"%s\", pwd = \"%s\", ssid_length = %d, pwd_length = %d, random = 0x%02x\r\n",
					result.ssid, result.pwd, result.ssid_length,
					result.pwd_length, result.random);

					return SUCCESS;
			}
			else
			{
					UART_PRINT("airkiss_get_result() failed !\r\n");
			}
		}

	}	
}

int AirKissProcess(int gAirKissChannelOrder[], Sl_WlanNetworkEntry_t gNetEntries[], unsigned char *rand_num, unsigned char *ssid, unsigned char *pwd)
{
      _i32 retVal = -1;
      _u8   filterData[MAC_ADDR_LENGTH] = {'\0'}; 

	  retVal = airkiss_init(&akcontex, &akconf);
		
	  if (retVal < 0)
	  {
			UART_PRINT("Airkiss init failed!\r\n");
			return FAILURE;
	  }

      *(_u32 *)filterData = SWAP_UINT32(80);
      RxFiltersExample('1', 8, filterData, 'l', 'y', 0);
	
      retVal = CreateFilters();
      ASSERT_ON_ERROR(retVal);

      AirKissPrecursorProcess(gAirKissChannelOrder);

      RxFiltersExample('2',0,NULL,0,0,0);
	
      /* Create NWP filter for MAGIC/PREFIX/SEQUENCE process */
      RxFiltersExample('1', 1, AirKissSrcMac, 'n', 'y', 0);
					 
      CreateFilters();

      retVal = AirKissMagicPrefixSequenceProcess(gNetEntries);

      if(retVal == SUCCESS)
      {
            UART_PRINT("Has received SSID and password over airkiss!\r\n");

      		retVal = sl_Close(sd);
      		ASSERT_ON_ERROR(retVal);

			*rand_num = result.random;
			memcpy(ssid, result.ssid, result.ssid_length);
			memcpy(pwd, result.pwd, result.pwd_length);
	  
      		return SUCCESS;
      }

	  retVal = sl_Close(sd);
      ASSERT_ON_ERROR(retVal);

	  return FAILURE;
	  
}


//*****************************************************************************
//
// Close the Doxygen group.
//! @}
//
//*****************************************************************************
