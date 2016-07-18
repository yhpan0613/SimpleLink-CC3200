#include "ota1.h"
#include "gagent_md5.h"
#include "http.h"
#include "cloud.h"

#if 1
#include "flc_api.h"
#include "ota_api.h"

#define kCRLFNewLine     "\r\n"
#define kCRLFLineEnding  "\r\n\r\n"

#define FILESIZE (64*1024)

typedef struct
{
     _i32 lFileHandle;
     FlcCb_t    *pFlcSflashCb;
}FlcSFlash;

FlcSFlash g_flcSFlash;

_i32 sl_openStorageFile( _u8 *file_name, _i32 file_size, _u32 *ulToken, _i32 *lFileHandle);

void  sl_extLib_FlcInit(void)
{
    FlcSFlash *flcSFlash = &g_flcSFlash;
    
    /* init SFLASH file access callbacks */
    flcSFlash->pFlcSflashCb->pOpenFile = sl_extlib_FlcOpenFile;
    flcSFlash->pFlcSflashCb->pReadFile = sl_extlib_FlcReadFile;
    flcSFlash->pFlcSflashCb->pWriteFile = sl_extlib_FlcWriteFile;
    flcSFlash->pFlcSflashCb->pCloseFile = sl_extlib_FlcCloseFile;
    flcSFlash->pFlcSflashCb->pAbortFile = sl_extlib_FlcAbortFile;
}


/* default file access is SFLASH, user should overwrite this functions in order to save on Host files */
_i32 sl_openStorageFile( _u8 *file_name, _i32 file_size, _u32 *ulToken, _i32 *lFileHandle)
{
    _i32 fs_open_flags=0;
    _i32 status;

    /*  create a user file with mirror */
    fs_open_flags  = _FS_FILE_OPEN_FLAG_COMMIT;

    status = g_flcSFlash.pFlcSflashCb->pOpenFile((_u8 *)file_name, file_size, ulToken, lFileHandle, fs_open_flags);
    if(status < 0)
    {
        GAgent_Printf( GAGENT_CRITICAL,"g_FlcSflashCb.pOpenFile failed!!!");
        return RET_FAILED;
    }
    return RET_SUCCESS;
}


/*********************************************************************************************************
 *       FUNCTION        :   OTA GAgent.
 *       buf             :   firmware bin prt.
 *       Len             :   first time write to flash data length. 
 *       FirmwareLen     :   firmware bin size.
 *       FV              :   firmware version(MAX 32B)
 *       MD5             :   MD5 from http(16B).
 *       fid             :   GAgent V4 is 4B.
 *       socket_cloud    :   socket cloud.
 * add by alex.lin ---2015.07.02
 *********************************************************************************************************/
static int32 DRV_SaveOTApacket( pgcontext pgc,int8 *buf,int32 Len,int32 FirmwareLen,
                                    int8 *filename,uint8 * MD5,int32 socket_cloud)
{
        return 0;
}
/****************************************************************
Function    :   DRV_ReadOTApacket
Description :   read ota date from ota address(UPDATE_START_ADDRESS).
offset      :   address offset.
buf         :   save data from ota address.
len         :   the len want to read .
return      :   the realy length of read.
Add by Alex.lin     --2015-07-09
****************************************************************/
static int32 DRV_ReadOTApacket( uint32 offset,int8* buf,uint32 len )
{
        return 0;
}

/****************************************************************
Function    :   GAgent_GetFirmwareLen
Description :   get the firmware length by the buf.
buf         :   data from http .
return      :   the length of firmware length.
Add by Alex.lin     --2015-07-02
****************************************************************/
static int32 GAgent_GetFirmwareLen( int8 *buf )
{
    int8 *p_start=NULL;
    int8 *p_end=NULL;
    int8 *CL="Content-Length: ";
    uint32 binlen=0;
    int8 szBinlen[10] = {0};
    p_start = strstr( buf,CL );
    if( p_start==NULL )
        return 0;
    p_start+=strlen( CL );
    p_end = strstr( p_start,kCRLFNewLine );
    if( p_end==NULL )
        return 0;
    if( p_end-p_start >10 )
        return 0;
    memcpy( szBinlen,p_start,( p_end-p_start ));
    szBinlen[p_end-p_start]='\0';
    binlen = atoi( szBinlen );
    return binlen;
}
/****************************************************************
Function    :   GAgent_GetFirmwareName
Description :   get the firmware name by the buf.
buf         :   data from http .
filename    :   the pointer of firmwarename
return      :   the pointer of firmwarename.
WARNING     :   filename will malloc memory in this function.
                so you need to free it if you needn't use it.
Add by Alex.lin     --2015-07-02
****************************************************************/
static int8 *GAgent_GetFirmwareName( int8* buf,int8* filename )
{
    int8 *p_start=NULL;
    int8 *p_end=NULL;
    int8 *cfilename="filename=";
    int32 filenamelen=0; 
    p_start = strstr( buf,cfilename ); 
    if( NULL==p_start )
    {
        return NULL;
    }
    p_start+=strlen( cfilename )+1;

    p_end = strstr( p_start,kCRLFNewLine );
    if( NULL==p_end )
    {
        return NULL;
    }
    p_end-=1;
    filenamelen = p_end-p_start;
    filename = (int8*)malloc( filenamelen+1 );
    if( filename==NULL )
        return NULL;
    memcpy( filename,p_start,filenamelen );
    filename[filenamelen]='\0';
    return filename;
}
/****************************************************************
Function    :   GAgent_GetOtaHostFromUrl
Description :   get the OTA firmware host.
buf         :   data from http .
OTA_Host    :   the pointer of host
return      :   the pointer of host.
WARNING     :   "OTA_Host" will malloc memory in this function.
                so you must to free it if you needn't use it.
Add by Alex.lin     --2015-07-02
****************************************************************/
static int8* GAgent_GetOtaHostFromUrl( int8 *szdownloadUrl,int8 *OTA_Host )
{
    //int8 *OTA_Host=NULL;
    int8 *p_start=NULL;
    int8 *p_end=NULL;
    int8 *host_start="http://";
    int8 *host_end="/";

    p_start = strstr( szdownloadUrl,host_start );
    if( p_start==NULL )
        return NULL;
    p_start +=strlen( host_start );

    p_end = strstr( p_start,host_end );
    if( p_end==NULL )
        return NULL;
    p_start = strstr( szdownloadUrl,host_start );
    if( p_start==NULL )
        return NULL;
    p_start +=strlen( host_start );

    p_end = strstr( p_start,host_end );
    if( p_end==NULL )
        return NULL;

    OTA_Host = (int8*)malloc( (p_end-p_start)+1 );
    if( OTA_Host==NULL )
        return NULL;
    memcpy(  OTA_Host,p_start,(p_end-p_start) );
    OTA_Host[ (p_end-p_start) ]= '\0';

    return OTA_Host;
}
/****************************************************************
Function        :   GAgent_BuildDownLoadFirmwareBuf
Description     :   build the OTA http req buf from download url.
szdownloadUrl   :   download url.
OTA_Host        :   the pointer of httpbuf
return          :   the pointer of httpbuf.
WARNING         :   "httpbuf" will malloc memory in this function.
                so you must to free it if you needn't use it.
Add by Alex.lin     --2015-07-02
****************************************************************/
static int8* GAgent_BuildDownLoadFirmwareBuf( int8 *szdownloadUrl,int8 *httpbuf )
{
    int32 pos=0;
    int32 buflen=0;
    int32 templen=0;
    int8 *GET=NULL;
    int8 *HOST=NULL;
    int8 *OTA_Host=NULL;
    int8 *url=NULL;
    int8 *Type="Content-Type: application/text\r\n";
    int8 *Control="Cache-Control: no-cache\r\n\r\n";


    OTA_Host = GAgent_GetOtaHostFromUrl( szdownloadUrl,OTA_Host );
    if( OTA_Host==NULL )
        return NULL;
    url = szdownloadUrl+(strlen("http://")+strlen(OTA_Host) );
    templen = strlen("Host: ")+strlen(OTA_Host)+strlen(kCRLFNewLine)+1;

    HOST = (int8*)malloc( templen );
    if( HOST==NULL )
    {
        free( OTA_Host );
        return NULL;
    }
    memset( HOST,0,templen );
    memcpy( HOST+pos,"Host: ",strlen("Host: "));
    pos+=strlen("Host: ");
    memcpy( HOST+pos,OTA_Host,strlen( OTA_Host ) );
    pos+=strlen( OTA_Host );
    memcpy( HOST+pos,kCRLFNewLine,strlen(kCRLFNewLine));
    pos+=strlen(kCRLFNewLine);
    HOST[pos] = '\0';

    GET = (int8*)malloc( strlen("GET ")+strlen(url)+strlen(" HTTP/1.1")+strlen(kCRLFNewLine)+1 );
    if( GET==NULL )
    {
        free( HOST );
        free( OTA_Host );
        return NULL;
    }
    sprintf(GET,"GET %s HTTP/1.1%s",url,kCRLFNewLine );
    buflen = strlen(GET)+strlen(HOST)+strlen(Type)+strlen(Control)+1;
    httpbuf = (int8*)malloc( buflen );
    if( httpbuf==NULL)
    {
        free( GET );
        free( HOST );
        free( OTA_Host );
        return NULL;
    }
    sprintf( httpbuf,"%s%s%s%s",GET,HOST,Type,Control );


    free( GET );
    free( HOST );
    free( OTA_Host );
    return httpbuf;
}

/****************************************************************
return      :       RET_SUCCESS  success
            :       RET_FAILED   fail
Add by Alex.lin     --2015-07-01
****************************************************************/
int32 GAgent_WIFIOTAByUrl( pgcontext pgc,int8 *szdownloadUrl )
{

    int8 *httpbuf=NULL;
    int8 *OTA_Host=NULL;
    int8 *filename = NULL;
    int32 OTA_socket=0;
    uint8 OTA_IP[32]={0},MD5[16],szMD5[33];
    int8 dnsTime=0;
    int32 ret=0;
    fd_set readfd;
    int8 ota_time=0;
    int32 ota_WaitTimeMS=0;
    int8 buf[1024];
    int32 OTA_Code=0,firmwarelen=0,httpHeadlen=0;
    uint32 status =RET_SUCCESS;
    _u8* pFilename = "f80_sys_mcuimgA.bin";

    sl_extLib_FlcInit();

OTA_START:
    ota_time++;
    OTA_socket=0;
    GAgent_Printf( GAGENT_CRITICAL,"GAgent_WIFIOTAByUrl enter...\n");
    httpbuf = GAgent_BuildDownLoadFirmwareBuf( szdownloadUrl,httpbuf );
    if( httpbuf==NULL )
    {
        GAgent_Printf( GAGENT_INFO,"httpbuf ==NULL ");
        return RET_FAILED;
    }
    OTA_Host = GAgent_GetOtaHostFromUrl( szdownloadUrl,OTA_Host );
    if( OTA_Host==NULL )
    {
        GAgent_Printf( GAGENT_INFO,"OTA_Host ==NULL ");
        free( httpbuf );
        return NULL;
    }

    status = sl_openStorageFile(pFilename, FILESIZE, NULL, &g_flcSFlash.lFileHandle);
     if( status==RET_FAILED )
    {
         GAgent_Printf( GAGENT_CRITICAL,"sl_openStorageFile failed!!!");
         return NULL;
    }
    
    while( 1 )
    {
        if( dnsTime>=5 )
        {   
            break;
        }
        
        if( 0==GAgent_GetHostByName( OTA_Host, OTA_IP ) )
        {
            break;
        }
        sleep(1);
        dnsTime++;
        
    }
    if( OTA_IP!=NULL )
    {
        OTA_socket = Cloud_InitSocket( OTA_socket, OTA_IP, 80, 0 );
    }
    GAgent_Printf( GAGENT_DEBUG,"OTA_socket:%d",OTA_socket );
    GAgent_Printf( GAGENT_DEBUG, "HTTP OTA BUF :\n%s\n----------------------\nlen =%d\n",httpbuf,strlen(httpbuf) );
    GAgent_Printf( GAGENT_DEBUG,"host:%s\n",OTA_Host );
    GAgent_Printf( GAGENT_DEBUG, "ip:%s\n",OTA_IP );
    if( OTA_socket<=0 )
    {
     if( ota_time>5)
        return RET_FAILED;
      else
        goto OTA_START;
    }

    ret = sl_Send(OTA_socket, httpbuf, (_i16)strlen((const char *)httpbuf), 0);
    if (ret <= 0)
    {
        GAgent_Printf( GAGENT_DEBUG,"OTA sl_Send failed ret =%d",ret );
        return RET_FAILED;
    }
    GAgent_Printf( GAGENT_DEBUG,"OTA Send ok, ret =%d",ret );
    
    while( 1 )
    {
        if( ota_WaitTimeMS>=10 )
        {
            ota_WaitTimeMS++;
            break;
        }
        FD_ZERO( &readfd );
        FD_SET( OTA_socket,&readfd );
        ret = GAgent_select(OTA_socket+1,&readfd,NULL,NULL,1,0 );
        
        #if 0
         if( ret>0 )
           setselFlag(1);
        else
           setselFlag(0);
        #endif
        
        if( ret>0 && FD_ISSET( OTA_socket,&readfd ) )
        {
            ret = recv(OTA_socket, buf, 1024, 0 );
           //ret = sl_Recv_eagain(pOtaClient->serverSockId, response_buf, HTTP_RECV_BUF_LEN, 0, MAX_EAGAIN_RETRIES);
  
            OTA_Code = Http_Response_Code( (uint8*)buf );
            firmwarelen = GAgent_GetFirmwareLen( buf );
            Http_GetMD5( (uint8*)buf,MD5,szMD5 );
            httpHeadlen =  Http_HeadLen( (uint8*)buf );
            filename = GAgent_GetFirmwareName( buf,filename );
            GAgent_Printf( GAGENT_DEBUG,"%s",szMD5 );
            GAgent_Printf( GAGENT_DEBUG,"( response = %d)",OTA_Code );
            GAgent_Printf( GAGENT_DEBUG,"(binlen = %d )",firmwarelen );
            GAgent_Printf( GAGENT_DEBUG,"%s",szMD5 );
            GAgent_Printf( GAGENT_DEBUG,"headlen = %d",ret );
            GAgent_Printf( GAGENT_DEBUG,"filename:%s\n",filename );
            if( 200!=OTA_Code )
            {
                free(filename );
                free( OTA_Host );
                free( httpbuf );
                return RET_FAILED;
            }
            //ret = DRV_SaveOTApacket(pgc, buf+httpHeadlen,ret-httpHeadlen,firmwarelen,filename,MD5,OTA_socket);
            ret = g_flcSFlash.pFlcSflashCb->pWriteFile((_i32)g_flcSFlash.lFileHandle, 0, buf, httpHeadlen); 
            if( ret>0 )
            {
               // DRV_SavebootTable( &bootTable,&(pgc->gc),ret );
                g_flcSFlash.pFlcSflashCb->pAbortFile(g_flcSFlash.lFileHandle);
                GAgent_DevReset( );
            }
            break;
        }
    }
    free(filename );
    free( OTA_Host );
    free( httpbuf );
    if( ota_WaitTimeMS>=10 )
        return RET_FAILED;
    else
        return RET_SUCCESS;
}
uint32 Http_ResGetFirmware( pgcontext pgc,int32 socketid )
{
        return 0;
}
uint32 GAgent_ReadOTAFile( uint32 offset, int8* buf,uint32 len )
{
    return 0;
}
/****************************************************************
Function    :   GAgent_DeleteFirmware
Description :   Erase the flash address from UPDATE_START_ADDRESS
                to UPDATE_END_ADDRESS.

Add by Alex.lin        ---2015-07-09
****************************************************************/
uint32 GAgent_DeleteFirmware( int32 offset,int32 filelen )
{
    GAgent_Printf( GAGENT_DEBUG,"%s ok.",__FUNCTION__ );
    return 0;
}
#endif
