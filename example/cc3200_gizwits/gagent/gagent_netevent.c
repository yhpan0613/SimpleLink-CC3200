#include "gagent.h"
#include "cloud.h"
#include "platform.h"
extern unsigned int switchSmartlink;

void GAgent_WiFiInit( pgcontext pgc )
{
    uint16 tempWiFiStatus=0;
    tempWiFiStatus = pgc->rtinfo.GAgentStatus;

    #if 1
    GAgent_Printf( GAGENT_INFO,"onboarding:%d, apMode:%d, staMode:%d", ( tempWiFiStatus&WIFI_MODE_ONBOARDING ),(tempWiFiStatus&WIFI_MODE_AP),(tempWiFiStatus&WIFI_MODE_STATION));
    //return;
    #endif

    #if 0
    if( (pgc->gc.flag & XPG_CFG_FLAG_CONFIG) ==XPG_CFG_FLAG_CONFIG )
    {
        GAgent_Printf( GAGENT_INFO," in STA mode and open airlink sould reset !!");
        GAgent_DevReset();
    }
    #endif
    
   if( ((pgc)->gc.flag & XPG_CFG_FLAG_CONFIG_AP) == XPG_CFG_FLAG_CONFIG_AP )
    {
        GAgent_Printf( GAGENT_DEBUG," GAgent XPG_CFG_FLAG_CONFIG_AP." );
        GAgent_DevCheckWifiStatus( WIFI_MODE_ONBOARDING,1 );
    }
    
    if((GAgent_DRVGetWiFiMode(pgc)== GAgent_DRVBootConfigWiFiMode()))
    {
        GAgent_Printf( GAGENT_INFO,"In Station mode");
        GAgent_Printf( GAGENT_INFO,"SSID:%s,KEY:%s",pgc->gc.wifi_ssid,pgc->gc.wifi_key );
        tempWiFiStatus |=WIFI_MODE_STATION;

        GAgent_Printf( GAGENT_INFO,"airkiss_value=%d !!!", pgc->gc.airkiss_value);
        if(0x53 == pgc->gc.airkiss_value)
            tempWiFiStatus |= GAgent_DRVWiFi_StationCustomModeStart( NULL, NULL, tempWiFiStatus );
        else if(0x55 == pgc->gc.airkiss_value)
        {
            
             tempWiFiStatus |= GAgent_DRVWiFi_StationCustomModeStart( pgc->gc.wifi_ssid,pgc->gc.wifi_key, tempWiFiStatus );
        }
        else if(0x13 == pgc->gc.airkiss_value)
        {
        pgc->gc.airkiss_value = 0x23;
        GAgent_DevSaveConfigData(&(pgc->gc));
        
        //ResetNwp();
        //ConfigureSimpleLinkToDefaultState2();
        GAgent_DevReset();
        //CC3200_SwitchSTAMode();
        //tempWiFiStatus |= GAgent_DRVWiFi_StationCustomModeStart( pgc->gc.wifi_ssid,pgc->gc.wifi_key, 0 );
       // setconnFlag(2); 
        }
        else if(0x23 == pgc->gc.airkiss_value)
        {
        pgc->gc.airkiss_value = 0x55;
        GAgent_DevSaveConfigData(&(pgc->gc));
        //  ResetNwp();
        
        //ResetNwp();
        //CC3200_SwitchSTAMode();
        tempWiFiStatus |= GAgent_DRVWiFi_StationCustomModeStart( pgc->gc.wifi_ssid,pgc->gc.wifi_key, 0 );
       // setconnFlag(2); 
        }
        else if(0x77 == pgc->gc.airkiss_value)
        {
           
            pgc->gc.airkiss_value = 0x55;
            GAgent_DevSaveConfigData( &(pgc->gc) );
            GAgent_DevGetConfigData( &(pgc->gc) );
            setStaMode();
            GAgent_SetWiFiStatus( pgc,WIFI_MODE_TEST,1 );
            GAgent_DRVWiFiStartScan();
        }
    }
    else
    {
        #if 0
         long lRetVal = -1;
         lRetVal = testSwitchStaMode();
        GAgent_Printf( GAGENT_CRITICAL,"lRetVal =%d \n\r", lRetVal);
        #else
         GAgent_DevCheckWifiStatus( WIFI_MODE_ONBOARDING,1 );
         GAgent_Printf( GAGENT_INFO,"airkiss_value=%d !!!", pgc->gc.airkiss_value);
       
        GAgent_Printf( GAGENT_CRITICAL,"enter AP mode");
        tempWiFiStatus |=WIFI_MODE_AP;
        tempWiFiStatus |= GAgent_DRV_WiFi_SoftAPModeStart( pgc->minfo.ap_name,AP_PASSWORD,tempWiFiStatus );
       #endif
    }
}
uint16 GAgent_DevCheckWifiStatus( uint16 wifistatus,int8 flag  )
{
    static uint16 halWiFiStatus=0;
    
    if( 0xFFFF==wifistatus )
    {
        GAgent_Printf( GAGENT_DEBUG," GAgent Get Hal wifiStatus :%04X ",halWiFiStatus );
       return halWiFiStatus;
    }
    else
    {
        if( 1==flag )
        {
            halWiFiStatus |=wifistatus;
            GAgent_Printf( GAGENT_DEBUG,"GAgent Hal Set wifiStatus%04X",wifistatus);
        }
        else
        {   //对应位清零
            uint16 tempstatus=0;
            tempstatus = ( 0xFFFF - wifistatus );
            halWiFiStatus &=tempstatus;
            GAgent_Printf( GAGENT_DEBUG,"GAgent Hal ReSet wifiStatus%04X",wifistatus);
        }
    }
    GAgent_Printf( GAGENT_DEBUG," GAgent Hal wifiStatus :%04X ",halWiFiStatus );
    return halWiFiStatus;
}
void GAgentSetLedStatus( uint16 gagentWiFiStatus )
{
    static uint32 Router_Connect_count = 0;
    static uint8  Cloud_Connect_flag = 0;
    static uint32 Cloud_Connect_count = 0;
    static uint32 Station_Mode_count = 0;
    static uint8  led_flag=0;
    if( (gagentWiFiStatus&0x37)!= (WIFI_CLOUD_CONNECTED|WIFI_STATION_CONNECTED|WIFI_MODE_STATION) )
    {
        GAgent_Printf( GAGENT_INFO,"gagentWiFiStatus&0x37:%02X",(gagentWiFiStatus&0x37) );
        Cloud_Connect_count=0;
    }
    switch( gagentWiFiStatus&0x37 )
    {
        case WIFI_MODE_AP: 
        case (WIFI_MODE_AP|WIFI_MODE_ONBOARDING):
            GAgent_Printf( GAGENT_INFO,"WIFI_MODE_AP");
            GAgent_DevLED_Red( 1 );
            GAgent_DevLED_Green( 0 ); 
            break;
        case WIFI_MODE_STATION:
        GAgent_Printf( GAGENT_INFO,"WIFI_MODE_STATION");
            Station_Mode_count++;
            GAgent_DevLED_Red( 1 );
            if( Station_Mode_count%4==0 )
            {
                GAgent_DevLED_Green( led_flag );
                led_flag =!led_flag;
            }
            break;
        case (WIFI_STATION_CONNECTED|WIFI_MODE_STATION):          
            GAgent_Printf( GAGENT_INFO,"WIFI_STATION_CONNECTED|WIFI_MODE_STATION");
            GAgent_DevLED_Red( 1 );
            GAgent_DevLED_Green( 1 );      
            break;
        case ( WIFI_CLOUD_CONNECTED|WIFI_STATION_CONNECTED|WIFI_MODE_STATION ):
            if( Cloud_Connect_count < ONE_MINUTE*10 )
            {
                GAgent_Printf( GAGENT_INFO,"WIFI_CLOUD_CONNECTED|WIFI_STATION_CONNECTED|WIFI_MODE_STATION");
                Cloud_Connect_count++;
                if( Cloud_Connect_count%4 == 0)
                {
                    Cloud_Connect_flag = !Cloud_Connect_flag;           
                }
                //GAgent_Printf( GAGENT_INFO,"Cloud_Connect_count = %d,Cloud_Connect_flag=%d",Cloud_Connect_count,Cloud_Connect_flag);    
                GAgent_DevLED_Red( Cloud_Connect_flag );
                GAgent_DevLED_Green( !Cloud_Connect_flag );
            }
            else
            {
                GAgent_DevLED_Red( 0 );
                GAgent_DevLED_Green( 0 );  
            }
            break;
        default: //don't connect router           
            GAgent_Printf( GAGENT_INFO," unkonw GAgent Status ");
            Router_Connect_count++;            
            GAgent_DevLED_Red( (Router_Connect_count%2) );
            GAgent_DevLED_Green( (Router_Connect_count%2) );
            break;
    }
   
}

/****************************************************************
FunctionName    :   GAgentFindTestApHost.
Description     :   find the test ap host,like:GIZWITS_TEST_*
NetHostList_str :   GAgent wifi scan result .
return          :   1-GIZWITS_TEST_1
                    2-GIZWITS_TEST_2
                    fail :0.
Add by Alex.lin         --2015-05-06
****************************************************************/
uint8 GAgentFindTestApHost( NetHostList_str *pAplist )
{
    int16 i=0;
    int8 apNum=0,ret=0;

    for( i=0;i<pAplist->ApNum;i++ )
    {
        if( 0==memcmp(pAplist->ApList[i].ssid,(int8 *)GAGENT_TEST_AP1,strlen(GAGENT_TEST_AP1)) )
        {
            apNum=1;
        }
        if( 0==memcmp(pAplist->ApList[i].ssid,(int8 *)GAGENT_TEST_AP2,strlen(GAGENT_TEST_AP2)) )
        {
            /* 两个热点都能找到 */
            if( 1==apNum)
              apNum=3;
            else
              apNum=2;
        }
    }
    switch( apNum )
    {
        /* only the GIZWITS_TEST_1 */
        case 1:
            ret=1;
        break;
        /* only the GIZWITS_TEST_2 */
        case 2:
            ret=2;
        break;
        /* both of the test ap */
        case 3:
           srand(GAgent_GetDevTime_MS());
           ret = rand()%100;
           ret = (ret%2)+1; 
        break;
        default:
        ret =0;
        break;
    }
  //  if(NULL !=  pAplist->ApList)
  //          free( pAplist->ApList); 

    return ret;
}
void GAgent_EnterNullMode( pgcontext pgc )
{
      GAgent_Printf( GAGENT_INFO,"%s %d",__FUNCTION__,__LINE__ );
	GAgent_DevCheckWifiStatus( WIFI_STATION_CONNECTED,0);
	GAgent_DevCheckWifiStatus( WIFI_MODE_STATION,0);
	GAgent_SetWiFiStatus( pgc,WIFI_STATION_CONNECTED,0);
	GAgent_SetWiFiStatus( pgc,WIFI_MODE_STATION,0);
	GAgent_SetWiFiStatus( pgc,WIFI_CLOUD_CONNECTED,0);
	GAgent_DevCheckWifiStatus( WIFI_MODE_ONBOARDING,1 );
	setMode(0);
	GAgent_DRVWiFi_StationDisconnect();
}
void  GAgent_WiFiEventTick( pgcontext pgc,uint32 dTime_s )
{
    uint16 newStatus=0;
    uint16 gagentWiFiStatus=0;
    static uint32 gagentOnboardingTime=0;
    uint32 preTime;
    static uint32 nowTime = 0;
    uint32 dTime;

    gagentWiFiStatus = ( (pgc->rtinfo.GAgentStatus)&(LOCAL_GAGENTSTATUS_MASK) ) ;
    newStatus = GAgent_DevCheckWifiStatus( 0xffff,1 );
    GAgent_Printf( GAGENT_INFO,"wifiStatus : %04x new:%04x", gagentWiFiStatus,newStatus );
    GAgent_Printf( GAGENT_INFO,"onboarding :%d, apMode:%d, staMode:%d", ( newStatus&WIFI_MODE_ONBOARDING ),(newStatus&WIFI_MODE_AP),(newStatus&WIFI_MODE_STATION));
   
    if( (gagentWiFiStatus&WIFI_MODE_AP) != (newStatus&WIFI_MODE_AP) )
    {
        if( newStatus&WIFI_MODE_AP )
        {
            //WIFI_MODE_AP UP
            GAgent_SetWiFiStatus( pgc,WIFI_MODE_AP,1 );
             GAgent_Printf( GAGENT_INFO,"WIFI_MODE_AP UP." );
        }
        else
        {
            //WIFI_MODE_AP DOWN
            GAgent_SetWiFiStatus( pgc,WIFI_MODE_AP,0 );
             GAgent_Printf( GAGENT_INFO,"WIFI_MODE_AP DOWN." );
        }
        pgc->rtinfo.waninfo.wanclient_num=0;
        pgc->ls.tcpClientNums=0;
        if( pgc->rtinfo.waninfo.CloudStatus == CLOUD_CONFIG_OK )
        {
        GAgent_SetCloudServerStatus( pgc,MQTT_STATUS_START );
        }
        else
        {
            GAgent_SetCloudConfigStatus( pgc,CLOUD_INIT );
        }
        newStatus = GAgent_DevCheckWifiStatus( WIFI_CLOUD_CONNECTED,0 );
        gagentWiFiStatus = ( (pgc->rtinfo.GAgentStatus)&(LOCAL_GAGENTSTATUS_MASK) ) ;
        //GAgent_SetWiFiStatus( pgc,WIFI_CLOUD_CONNECTED,0 );
    }
    if( (gagentWiFiStatus&WIFI_MODE_STATION) != (newStatus&WIFI_MODE_STATION) )
    {
        if( newStatus&WIFI_MODE_STATION )
        {
            //WIFI_MODE_STATION UP
            GAgent_Printf( GAGENT_INFO,"WIFI_MODE_STATION UP." );
            GAgent_SetWiFiStatus( pgc,WIFI_MODE_STATION,1 );
        }
        else
        {
            //WIFI_MODE_STATION DOWN
            GAgent_Printf( GAGENT_INFO,"WIFI_MODE_STATION Down." );
            GAgent_SetWiFiStatus( pgc,WIFI_MODE_STATION,0 );
            newStatus = GAgent_DevCheckWifiStatus( WIFI_CLOUD_CONNECTED,0 );
            if( pgc->rtinfo.waninfo.CloudStatus == CLOUD_CONFIG_OK )
            {
            GAgent_SetCloudServerStatus( pgc,MQTT_STATUS_START );
            }
            else
            {
                GAgent_SetCloudConfigStatus( pgc,CLOUD_INIT );
            }
        }
        pgc->rtinfo.waninfo.wanclient_num=0;
        pgc->ls.tcpClientNums=0;
        gagentWiFiStatus = ( (pgc->rtinfo.GAgentStatus)&(LOCAL_GAGENTSTATUS_MASK) ) ;
    }
    if( (gagentWiFiStatus&WIFI_MODE_ONBOARDING) != (newStatus&WIFI_MODE_ONBOARDING) )
    {
        if( newStatus&WIFI_MODE_ONBOARDING )
        {
            uint16 tempWiFiStatus=0;
            //WIFI_MODE_ONBOARDING UP
            //pgc->gc.flag |= XPG_CFG_FLAG_CONFIG_AP; //Nik.chen mark for test?
            GAgent_DevSaveConfigData( &(pgc->gc) );
            GAgent_Printf( GAGENT_INFO,"WIFI_MODE_ONBOARDING UP." );
            if( (newStatus&WIFI_STATION_CONNECTED) == WIFI_STATION_CONNECTED
                   || (newStatus&WIFI_MODE_STATION) == WIFI_MODE_STATION
                )
            {
                GAgent_Printf( GAGENT_INFO,"STA mode, disconnect." );
                tempWiFiStatus = GAgent_DRVWiFi_StationDisconnect();
                //setDnsFlag(FALSE);
            }
            if( !(newStatus&WIFI_MODE_AP)) //Nik.chen //&&((0x53 != pgc->gc.airkiss_value))
            {
                tempWiFiStatus |= GAgent_DRV_WiFi_SoftAPModeStart( pgc->minfo.ap_name,AP_PASSWORD,tempWiFiStatus ); //Nik chen mark
                //GAgent_DevReset();
                #if 1
                RecreatLanSocket(pgc);
                #endif

            }
            GAgent_SetWiFiStatus( pgc,WIFI_MODE_ONBOARDING,1 );
            gagentOnboardingTime = GAGENT_ONBOARDING_TIME;
        }
        else
        {
            //WIFI_MODE_ONBOARDING DOWN
            pgc->gc.flag &=~ XPG_CFG_FLAG_CONFIG_AP;
            GAgent_DevSaveConfigData( &(pgc->gc) );
            GAgent_Printf( GAGENT_INFO,"WIFI_MODE_ONBOARDING DOWN." );
            if( gagentOnboardingTime <= 0 )
            { 
                GAgent_Printf( GAGENT_INFO,"WIFI_MODE_ONBOARDING Time out ...");
                //GAgent_EnterNullMode( pgc );
            }
            else
            {
              GAgent_Printf( GAGENT_INFO,"Receive OnBoarding data.");
              // GAgent_DRVWiFi_APModeStop( pgc );
              pgc->ls.onboardingBroadCastTime = SEND_UDP_DATA_TIMES;
              pgc->gc.flag |=XPG_CFG_FLAG_CONFIG;
              //setMode(0);
              GAgent_DevSaveConfigData( &(pgc->gc) );
              GAgent_WiFiInit( pgc );
            }
            GAgent_SetWiFiStatus( pgc,WIFI_MODE_ONBOARDING,0 );
            gagentOnboardingTime = 0;
        }
        if( pgc->rtinfo.waninfo.CloudStatus == CLOUD_CONFIG_OK )
        {
            GAgent_SetCloudServerStatus( pgc, MQTT_STATUS_START );
        }
        else
        {
            GAgent_SetCloudConfigStatus( pgc,CLOUD_INIT );
        }
        pgc->rtinfo.waninfo.wanclient_num=0;
        pgc->ls.tcpClientNums=0;
        newStatus = GAgent_DevCheckWifiStatus( WIFI_CLOUD_CONNECTED,0 );
        gagentWiFiStatus = ( (pgc->rtinfo.GAgentStatus)&(LOCAL_GAGENTSTATUS_MASK) ) ;
    }
    if( (gagentWiFiStatus&WIFI_STATION_CONNECTED) != (newStatus & WIFI_STATION_CONNECTED) )
    {
        if( newStatus&WIFI_STATION_CONNECTED )
        {
            GAgent_Printf( GAGENT_INFO," WIFI_STATION_CONNECTED UP" );
            pgc->rtinfo.waninfo.ReConnectMqttTime = GAGENT_MQTT_TIMEOUT;
            pgc->rtinfo.waninfo.ReConnectHttpTime = GAGENT_HTTP_TIMEOUT;
            if( !(newStatus&WIFI_MODE_ONBOARDING) )
            {
                if( (newStatus&WIFI_MODE_AP)==WIFI_MODE_AP )
                {
                    GAgent_DRVWiFi_APModeStop( pgc );
                    //GAgent_SetWiFiStatus( pgc,WIFI_MODE_AP,0 );
                   // GAgent_DevCheckWifiStatus( WIFI_MODE_AP,0 );
                }
            }
            //WIFI_STATION_CONNECTED UP
            GAgent_Printf( GAGENT_WARNING,"GAgent_DRVWiFiPowerScan!!!");
            GAgent_DRVWiFiPowerScan( pgc );
            GAgent_SetWiFiStatus( pgc,WIFI_STATION_CONNECTED,1 );
        }
        else
        {
            //WIFI_STATION_CONNECTED DOWN
            GAgent_Printf( GAGENT_INFO," WIFI_STATION_CONNECTED Down" );
            GAgent_SetWiFiStatus( pgc,WIFI_STATION_CONNECTED,0 );
            GAgent_SetWiFiStatus( pgc,WIFI_CLIENT_ON,0 );
            GAgent_SetWiFiStatus( pgc,WIFI_CLOUD_CONNECTED,0 ); /* 路由掉线，设置云端未连接状态 */
            GAgent_SetCloudServerStatus( pgc,MQTT_STATUS_START );
            newStatus = GAgent_DevCheckWifiStatus( WIFI_CLOUD_CONNECTED,0 );
        }
        pgc->rtinfo.waninfo.wanclient_num=0;
        pgc->ls.tcpClientNums=0;
        gagentWiFiStatus = ( (pgc->rtinfo.GAgentStatus)&(LOCAL_GAGENTSTATUS_MASK) ) ;
    }
    if( (gagentWiFiStatus&WIFI_CLOUD_CONNECTED) != (newStatus & WIFI_CLOUD_CONNECTED) )
    {
        if( newStatus&WIFI_CLOUD_CONNECTED )
        {
            //WIFI_CLOUD_CONNECTED UP
            GAgent_Printf( GAGENT_INFO," WIFI_CLOUD_CONNECTED Up." );
            GAgent_SetWiFiStatus( pgc,WIFI_CLOUD_CONNECTED,1 );
        }
        else
        {
            //WIFI_CLOUD_CONNECTED DOWN
            GAgent_Printf( GAGENT_INFO," WIFI_CLOUD_CONNECTED Down." );
            pgc->rtinfo.waninfo.wanclient_num=0;
            GAgent_SetCloudServerStatus( pgc,MQTT_STATUS_START );
            GAgent_SetWiFiStatus( pgc,WIFI_CLOUD_CONNECTED,0 );
        }
        gagentWiFiStatus = ( (pgc->rtinfo.GAgentStatus)&(LOCAL_GAGENTSTATUS_MASK) ) ;
    }
    if( gagentWiFiStatus&WIFI_MODE_TEST )//test mode
    {
        static int8 cnt=0;
        int8 ret =0;
        NetHostList_str *aplist=NULL;
		
        if( 0 == pgc->rtinfo.scanWifiFlag )
        {
        pgc->rtinfo.testLastTimeStamp+=dTime_s;

            if( cnt>=18 )
        {
            cnt=0;
		 //GAgent_DRVWiFiStopScan( );
	      GAgent_Printf( GAGENT_INFO,"Exit Test Mode...");
            GAgent_SetWiFiStatus( pgc,WIFI_MODE_TEST,0 );
            GAgent_EnterNullMode( pgc );
        }

            if( pgc->rtinfo.testLastTimeStamp >= 10 )
        {
            cnt++;
            pgc->rtinfo.testLastTimeStamp = 0;
            GAgent_Printf( GAGENT_INFO,"IN TEST MODE...");
            if( !(newStatus&WIFI_MODE_AP))
                GAgent_DRVWiFiStartScan();
        }

        aplist = GAgentDRVWiFiScanResult( aplist );
            if( NULL==aplist )
            {
                ret = 0;
            }
            else
            {
                if( aplist->ApNum <= 0 )
                {
                    ret = 0;
			
                }
                else
                {
        ret = GAgentFindTestApHost( aplist );
                }
            }
        }
        if( ret>0 )
        {
             uint16 tempWiFiStatus=0;
             pgc->rtinfo.scanWifiFlag = 1;
             cnt=0;
		GAgent_Printf( GAGENT_INFO,"FindTestApHost..");
             //GAgent_DRVWiFiStopScan( );
             if( 1==ret )
             {
                GAgent_Printf( GAGENT_INFO,"Connect to TEST AP 1:%s",GAGENT_TEST_AP1 );
                strcpy( pgc->gc.wifi_ssid,GAGENT_TEST_AP1 );
                strcpy( pgc->gc.wifi_key,GAGENT_TEST_AP_PASS );
                GAgent_DevSaveConfigData( &(pgc->gc) );
                tempWiFiStatus |= GAgent_DRVWiFi_StationCustomModeStart( GAGENT_TEST_AP1,GAGENT_TEST_AP_PASS,pgc->rtinfo.GAgentStatus );
             }
             else
             {
                GAgent_Printf( GAGENT_INFO,"Connect to TEST AP 2:%s",GAGENT_TEST_AP2 );
                strcpy( pgc->gc.wifi_ssid,GAGENT_TEST_AP2 );
                strcpy( pgc->gc.wifi_key,GAGENT_TEST_AP_PASS );
                GAgent_DevSaveConfigData( &(pgc->gc) );
                tempWiFiStatus |= GAgent_DRVWiFi_StationCustomModeStart( GAGENT_TEST_AP2,GAGENT_TEST_AP_PASS,pgc->rtinfo.GAgentStatus );
             }

             #if 1
            //GAgent_SetWiFiStatus( pgc,WIFI_MODE_BINDING,0);
             RecreatLanSocket(pgc);
             #endif
        }
	else
		GAgent_Printf( GAGENT_INFO,"not FindTestApHost...");
        gagentWiFiStatus = ( (pgc->rtinfo.GAgentStatus)&(LOCAL_GAGENTSTATUS_MASK) ) ;
    }
    pgc->rtinfo.wifiLastScanTime+=dTime_s;
    if( gagentWiFiStatus&WIFI_STATION_CONNECTED )
    {
        static int8 tempwifiRSSI=0;
        int8 wifiRSSI=0;
        uint16 wifiLevel=0;
        if( pgc->rtinfo.wifiLastScanTime >= GAGENT_STA_SCANTIME )
        {
            pgc->rtinfo.wifiLastScanTime=0;
            GAgent_Printf( GAGENT_INFO,"PowerScan start to scan wifi ...");
            GAgent_DRVWiFiPowerScan( pgc );
        }
        wifiRSSI = GAgent_DRVWiFiPowerScanResult( pgc );
        GAgent_Printf( GAGENT_INFO,"wifiRSSI=%d",wifiRSSI);
        if( abs( wifiRSSI-tempwifiRSSI )>=10 )
        {
            tempwifiRSSI = wifiRSSI;
            wifiLevel = GAgent_GetStaWiFiLevel( wifiRSSI );
            gagentWiFiStatus =gagentWiFiStatus|(wifiLevel<<8);
            pgc->rtinfo.GAgentStatus = gagentWiFiStatus;
            GAgent_Printf( GAGENT_INFO,"SSID power:%d level:%d wifistatus:%04x",wifiRSSI,wifiLevel,gagentWiFiStatus );
        }
        gagentWiFiStatus = ( (pgc->rtinfo.GAgentStatus)&(LOCAL_GAGENTSTATUS_MASK) ) ;
    }
    if( gagentWiFiStatus&WIFI_MODE_AP )  
    {
        NetHostList_str *pAplist=NULL;
        int32 i=0;
        #if 0 //Nik.chen shoud modify
        if( pgc->rtinfo.wifiLastScanTime >=GAGENT_AP_SCANTIME )
        {
            pgc->rtinfo.wifiLastScanTime=0;
            GAgent_Printf( GAGENT_WARNING,"AP mode, start scan!!!");
           GAgent_DRVWiFiStartScan( );
        }
        #endif
        pAplist = GAgentDRVWiFiScanResult( pAplist );
        if( NULL == pAplist )
        {
           GAgent_Printf( GAGENT_WARNING,"pAplist is NULL!");
        }
        else
        {
           #if 0
            if( (pgc->rtinfo.aplist.ApList)!=NULL )
            {
                GAgent_Printf( GAGENT_CRITICAL,"free xpg aplist...");
                free( (pgc->rtinfo.aplist.ApList) );
            }
            #endif
            if( (pgc->rtinfo.aplist.ApList) ==NULL && ( pAplist->ApNum>0 ))
            {
                GAgent_Printf( GAGENT_CRITICAL,"malloc xpg aplist...");
                 pgc->rtinfo.aplist.ApNum = pAplist->ApNum;
                (pgc->rtinfo.aplist.ApList) = (ApHostList_str *)malloc( (pAplist->ApNum)*sizeof(ApHostList_str) );
            }
            if( pAplist->ApNum>0 )
            {
                if( (pgc->rtinfo.aplist.ApList)!=NULL )
                {
                    for( i=0;i<pAplist->ApNum;i++ )
                    {
                        strcpy( (char *)pgc->rtinfo.aplist.ApList[i].ssid, (char *)pAplist->ApList[i].ssid);
                        pgc->rtinfo.aplist.ApList[i].ApPower = pAplist->ApList[i].ApPower;
                    }
                }
            }
        }
        gagentWiFiStatus = ( (pgc->rtinfo.GAgentStatus)&(LOCAL_GAGENTSTATUS_MASK) ) ;
    }
    if( (gagentWiFiStatus&WIFI_MODE_ONBOARDING)==WIFI_MODE_ONBOARDING )
    {
        if( (gagentWiFiStatus&WIFI_MODE_TEST)==WIFI_MODE_TEST )
        {
            GAgent_Printf( GAGENT_INFO," GAgent in WIFI_MODE_TEST|WIFI_MODE_ONBOARDING !!! ");
        }
        else
        {
            GAgent_Printf( GAGENT_INFO,"GAGENT_ONBOARDING_TIME = %d",gagentOnboardingTime );
        if( gagentOnboardingTime>0 )
        {
            gagentOnboardingTime--;
        }
        else
        {
            GAgent_DevCheckWifiStatus( WIFI_MODE_ONBOARDING,0 );
                GAgent_Printf( GAGENT_INFO,"GAGENT_ONBOARDING_TIME Time out!");
            GAgent_Printf( GAGENT_DEBUG,"file:%s function:%s line:%d ",__FILE__,__FUNCTION__,__LINE__ );
            }
        }
        gagentWiFiStatus = ( (pgc->rtinfo.GAgentStatus)&(LOCAL_GAGENTSTATUS_MASK) ) ;
    }
    #if 1 //Nik.chen for test
    if( gagentWiFiStatus&WIFI_MODE_STATION )
    {
         if(3 == getconnFlag())
         {
             pgc->rtinfo.waninfo.reconnInteralTime +=dTime_s;
             GAgent_Printf(GAGENT_DEBUG, "reconnInteralTime=%d", (pgc->rtinfo.waninfo.reconnInteralTime));
             
            if(pgc->rtinfo.waninfo.reconnInteralTime >= 10)
            {
                 pgc->rtinfo.waninfo.reconnInteralTime  = 0;
                 GAgent_Printf( GAGENT_INFO," reconnect, airlink mode,start again! ");
                 CC3200_ReconAP_Smartconfig();
            }
         }
        else if(2 == getconnFlag())
         {
            pgc->rtinfo.waninfo.reconnInteralTime +=dTime_s;
            GAgent_Printf(GAGENT_DEBUG, "reconnInteralTime=%d", (pgc->rtinfo.waninfo.reconnInteralTime));
            if(pgc->rtinfo.waninfo.reconnInteralTime >= 10)
            {
            GAgent_Printf( GAGENT_INFO," reconnect, softAp mode! ");
            pgc->rtinfo.waninfo.reconnInteralTime  = 0;
            CC3200_ReconAP(pgContextData->gc.wifi_ssid,pgContextData->gc.wifi_key);
            }
        }
      }
    #endif

    GAgent_LocalSendGAgentstatus(pgc,dTime_s); //Nik.chen mark
    GAgentSetLedStatus( gagentWiFiStatus );

    if(switchSmartlink == 1)
    {
    #if 0 //Nik.chen test smart config
     int8 timeout;
    timeout = 60;
    static int8 resetFlag = 0;
    uint16 tempWiFiStatus=0;
    tempWiFiStatus = pgc->rtinfo.GAgentStatus;
    pgc->gc.flag  &=~ XPG_CFG_FLAG_CONFIG;
    GAgent_DevLED_Red( 0 );
    GAgent_Printf( GAGENT_CRITICAL,"openairlink !!!\r\n");
    #if 1 //Nik.chen add
    pgc->gc.flag &=~ XPG_CFG_FLAG_CONFIG_AP;
    #endif
    GAgent_OpenAirlink(timeout);
    if((tempWiFiStatus&WIFI_MODE_AP))
    {
    resetFlag = 1;
    GAgent_Printf( GAGENT_INFO,"ap mode,should reset...");
    }
    while( timeout )
    {
        timeout--;
        sleep(1); //Nik.chen MAP_UtilsDelay ->
        if( (pgc->gc.flag & XPG_CFG_FLAG_CONFIG) ==XPG_CFG_FLAG_CONFIG )
        {    
            GAgent_Printf( GAGENT_INFO,"AirLink result ssid:%s key:%s",pgc->gc.wifi_ssid,pgc->gc.wifi_key );
            switchSmartlink = 0;
            tempWiFiStatus |=WIFI_MODE_STATION;
            pgc->gc.flag |= XPG_CFG_FLAG_CONNECTED;
            pgc->ls.onboardingBroadCastTime = SEND_UDP_DATA_TIMES;
            pgc->gc.airkiss_value = 0x53;
            GAgent_DevSaveConfigData( &(pgc->gc) );  
            GAgent_Printf( GAGENT_INFO,"After airkiss_value=%d !!!", pgc->gc.airkiss_value); 
             if(resetFlag)
            {
                GAgent_Printf( GAGENT_INFO,"ap mode, reset now...");
                GAgent_DevReset();
            }
            GAgent_WiFiInit( pgc );
            CreateUDPBroadCastServer( pgc );
            break;
        }
        GAgent_DevLED_Green( (timeout%2) );
    }      
    GAgent_DevCheckWifiStatus( WIFI_MODE_ONBOARDING,0 );
    if( timeout<=0 )
    {
            GAgent_DevCheckWifiStatus( WIFI_MODE_ONBOARDING,1 );
            GAgent_Printf( GAGENT_INFO,"AirLink Timeout ...");
            GAgent_Printf( GAGENT_INFO,"Into SoftAp Config...");
    }
    #else //Nik.chen test softap config
    GAgent_Printf( GAGENT_INFO,"set to config AP mode...");
    //pgc->gc.airkiss_value = 0x55;
    //wlanscan();
    GAgent_DevCheckWifiStatus( WIFI_MODE_ONBOARDING,1 );
    switchSmartlink = 0;
    #endif   
}
else if(switchSmartlink == 2)
{
    switchSmartlink = 0;
}

    return ;
}
