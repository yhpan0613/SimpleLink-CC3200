
#include "gagent.h"

#include "simplelink.h"
#include "osi.h"
#include "uart_if.h"

#include "common.h"
#include "timer_if.h"
#include "gpio_if.h"
#include "rom_map.h"

#include "hw_types.h"
#include "prcm.h"
#include "gpio_if.h"

#include "hw_memmap.h"
#include "wdt.h"
#include "wdt_if.h"



#define GAGENT_CONFIG_FILE "/tmp/gizwitsConfig"
#define GAGENT_HOSTPOTS  "/tmp/hostpots"
#define SH_GPIO_3   (3)  /* P58 - Device Mode */
#define AUTO_CONNECTION_TIMEOUT_COUNT   80      /* ? Sec */
#define SMARTCONFIG_TIMEOUT_COUNT   550      /* 60 Sec */

#define GPIO_LED2 10
static unsigned int LED2Port = 0;
static unsigned char LED2Pin;

extern unsigned long g_ulSeconds;
uint16 *halWiFiStatus=0;
static NetHostList_str gAplist;
extern unsigned long  g_ulStatus;
#define WLAN_SCAN_COUNT         30

Sl_WlanNetworkEntry_t netEntries[30]; // Wlan Network Entry

/* Application specific status/error codes */
typedef enum{
    // Choosing this number to avoid overlap w/ host-driver's error codes
    FILE_ALREADY_EXIST = -0x7D0,
    FILE_CLOSE_ERROR = FILE_ALREADY_EXIST - 1,
    FILE_NOT_MATCHED = FILE_CLOSE_ERROR - 1,
    FILE_OPEN_READ_FAILED = FILE_NOT_MATCHED - 1,
    FILE_OPEN_WRITE_FAILED = FILE_OPEN_READ_FAILED -1,
    FILE_READ_FAILED = FILE_OPEN_WRITE_FAILED - 1,
    FILE_WRITE_FAILED = FILE_READ_FAILED - 1,

    DEVICE_NOT_IN_STATION_MODE = FILE_WRITE_FAILED - 1,

    STATUS_CODE_MAX = -0xBB8
}e_AppStatusCodes;

static int8 g_ulMode = 0; // 1:STA 2:AP 3:P2P
static char selectUartFlag = 0; //0:no 1:select
static int8 connectFlag = 0; 
extern unsigned int setApMode;
static int8 airlinkTimes = 0; 

int32  wlanscan(void);
long testSwitchStaMode(void);
static long CC3200_ConfigtoSmartconfig();
static int SmartConfigConnect();

void setMode(int8 mode)
{
    g_ulMode = mode;
}
char getMode()
{
     return g_ulMode;
}

void setselFlag(char flag)
{
     if(1 == flag)
        selectUartFlag = 1;
     else
        selectUartFlag = 0;
}
char getselFlag()
{
     return selectUartFlag;
}

void setconnFlag(int8 flag)
{
    connectFlag = flag;
}
int8 getconnFlag(void)
{
     return connectFlag;
}
#if 1
void setAirlinkTimes(int8 airTimes)
{
    airlinkTimes = airTimes;
}
int8 getAirlinkTimes(void)
{
     return airlinkTimes;
}
#endif
void msleep(int m_seconds)
{ 
    osi_Sleep(m_seconds);
}

void sleep(int seconds)
{ 
    osi_Sleep(seconds*1000);
}

void mdelay(int m_seconds)
{ 
    MAP_UtilsDelay(m_seconds);
}

uint32 GAgent_GetHostByName( int8 *domain, int8 *IPAddress)
{
    long lStatus = 0;
	unsigned long pDestinationIP;
		
    lStatus = sl_NetAppDnsGetHostByName((signed char *) domain,
                                            strlen(domain),
                                            &pDestinationIP, SL_AF_INET);
    if(lStatus < 0)
		return lStatus;

	sprintf(IPAddress, "%d.%d.%d.%d", 
					SL_IPV4_BYTE(pDestinationIP,3),
                    SL_IPV4_BYTE(pDestinationIP,2),
                    SL_IPV4_BYTE(pDestinationIP,1),
                    SL_IPV4_BYTE(pDestinationIP,0));
	
    return lStatus;
	
}

int Gagent_setsocketnonblock(int socketfd)
{
    	long nonBlocking = 1;
       _i32 Status;
#if 0
       sl_SetSockOpt(socketfd, SOL_SOCKET, SO_NONBLOCKING, \
                           &nonBlocking,
                        sizeof(nonBlocking));
#else
        SlSockNonblocking_t enableOption;
        enableOption.NonblockingEnabled = 1;
        Status = sl_SetSockOpt(socketfd,SOL_SOCKET,SO_NONBLOCKING, (u8 *)&enableOption,sizeof(enableOption)); // Enable/disable nonblocking mode
        if( Status < 0 )
        {
            //sl_Close(socketfd);
            //Report("http_connect_server: ERROR sl_SetSockOpt, status=%d\r\n", SockID);
             GAgent_Printf(GAGENT_ERROR, "ERROR sl_SetSockOpt, SO_NONBLOCKING status=%d", Status);
            return -1;
        }
    return 0;
#endif
}

int Gagent_setsocketrectime(int socketfd)
{
#if 0
    struct SlTimeval_t timeVal;
	 //*struct SlTimeval_t timeVal;
    timeVal.tv_sec =  2;             // Seconds
    timeVal.tv_usec = 0;             // Microseconds. 10000 microseconds resolution
    sl_SetSockOpt(socketfd,     // Enable receive timeout
    		      SL_SOL_SOCKET,
    		      SL_SO_RCVTIMEO,
    		      (_u8 *)&timeVal,
    		      sizeof(timeVal));//s*/
#else
        struct SlTimeval_t timeVal;
        _i32 Status;

        timeVal.tv_sec=1;
        timeVal.tv_usec=0;
        Status = sl_SetSockOpt(socketfd, SL_SOL_SOCKET, SL_SO_RCVTIMEO, (_u8 *)&timeVal, sizeof(timeVal));
        if( Status < 0 )
        {
            //sl_Close(socketfd);
            //Report("http_connect_server: ERROR sl_SetSockOpt, status=%d\r\n", SockID);
             GAgent_Printf(GAGENT_ERROR, "ERROR sl_SetSockOpt, status=%d", Status);
            return -1;
        }

        return 0;
#endif
}


uint32 GAgent_GetDevTime_MS()
{
	return g_ulSeconds*1000;
}
uint32 GAgent_GetDevTime_S()
{	return g_ulSeconds;
}
/****************************************************************
FunctionName    :   GAgent_DevReset
Description     :   dev exit but not clean the config data                   
pgc             :   global staruc 
return          :   NULL
Add by Alex.lin     --2015-04-18
****************************************************************/
void GAgent_DevReset()
{
    GAgent_Printf( GAGENT_CRITICAL,"Please restart GAgent !!!\r\n");
    #if 0
    sl_Stop(30); 
    MAP_PRCMHibernateIntervalSet(330);
    MAP_PRCMHibernateWakeupSourceEnable(PRCM_HIB_SLOW_CLK_CTR);
    MAP_PRCMHibernateEnter();
    #else
  //
  // Configure hibernate RTC wakeup
  //
  PRCMHibernateWakeupSourceEnable(PRCM_HIB_SLOW_CLK_CTR);
  //
  // Delay loop
  //
  MAP_UtilsDelay(8000000);
  //
  // Set wake up time
  //
  PRCMHibernateIntervalSet(330);
  //
  // Request hibernate
  //
  PRCMHibernateEnter();
  //
  // Control should never reach here
  //
  while(1)
  {
  }
  #endif
}

long ResetNwp()
{
#if 0
    long lRetVal = -1;
    /* Restart Network processor */
    lRetVal = sl_Stop(SL_STOP_TIMEOUT);
    
    lRetVal = sl_Start(NULL,NULL,NULL);
    ASSERT_ON_ERROR(lRetVal);
    return lRetVal;
#else
    MAP_UtilsDelay(8000000);
    MAP_PRCMMCUReset(0);
#endif
}


void GAgent_DevInit( pgcontext pgc )
{
     halWiFiStatus = &pgc->rtinfo.GAgentStatus;
}
int8 GAgent_DevGetMacAddress( uint8* szmac )
{
	_u8                 macAddressLen = SL_MAC_ADDR_LEN ;
	_u8					tmpMac[SL_MAC_ADDR_LEN];
	
	sl_NetCfgGet (SL_MAC_ADDRESS_GET , NULL , &macAddressLen , tmpMac) ;

	sprintf(szmac, "%02x%02x%02x%02x%02x%02x", 
		tmpMac[0], tmpMac[1],tmpMac[2],
		tmpMac[3],tmpMac[4],tmpMac[5]);

	return 0;
}
uint32 GAgent_DevGetConfigData( gconfig *pConfig )
{
    int ret=0;
#if 1
	long lFileHandle;
    unsigned long ulToken;
	long lRetVal = -1;

	    //
    // open a user file for reading
    //
    lRetVal = sl_FsOpen((unsigned char *)GAGENT_CONFIG_FILE,
                        FS_MODE_OPEN_READ,
                        &ulToken,
                        &lFileHandle);
    if(lRetVal < 0)
    {
        lRetVal = sl_FsClose(lFileHandle, 0, 0, 0);
        return 0;
    }

	lRetVal = sl_FsRead(lFileHandle,
                    0,
                     (unsigned char *)pConfig, sizeof(gconfig));
    if ((lRetVal < 0) || (lRetVal != sizeof(gconfig)))
    {
    	lRetVal = sl_FsClose(lFileHandle, 0, 0, 0);
        return 0;
    }
	
    GAgent_Printf( GAGENT_INFO,"%s %d ",__FUNCTION__,ret );
    sl_FsClose(lFileHandle, 0, 0, 0);
#else
	pConfig->magicNumber = GAGENT_MAGIC_NUM;
	memcpy(pConfig->wifi_ssid, SSID_NAME, strlen(SSID_NAME));
	memcpy(pConfig->wifi_key, SECURITY_KEY, strlen(SECURITY_KEY));
#endif
    return 0;
}
uint32 GAgent_DevSaveConfigData( gconfig *pConfig )
{
	long lRetVal = -1;
	long lFileHandle;
    unsigned long ulToken;
	

    //
    //  create a user file
    //
    lRetVal = sl_FsOpen((unsigned char *)GAGENT_CONFIG_FILE,
                FS_MODE_OPEN_CREATE(2048, \
                          _FS_FILE_OPEN_FLAG_COMMIT|_FS_FILE_PUBLIC_WRITE),
                        &ulToken,
                        &lFileHandle);
    if(lRetVal < 0)
    {
        //
        // File may already be created
        //
        lRetVal = sl_FsClose(lFileHandle, 0, 0, 0);
        return lRetVal;
    }
    else
    {
        //
        // close the user file
        //
        lRetVal = sl_FsClose(lFileHandle, 0, 0, 0);
        if (SL_RET_CODE_OK != lRetVal)
        {
            return lRetVal;
        }
    }
    
    //
    //  open a user file for writing
    //
    lRetVal = sl_FsOpen((unsigned char *)GAGENT_CONFIG_FILE,
                        FS_MODE_OPEN_WRITE, 
                        &ulToken,
                        &lFileHandle);
    if(lRetVal < 0)
    {
        lRetVal = sl_FsClose(lFileHandle, 0, 0, 0);
		
		//
		//	create a user file
		//
		lRetVal = sl_FsOpen((unsigned char *)GAGENT_CONFIG_FILE,
					FS_MODE_OPEN_CREATE(2048, \
							  _FS_FILE_OPEN_FLAG_COMMIT|_FS_FILE_PUBLIC_WRITE),
							&ulToken,
							&lFileHandle);
		if(lRetVal < 0)
		{
			//
			// File may already be created
			//
			lRetVal = sl_FsClose(lFileHandle, 0, 0, 0);
			return lRetVal;
		}
		else
		{
			//
			// close the user file
			//
			lRetVal = sl_FsClose(lFileHandle, 0, 0, 0);
			if (SL_RET_CODE_OK != lRetVal)
			{
				return lRetVal;
			}
		}

    }
    
    //
    // write "Old MacDonalds" child song as many times to get just below a 64KB file
    //
    lRetVal = sl_FsWrite(lFileHandle,
                    0, 
                    (unsigned char *)pConfig, sizeof(gconfig));
    if (lRetVal < 0)
    {
            lRetVal = sl_FsClose(lFileHandle, 0, 0, 0);
            return lRetVal;
    }
    
    
    //
    // close the user file
    //
    lRetVal = sl_FsClose(lFileHandle, 0, 0, 0);
    if (SL_RET_CODE_OK != lRetVal)
    {
        return lRetVal;
    }
	
    return 0;
}
void WifiStatusHandler(int event)
{

}

unsigned long ipToNum(int8* ip)
{
	int8* p;
	int sections[4]={0};
	int i=0;
	unsigned long num =0;
	
	p = strtok(ip,".");
	while( p )
	{
		sections[i] = atoi(p);
		p = strtok(NULL,".");
		i++;
	}

	
	for( int j=3,i=0 ; j>=0 ; j--,i++ )
	{
		num += (sections[i] <<(8*j));
	}
	
	return num;
}

int32 GAgent_connect( int32 iSocketId, uint16 port,
                        int8 *ServerIpAddr,int8 flag)
{
    int8 ret= -1;
    unsigned long pDestinationIP;
    int8  tmpSvrip[20];
    
    struct sockaddr_in Msocket_address;
    GAgent_Printf(GAGENT_INFO,"do connect ip:%s port=%d",ServerIpAddr,port );

    memset(&tmpSvrip[0],0,sizeof(tmpSvrip) );
    memcpy(&tmpSvrip[0],ServerIpAddr,strlen(ServerIpAddr));
    pDestinationIP = ipToNum(tmpSvrip);

     GAgent_Printf(GAGENT_INFO,"iptoNumber:%s port=%d, destination=%lld",ServerIpAddr,port, pDestinationIP );

    Msocket_address.sin_family = SL_AF_INET;
    Msocket_address.sin_port= sl_Htons((unsigned short)port);
    Msocket_address.sin_addr.s_addr = sl_Htonl((unsigned int)pDestinationIP); //inet_addr(ServerIpAddr);
    #if 0
    ret = sl_Connect(iSocketId, (struct sockaddr *)&Msocket_address, sizeof( struct sockaddr_in));  
	if(ret >= 0)
	    {
		 Gagent_setsocketnonblock(iSocketId);
              //Gagent_setsocketrectime(iSocketId);
               GAgent_Printf(GAGENT_INFO,"iSocketId(%d), port:%d GAgent_connect success!!",iSocketId,port );
	    }
    #else
    // connecting to TCP server
    long retry = 0;
    while(1)
    {
        ret = sl_Connect(iSocketId, (struct sockaddr *)&Msocket_address, sizeof( struct sockaddr_in));
        if( (ret == SL_EALREADY) && (retry < 5))
        {
            //MAP_UtilsDelay(800000);
            retry++;
	      GAgent_Printf(GAGENT_ERROR, "sl_Connect SL_EALREADY retry=%d", retry);
        }
        else if( ret < 0 )
        {
            // error
            //sl_Close(iSocketId);
	     GAgent_Printf(GAGENT_ERROR, "sl_Connect connect failed!");
            break;
        }
        else
        {
            Gagent_setsocketnonblock(iSocketId);
            Gagent_setsocketrectime(iSocketId);
            GAgent_Printf(GAGENT_INFO,"iSocketId(%d), port:%d GAgent_connect success!!",iSocketId,port );
            break;
        }
    }
    #endif
    return ret;
}


//return the new wifimode 
int8 GAgent_DRVSetWiFiStartMode( pgcontext pgc,uint32 mode )
{
    return ( pgc->gc.flag +=mode );
}
void DRV_ConAuxPrint( char *buffer, int len, int level )
{
    
    buffer[len]='\0';
}
int32 GAgent_OpenUart( int32 BaudRate,int8 number,int8 parity,int8 stopBits,int8 flowControl )
{
	return 0;
}
void GAgent_LocalDataIOInit( pgcontext pgc )
{
    return ;
}

static int ConfigureMode(int iMode, const int8* ap_name,const int8 *ap_password)
{
    char secType = SL_SEC_TYPE_WPA_WPA2;//SL_SEC_TYPE_WPA_WPA2;
    long   lRetVal = -1;
    unsigned char ucConfigOpt = 0;
    unsigned char ucConfigLen = 0;
    unsigned char ucPower = 0;

    
    UART_PRINT("%s, %d \n", __FUNCTION__,__LINE__);
  
    lRetVal = sl_WlanSetMode(iMode);
    if(lRetVal < 0)
    {
       ASSERT_ON_ERROR(lRetVal);
       return 0; 
    }

    lRetVal = sl_WlanSet(SL_WLAN_CFG_AP_ID, WLAN_AP_OPT_SSID, strlen(ap_name),
                            (unsigned char*)ap_name);
    if(lRetVal < 0)
    {
       ASSERT_ON_ERROR(lRetVal);
       return 0; 
    }
    
    lRetVal = sl_WlanSet(SL_WLAN_CFG_AP_ID, WLAN_AP_OPT_PASSWORD, strlen(ap_password),
                            (unsigned char*)ap_password);
    if(lRetVal < 0)
    {
       ASSERT_ON_ERROR(lRetVal);
       return 0; 
    }
    
    lRetVal = sl_WlanSet(SL_WLAN_CFG_AP_ID, WLAN_AP_OPT_SECURITY_TYPE, 1,
                            (unsigned char*)&secType);
    if(lRetVal < 0)
    {
       ASSERT_ON_ERROR(lRetVal);
       return 0; 
    }

        #if 0
    

  
      SlNetCfgIpV4Args_t ipV4;
    ipV4.ipV4          = (_u32)SL_IPV4_VAL(10,10,100,254);            // _u32 IP address 
    ipV4.ipV4Mask      = (_u32)SL_IPV4_VAL(255,255,255,0);         // _u32 Subnet mask for this AP/P2P
    ipV4.ipV4Gateway   = (_u32)SL_IPV4_VAL(10,10,100,254);              // _u32 Default gateway address
    ipV4.ipV4DnsServer = (_u32)SL_IPV4_VAL(10,10,100,254);            // _u32 DNS server address
    //sl_NetCfgSet(SL_IPV4_AP_P2P_GO_STATIC_ENABLE,IPCONFIG_MODE_ENABLE_IPV4,sizeof(SlNetCfgIpV4Args_t),(_u8*)&ipV4);
    ASSERT_ON_ERROR(sl_NetCfgSet(SL_IPV4_AP_P2P_GO_STATIC_ENABLE, IPCONFIG_MODE_ENABLE_IPV4,
                                     sizeof(SlNetCfgIpV4Args_t), (_u8 *)&ipV4));
    
     SlNetAppDhcpServerBasicOpt_t dhcpParams= {0}; 
    _u8 outLen = sizeof(SlNetAppDhcpServerBasicOpt_t); 
    dhcpParams.lease_time      = 4096;                         // lease time (in seconds) of the IP Address
    dhcpParams.ipv4_addr_start =  SL_IPV4_VAL(10,10,100,252);   // first IP Address for allocation. IP Address should be set as Hex number - i.e. 0A0B0C01 for (10.11.12.1)
    dhcpParams.ipv4_addr_last  =  SL_IPV4_VAL(10,10,100,254);   // last IP Address for allocation. IP Address should be set as Hex number - i.e. 0A0B0C01 for (10.11.12.1)
    //sl_NetAppStop(SL_NET_APP_DHCP_SERVER_ID);                  // Stop DHCP server before settings
    //sl_NetAppSet(SL_NET_APP_DHCP_SERVER_ID, NETAPP_SET_DHCP_SRV_BASIC_OPT, outLen, (_u8* )&dhcpParams);  // set parameters
    //sl_NetAppStart(SL_NET_APP_DHCP_SERVER_ID);
    ASSERT_ON_ERROR(sl_NetAppStop(SL_NET_APP_DHCP_SERVER_ID));      // Stop DHCP server before settings
    ASSERT_ON_ERROR(sl_NetAppSet(SL_NET_APP_DHCP_SERVER_ID, NETAPP_SET_DHCP_SRV_BASIC_OPT,
                                     sizeof(SlNetAppDhcpServerBasicOpt_t), (_u8* )&dhcpParams));  // set parameters
    //ASSERT_ON_ERROR(sl_NetAppStart(SL_NET_APP_DHCP_SERVER_ID));     // Start DHCP server with new settings
    ASSERT_ON_ERROR(sl_NetAppStart(SL_NET_APP_DHCP_SERVER_ID));     // Start DHCP server with new settings
    #else
     SlNetCfgIpV4Args_t ipV4;
    ipV4.ipV4          = (_u32)SL_IPV4_VAL(192,168,1,1);            // _u32 IP address 
    ipV4.ipV4Mask      = (_u32)SL_IPV4_VAL(255,255,255,0);         // _u32 Subnet mask for this AP/P2P
    ipV4.ipV4Gateway   = (_u32)SL_IPV4_VAL(192,168,1,1);              // _u32 Default gateway address
    ipV4.ipV4DnsServer = (_u32)SL_IPV4_VAL(192,168,1,1);            // _u32 DNS server address
    sl_NetCfgSet(SL_IPV4_AP_P2P_GO_STATIC_ENABLE,IPCONFIG_MODE_ENABLE_IPV4,sizeof(SlNetCfgIpV4Args_t),(_u8*)&ipV4);
    #endif


    /* Restart Network processor */
    lRetVal = sl_Stop(SL_STOP_TIMEOUT);

    // reset status bits
    CLR_STATUS_BIT_ALL(g_ulStatus);

    lRetVal = sl_Start(NULL,NULL,NULL);
    if(lRetVal < 0)
    {
       ASSERT_ON_ERROR(lRetVal);
       return 0; 
    }

        #if 0   //Nik.chen for test
   
    //Stop Internal HTTP Server
    lRetVal = sl_NetAppStop(SL_NET_APP_HTTP_SERVER_ID);
    if(lRetVal < 0)
    {
        UART_PRINT("Failed to sl_NetAppStop HTTP_SERVER \n\r");
        LOOP_FOREVER();
    }
#endif


    UART_PRINT("Device is configured in AP mode, lRetVal=%d\n\r", lRetVal);
    setMode(lRetVal);
    return lRetVal;
}

int16 GAgent_DRV_WiFi_SoftAPModeStart( const int8* ap_name,const int8 *ap_password,int16 wifiStatus )
{
    long   lRetVal = -1;
    unsigned char ucDHCP;
     //
    // Configure the networking mode and ssid name(for AP mode)
    //
    if(getMode() != ROLE_AP)
   // if(ROLE_AP!= sl_Start(NULL,NULL,NULL))
    {
        if(getMode() == ROLE_STA)
            wlanscan();
        if(ConfigureMode(ROLE_AP, ap_name, ap_password) != ROLE_AP)
        {
            UART_PRINT("Unable to set AP mode, exiting Application...\n\r");
            sl_Stop(SL_STOP_TIMEOUT);
            LOOP_FOREVER();
        }

        while(!IS_IP_ACQUIRED(g_ulStatus))
        {
          //looping till ip is acquired
        }
    } 

   
#if 1   //Nik.chen for test
   
    //Stop Internal HTTP Server
    lRetVal = sl_NetAppStop(SL_NET_APP_HTTP_SERVER_ID);
    if(lRetVal < 0)
    {
        UART_PRINT("Failed to sl_NetAppStop HTTP_SERVER \n\r");
        LOOP_FOREVER();
    }
#endif

    unsigned char len = sizeof(SlNetCfgIpV4Args_t);
    SlNetCfgIpV4Args_t ipV4 = {0};

    // get network configuration
    lRetVal = sl_NetCfgGet(SL_IPV4_AP_P2P_GO_GET_INFO,&ucDHCP,&len,
                            (unsigned char *)&ipV4);
    if (lRetVal < 0)
    {
        UART_PRINT("Failed to get network configuration \n\r");
        LOOP_FOREVER();
    }

    UART_PRINT("IP %d.%d.%d.%d MASK %d.%d.%d.%d GW %d.%d.%d.%d DNS %d.%d.%d.%d\n",                                                             
        SL_IPV4_BYTE(ipV4.ipV4,3),SL_IPV4_BYTE(ipV4.ipV4,2),SL_IPV4_BYTE(ipV4.ipV4,1),SL_IPV4_BYTE(ipV4.ipV4,0), 
        SL_IPV4_BYTE(ipV4.ipV4Mask,3),SL_IPV4_BYTE(ipV4.ipV4Mask,2),SL_IPV4_BYTE(ipV4.ipV4Mask,1),SL_IPV4_BYTE(ipV4.ipV4Mask,0),         
        SL_IPV4_BYTE(ipV4.ipV4Gateway,3),SL_IPV4_BYTE(ipV4.ipV4Gateway,2),SL_IPV4_BYTE(ipV4.ipV4Gateway,1),SL_IPV4_BYTE(ipV4.ipV4Gateway,0),                 
        SL_IPV4_BYTE(ipV4.ipV4DnsServer,3),SL_IPV4_BYTE(ipV4.ipV4DnsServer,2),SL_IPV4_BYTE(ipV4.ipV4DnsServer,1),SL_IPV4_BYTE(ipV4.ipV4DnsServer,0));
    
    UART_PRINT("WIFI_MODE_AP successfully!!\n\r");
    GAgent_DevCheckWifiStatus( WIFI_MODE_AP,1 );
    setMode(2);
    #if 0
    while(!IS_IP_LEASED(g_ulStatus))
    {
      //wating for the client to connect
    }
    #endif
    return WIFI_MODE_AP;
}

void GAgent_DRVWiFi_APModeStop( pgcontext pgc )
{
    long lRetVal = -1;  
    lRetVal = ConfigureSimpleLinkToDefaultState();
    if(lRetVal < 0)
    {
        if (DEVICE_NOT_IN_STATION_MODE == lRetVal)
        {
            UART_PRINT("Failed to configure the device in its default state\n\r");
        }

        LOOP_FOREVER();
    }
    GAgent_Printf( GAGENT_INFO,"ToDo SoftAp Stop!");
}

int CC3200_SwitchSTAMode(void)
{ 
#if 1
          long lRetVal = -1;
         unsigned int uiConnectTimeoutCnt =0;

        while(uiConnectTimeoutCnt<AUTO_CONNECTION_TIMEOUT_COUNT &&
        ((!IS_CONNECTED(g_ulStatus)) || (!IS_IP_ACQUIRED(g_ulStatus))))  
        { 
            // Toggle LEDs to Indicate Connection Progress
            GPIO_IF_LedOff(MCU_IP_ALLOC_IND);
            MAP_UtilsDelay(800000);
            GPIO_IF_LedOn(MCU_IP_ALLOC_IND);
            MAP_UtilsDelay(800000);
            uiConnectTimeoutCnt++;
        }

        CLR_STATUS_BIT_ALL(g_ulStatus);
      
         UART_PRINT("CC3200_SwitchSTAMode uiConnectTimeoutCnt =%d!!! \n\r", uiConnectTimeoutCnt);
#else
        long lRetVal = -1;
         unsigned char ucVal = 1;
      
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

        // Switch to STA role and restart 
        lRetVal = sl_WlanSetMode(ROLE_STA);
         if (lRetVal < 0)
        {
            UART_PRINT("Failed to get network configuration \n\r");
            ASSERT_ON_ERROR(lRetVal);
            LOOP_FOREVER();
        }

        // Enable DHCP client
        lRetVal = sl_NetCfgSet(SL_IPV4_STA_P2P_CL_DHCP_ENABLE,1,1,&ucVal);
        ASSERT_ON_ERROR(lRetVal);

        lRetVal = sl_Stop(0xFF);
         if (lRetVal < 0)
        {
            UART_PRINT("sl_Stop failed!!! \n\r");
            ASSERT_ON_ERROR(lRetVal);
            LOOP_FOREVER();
        }

        lRetVal = sl_Start(NULL, NULL, NULL);
         if (lRetVal < 0)
        {
            UART_PRINT("sl_Start failed!!! \n\r");
            ASSERT_ON_ERROR(lRetVal);
            LOOP_FOREVER();
        }

        // Check if the device is in station again 
        if (ROLE_STA != lRetVal)
        {
            // We don't want to proceed if the device is not coming up in STA-mode 
            ASSERT_ON_ERROR(DEVICE_NOT_IN_STATION_MODE);
             LOOP_FOREVER();
        }
         UART_PRINT("CC3200_SwitchSTAMode switch to STA OK!!! \n\r");
         //setMode(lRetVal);
   
        GAgent_DevReset();

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

        lRetVal = sl_Start(NULL, NULL, NULL);
         if (lRetVal < 0)
        {
            UART_PRINT("sl_Start failed!!! \n\r");
            ASSERT_ON_ERROR(lRetVal);
            LOOP_FOREVER();
        }

           GAgent_DevReset();
#endif
}

int16 GAgent_DRVWiFi_StationCustomModeStart(int8 *StaSsid,int8 *StaPass,uint16 wifiStatus )
{
    SlSecParams_t secParams = {0};
    long lRetVal = -1;
     unsigned char policyVal;
     SlVersionFull   ver = {0};
    _WlanRxFilterOperationCommandBuff_t  RxFilterIdMask = {0};
     unsigned int uiConnectTimeoutCnt =0;

    unsigned char ucVal = 1;
    unsigned char ucConfigOpt = 0;
    unsigned char ucConfigLen = 0;
    unsigned char ucPower = 0;

    #if 1
     if(ROLE_STA!= getMode())
    // if(ROLE_STA!= sl_Start(NULL,NULL,NULL))
    { 
        long lRetVal = -1;

         GAgent_DevReset();
      
        // If the device is in AP mode, we need to wait for this event 
        // before doing anything 
        while(!IS_IP_ACQUIRED(g_ulStatus))
        {
#ifndef SL_PLATFORM_MULTI_THREADED
          _SlNonOsMainLoopTask(); 
#endif
        }

        // Switch to STA role and restart 
        lRetVal = sl_WlanSetMode(ROLE_STA);
         if (lRetVal < 0)
        {
            UART_PRINT("Failed to get network configuration \n\r");
            ASSERT_ON_ERROR(lRetVal);
            LOOP_FOREVER();
        }

        lRetVal = sl_Stop(0xFF);
         if (lRetVal < 0)
        {
            UART_PRINT("sl_Stop failed!!! \n\r");
            ASSERT_ON_ERROR(lRetVal);
            LOOP_FOREVER();
        }

        lRetVal = sl_Start(NULL, NULL, NULL);
         if (lRetVal < 0)
        {
            UART_PRINT("sl_Start failed!!! \n\r");
            ASSERT_ON_ERROR(lRetVal);
            LOOP_FOREVER();
        }

        // Check if the device is in station again 
        if (ROLE_STA != lRetVal)
        {
            // We don't want to proceed if the device is not coming up in STA-mode 
            ASSERT_ON_ERROR(DEVICE_NOT_IN_STATION_MODE);
             LOOP_FOREVER();
        }
         UART_PRINT("switch to STA OK!!! \n\r");
        GAgent_DevReset();
     }
    #endif
#if 1
    if((NULL == (signed char*)StaPass)&&(NULL == (signed char*)StaSsid))
    {
          // Enable DHCP client
        lRetVal = sl_NetCfgSet(SL_IPV4_STA_P2P_CL_DHCP_ENABLE,1,1,&ucVal);
        ASSERT_ON_ERROR(lRetVal);
#if 0  //Nik.chen mark or not?
         // Disable scan
    ucConfigOpt = SL_SCAN_POLICY(0);
    lRetVal = sl_WlanPolicySet(SL_POLICY_SCAN , ucConfigOpt, NULL, 0);
    ASSERT_ON_ERROR(lRetVal);
#endif

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
    
         //set AUTO policy
        lRetVal = sl_WlanPolicySet(SL_POLICY_CONNECTION,SL_CONNECTION_POLICY(1,0,0,0,0),&policyVal, 1 /*PolicyValLen*/);    
        ASSERT_ON_ERROR(lRetVal);

        lRetVal = sl_Stop(SL_STOP_TIMEOUT);

        lRetVal = sl_Start(NULL, NULL, NULL);
         if (lRetVal < 0)
        {
            UART_PRINT("sl_Start failed!!! \n\r");
            ASSERT_ON_ERROR(lRetVal);
            LOOP_FOREVER();
        }

          //waiting for the device to Auto Connect
        while(uiConnectTimeoutCnt<AUTO_CONNECTION_TIMEOUT_COUNT &&
        ((!IS_CONNECTED(g_ulStatus)) || (!IS_IP_ACQUIRED(g_ulStatus))))  
        { 
            // Toggle LEDs to Indicate Connection Progress
            GPIO_IF_LedOff(MCU_IP_ALLOC_IND);
            MAP_UtilsDelay(800000);
            GPIO_IF_LedOn(MCU_IP_ALLOC_IND);
            MAP_UtilsDelay(800000);
            uiConnectTimeoutCnt++;
        }
         //Couldn't connect Using Auto Profile
        if(uiConnectTimeoutCnt == AUTO_CONNECTION_TIMEOUT_COUNT)
        {
            //Blink Red LED to Indicate Connection Error
            GPIO_IF_LedOn(MCU_RED_LED_GPIO);
            CLR_STATUS_BIT_ALL(g_ulStatus);
            #if 0
            //Connect Using Smart Config
            lRetVal = SmartConfigConnect();
            ASSERT_ON_ERROR(lRetVal);
            #else
            UART_PRINT("connect timeout, before is smartconfig mode!!! \n\r");
            //GAgent_DevCheckWifiStatus( WIFI_MODE_ONBOARDING,1 );
             lRetVal = sl_WlanPolicySet(SL_POLICY_CONNECTION , \
                                SL_CONNECTION_POLICY(1,1,0,0,0), 0, 0); //Nik.chen for test delete or not?
             if(lRetVal<0)
            {
                UART_PRINT("sl_WlanPolicySet failed!!! \n\r");
            }
            #endif
        }

         //set AUTO policy
        lRetVal = sl_WlanPolicySet(SL_POLICY_CONNECTION,SL_CONNECTION_POLICY(1,0,0,0,0),&policyVal, 1 /*PolicyValLen*/);    
        ASSERT_ON_ERROR(lRetVal);
    }
    else
    {
         #if 0
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
        #endif

        // Enable DHCP client
        lRetVal = sl_NetCfgSet(SL_IPV4_STA_P2P_CL_DHCP_ENABLE,1,1,&ucVal);
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
    
        secParams.Key = (signed char*)StaPass;
        secParams.KeyLen = strlen(StaPass);
        secParams.Type = SECURITY_TYPE;

        lRetVal = sl_WlanProfileAdd((signed char*)StaSsid, strlen(StaSsid),0,&secParams,0,1,0);
         if (lRetVal < 0)
        {
            UART_PRINT("sl_WlanProfileAdd failed!!! \n\r");
        }

         #if 1
         if(0 == wifiStatus)
         {
                lRetVal = sl_Stop(SL_STOP_TIMEOUT);
                if (lRetVal < 0 ) 
                {
                    UART_PRINT("Failed to sl_Stop the device \n\r");
                    LOOP_FOREVER();
                }
                
                 lRetVal = sl_Start(0, 0, 0); //
                if (lRetVal < 0 ||( ROLE_STA != lRetVal)) 
                {
                    UART_PRINT("Failed to sl_Start the device \n\r");
                    LOOP_FOREVER();
                }
         }
         #endif


         //set AUTO policy
        lRetVal = sl_WlanPolicySet(SL_POLICY_CONNECTION,SL_CONNECTION_POLICY(1,0,0,0,0),&policyVal, 1 /*PolicyValLen*/);    
        ASSERT_ON_ERROR(lRetVal);

        lRetVal = sl_WlanConnect((signed char*)StaSsid, strlen(StaSsid), 0, &secParams, 0);
        while(lRetVal < 0&&(uiConnectTimeoutCnt<AUTO_CONNECTION_TIMEOUT_COUNT))
        {
        UART_PRINT("sl_Start failed, try again!!! \n\r");
         // Toggle LEDs to Indicate Connection Progress
        GPIO_IF_LedOff(MCU_IP_ALLOC_IND);
        MAP_UtilsDelay(800000);
        GPIO_IF_LedOn(MCU_IP_ALLOC_IND);
        MAP_UtilsDelay(800000);
        uiConnectTimeoutCnt++;
        lRetVal = sl_WlanConnect((signed char*)StaSsid, strlen(StaSsid), 0, &secParams, 0);
        }

        uiConnectTimeoutCnt = 0;

           //waiting for the device to Auto Connect
        while(uiConnectTimeoutCnt<AUTO_CONNECTION_TIMEOUT_COUNT &&
        ((!IS_CONNECTED(g_ulStatus)) || (!IS_IP_ACQUIRED(g_ulStatus))))  
        { 
            // Toggle LEDs to Indicate Connection Progress
            GPIO_IF_LedOff(MCU_IP_ALLOC_IND);
            MAP_UtilsDelay(800000);
            GPIO_IF_LedOn(MCU_IP_ALLOC_IND);
            MAP_UtilsDelay(800000);
            uiConnectTimeoutCnt++;
        }
      
        UART_PRINT("uiConnectTimeoutCnt =%d!!! \n\r", uiConnectTimeoutCnt);

         //Couldn't connect Using Auto Profile
        if(uiConnectTimeoutCnt == AUTO_CONNECTION_TIMEOUT_COUNT)
        {
            //Blink Red LED to Indicate Connection Error
            GPIO_IF_LedOn(MCU_RED_LED_GPIO);    
            CLR_STATUS_BIT_ALL(g_ulStatus);
            #if 0
            //Connect Using Smart Config
            lRetVal = SmartConfigConnect();
            ASSERT_ON_ERROR(lRetVal);
            #else
            UART_PRINT("connect timeout, before is softAP config!!! \n\r");
            setconnFlag(2); 
            //GAgent_DevCheckWifiStatus( WIFI_MODE_ONBOARDING,1 );
            #endif
        }

        // if(0 == wifiStatus)
          //  setconnFlag(2); 
    }
    #endif

     //Turn RED LED Off
    GPIO_IF_LedOff(MCU_RED_LED_GPIO);

    return WIFI_MODE_STATION;
}
int16 GAgent_DRVWiFi_StationDisconnect(void)
{
         long lRetVal = -1;

       //  lRetVal = sl_Start(0, 0, 0);
        
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
    
    return 0;
}
void GAgent_DevTick( )
{
	uint32 dTime=0;

	WatchdogIntClear(WDT_BASE);

    return;
}
void GAgent_DevLED_Red( uint8 onoff )
{

}
void GAgent_DevLED_Green( uint8 onoff )
{

}
int Socket_sendto(int sockfd, u8 *data, int len, void *addr, int addr_size)
{
    return sendto(sockfd, data, len, 0, (const struct sockaddr*)addr, addr_size);
}
int Socket_accept(int sockfd, void *addr, int *addr_size)
{
    return accept(sockfd, (struct sockaddr*)addr, (SlSocklen_t *)addr_size);
}
int Socket_recvfrom(int sockfd, u8 *buffer, int len, void *addr, int *addr_size)
{
    return recvfrom(sockfd, buffer, len, 0, (struct sockaddr *)addr, (SlSocklen_t *)addr_size);
}
int connect_mqtt_socket(int iSocketId, struct sockaddr_t *Msocket_address, unsigned short port, char *MqttServerIpAddr)
{
    return 0;

}
int8 GAgent_OpenAirlink( int32 timeout_s )
{
      long lRetVal = -1;
	
     lRetVal =CC3200_ConfigtoSmartconfig(); 
     if (lRetVal < 0)
    {
        UART_PRINT("CC3200_ConfigtoSmartconfig failed!!! \n\r");
         return lRetVal;
    }

     CLR_STATUS_BIT_ALL(g_ulStatus);

      /* Connect to our AP using SmartConfig method */
   lRetVal = SmartConfigConnect();
   if(lRetVal < 0)
   {
     //UART_PRINT("GAgent_OpenAirlink failed!!! \n\r");
         return lRetVal;
   }  

    GAgent_Printf( GAGENT_CRITICAL,"GAgent_OpenAirlink success !!!\r\n");
     return lRetVal;
}
void GAgent_AirlinkResult( pgcontext pgc )
{
    pgc->gc.flag |=XPG_CFG_FLAG_CONFIG;
    setMode(0);
    setconnFlag(1);
    return ;
}

 int32 wlanscan(void)
{
   
    long lRetVal = -1;
    int32  i=0;
    int32 byteSize=0;
    long lFileHandle;
    unsigned long ulToken;
	

    unsigned short ucIndex;
    unsigned char ucpolicyOpt;
    union
    {
        unsigned char ucPolicy[4];
        unsigned int uiPolicyLen;
    }policyVal;
  

    //
    // Assumption is that the device is configured in station mode already
    // and it is in its default state
    //
    #if 0 //Nik.chen delete or not?
    lRetVal = sl_Start(0, 0, 0); //Nik.chen tinking to delete or not?
    if (lRetVal < 0 ) //|| ROLE_STA != lRetVal
    {
        UART_PRINT("Failed to start the device \n\r");
        return lRetVal;
    }

    UART_PRINT("Device started as STATION \n\r");
    #endif

    //
    // make sure the connection policy is not set (so no scan is run in the
    // background)
    //
    ucpolicyOpt = SL_CONNECTION_POLICY(0, 0, 0, 0,0);
    lRetVal = sl_WlanPolicySet(SL_POLICY_CONNECTION , ucpolicyOpt, NULL, 0);
    if(lRetVal != 0)
    {
        GPIO_IF_LedOn(MCU_EXECUTE_FAIL_IND);
        UART_PRINT("Unable to clear the Connection Policy\n\r");
        return lRetVal;
    }
    
    //
    // enable scan 
    //
    ucpolicyOpt = SL_SCAN_POLICY(1);
    //
    // set scan cycle to 10 seconds 
    //
    policyVal.uiPolicyLen = 10;
    //
    // set scan policy - this starts the scan 
    //
    lRetVal = sl_WlanPolicySet(SL_POLICY_SCAN , ucpolicyOpt,
                               (unsigned char*)(policyVal.ucPolicy), sizeof(policyVal));
    if(lRetVal!=0)
    {
        GPIO_IF_LedOn(MCU_EXECUTE_FAIL_IND);
        UART_PRINT("Unable to set the Scan Policy\n\r");
        return lRetVal;
    }
    MAP_UtilsDelay(8000000);
    //
    // get scan results - all 20 entries in one transaction 
    //
    ucIndex = 0;
    //
    // retVal indicates the valid number of entries 
    // The scan results are occupied in netEntries[] 
    //
    lRetVal = sl_WlanGetNetworkList(ucIndex, (unsigned char)WLAN_SCAN_COUNT,
                                    &netEntries[ucIndex]);
    if(lRetVal==0)
    {
        GPIO_IF_LedOn(MCU_EXECUTE_FAIL_IND);
        UART_PRINT("Unable to retreive the network list\n\r");
        return lRetVal;
    }
    /* put a break point here and check netEntries[] value for scan ssid list */
    //
    // get scan results - 4 transactions of 5 entries 
    //
    ucIndex = 0;
    memset(netEntries, 0, sizeof(netEntries));

    do
    {
        lRetVal = sl_WlanGetNetworkList(ucIndex,
                                        (unsigned char)WLAN_SCAN_COUNT/4,
                                        &netEntries[ucIndex]);
        ucIndex += lRetVal;
    }
    while ((lRetVal == WLAN_SCAN_COUNT/4) && (ucIndex < WLAN_SCAN_COUNT));

     if( (gAplist.ApList) == NULL )
    {
	GAgent_Printf( GAGENT_CRITICAL,"Malloc buf for ApHostList...");
	//free( gAplist.ApList );

	gAplist.ApNum = WLAN_SCAN_COUNT;
	byteSize = (gAplist.ApNum)*sizeof(ApHostList_str);
	gAplist.ApList = (ApHostList_str *)malloc( byteSize );
	  GAgent_Printf( GAGENT_CRITICAL,"NUM=%d size:%d",gAplist.ApNum ,byteSize );
    }
    else
    {
    	gAplist.ApNum = WLAN_SCAN_COUNT;
    }
    
  
    if( (gAplist.ApList)==NULL )
    {
         UART_PRINT("gAplist.ApList hasn't malloced buffer!!!\n\r");
        return FAILURE;
    }
    
    memset( (gAplist.ApList),0,byteSize );

    for( i=0;i<gAplist.ApNum;i++ )
    {
        strcpy( (char*)gAplist.ApList[i].ssid,(netEntries[i].ssid) );
        gAplist.ApList[i].ApPower = netEntries[i].rssi;
    }
    for( i=0;i<gAplist.ApNum;i++ )
    {
        GAgent_Printf( GAGENT_CRITICAL,"SSID = %s power = %d", gAplist.ApList[i].ssid,
                                                          gAplist.ApList[i].ApPower );

    }

    #if 1
    //
    //  create a user file
    //
    lRetVal = sl_FsOpen((unsigned char *)GAGENT_HOSTPOTS,
                FS_MODE_OPEN_CREATE(1024, \
                          _FS_FILE_OPEN_FLAG_COMMIT|_FS_FILE_PUBLIC_WRITE),
                        &ulToken,
                        &lFileHandle);
    if(lRetVal < 0)
    {
        //
        // File may already be created
        //
        lRetVal = sl_FsClose(lFileHandle, 0, 0, 0);
        return lRetVal;
    }
    else
    {
        //
        // close the user file
        //
        lRetVal = sl_FsClose(lFileHandle, 0, 0, 0);
        if (SL_RET_CODE_OK != lRetVal)
        {
            return lRetVal;
        }
    }
    
    //
    //  open a user file for writing
    //
    lRetVal = sl_FsOpen((unsigned char *)GAGENT_HOSTPOTS,
                        FS_MODE_OPEN_WRITE, 
                        &ulToken,
                        &lFileHandle);
    if(lRetVal < 0)
    {
        lRetVal = sl_FsClose(lFileHandle, 0, 0, 0);
		
		//
		//	create a user file
		//
		lRetVal = sl_FsOpen((unsigned char *)GAGENT_HOSTPOTS,
					FS_MODE_OPEN_CREATE(1024, \
							  _FS_FILE_OPEN_FLAG_COMMIT|_FS_FILE_PUBLIC_WRITE),
							&ulToken,
							&lFileHandle);
		if(lRetVal < 0)
		{
			//
			// File may already be created
			//
			lRetVal = sl_FsClose(lFileHandle, 0, 0, 0);
			return lRetVal;
		}
		else
		{
			//
			// close the user file
			//
			lRetVal = sl_FsClose(lFileHandle, 0, 0, 0);
			if (SL_RET_CODE_OK != lRetVal)
			{
				return lRetVal;
			}
		}

    }
    
    //
    // write "Old MacDonalds" child song as many times to get just below a 64KB file
    //
    lRetVal = sl_FsWrite(lFileHandle,
                    0, 
                    (unsigned char *)&gAplist, sizeof(NetHostList_str));
    if (lRetVal < 0)
    {
            lRetVal = sl_FsClose(lFileHandle, 0, 0, 0);
            return lRetVal;
    }
    
    
    //
    // close the user file
    //
    lRetVal = sl_FsClose(lFileHandle, 0, 0, 0);
    if (SL_RET_CODE_OK != lRetVal)
    {
        return lRetVal;
    }
    #endif
    
    /* put a break point here and check netEntries[] value for scan ssid list */
    //
    // disable scan 
    //
    ucpolicyOpt = SL_SCAN_POLICY(0);
    lRetVal = sl_WlanPolicySet(SL_POLICY_SCAN , ucpolicyOpt, NULL, 0);
    if(lRetVal != 0)
    {
        GPIO_IF_LedOn(MCU_EXECUTE_FAIL_IND);
        UART_PRINT("Unable to Clear the Scan Policy\n\r");
        return lRetVal;
    }

    GPIO_IF_LedOn(MCU_EXECUTE_SUCCESS_IND);
    UART_PRINT("SUCCESS\n\r");

    return SUCCESS;
}

void GAgent_DRVWiFiStartScan( )
{
#if 0
     static int32 stime=0;
    if( ( stime%3 )==0 )
    {
      stime++;
      GAgent_Printf( GAGENT_CRITICAL,"GAgent_DRVWiFiStartScan->wlanscan...");
      wlanscan();
    }
#else
    wlanscan();
#endif
}
void GAgent_DRVWiFiStopScan( )
{

}

NetHostList_str *GAgentDRVWiFiScanResult( NetHostList_str *aplist )
{
     if( gAplist.ApNum==0 )
     	{
     	GAgent_Printf( GAGENT_INFO,"%s %d",__FUNCTION__,__LINE__ );
        return NULL;
     	}
    aplist  = &(gAplist);
    return  aplist;
}

void GAgent_DRVWiFiPowerScan( pgcontext pgc )
{
    wlanscan(); //Nik.chen mark for test
}

/****************************************************************
FunctionName    :   GAgent_DRVWiFiPowerResult
pgc             :   global struct;
returen         :   0-100.max:100.
Add by Alex.lin     --2015-05-26
****************************************************************/
int8 GAgent_DRVWiFiPowerScanResult( pgcontext pgc )
{
    int32 i=0;
    NetHostList_str *pAplist=NULL;
    pAplist = GAgentDRVWiFiScanResult( pAplist );
    if( pAplist->ApNum<=0||pAplist==NULL )
        return 0;
    #if 0
    for( i=0;i<pAplist->ApNum;i++ )
    {
        
       GAgent_Printf( GAGENT_CRITICAL,"SSID = %s power = %d", pAplist->ApList[i].ssid,
                                                           pAplist->ApList[i].ApPower );
    }
    #endif
    for( i=0;i<pAplist->ApNum;i++ )
    {
        if( 0==memcmp(pAplist->ApList[i].ssid,pgc->gc.wifi_ssid,strlen(pgc->gc.wifi_ssid)) )
        {
            return pAplist->ApList[i].ApPower;
        }
    }
    
    return 0;
}

int8 GAgent_DRVBootConfigWiFiMode( void )
{
     //STA Mode
    return ROLE_STA; 
}

int8 GAgent_DRVGetWiFiMode(pgcontext pgc)
{
    int8 ret =0;
    int8 ssidlen=0,keylen=0;
    ssidlen = strlen( pgc->gc.wifi_ssid );
    keylen = strlen( pgc->gc.wifi_key );
    GAgent_Printf( GAGENT_INFO,"SSIDLEN=%d, keyLEN=%d, default mode=%d ",ssidlen,keylen, getMode());

    if(2 == getMode())
    {
        ret = ROLE_AP;
    }
    
    if(( ssidlen>0 && ssidlen<=SSID_LEN_MAX) && keylen<=WIFIKEY_LEN_MAX )
    {
        pgc->gc.flag |= XPG_CFG_FLAG_CONNECTED;
         ret = ROLE_STA;
    }
    else
    {
        memset( pgc->gc.wifi_ssid,0,SSID_LEN_MAX+1 );
        memset( pgc->gc.wifi_key,0,WIFIKEY_LEN_MAX+1 );
        pgc->gc.flag &=~ XPG_CFG_FLAG_CONNECTED;
        ret = ROLE_AP;
    }

    if(0x53 == pgc->gc.airkiss_value)
    {
        pgc->gc.flag |= XPG_CFG_FLAG_CONNECTED;
         ret = ROLE_STA;
    }

    if(0x77 == pgc->gc.airkiss_value)
    {
         ret = ROLE_STA;
    }

    GAgent_DevSaveConfigData( &(pgc->gc) );
    GAgent_DevGetConfigData( &(pgc->gc) );

#if 0
    static unsigned char modeCout = 0;

    if(modeCout++ == 0)
       return ROLE_AP;
    else
       return ROLE_STA;
#elif defined(P58_VCC)
       unsigned int uiGPIOPort;
    unsigned char pucGPIOPin;
    unsigned char ucPinValue;
       
        //Read GPIO
    GPIO_IF_GetPortNPin(SH_GPIO_3,&uiGPIOPort,&pucGPIOPin);
    ucPinValue = GPIO_IF_Get(SH_GPIO_3,uiGPIOPort,pucGPIOPin);

    // return ROLE_STA;

    //If Connected to VCC, Mode is AP
    if(ucPinValue == 1)
    {
      //AP Mode
        GAgent_Printf(GAGENT_DEBUG,"set to AP mode!!! \n");
        ret = ROLE_AP;
    }
   
#elif defined(LEDSTATUS)
    unsigned char ucLEDStatus = 0;
    GPIO_IF_GetPortNPin(GPIO_LED2, &LED2Port, &LED2Pin);
    ucLEDStatus = GPIO_IF_Get(GPIO_LED2, LED2Port, LED2Pin);
    
    GAgent_Printf(GAGENT_DEBUG,"ucLEDStatus =%d!!! \n", ucLEDStatus);

    if(ucLEDStatus == 1)
    {
        GPIO_IF_GetPortNPin(GPIO_LED2, &LED2Port, &LED2Pin);
        GPIO_IF_Set(GPIO_LED2, LED2Port, LED2Pin, 0);
        ucLEDStatus = GPIO_IF_Get(GPIO_LED2, LED2Port, LED2Pin);
        //AP Mode
        GAgent_Printf(GAGENT_DEBUG,"set to AP mode, ucLEDStatus =%d!!! \n", ucLEDStatus);
        ret = ROLE_AP;
    }

    GPIO_IF_Set(GPIO_LED2, LED2Port, LED2Pin, 0);
#else
     if(setApMode== 1)
    {
         setApMode = 0;
         //AP Mode
        GAgent_Printf(GAGENT_DEBUG,"set to AP mode!!! \n");
        ret = ROLE_AP;
    }
#endif


    #if 1
     GAgent_Printf( GAGENT_INFO,"ssid=%s, key=%s, mode=%d\n ", pgc->gc.wifi_ssid,pgc->gc.wifi_key, ret);
     GAgent_Printf(GAGENT_DEBUG,"GAgent M2M IP :%s \n",pgc->gc.m2m_ip );
    GAgent_Printf(GAGENT_DEBUG,"GAgent GService IP :%s \n",pgc->gc.GServer_ip );
    #endif
    return ret;
}

static long CC3200_ConfigtoSmartconfig()
{
    SlSecParams_t secParams = {0};
    long lRetVal = -1;
     SlVersionFull   ver = {0};
    _WlanRxFilterOperationCommandBuff_t  RxFilterIdMask = {0};

    unsigned char ucVal = 1;
    unsigned char ucConfigOpt = 0;
    unsigned char ucConfigLen = 0;
    unsigned char ucPower = 0;

    UART_PRINT("CC3200_ConfigtoSmartconfig enter!!! \n\r");

    #if 1
     if(0 == getAirlinkTimes())
    {
	 if(ROLE_STA!= sl_Start(NULL,NULL,NULL) )
	{ 
	    UART_PRINT("sl_Start enter!!! \n\r");
	    // If the device is in AP mode, we need to wait for this event 
	    // before doing anything 
	    while(!IS_IP_ACQUIRED(g_ulStatus))
	    {
#ifndef SL_PLATFORM_MULTI_THREADED
	      _SlNonOsMainLoopTask(); 
#endif
	    }
	 }
    }

	  setAirlinkTimes(1);

        // Switch to STA role and restart 
        lRetVal = sl_WlanSetMode(ROLE_STA);
         if (lRetVal < 0)
        {
            UART_PRINT("Failed to get network configuration \n\r");
            ASSERT_ON_ERROR(lRetVal);
             return lRetVal;
        }

          lRetVal = sl_Stop(0xFF);
         if (lRetVal < 0)
        {
            UART_PRINT("sl_Stop failed!!! \n\r");
            ASSERT_ON_ERROR(lRetVal);
            LOOP_FOREVER();
        }

        lRetVal = sl_Start(NULL, NULL, NULL);
         if (lRetVal < 0)
        {
            UART_PRINT("sl_Start failed!!! \n\r");
            ASSERT_ON_ERROR(lRetVal);
             return lRetVal;
        }

        // Check if the device is in station again 
        if (ROLE_STA != lRetVal)
        {
            // We don't want to proceed if the device is not coming up in STA-mode 
            ASSERT_ON_ERROR(DEVICE_NOT_IN_STATION_MODE);
              return lRetVal;
        }

         GAgent_DevCheckWifiStatus( WIFI_MODE_AP,0 );
         UART_PRINT("Smartconfig switch to STA OK!!! \n\r");

//}
    #endif
    #if 1
    // Get the device's version-information
    ucConfigOpt = SL_DEVICE_GENERAL_VERSION;
    ucConfigLen = sizeof(ver);
    lRetVal = sl_DevGet(SL_DEVICE_GENERAL_CONFIGURATION, &ucConfigOpt, 
                                &ucConfigLen, (unsigned char *)(&ver));
     if(lRetVal < 0)
    {
            UART_PRINT("sl_DevGet failed!!! \n\r");
            ASSERT_ON_ERROR(lRetVal);
            return lRetVal;
    }
    
    UART_PRINT("Host Driver Version: %s\n\r",SL_DRIVER_VERSION);
    UART_PRINT("Build Version %d.%d.%d.%d.31.%d.%d.%d.%d.%d.%d.%d.%d\n\r",
    ver.NwpVersion[0],ver.NwpVersion[1],ver.NwpVersion[2],ver.NwpVersion[3],
    ver.ChipFwAndPhyVersion.FwVersion[0],ver.ChipFwAndPhyVersion.FwVersion[1],
    ver.ChipFwAndPhyVersion.FwVersion[2],ver.ChipFwAndPhyVersion.FwVersion[3],
    ver.ChipFwAndPhyVersion.PhyVersion[0],ver.ChipFwAndPhyVersion.PhyVersion[1],
    ver.ChipFwAndPhyVersion.PhyVersion[2],ver.ChipFwAndPhyVersion.PhyVersion[3]);
#if 1
    // Set connection policy to Auto + SmartConfig 
    //      (Device's default connection policy)
    lRetVal = sl_WlanPolicySet(SL_POLICY_CONNECTION, 
                                SL_CONNECTION_POLICY(1, 0, 0, 0, 1), NULL, 0);
    if(lRetVal < 0)
    {
            UART_PRINT("sl_WlanPolicySet failed!!! \n\r");
            ASSERT_ON_ERROR(lRetVal);
            return lRetVal;
    }

    // Remove all profiles
    lRetVal = sl_WlanProfileDel(0xFF);
    if(lRetVal < 0)
    {
            UART_PRINT("sl_WlanProfileDel failed!!! \n\r");
            ASSERT_ON_ERROR(lRetVal);
            return lRetVal;
    }  
#endif
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
    if(lRetVal < 0)
    {
            UART_PRINT("sl_NetCfgSet failed!!! \n\r");
            ASSERT_ON_ERROR(lRetVal);
            return lRetVal;
    }  

    // Disable scan
    ucConfigOpt = SL_SCAN_POLICY(0);
    lRetVal = sl_WlanPolicySet(SL_POLICY_SCAN , ucConfigOpt, NULL, 0);
    if(lRetVal < 0)
    {
            UART_PRINT("sl_NetCfgSet failed!!! \n\r");
            ASSERT_ON_ERROR(lRetVal);
            return lRetVal;
    }  

    // Set Tx power level for station mode
    // Number between 0-15, as dB offset from max power - 0 will set max power
    ucPower = 0;
    lRetVal = sl_WlanSet(SL_WLAN_CFG_GENERAL_PARAM_ID, 
            WLAN_GENERAL_PARAM_OPT_STA_TX_POWER, 1, (unsigned char *)&ucPower);
     if(lRetVal < 0)
    {
            UART_PRINT("sl_WlanSet failed!!! \n\r");
            ASSERT_ON_ERROR(lRetVal);
            return lRetVal;
    }  

    // Set PM policy to normal
    lRetVal = sl_WlanPolicySet(SL_POLICY_PM , SL_NORMAL_POLICY, NULL, 0);
      if(lRetVal < 0)
    {
            UART_PRINT("sl_WlanPolicySet failed!!! \n\r");
            ASSERT_ON_ERROR(lRetVal);
            return lRetVal;
    }  

    // Unregister mDNS services
    lRetVal = sl_NetAppMDNSUnRegisterService(0, 0);
     if(lRetVal < 0)
    {
            UART_PRINT("sl_NetAppMDNSUnRegisterService failed!!! \n\r");
            ASSERT_ON_ERROR(lRetVal);
            return lRetVal;
    }  

    // Remove  all 64 filters (8*8)
    memset(RxFilterIdMask.FilterIdMask, 0xFF, 8);
    lRetVal = sl_WlanRxFilterSet(SL_REMOVE_RX_FILTER, (_u8 *)&RxFilterIdMask,
                       sizeof(_WlanRxFilterOperationCommandBuff_t));
      if(lRetVal < 0)
    {
            UART_PRINT("sl_WlanRxFilterSet failed!!! \n\r");
            ASSERT_ON_ERROR(lRetVal);
            return lRetVal;
    }  

    lRetVal = sl_Stop(0xFF);
     if(lRetVal < 0)
    {
            UART_PRINT("sl_Stop failed!!! \n\r");
            ASSERT_ON_ERROR(lRetVal);
            return lRetVal;
    }  
    #endif

    lRetVal = sl_Start(NULL, NULL, NULL);
     if (lRetVal < 0)
    {
        UART_PRINT("sl_Start failed!!! \n\r");
        ASSERT_ON_ERROR(lRetVal);
         return lRetVal;
    }
    
    return  lRetVal;
}


//*****************************************************************************
//
//! \brief Connecting to a WLAN Accesspoint using SmartConfig provisioning
//!
//! Enables SmartConfig provisioning for adding a new connection profile
//! to CC3200. Since we have set the connection policy to Auto, once
//! SmartConfig is complete, CC3200 will connect automatically to the new
//! connection profile added by smartConfig.
//!
//! \param[in]                     None
//!
//! \return                        None
//!
//! \note
//!
//! \warning                    If the WLAN connection fails or we don't
//!                             acquire an IP address, We will be stuck in this
//!                             function forever.
//
//*****************************************************************************
int SmartConfigConnect()
{
    unsigned char policyVal;
    long lRetVal = -1;
    unsigned int uiConnectTimeoutCnt =0;

    // Clear all profiles 
    // This is of course not a must, it is used in this example to make sure
    // we will connect to the new profile added by SmartConfig
    //
    lRetVal = sl_WlanProfileDel(0xFF);
    ASSERT_ON_ERROR(lRetVal);

     //Set connection policy to Auto + SmartConfig 
    lRetVal = sl_WlanPolicySet(  SL_POLICY_CONNECTION,
                      SL_CONNECTION_POLICY(1,0,0,0,1),
                      &policyVal,
                      1 /*PolicyValLen*/);
    ASSERT_ON_ERROR(lRetVal);

    // Start SmartConfig
    // This example uses the unsecured SmartConfig method
    //
    lRetVal = sl_WlanSmartConfigStart(0,                /*groupIdBitmask*/
                           SMART_CONFIG_CIPHER_NONE,    /*cipher*/
                           0,                           /*publicKeyLen*/
                           0,                           /*group1KeyLen*/
                           0,                           /*group2KeyLen */
                           NULL,                        /*publicKey */
                           NULL,                        /*group1Key */
                           NULL);                       /*group2Key*/
    ASSERT_ON_ERROR(lRetVal);

    UART_PRINT("SmartConfigConnect waiting!!! \n\r");

#if 0
    // Wait for WLAN Event
    while((!IS_CONNECTED(g_ulStatus)) || (!IS_IP_ACQUIRED(g_ulStatus)))
    {
        #ifndef SL_PLATFORM_MULTI_THREADED
          _SlNonOsMainLoopTask(); 
        #endif
    }
#else
        //waiting for the device to Auto Connect
        while(uiConnectTimeoutCnt<SMARTCONFIG_TIMEOUT_COUNT &&
        ((!IS_CONNECTED(g_ulStatus)) || (!IS_IP_ACQUIRED(g_ulStatus))))  
        { 
            // Toggle LEDs to Indicate Connection Progress
            GPIO_IF_LedOff(MCU_IP_ALLOC_IND);
            MAP_UtilsDelay(800000);
            GPIO_IF_LedOn(MCU_IP_ALLOC_IND);
            MAP_UtilsDelay(800000);
            uiConnectTimeoutCnt++;
        }
         //Couldn't connect Using Auto Profile
        if(uiConnectTimeoutCnt == SMARTCONFIG_TIMEOUT_COUNT)
        {
            CLR_STATUS_BIT_ALL(g_ulStatus);
            #if 0
            //Connect Using Smart Config
            lRetVal = SmartConfigConnect();
            ASSERT_ON_ERROR(lRetVal);
            #else
            //GAgent_DRV_WiFi_SoftAPModeStart( pgContextData->minfo.ap_name,AP_PASSWORD,0 );
            UART_PRINT("waiting timeout, SmartConfig failed!!! \n\r");
             return FAILURE;
            #endif
        }else
            UART_PRINT("SmartConfig received ssid and key!!! \n\r");
#endif

     //
     // Turn ON the RED LED to indicate connection success
     //
     GPIO_IF_LedOn(MCU_RED_LED_GPIO);
     //wait for few moments
     MAP_UtilsDelay(80000000);
     //reset to default AUTO policy
     lRetVal = sl_WlanPolicySet(  SL_POLICY_CONNECTION,
                           SL_CONNECTION_POLICY(1,0,0,0,0),
                           &policyVal,
                           1 /*PolicyValLen*/);
     ASSERT_ON_ERROR(lRetVal);

     return SUCCESS;
}

#if 1
void CC3200_ReconAP(int8 *StaSsid,int8 *StaPass)
{
    SlSecParams_t secParams = {0};
    long lRetVal = -1;
    
     lRetVal = sl_WlanPolicySet(SL_POLICY_CONNECTION , \
                                SL_CONNECTION_POLICY(1,1,0,0,0), 0, 0);
    if(lRetVal<0)
    {
        UART_PRINT("sl_WlanPolicySet failed!!! \n\r");
        return;  
    }
    
    secParams.Key = (signed char*)StaPass;
    secParams.KeyLen = strlen(StaPass);
    secParams.Type = SECURITY_TYPE;

    lRetVal = sl_WlanConnect((signed char*)StaSsid, strlen(StaSsid), 0, &secParams, 0);

    if(lRetVal<0)
        UART_PRINT("reconnectAP failed!!! \n\r");
    else
        UART_PRINT("reconnectAP success!!! \n\r");
}

void CC3200_ReconAP_Smartconfig(void)
{
    long lRetVal = -1;
    unsigned char policyVal;
    #if 0
     /* Set connection policy to Fast, Device will connect to last connected AP.
     * This feature can be used to reconnect to AP */
    lRetVal = sl_WlanPolicySet(SL_POLICY_CONNECTION , \
                                SL_CONNECTION_POLICY(1,1,0,0,0), 0, 0);
     if(lRetVal<0)
    {
        UART_PRINT("sl_WlanPolicySet failed!!! \n\r");
        return;  
    }

    /* Connect to the open AP */
    lRetVal = sl_WlanConnect((signed char*)StaSsid, strlen(StaSsid), 0, 0, 0);
    if(lRetVal<0)
        UART_PRINT("CC3200_ReconAP_Smartconfig failed!!! \n\r");
    else
        UART_PRINT("CC3200_ReconAP_Smartconfig success!!! \n\r");

      //set AUTO policy
    lRetVal = sl_WlanPolicySet(SL_POLICY_CONNECTION,
                      SL_CONNECTION_POLICY(1,0,0,0,0),
                      &policyVal, 1 /*PolicyValLen*/); 
     if(lRetVal<0)
    {
        UART_PRINT("sl_WlanPolicySet failed!!! \n\r");
        return;  
    }

    lRetVal = sl_Start(NULL,NULL,NULL);
     if(lRetVal<0)
    {
        UART_PRINT("sl_Start failed!!! \n\r");
        return;  
    }
    #endif

     lRetVal = sl_WlanPolicySet(SL_POLICY_CONNECTION , \
                                SL_CONNECTION_POLICY(1,1,0,0,0), 0, 0); //Nik.chen for test delete or not?
     if(lRetVal<0)
    {
        UART_PRINT("sl_WlanPolicySet failed!!! \n\r");
        return;  
    }

   
    
}
long testSwitchStaMode()
{ 
        long lRetVal = -1;
      
        // If the device is in AP mode, we need to wait for this event 
        // before doing anything 
        while(!IS_IP_ACQUIRED(g_ulStatus))
        {
#ifndef SL_PLATFORM_MULTI_THREADED
          _SlNonOsMainLoopTask(); 
#endif
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
             LOOP_FOREVER();
        }
}
#endif
