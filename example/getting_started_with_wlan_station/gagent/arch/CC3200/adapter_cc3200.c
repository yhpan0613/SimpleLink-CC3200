
#include "gagent.h"

#include "simplelink.h"
#include "osi.h"
#include "uart_if.h"

#define GAGENT_CONFIG_FILE "/tmp/gizwitsConfig"

extern unsigned long g_ulSeconds;

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

    STATUS_CODE_MAX = -0xBB8
}e_AppStatusCodes;


void msleep(int m_seconds)
{ 
    osi_Sleep(m_seconds);
}

void sleep(int seconds)
{ 
    osi_Sleep(seconds*1000);
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
#if 0
	int flags = fcntl(socketfd, F_GETFL, 0);
	flags |= O_NONBLOCK;
	return fcntl(socketfd, F_SETFL, flags | O_NONBLOCK);
#else
    return 0;
#endif
}

uint32 GAgent_GetDevTime_MS()
{
	return g_ulSeconds*1000;
}
uint32 GAgent_GetDevTime_S()
{
	return g_ulSeconds;
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
    exit(0);
}
void GAgent_DevInit( pgcontext pgc )
{

}
int8 GAgent_DevGetMacAddress( uint8* szmac )
{
	_u8                 macAddressLen = SL_MAC_ADDR_LEN ;
	
	sl_NetCfgGet (SL_MAC_ADDRESS_GET , NULL , &macAddressLen , szmac) ;

	return 0;
}
uint32 GAgent_DevGetConfigData( gconfig *pConfig )
{
    int ret=0;

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
int32 GAgent_connect( int32 iSocketId, uint16 port,
                        int8 *ServerIpAddr,int8 flag)
{
    int8 ret=0;
	unsigned long pDestinationIP;
	unsigned char TmpIp[5];
    
    struct sockaddr_in Msocket_address;
    GAgent_Printf(GAGENT_INFO,"do connect ip:%s port=%d",ServerIpAddr,port );

	sscanf(ServerIpAddr, "%d.%d.%d.%d", TmpIp);
	pDestinationIP = (TmpIp[0]<<24) + (TmpIp[1]<<16) + (TmpIp[2]<<8) + TmpIp[3];

    Msocket_address.sin_family = SL_AF_INET;
    Msocket_address.sin_port= sl_Htons((unsigned short)port);
    Msocket_address.sin_addr.s_addr = sl_Htonl((unsigned int)pDestinationIP); //inet_addr(ServerIpAddr);
    ret = sl_Connect(iSocketId, (struct sockaddr *)&Msocket_address, sizeof( struct sockaddr_in));  

    return ret;
}
int8 GAgent_DRVGetWiFiMode( pgcontext pgc )
{
    return ( pgc->gc.flag  |= XPG_CFG_FLAG_CONNECTED );

  
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

int16 GAgent_DRV_WiFi_SoftAPModeStart( const int8* ap_name,const int8 *ap_password,int16 wifiStatus )
{
    return WIFI_MODE_AP;
}
int16 GAgent_DRVWiFi_StationCustomModeStart(int8 *StaSsid,int8 *StaPass,uint16 wifiStatus )
{
    GAgent_Printf( GAGENT_INFO," Station ssid:%s StaPass:%s",StaSsid,StaPass );
    return WIFI_STATION_CONNECTED;
    //return 0;
}
int16 GAgent_DRVWiFi_StationDisconnect()
{
    return 0;
}
void GAgent_DevTick()
{
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
void GAgent_OpenAirlink( int32 timeout_s )
{
    //TODO
    return ;
}
void GAgent_AirlinkResult( pgcontext pgc )
{
    return ;
}
void GAgent_DRVWiFiStartScan( )
{

}
void GAgent_DRVWiFiStopScan( )
{

}
NetHostList_str *GAgentDRVWiFiScanResult( NetHostList_str *aplist )
{
    //需要再平台相关的扫描结果调用该函数。
    //把平台相关扫描函数的结果拷贝到NetHostList_str这个结构体上。
    return  aplist;
}
/*
void Socket_CreateTCPServer(int tcp_port)
{
    return;
}

void Socket_CreateUDPServer(int udp_port)
{
    return;
}

void Socket_CreateUDPBroadCastServer( int udp_port )
{
    return;
}
*/
