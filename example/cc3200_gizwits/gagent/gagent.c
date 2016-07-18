#include "gagent.h"
#include "lan.h"
#include "cloud.h"

pgcontext pgContextData=NULL;
void GAgent_NewVar( pgcontext *pgc );

/****************************************************************
Function    :   GAgent_Init
Description :   GAgent init 
pgc         :   global staruc pointer.
return      :   NULL
Add by Alex.lin     --2015-03-27
****************************************************************/
void GAgent_Init( pgcontext *pgc )
{
    GAgent_DevInit( *pgc );
    GAgent_NewVar( pgc );
    GAgent_loglevelSet( /*-1*/GAGENT_DUMP/*GAGENT_INFO*//*GAGENT_WARNING*/ );

    GAgent_VarInit( pgc );
    GAgent_LocalInit( *pgc ); //Nik.chen mark
    GAgent_WiFiInit( *pgc );
    GAgent_LANInit(*pgc);
    
    GAgent_Printf( GAGENT_CRITICAL,"GAgent Start...");
}
/****************************************************************
Function    :   GAgent_NewVar
Description :   malloc New Var 
pgc         :   global staruc pointer.
return      :   NULL
Add by Alex.lin     --2015-03-27
****************************************************************/
void GAgent_NewVar( pgcontext *pgc )
{
    *pgc = (pgcontext)malloc( sizeof( gcontext ));
    while(NULL == *pgc)
    {
        *pgc = (pgcontext)malloc( sizeof( gcontext ));
        sleep(1); //Nik.chen MAP_UtilsDelay ->
    }
    memset(*pgc,0,sizeof(gcontext) );

    return ;
}

/****************************************************************
Function    :   GAgent_VarInit
Description :   init global var and malloc memory 
pgc         :   global staruc pointer.
return      :   NULL
Add by Alex.lin     --2015-03-27
****************************************************************/
void GAgent_VarInit( pgcontext *pgc )
{
    int totalCap = BUF_LEN + BUF_HEADLEN;
    int bufCap = BUF_LEN;
    (*pgc)->rtinfo.firstStartUp = 1;

    (*pgc)->rtinfo.Rxbuf = (ppacket)malloc( sizeof(packet) );
    (*pgc)->rtinfo.Rxbuf->allbuf = (uint8 *)malloc( totalCap );
    while( (*pgc)->rtinfo.Rxbuf->allbuf==NULL )
    {
        (*pgc)->rtinfo.Rxbuf->allbuf = (uint8 *)malloc( totalCap );
        sleep(1);  //Nik.chen MAP_UtilsDelay ->
    }
    memset( (*pgc)->rtinfo.Rxbuf->allbuf,0,totalCap );
    (*pgc)->rtinfo.Rxbuf->totalcap = totalCap;
    (*pgc)->rtinfo.Rxbuf->bufcap = bufCap;
    resetPacket( (*pgc)->rtinfo.Rxbuf );

    (*pgc)->rtinfo.Txbuf = (ppacket)malloc( sizeof(packet) );
    (*pgc)->rtinfo.Txbuf->allbuf = (uint8 *)malloc( totalCap );
    while( (*pgc)->rtinfo.Txbuf->allbuf==NULL )
    {
        (*pgc)->rtinfo.Txbuf->allbuf = (uint8 *)malloc( totalCap );
        sleep(1);
    }
    memset( (*pgc)->rtinfo.Txbuf->allbuf,0,totalCap );
    (*pgc)->rtinfo.Txbuf->totalcap = totalCap;
    (*pgc)->rtinfo.Txbuf->bufcap = bufCap;
    resetPacket( (*pgc)->rtinfo.Txbuf );
    /* get config data form flash */
    GAgent_DevGetConfigData( &(*pgc)->gc );
    //GAgent_CheckConfigData( &(*pgc)->gc );
    (*pgc)->rtinfo.waninfo.CloudStatus=CLOUD_INIT;

    /* get mac address */
    GAgent_DevGetMacAddress((*pgc)->minfo.szmac);
    memcpy( (*pgc)->minfo.ap_name,AP_NAME,strlen(AP_NAME));
    memcpy( (*pgc)->minfo.ap_name+strlen(AP_NAME),(*pgc)->minfo.szmac+8,4);

    (*pgc)->minfo.ap_name[strlen(AP_NAME)+4]= '\0';

    if((*pgc)->gc.magicNumber != GAGENT_MAGIC_NUM)
    {
        memset(&((*pgc)->gc), 0, sizeof(GAGENT_CONFIG_S));
        (*pgc)->gc.magicNumber = GAGENT_MAGIC_NUM;
    }
    else
    {
        if( strlen( (*pgc)->gc.DID )!=(DID_LEN-2) )
            memset( ((*pgc)->gc.DID ),0,DID_LEN );

        if( strlen( (*pgc)->gc.old_did )!=(DID_LEN-2))
            memset( ((*pgc)->gc.old_did ),0,DID_LEN );
        
        if( strlen( ((*pgc)->gc.wifipasscode) ) != PASSCODE_LEN )
        {    
            memset( ((*pgc)->gc.wifipasscode ),0,PASSCODE_MAXLEN + 1);
            make_rand( (*pgc)->gc.wifipasscode );
        }
        if( strlen( ((*pgc)->gc.old_wifipasscode) )!=PASSCODE_LEN || strlen( ((*pgc)->gc.old_did) )!= (DID_LEN-2) )
        {    
            memset( ((*pgc)->gc.old_wifipasscode ),0,PASSCODE_LEN );
            memset( ((*pgc)->gc.old_did ),0,DID_LEN );
        }
        
        if( strlen( ((*pgc)->gc.old_productkey) )!=(PK_LEN) )
            memset( (*pgc)->gc.old_productkey,0,PK_LEN + 1 );

        if( strlen( (*pgc)->gc.m2m_ip)>IP_LEN_MAX || strlen( (*pgc)->gc.m2m_ip)<IP_LEN_MIN )
            memset( (*pgc)->gc.m2m_ip,0,IP_LEN_MAX + 1 );
        
        if( strlen( (*pgc)->gc.GServer_ip)>IP_LEN_MAX || strlen( (*pgc)->gc.GServer_ip)<IP_LEN_MIN )
            memset( (*pgc)->gc.GServer_ip,0,IP_LEN_MAX + 1 );
        
        if( strlen( (*pgc)->gc.cloud3info.cloud3Name )>CLOUD3NAME )
            memset( (*pgc)->gc.cloud3info.cloud3Name,0,CLOUD3NAME );  

         GAgent_Printf(GAGENT_DEBUG,"GAgent flag :%d",(*pgc)->gc.flag );
           //(pgc)->gc.flag
    }
    

    (*pgc)->rtinfo.waninfo.ReConnectMqttTime = GAGENT_MQTT_TIMEOUT;
	(*pgc)->rtinfo.waninfo.ReConnectHttpTime = GAGENT_HTTP_TIMEOUT;
    (*pgc)->rtinfo.waninfo.send2HttpLastTime = GAgent_GetDevTime_S();
    (*pgc)->rtinfo.waninfo.firstConnectHttpTime = GAgent_GetDevTime_S();
    (*pgc)->rtinfo.waninfo.httpCloudPingTime = 0;
    (*pgc)->rtinfo.waninfo.http_socketid = INVALID_SOCKET;
    (*pgc)->rtinfo.waninfo.m2m_socketid = INVALID_SOCKET;

     (*pgc)->ls.udpServerFd = INVALID_SOCKET;
     (*pgc)->ls.tcpServerFd = INVALID_SOCKET;
     (*pgc)->ls.tcpWebConfigFd = INVALID_SOCKET;
    (*pgc)->rtinfo.OTATypeflag = OTATYPE_WIFI;
    (*pgc)->rtinfo.onlinePushflag = 0;
    Cloud_ClearClientAttrs(*pgc, &((*pgc)->rtinfo.waninfo.srcAttrs));
   // (*pgc)->rtinfo.stChannelAttrs.lanClient.fd = INVALID_SOCKET;
    GAgent_DevSaveConfigData( &((*pgc)->gc) );
    setconnFlag(0);
}

void GAgent_dumpInfo( pgcontext pgc )
{
    GAgent_Printf(GAGENT_DEBUG,"Product Soft Version: %s. Hard Version: %s", WIFI_SOFTVAR,WIFI_HARDVER);
    GAgent_Printf(GAGENT_DEBUG,"GAgent Compiled Time: %s, %s.\r\n",__DATE__, __TIME__);
    GAgent_Printf(GAGENT_DEBUG,"GAgent mac :%s",pgc->minfo.szmac );
    GAgent_Printf(GAGENT_DEBUG,"GAgent passcode :%s len=%d",pgc->gc.wifipasscode,strlen( pgc->gc.wifipasscode ) );
    GAgent_Printf(GAGENT_DEBUG,"GAgent did :%s len:%d",pgc->gc.DID,strlen(pgc->gc.DID) );
    GAgent_Printf(GAGENT_DEBUG,"GAgent old did :%s len:%d",pgc->gc.old_did,strlen(pgc->gc.old_did) );
    GAgent_Printf(GAGENT_DEBUG,"GAgent old pk :%s len:%d",pgc->gc.old_productkey,strlen(pgc->gc.old_productkey) );
    GAgent_Printf(GAGENT_DEBUG,"GAgent AP name:%s",pgc->minfo.ap_name );
    GAgent_Printf(GAGENT_DEBUG,"GAgent 3rd cloud :%s",pgc->gc.cloud3info.cloud3Name );
    GAgent_Printf(GAGENT_DEBUG,"GAgent M2M IP :%s",pgc->gc.m2m_ip );
    GAgent_Printf(GAGENT_DEBUG,"GAgent GService IP :%s",pgc->gc.GServer_ip );
    return ;
}
/****************************** running status ******************************/
/*
    flag=1 set GAgentStatus 
    flag=0 reset GAgentStatus
*/
void GAgent_SetWiFiStatus( pgcontext pgc,uint16 GAgentStatus,int8 flag )
{
    if(flag==1)
    {
        pgc->rtinfo.GAgentStatus |= GAgentStatus;
    }
    else
    {
        pgc->rtinfo.GAgentStatus &=~ GAgentStatus;
    }
    return ;
}

void GAgent_SetCloudConfigStatus( pgcontext pgc,int16 cloudstauts )
{
    pgc->rtinfo.waninfo.CloudStatus = cloudstauts;
    /*g_globalvar.waninfo.CloudStatus = cloudstauts;*/
    return ;
}

void GAgent_SetCloudServerStatus( pgcontext pgc,int16 serverstatus )
{
    pgc->rtinfo.waninfo.mqttstatus = serverstatus;
    /*g_globalvar.waninfo.mqttstatus = serverstatus;*/
    return ;
}


void  GAgent_AddSelectFD( pgcontext pgc )
{
    int32 i=0;
    FD_ZERO( &(pgc->rtinfo.readfd) );
    //FD_CLR(BSD_SOCKET_ID_MASK ,&(pgc->rtinfo.readfd) );
          #if 1
         FD_CLR(pgc->ls.tcpServerFd ,&(pgc->rtinfo.readfd) );
         FD_CLR(pgc->ls.udpServerFd ,&(pgc->rtinfo.readfd) );
         FD_CLR(pgc->rtinfo.waninfo.http_socketid ,&(pgc->rtinfo.readfd) );
         FD_CLR(pgc->rtinfo.waninfo.m2m_socketid ,&(pgc->rtinfo.readfd) );
         #endif

    if( pgc->rtinfo.waninfo.http_socketid>=0 )
        {
         GAgent_Printf(GAGENT_CRITICAL," ,line:%d,httpfd=%d \n",__LINE__, (pgc->rtinfo.waninfo.http_socketid));
        FD_SET( pgc->rtinfo.waninfo.http_socketid,&(pgc->rtinfo.readfd) );
        }

    if( pgc->rtinfo.waninfo.m2m_socketid>=0 )
        {
        	FD_SET( pgc->rtinfo.waninfo.m2m_socketid,&(pgc->rtinfo.readfd) );
        }
#if 0
    if( pgc->rtinfo.local.uart_fd>0 )
        {
         GAgent_Printf(GAGENT_CRITICAL," GAgent_AddSelectFD, line:%d, \n",__LINE__);
        FD_SET( pgc->rtinfo.local.uart_fd,&(pgc->rtinfo.readfd));
        }

 if( pgc->ls.udp3rdCloudFd >0 )
  {
         //GAgent_Printf(GAGENT_CRITICAL," GAgent_AddSelectFD, line:%d, \n",__LINE__);
         FD_SET( pgc->ls.udp3rdCloudFd, &(pgc->rtinfo.readfd) );
        }
#endif
 if( pgc->ls.tcpWebConfigFd >= 0 )
        {
         GAgent_Printf(GAGENT_CRITICAL," GAgent_AddSelectFD, line:%d, tcpWebConfigFd=%d \n",__LINE__, (pgc->ls.tcpWebConfigFd));
        FD_SET( pgc->ls.tcpWebConfigFd, &(pgc->rtinfo.readfd) );
        }

    if( pgc->ls.tcpServerFd >= 0 )
        {
          // FD_CLR(pgc->ls.tcpServerFd ,&(pgc->rtinfo.readfd) );
         GAgent_Printf(GAGENT_CRITICAL,"  line:%d,tcpfd=%d \n",__LINE__, (pgc->ls.tcpServerFd));
        FD_SET( pgc->ls.tcpServerFd, &(pgc->rtinfo.readfd) );
       
        }

    if( pgc->ls.udpServerFd >= 0 )

        {
         //  FD_CLR(pgc->ls.udpServerFd ,&(pgc->rtinfo.readfd) );
         GAgent_Printf(GAGENT_CRITICAL,"  line:%d,isset=%d,udpfd=%d \n",__LINE__, (FD_ISSET(pgc->ls.udpServerFd, &(pgc->rtinfo.readfd))), (pgc->ls.udpServerFd));
        FD_SET( pgc->ls.udpServerFd, &(pgc->rtinfo.readfd) );
        }

    for(i = 0; i < LAN_TCPCLIENT_MAX; i++)
    {
        if( pgc->ls.tcpClient[i].fd >= 0 )
        {
             GAgent_Printf(GAGENT_CRITICAL," GAgent_AddSelectFD, line:%d, i=%d\n",__LINE__, i);
            FD_SET( pgc->ls.tcpClient[i].fd, &(pgc->rtinfo.readfd) );
        }
    }
}

int32 GAgent_MaxFd( pgcontext pgc  ) 
{
    int i;
    int32 maxfd =0;

    if( maxfd<=pgc->rtinfo.waninfo.http_socketid )
        {
        maxfd = pgc->rtinfo.waninfo.http_socketid;
        GAgent_Printf(GAGENT_CRITICAL," http_socketid, maxfd:%d \n",maxfd);
        }

    if( maxfd<pgc->rtinfo.waninfo.m2m_socketid )
        {
        maxfd = pgc->rtinfo.waninfo.m2m_socketid;
        GAgent_Printf(GAGENT_CRITICAL," m2m_socketid, maxfd:%d \n",maxfd);
        }
#if 0
    if( maxfd<=pgc->rtinfo.local.uart_fd )
        maxfd = pgc->rtinfo.local.uart_fd;
#endif
       if( maxfd<pgc->ls.udp3rdCloudFd )
         maxfd = pgc->ls.udp3rdCloudFd;

       if( maxfd<pgc->ls.tcpWebConfigFd )
        {
        maxfd = pgc->ls.tcpWebConfigFd;
        GAgent_Printf(GAGENT_CRITICAL," tcpWebConfigFd, maxfd:%d \n",maxfd);
        }
         

    if( maxfd<pgc->ls.tcpServerFd )
        {
        maxfd = pgc->ls.tcpServerFd;
        GAgent_Printf(GAGENT_CRITICAL," tcpServerFd, maxfd:%d \n",maxfd);
        }

    if( maxfd<pgc->ls.udpServerFd )
        {
        maxfd = pgc->ls.udpServerFd;  
        GAgent_Printf(GAGENT_CRITICAL," udpServerFd, maxfd:%d \n",maxfd);
        }

    for(i = 0; i < LAN_TCPCLIENT_MAX; i++)
    {
        if(pgc->ls.tcpClient[i].fd >0 && maxfd< pgc->ls.tcpClient[i].fd )
        {
            maxfd = pgc->ls.tcpClient[i].fd;
        }
    }

    return maxfd;
}
#define NIK_CHEN_TEST
#if 0
int32 GAgent_SelectFd(pgcontext pgc,int32 sec,int32 usec )
{
    int32 ret=0;
    static int32 max_select_fd=0;
    int32 select_fd=0;
    u8 selFlag=0;

    select_fd = GAgent_MaxFd( pgc );
    if( max_select_fd < select_fd ) //Nik.chen !=
    {
        GAgent_AddSelectFD( pgc );
        max_select_fd = select_fd;
        selFlag = 1;
         GAgent_Printf(GAGENT_CRITICAL," GAgent_SelectFd, line:%d, max_select_fd=%d \n",__LINE__, max_select_fd);
    }
    
    //select_fd = GAgent_MaxFd( pgc );
    //GAgent_Printf(GAGENT_CRITICAL," GAgent_SelectFd, line:%d, select_fd=%d \n",__LINE__, select_fd);
    if( select_fd>0 && selFlag)
    {
         #if 0
         FD_CLR(pgc->ls.tcpServerFd ,&(pgc->rtinfo.readfd) );
         FD_CLR(pgc->ls.udpServerFd ,&(pgc->rtinfo.readfd) );
         FD_CLR(pgc->rtinfo.waninfo.http_socketid ,&(pgc->rtinfo.readfd) );
         FD_CLR(pgc->rtinfo.waninfo.m2m_socketid ,&(pgc->rtinfo.readfd) );
        
        GAgent_Printf( GAGENT_CRITICAL,"httpFD000= %d \n",(FD_ISSET( pgc->rtinfo.waninfo.http_socketid,&(pgc->rtinfo.readfd) ))); 
        GAgent_Printf( GAGENT_CRITICAL,"mqttFD000= %d \n",(FD_ISSET( pgc->rtinfo.waninfo.m2m_socketid,&(pgc->rtinfo.readfd) )));
        GAgent_Printf( GAGENT_CRITICAL,"udpFD000= %d \n",(FD_ISSET(pgc->ls.udpServerFd, &(pgc->rtinfo.readfd))));
        #endif
        GAgent_Printf(GAGENT_CRITICAL," GAgent_SelectFd, line:%d \n",__LINE__);
        ret = GAgent_select(select_fd+1,&(pgc->rtinfo.readfd),NULL,NULL,sec,usec );
        GAgent_Printf(GAGENT_CRITICAL," GAgent_AddSelectFD,select_fd=%d, ret=%d\n",select_fd, ret);

        GAgent_Printf( GAGENT_CRITICAL,"httpFD= %d \n",(FD_ISSET( pgc->rtinfo.waninfo.http_socketid,&(pgc->rtinfo.readfd) ))); 
        GAgent_Printf( GAGENT_CRITICAL,"mqttFD= %d \n",(FD_ISSET( pgc->rtinfo.waninfo.m2m_socketid,&(pgc->rtinfo.readfd) )));
        GAgent_Printf( GAGENT_CRITICAL,"udpFD= %d \n",(FD_ISSET(pgc->ls.udpServerFd, &(pgc->rtinfo.readfd))));
        if( ret>0 )
           setselFlag(1);
        else
           setselFlag(0);
    }
    return ret;
}
#elif defined(NIK_CHEN_TEST)

int32 GAgent_SelectFd(pgcontext pgc,int32 sec,int32 usec )
{
    int32 ret=0;
    int32 select_fd=0;
    GAgent_AddSelectFD( pgc );
    select_fd = GAgent_MaxFd( pgc );
    if( select_fd>=0 )
    {
        #if 1
        if(4 == getDnsFlag())
         {
             GAgent_Printf( GAGENT_INFO," GAgent_SelectFd set 2 seconds!!! ");
         	ret = GAgent_select(select_fd+1,&(pgc->rtinfo.readfd),NULL,NULL,3,usec );
         	
         }
	 else
	 #endif
	  ret = GAgent_select(select_fd+1,&(pgc->rtinfo.readfd),NULL,NULL,sec,usec );
        //setselBlockFlag(FALSE);
        GAgent_Printf(GAGENT_CRITICAL," GAgent_AddSelectFD,select_fd=%d, ret=%d\n",select_fd, ret);
#if 0
        GAgent_Printf( GAGENT_CRITICAL,"httpFD= %d \n",(FD_ISSET( pgc->rtinfo.waninfo.http_socketid,&(pgc->rtinfo.readfd) ))); 
        GAgent_Printf( GAGENT_CRITICAL,"mqttFD= %d \n",(FD_ISSET( pgc->rtinfo.waninfo.m2m_socketid,&(pgc->rtinfo.readfd) )));
        GAgent_Printf( GAGENT_CRITICAL,"udpFD= %d \n",(FD_ISSET(pgc->ls.udpServerFd, &(pgc->rtinfo.readfd))));
        GAgent_Printf( GAGENT_CRITICAL,"tcpServerFd= %d \n",(FD_ISSET(pgc->ls.tcpServerFd, &(pgc->rtinfo.readfd))));
        GAgent_Printf( GAGENT_CRITICAL,"tcpWebConfigFd= %d \n",(FD_ISSET(pgc->ls.tcpWebConfigFd, &(pgc->rtinfo.readfd))));
#endif
        if( ret>0 )
           setselFlag(1);
        else
           setselFlag(0);
    }
    return ret;
}
#else
#endif
/****************************************************************
*       functionName    :   GAgent_SetGServerIP
*       description     :   set the  Gserver ip into configdata
*       Input           :   gserver ip string like "192.168.1.1"
*       return          :   =0 set Gserver ip ok
*                       :   other fail 
*       add by Alex.lin     --2015-03-02
****************************************************************/
int8 GAgent_SetGServerIP( pgcontext pgc,int8 *szIP )
{
    /*strcpy( g_stGAgentConfigData.GServer_ip,szIP );*/
    strcpy( pgc->gc.GServer_ip,szIP );
    GAgent_DevSaveConfigData( &(pgc->gc) );
    return 0;
}
/****************************************************************
*       functionName    :   GAgent_SetGServerSocket
*       description     :   set the  Gserver socket val
*       pgc             :   global struct.
*       socketid        :   the socketid will set into GServer socket
*       return          :   =0 set ok
*                       :   other fail 
*       add by Alex.lin     --2015-03-02
****************************************************************/
int8 GAgent_SetGServerSocket( pgcontext pgc,int32 socketid )
{
    pgc->rtinfo.waninfo.http_socketid = socketid;
    /*g_globalvar.waninfo.http_socketid = socketid;*/
    return 0;
}
/****************************** config status ******************************/
uint8 GAgent_SetDeviceID( pgcontext pgc,int8 *p_szDeviceID )
{
    //int8 len=0;
    //len = strlen( p_szDeviceID );
    if( p_szDeviceID != NULL )
    {
        strcpy( pgc->gc.DID,p_szDeviceID );
    }
    else
    {
        memset( pgc->gc.DID,0,DID_LEN );
    }
    GAgent_DevSaveConfigData( &(pgc->gc) );
    return 0;
}
/****************************************************************
*       FunctionName      :     GAgent_SetOldDeviceID
*       Description       :     reset the old did and passcode
*       flag              :     0 reset to NULL
*                               1 set the new did and passcode 
*                                 to old info.
*      Add by Alex.lin      --2015-03-02
****************************************************************/
int8 GAgent_SetOldDeviceID( pgcontext pgc,int8* p_szDeviceID,int8* p_szPassCode,int8 flag )
{
    /*
    memset( g_stGAgentConfigData.old_did,0,24 );
    memset( g_stGAgentConfigData.old_wifipasscode,0,16 );
    */
    memset( pgc->gc.old_did,0,DID_LEN );
    memset( pgc->gc.old_wifipasscode,0,PASSCODE_LEN );
    if( 1 == flag )
    {
        strcpy( pgc->gc.old_did,p_szDeviceID );
        strcpy( pgc->gc.old_wifipasscode,p_szPassCode );
    }
    GAgent_DevSaveConfigData( &(pgc->gc) );
    return 0;
}

/*
    return 0 : don't need to disable did.
           1 : need to disable did
*/           
int8 GAgent_IsNeedDisableDID( pgcontext pgc )
{
    uint32 didLen=0,passcodeLen=0;
    didLen = strlen( pgc->gc.old_did );
    passcodeLen = strlen( pgc->gc.old_wifipasscode );
    if( (0==didLen)|| ( 22<didLen) || (passcodeLen==0) || (passcodeLen>16) ) /* invalid did length or passcode length */
    {
        memset( pgc->gc.old_did,0,DID_LEN );
        memset( pgc->gc.old_wifipasscode,0,PASSCODE_LEN );
        GAgent_DevSaveConfigData( &(pgc->gc) );
        return 0;
    }
    return 1;
}
void GAgent_loglevelSet( uint16 level )
{
    pgContextData->rtinfo.loglevel = level;
}
int8 GAgent_loglevelenable( uint16 level )
{
    if( level > pgContextData->rtinfo.loglevel )
        return 1;
    else 
        return 0;
}
/****************************************************************
*       FunctionName      :     GAgent_RefreshIPTick
*       Description       :     update ip tick,if gethostbyname 
                                ok will set time to one hour 
*      Add by Alex.lin      --2015-04-23
****************************************************************/
void GAgent_RefreshIPTick( pgcontext pgc,uint32 dTime_s )
{
    uint32 cTime=0;
    int8 tmpip[32] = {0},failed=0,ret=0;

   #if 1
    if(4 == getDnsFlag())
    {
        GAgent_Printf( GAGENT_INFO," GAgent_RefreshIPTick return!!! ");
        return;
    }
   #endif

    if( ((pgc->rtinfo.GAgentStatus)&WIFI_MODE_TEST) == WIFI_MODE_TEST )
    {
        GAgent_Printf( GAGENT_INFO,"In WIFI_MODE_TEST...");
        return ;
    }
    if( ((pgc->rtinfo.GAgentStatus)&WIFI_STATION_CONNECTED)!=WIFI_STATION_CONNECTED )
    {
        GAgent_Printf( GAGENT_INFO," not in WIFI_STATION_CONNECTED ");
        return ;
    }
    if( ((pgc->rtinfo.GAgentStatus)&WIFI_MODE_BINDING)!=WIFI_MODE_BINDING )
    {
        GAgent_Printf( GAGENT_INFO," in WIFI_MODE_BINDING ");
        return ;
    }
#if 1
    if(2 == getDnsFlag())
    {
        //GAgent_Printf( GAGENT_INFO," kill DnsParsingTick task!!! ");
        CC3200_DNSTaskDelete();
    }
#endif
    pgc->rtinfo.waninfo.RefreshIPLastTime+=dTime_s;
    GAgent_Printf( GAGENT_DEBUG,"RefreshIPTime=%ld, IPLastTime=%ld", (pgc->rtinfo.waninfo.RefreshIPTime), (pgc->rtinfo.waninfo.RefreshIPLastTime));

    if( (pgc->rtinfo.waninfo.RefreshIPLastTime) >= (pgc->rtinfo.waninfo.RefreshIPTime) )
    {
        GAgent_Printf( GAGENT_DEBUG,"GAgentStatus:%04x",(pgc->rtinfo.GAgentStatus) );
        GAgent_Printf( GAGENT_DEBUG,"RefreshIPTime=%d ms,lsst:%d,ctimd %d",(pgc->rtinfo.waninfo.RefreshIPTime),(pgc->rtinfo.waninfo.RefreshIPLastTime) ,cTime);
        GAgent_Printf( GAGENT_DEBUG,"RefreshIPTime=%d s",(pgc->rtinfo.waninfo.RefreshIPTime) );
        //pgc->rtinfo.waninfo.RefreshIPLastTime = 0;
#if 0
        if( ((pgc->rtinfo.GAgentStatus)&WIFI_STATION_CONNECTED) != WIFI_STATION_CONNECTED )
        {
            GAgent_Printf( GAGENT_DEBUG,"line %d",__LINE__ );
            pgc->rtinfo.waninfo.RefreshIPTime =  1;
        }
#endif
       if(pgContextData &&(1 != getDnsFlag())&&(2 != getDnsFlag()))
        {
         GAgent_Printf( GAGENT_DEBUG,"sleep 300ms,create and switch dnsparing task!" );
         ret = CC3200_CreateDnsTask(NULL);
          if(ret < 0)
         {
            setDnsFlag(0);
         }
         else
         {
             pgc->rtinfo.waninfo.RefreshIPLastTime = 0;   
         }
         osi_Sleep(300);
        }
       else if(pgContextData && (2 == getDnsFlag()))
       {
          setDnsFlag(1);
         CC3200_DNSTaskDelete();
         GAgent_Printf( GAGENT_DEBUG,"CC3200_CreateDnsTask" );
         ret = CC3200_CreateDnsTask(NULL);
         if(ret < 0)
         {
            setDnsFlag(2);
         }
         else
         {
             pgc->rtinfo.waninfo.RefreshIPLastTime = 0;   
         }
         osi_Sleep(300);
       }
        
    }
#if 0
    else if(2 == getDnsFlag())
    {
        GAgent_Printf( GAGENT_INFO," kill DnsParsingTick task!!! ");
        CC3200_DNSTaskDelete();
    }
#endif

}

/******************************************************
 *      FUNCTION        :   update info
 *      new_pk          :   new productkey
 *   Add by Alex lin  --2014-12-19
 *
 ********************************************************/
void GAgent_UpdateInfo( pgcontext pgc,uint8 *new_pk )
{
    GAgent_Printf(GAGENT_DEBUG,"a new productkey is :%s.",new_pk);
    /*the necessary information to disable devices*/
    memset( pgc->gc.old_did,0,DID_LEN);
    memset( pgc->gc.old_wifipasscode,0,PASSCODE_MAXLEN + 1);
    /*存到old字段用于注销设备*/
    memcpy( pgc->gc.old_did,pgc->gc.DID,DID_LEN);
    memcpy( pgc->gc.old_wifipasscode,pgc->gc.wifipasscode,PASSCODE_MAXLEN + 1);
    
    memset( pgc->gc.old_productkey,0,PK_LEN + 1);
    memcpy( pgc->gc.old_productkey,new_pk,PK_LEN + 1);
    pgc->gc.old_productkey[PK_LEN] = '\0';
    
    /*neet to reset info */
    memset( pgc->gc.FirmwareVer,0,FIRMWARE_LEN_MAX + 1);
    memset( pgc->gc.FirmwareVerLen,0,2);
    memset( &(pgc->gc.cloud3info),0,sizeof(pgc->gc.cloud3info));
    memset( pgc->gc.DID,0,DID_LEN );
    
   /*生成新的wifipasscode*/
    make_rand(pgc->gc.wifipasscode);

    GAgent_DevSaveConfigData( &(pgc->gc) );
}
/******************************************************
 *      FUNCTION        :   uGAgent_Config
 *      new_pk          :   new productkey
 *   Add by Alex lin  --2014-12-19
 *
 ********************************************************/
void GAgent_Config( uint8 typed,pgcontext pgc )
{   
    switch( typed )
    {
        //AP MODE
        case 1:
             GAgent_Printf( GAGENT_DEBUG,"file:%s function:%s line:%d ",__FILE__,__FUNCTION__,__LINE__ );
             //pgc->gc.airkiss_value = 0x55;
             // wlanscan();
             setconnFlag(0);
             setDnsFlag(3);
              GAgent_DevCheckWifiStatus( WIFI_STATION_CONNECTED,0);
              GAgent_DevCheckWifiStatus( WIFI_MODE_STATION,0);
             GAgent_DevCheckWifiStatus( WIFI_MODE_ONBOARDING,1 );
        break;
        
        //Airlink
        case 2:
        {
            int8 timeout ;
             int8 ret =0 ;
            uint16 tempWiFiStatus=0;
            timeout = 5;
            static int8 resetFlag = 0;
            tempWiFiStatus = pgc->rtinfo.GAgentStatus;
            pgc->gc.flag  &=~ XPG_CFG_FLAG_CONFIG;
            GAgent_DevLED_Red( 0 );
            setconnFlag(0); 
            setDnsFlag(3);
             //WIFI_MODE_ONBOARDING DOWN
             #if 1 //Nik.chen add
            pgc->gc.flag &=~ XPG_CFG_FLAG_CONFIG_AP;
            #endif
            ret = GAgent_OpenAirlink( timeout/* timeout */ );
            if(ret == 0) //((tempWiFiStatus&WIFI_MODE_AP))
            {
            resetFlag = 1;
            GAgent_Printf( GAGENT_INFO,"it is going to reset, pls wait...");
            }
            while( timeout )
            {
                timeout--;
                sleep(1); //Nik.chen MAP_UtilsDelay ->
                if( (pgc->gc.flag & XPG_CFG_FLAG_CONFIG) ==XPG_CFG_FLAG_CONFIG )
                {    
                    GAgent_Printf( GAGENT_INFO,"AirLink result ssid:%s key:%s",pgc->gc.wifi_ssid,pgc->gc.wifi_key );
                    tempWiFiStatus |=WIFI_MODE_STATION;
                    pgc->gc.flag |= XPG_CFG_FLAG_CONNECTED;
                    pgc->ls.onboardingBroadCastTime = SEND_UDP_DATA_TIMES;
                    pgc->gc.airkiss_value = 0x53;
                    GAgent_DevSaveConfigData( &(pgc->gc) );  
                    GAgent_Printf( GAGENT_INFO,"After airkiss_value=%d !!!", pgc->gc.airkiss_value);
                    if(resetFlag)
                    {
                        //GAgent_Printf( GAGENT_INFO,"ap mode, reset now...");
                        GAgent_DevReset();
                    }
                    GAgent_WiFiInit( pgc );
                    CreateUDPBroadCastServer( pgc );
                    break;
                }
                GAgent_DevLED_Green( (timeout%2) );
            }      
            GAgent_DevCheckWifiStatus( WIFI_MODE_ONBOARDING,0 );
            GAgent_SetWiFiStatus( pgc,WIFI_MODE_ONBOARDING,0 );
            if( timeout<=0 )
            {
                    GAgent_DevCheckWifiStatus( WIFI_STATION_CONNECTED,0);
                    GAgent_DevCheckWifiStatus( WIFI_MODE_STATION,0);
                    GAgent_SetWiFiStatus( pgc,WIFI_STATION_CONNECTED,0);
                    GAgent_SetWiFiStatus( pgc,WIFI_MODE_STATION,0);
                    GAgent_SetWiFiStatus( pgc,WIFI_CLOUD_CONNECTED,0);
                    GAgent_DevCheckWifiStatus( WIFI_MODE_ONBOARDING,1 );
			 setMode(0);
                    GAgent_Printf( GAGENT_INFO,"AirLink Timeout ...");
                    GAgent_Printf( GAGENT_INFO,"Into SoftAp Config...");
                    GAgent_Printf( GAGENT_INFO,"airlink config: onboarding :%d, apMode:%d, staMode:%d", ( tempWiFiStatus&WIFI_MODE_ONBOARDING ),(tempWiFiStatus&WIFI_MODE_AP),(tempWiFiStatus&WIFI_MODE_STATION));
                    GAgent_DRVWiFi_StationDisconnect();
            }
        break;
        }
        default :
        break;
    }
}
uint8 GAgent_EnterTest( pgcontext pgc )
{
    pgc->rtinfo.scanWifiFlag = 0;
    memset( pgc->gc.GServer_ip,0,IP_LEN_MAX+1);
    memset( pgc->gc.m2m_ip,0,IP_LEN_MAX+1);

    pgc->gc.airkiss_value = 0x77;

    GAgent_DevSaveConfigData( &(pgc->gc) );
    //setStaMode();
    //GAgent_EnterNullMode( pgc );
    //RecreatLanSocket(pgc);

     sleep(2);
     GAgent_DevReset();

    //GAgent_SetWiFiStatus( pgc,WIFI_MODE_TEST,1 );
    //GAgent_DRVWiFiStartScan();

    return 0;
}
uint8 GAgent_ExitTest( pgcontext pgc )
{
#if 0
	pgc->rtinfo.scanWifiFlag = 0;
	GAgent_DRVWiFi_StationDisconnect();
	GAgent_SetWiFiStatus( pgc,WIFI_MODE_TEST,0 );
	GAgent_DRVWiFiStopScan( );
	GAgent_EnterNullMode( pgc );
#else
	GAgent_Printf( GAGENT_INFO,"GAgent_ExitTest ...");
	pgc->rtinfo.scanWifiFlag = 0;
	GAgent_SetWiFiStatus( pgc,WIFI_MODE_TEST,0 );
	GAgent_EnterNullMode( pgc );
#endif
    return 0;
}
int32 GAgent_Cloud_OTAByUrl( pgcontext pgc,int8 *downloadUrl,OTATYPE otatype )
{
    if( OTATYPE_WIFI == otatype )
    {
         return GAgent_WIFIOTAByUrl( pgc, downloadUrl );
    }
    else
    {
         return GAgent_MCUOTAByUrl( pgc, downloadUrl );
    }   
}
int8 GAgent_GetStaWiFiLevel( int8 wifiRSSI )
{
    if( wifiRSSI==WIFI_LEVEL_0)
        return 0;
    if( (wifiRSSI>WIFI_LEVEL_0)&&(wifiRSSI<WIFI_LEVEL_1))
        return 1;
    if( (wifiRSSI>=WIFI_LEVEL_1)&&(wifiRSSI<WIFI_LEVEL_2))
        return 2;    
    if( (wifiRSSI>=WIFI_LEVEL_2)&&(wifiRSSI<WIFI_LEVEL_3))
        return 3;
    if( (wifiRSSI>=WIFI_LEVEL_3)&&(wifiRSSI<WIFI_LEVEL_4))
        return 4;
    if( (wifiRSSI>=WIFI_LEVEL_4)&&(wifiRSSI<WIFI_LEVEL_5))
        return 5;
    if( (wifiRSSI>=WIFI_LEVEL_5)&&(wifiRSSI<WIFI_LEVEL_6))
        return 6;
    if( wifiRSSI>=WIFI_LEVEL_6 )
        return 7;
    return RET_FAILED;
}
/****************************************************************
*       FunctionName      :     GAgent_BaseTick
*       Description       :     the function will return 1 second at least
*       return            :     return 1 second at least. 
*      Add by Alex.lin      --2015-04-23
****************************************************************/
uint32 GAgent_BaseTick()
{
    static uint32 preTime = 0;
    uint32 cTime=0,dTime=0;

    if(0 == preTime)
        preTime = GAgent_GetDevTime_S();//run in first time;

    cTime = GAgent_GetDevTime_S();

    if(cTime >= preTime)
    {
        dTime = cTime - preTime;
    }
    else
    {
        //memory overflow
        dTime = cTime + ( (~preTime)+1 );
    }
    preTime = cTime;
    
    if(dTime < 1)
    {
        return 0;
    }
    return dTime;
}

/****************************************************************
*       FunctionName      :     GAgent_Tick
*       Description       :     GAgent runing Tick.
*       return            :     return NULL. 
*      Add by Alex.lin      --2015-04-23
****************************************************************/
void GAgent_Tick( pgcontext pgc )
{
    uint32 dTime=0;

    dTime = GAgent_BaseTick();
    if( dTime<1 )
    {
    	//GAgent_Printf(GAGENT_WARNING, "dTime is:%d\r\n", dTime);
        return ;
    }

    //GAgent_Printf(GAGENT_WARNING, "##########dTime is:%d\r\n", dTime);
	
    GAgent_DevTick();
    GAgent_CloudTick( pgc,dTime );
    GAgent_LocalTick( pgc,dTime ); //Nik.chen mark for test
    GAgent_LanTick( pgc,dTime );
    GAgent_WiFiEventTick( pgc,dTime );
    GAgent_RefreshIPTick( pgc,dTime );
    GAgent_BigDataTick( pgc );
}
#if 0 //Nik.chen mark new
void GAgent_CheckConfigData( GAGENT_CONFIG_S *p_newgc )
{
    uint32 OLD_MAGIC_NUMBER=0x12345678;
    GAgent_OldCONFIG_S *p_oldgc=NULL;
    if( OLD_MAGIC_NUMBER==(p_newgc->magicNumber) )
    {
        uint8 retime=0;
        while( NULL==p_oldgc )
        {
            retime++;
            p_oldgc = (GAgent_OldCONFIG_S*)malloc(sizeof(GAgent_OldCONFIG_S));
            if( retime>10 )
            {
                return ;
            }
            msleep(100);
        }
        memset( p_oldgc,0,sizeof(GAgent_OldCONFIG_S) );
        Dev_GAgentGetOldConfigData( p_oldgc );
        //以下做结构体搬移操作
        p_newgc->magicNumber = GAGENT_MAGIC_NUM;
        p_newgc->flag = p_oldgc->flag;
        p_newgc->airkiss_value = p_oldgc->airkiss_value;

        //老结构体wifipasscode定义为16个字节
        memcpy( p_newgc->wifipasscode,p_oldgc->wifipasscode,16 );
        p_newgc->wifipasscode[16] = '\0';

        memcpy( p_newgc->wifi_ssid,p_oldgc->wifi_ssid,SSID_LEN_MAX );
        p_newgc->wifi_ssid[SSID_LEN_MAX] = '\0';
        
        memcpy( p_newgc->wifi_key,p_oldgc->wifi_key,WIFIKEY_LEN_MAX );
        p_newgc->wifi_key[WIFIKEY_LEN_MAX] = '\0';
        
        memcpy( p_newgc->DID,p_oldgc->Cloud_DId,DID_LEN );
        memcpy( p_newgc->FirmwareVerLen,p_oldgc->FirmwareVerLen,2 );
        memcpy( p_newgc->FirmwareVer,p_oldgc->FirmwareVer,FIRMWARELEN );
        p_newgc->FirmwareVer[FIRMWARELEN] = '\0';

        memcpy( p_newgc->old_did,p_oldgc->old_did,DID_LEN );
        //老结构体wifipasscode定义为16个字节
        memcpy( p_newgc->old_wifipasscode,p_oldgc->old_wifipasscode,16 );
        p_newgc->old_wifipasscode[16] = '\0';
        
        memcpy( p_newgc->old_productkey,p_oldgc->old_productkey,PK_LEN+1 );
        //老结构体ip大小定义为17个字节.
        memcpy( p_newgc->m2m_ip,p_oldgc->m2m_ip,17 );
        memcpy( p_newgc->GServer_ip,p_oldgc->api_ip,17 );

        memcpy( p_newgc->cloud3info.jdinfo.product_uuid,p_oldgc->jdinfo.product_uuid,PRODUCTUUID_LEN );
        p_newgc->cloud3info.jdinfo.product_uuid[PRODUCTUUID_LEN] = '\0';

        memcpy( p_newgc->cloud3info.jdinfo.feed_id,p_oldgc->jdinfo.feed_id,FEEDID_LEN );
        p_newgc->cloud3info.jdinfo.feed_id[FEEDID_LEN]='\0';

        p_newgc->cloud3info.jdinfo.ischanged = p_oldgc->jdinfo.ischanged;
        p_newgc->cloud3info.jdinfo.tobeuploaded = p_oldgc->jdinfo.tobeuploaded;

        if( 0==GAgent_DevSaveConfigData( p_newgc ) )
        {
            GAgent_Printf( GAGENT_INFO," %s %d updata GAGENT_CONFIG_S ok.",__FUNCTION__,__LINE__ );
        }
        else
        {
            GAgent_Printf( GAGENT_INFO," %s %d updata GAGENT_CONFIG_S fail.",__FUNCTION__,__LINE__ );   
        }
        free( p_oldgc );
    }
}
#endif
