#include "ota1.h"
#include "gagent_md5.h"
#include "http.h"
#include "cloud.h"

#if 1
#include "flc_api.h"
#include "ota_api.h"

#define kCRLFNewLine     "\r\n"
#define kCRLFLineEnding  "\r\n\r\n"

#define FILESIZE (1*1024)

#if 0
typedef struct
{
     _i32 lFileHandle;
     FlcCb_t    *pFlcSflashCb;
}FlcSFlash;

void  sl_extLib_FlcInit(void)
{
    FlcSFlash *flcSFlash = &g_flcSFlash;
    
    /* init SFLASH file access callbacks */
    flcSFlash->pFlcSflashCb->pOpenFile = cc3200_FlcOpenFile; //sl_extlib_FlcOpenFile
    flcSFlash->pFlcSflashCb->pReadFile = sl_extlib_FlcReadFile;
    flcSFlash->pFlcSflashCb->pWriteFile = sl_extlib_FlcWriteFile;
    flcSFlash->pFlcSflashCb->pCloseFile = sl_extlib_FlcCloseFile;
    flcSFlash->pFlcSflashCb->pAbortFile = sl_extlib_FlcAbortFile;
}
#endif

#if 1
/******************************************************************************
   Image file names
*******************************************************************************/
#define IMG_BOOT_INFO           "/sys/mcubootinfo.bin"
#define IMG_FACTORY_DEFAULT     "/sys/mcuimg1.bin"
#define IMG_USER_1              "/sys/mcuimg2.bin"
#define IMG_USER_2              "/sys/mcuimg3.bin"

/******************************************************************************
   Image status
*******************************************************************************/
#define IMG_STATUS_TESTING      0x12344321
#define IMG_STATUS_TESTREADY    0x56788765
#define IMG_STATUS_NOTEST       0xABCDDCBA

/******************************************************************************
   Active Image
*******************************************************************************/
#define IMG_ACT_FACTORY         0
#define IMG_ACT_USER1           1
#define IMG_ACT_USER2           2

/******************************************************************************
   Boot Info structure
*******************************************************************************/
typedef struct sBootInfo
{
  _u8  ucActiveImg;
  _u32 ulImgStatus;

}sBootInfo_t;

#endif

_i32 cc3200_FlcOpenFile(_u8 *file_name, _i32 file_size, _u32 *ulToken, _i32 *lFileHandle, _i32 open_flags)
{
    /* MCU image name should be changed */
    #if 0
    if (strstr((char *)file_name, "/sys/mcuimgA") != NULL)
    {
        file_name[11] = (_u8)_McuImageGetNewIndex() + '1'; /* mcuimg1 is for factory default, mcuimg2,3 are for OTA updates */
        Report("sl_extlib_FlcOpenFile: MCU image name converted to %s \r\n", file_name);
    }
    #else

    _u8* pFilename = "/sys/mcuimg2.bin";  //"/sys/mcuimg2.bin"

    GAgent_Printf( GAGENT_CRITICAL,"Line:%d ...\n", __LINE__);
    if (open_flags == _FS_MODE_OPEN_READ)
        return sl_FsOpen((_u8 *)pFilename, _FS_MODE_OPEN_READ, ulToken, lFileHandle);
    else
        return sl_FsOpen((_u8 *)pFilename, FS_MODE_OPEN_CREATE(file_size, open_flags), ulToken, lFileHandle);
       //    return  sl_FsOpen((_u8 *)pFilename, _FS_MODE_OPEN_WRITE_CREATE_IF_NOT_EXIST, ulToken, lFileHandle);
    #endif
}

_i32 cc3200_checkFile(int32 FirmwareLen, _i32 * lFileHandle)
{
      int  iRetVal = -1;
    //  long lFileHandle;
      long ulToken;
      unsigned char  data[512];
      static SlFsFileInfo_t pFsFileInfo;
      _u8* pFilename = "/sys/mcuimg2.bin"; 
      int ii =0;

       GAgent_Printf( GAGENT_CRITICAL,"Line:%d ...\n", __LINE__); 
      
     // iRetVal = sl_FsOpen(pFilename, FS_MODE_OPEN_READ, &ulToken, &lFileHandle);
     // if( 0 == iRetVal )
     {
    //
    // Get the file size using File Info structure
    //
    // GAgent_Printf( GAGENT_CRITICAL,"Line:%d ...\n", __LINE__); 
    //iRetVal = sl_FsGetInfo(pFilename, ulToken,&pFsFileInfo);

    //
    // Check for failure
    //
 //   if( 0 == iRetVal )
    {

      //
      // Read the application into SRAM
      //
     //  GAgent_Printf( GAGENT_CRITICAL,"Line:%d ...\n", __LINE__); 
      iRetVal = sl_FsRead((_i32)lFileHandle,0, (unsigned char *)data,
                 FirmwareLen );
       if( 0 < iRetVal )
        {
        GAgent_Printf( GAGENT_CRITICAL,"len: %d!!!", pFsFileInfo.FileLen);
         return RET_FAILED;
        }
      
      for(ii =0;ii<FirmwareLen;ii++)
            GAgent_Printf( GAGENT_CRITICAL,"data[%d]: %d!!!", ii, data[ii]);

      return RET_SUCCESS;
     }
      // else
       //// {
         //   GAgent_Printf( GAGENT_CRITICAL,"open /sys/mcuimg2 faile!!!");
        //    return RET_FAILED;
     //   }
   }
    //  else
     //    GAgent_Printf( GAGENT_CRITICAL,"open /sys/mcuimg2 faile!!!");
      return RET_FAILED;
}

_i32 cc3200_FlcWriteFile(_i32 fileHandle, _i32 offset, _u8 *buf, _i32 len)
{
    return sl_FsWrite(fileHandle, (_u32)(offset), (_u8 *)buf, len);
}


_i32 cc3200_FlcCloseFile(_i32 fileHandle, _u8 *pCeritificateFileName,_u8 *pSignature ,_u32 SignatureLen)
{
   if(NULL == pSignature)
     return  sl_FsClose(fileHandle, 0, 0, 0);
   else 
     return sl_FsClose(fileHandle, pCeritificateFileName, pSignature, SignatureLen);
   
}

_i32 cc3200_FlcAbortFile(_i32 fileHandle)
{
    _u8 abortSig = 'A';
    /* Close the file with signature 'A' which is ABORT */
    return cc3200_FlcCloseFile(fileHandle, 0, &abortSig, 1);
}


static _i32 cc3200_readBootInfo(sBootInfo_t *psBootInfo)
{
    _i32 lFileHandle;
    _u32 ulToken;
    _i32 status = -1;

    if( 0 == sl_FsOpen((_u8 *)IMG_BOOT_INFO, FS_MODE_OPEN_READ, &ulToken, &lFileHandle) )
    {
        if( 0 < sl_FsRead(lFileHandle, 0, (_u8 *)psBootInfo, sizeof(sBootInfo_t)) )
        {
            status = 0;
            GAgent_Printf( GAGENT_CRITICAL,"ReadBootInfo: ucActiveImg=%d, ulImgStatus=0x%x", psBootInfo->ucActiveImg, psBootInfo->ulImgStatus);
        }
        sl_FsClose(lFileHandle, 0, 0, 0);
    }

    return status;
}

static _i32 cc3200_writeBootInfo(sBootInfo_t *psBootInfo)
{
    _i32 lFileHandle;
    _u32 ulToken;
    _i32 status = -1;

    if( 0 == sl_FsOpen((_u8 *)IMG_BOOT_INFO, FS_MODE_OPEN_WRITE, &ulToken, &lFileHandle) )
    {
        if( 0 < sl_FsWrite(lFileHandle, 0, (_u8 *)psBootInfo, sizeof(sBootInfo_t)) )
        {
            GAgent_Printf( GAGENT_CRITICAL,"WriteBootInfo: ucActiveImg=%d, ulImgStatus=0x%x\n", psBootInfo->ucActiveImg, psBootInfo->ulImgStatus);
            status = 0;
        }
        sl_FsClose(lFileHandle, 0, 0, 0);
    }

    return status;
}


_i32 cc3200_FlcCommit(_i32 CommitFlag)
{
    _i32 ret_status = 0;
    sBootInfo_t sBootInfo;

        /* commit TRUE: set status IMG_STATUS_NOTEST and switch to new image */
        if (CommitFlag == FLC_COMMITED)
        {
            GAgent_Printf( GAGENT_CRITICAL,"sl_extlib_FlcCommit: Booted in testing mode.!!!");
            sBootInfo.ulImgStatus = IMG_STATUS_NOTEST;
            sBootInfo.ucActiveImg = IMG_ACT_USER1;
            /* prepare switch image condition to the MCU boot loader */
            ret_status = cc3200_writeBootInfo(&sBootInfo);

            /* no need to reset the device */
        }
       

    return ret_status;
}

_i32 cc3200_FlcTest(_i32 flags)
{
    _i32 ret_status = 0;
     sBootInfo_t sBootInfo;

    if (flags & FLC_TEST_RESET_MCU)
    {
    	if(flags & FLC_TEST_RESET_MCU_WITH_APP )
    	{
            /* set status IMG_STATUS_TESTREADY to test the new image */
            GAgent_Printf( GAGENT_CRITICAL,"sl_extlib_FlcTest: change image status to IMG_STATUS_TESTREADY!!!");
            cc3200_readBootInfo(&sBootInfo);
            sBootInfo.ucActiveImg = IMG_ACT_USER1;//IMG_ACT_FACTORY;
            sBootInfo.ulImgStatus = IMG_STATUS_TESTREADY;
            cc3200_writeBootInfo(&sBootInfo);
    	}

        /* reboot, the MCU boot loader should test the new image */
         ret_status |= FLC_TEST_RESET_MCU;
    }

    if (flags & FLC_TEST_RESET_NWP)
    {
        /* Image test is not implemented yet, just reset the NWP */
        ret_status |= FLC_TEST_RESET_NWP;
    }

    return ret_status;
}

_i32 cc3200_setFactory(void)
{
        sBootInfo_t sBootInfo;
        unsigned long ulToken;
        _i32 lFileHandle;
        //
        // Set the factory default
        //
        sBootInfo.ucActiveImg = IMG_ACT_USER1;//IMG_ACT_FACTORY;
        sBootInfo.ulImgStatus = IMG_STATUS_NOTEST;

        //
        // Save the new configuration
        //
        if( 0 == sl_FsOpen((unsigned char *)IMG_BOOT_INFO, FS_MODE_OPEN_WRITE,
                         &ulToken, &lFileHandle) )
        {
          sl_FsWrite(lFileHandle, 0, (unsigned char *)&sBootInfo,
                     sizeof(sBootInfo_t));
          sl_FsClose(lFileHandle, 0, 0, 0);
          GAgent_Printf( GAGENT_CRITICAL,"cc3200_setFactory: OK!!!");
           return RET_SUCCESS;
        }
         return RET_FAILED;
}


/* default file access is SFLASH, user should overwrite this functions in order to save on Host files */
_i32 sl_openStorageFile( _u8 *file_name, _i32 file_size, _u32 *ulToken, _i32 *lFileHandle)
{
    _i32 fs_open_flags=0;
    _i32 status;
    _u32 ulTokentmp;

    /*  create a user file with mirror */
    fs_open_flags  = _FS_FILE_OPEN_FLAG_COMMIT; //|_FS_FILE_PUBLIC_WRITE

    //status = g_flcSFlash.pFlcSflashCb->pOpenFile((_u8 *)file_name, file_size, &ulTokentmp, lFileHandle, fs_open_flags);
    status =sl_extlib_FlcOpenFile((_u8 *)file_name, file_size, &ulTokentmp, lFileHandle, fs_open_flags);
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
#if 1
//static int32 DRV_SaveOTApacket( pgcontext pgc,uint8 *buf,int32 Len,int32 FirmwareLen,_i32 * lFileHandle,uint8 * MD5,int32 socket_cloud)
static int32 DRV_SaveOTApacket( pgcontext pgc,uint8 *buf,int32 Len,int32 FirmwareLen, uint8 * MD5,int32 socket_cloud)
{
    int32 all=0,in=0;
    uint8* p_bin=NULL;
    int32 bin_len, total_len;
    uint32 status = -1;
    uint32 totalBytesReceived = 0;
    //char MD5[16] = {0};
    uint8 md5_calc[16] = {0};
    //uint8 md5V4_ret[16]={0};
    MD5_CTX ctx;
    fd_set readfds;
    struct timeval t;
    int32 i=0;

    long lRetVal = -1;
	long lFileHandle;
    unsigned long ulToken;
    _u8* pFilename = NULL;

    if(0 == pgc->mcu.mcu_firmware_type)
        pFilename = "/sys/mcuimg2.bin";
    else
        pFilename = "/tmn/mcuota.bin";
    
    if( MD5==NULL )
        return RET_FAILED;
    all = FirmwareLen;
    in = Len;
    total_len = FirmwareLen;
    bin_len = Len;
    GAgent_Printf( GAGENT_DEBUG,"fimware_type=%d, name=%s, bin size = %d\r\n",(pgc->mcu.mcu_firmware_type), pFilename,FirmwareLen);
    GAgent_MD5Init(&ctx);

    #if 1
	
    //
    //  create a user file
    //
     lRetVal =  sl_FsOpen((_u8 *)pFilename, FS_MODE_OPEN_CREATE(FirmwareLen, _FS_FILE_OPEN_FLAG_COMMIT|_FS_FILE_PUBLIC_WRITE), &ulToken, &lFileHandle);

    if(lRetVal < 0)
    {
        //
        // File may already be created
        //
        lRetVal = sl_FsClose(lFileHandle, 0, 0, 0);
         GAgent_Printf( GAGENT_CRITICAL,"Line:%d, lRetVal=%d ...\n", __LINE__, lRetVal);
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
           GAgent_Printf( GAGENT_CRITICAL,"Line:%d ...\n", __LINE__);
            return lRetVal;
        }
    }
     #endif
    //
    //  open a user file for writing
    //
    lRetVal = sl_FsOpen((_u8 *)pFilename,
                        FS_MODE_OPEN_WRITE, 
                        &ulToken,
                        &lFileHandle);
    if(lRetVal < 0)
    {
        lRetVal = sl_FsClose(lFileHandle, 0, 0, 0);
		
		//
		//	create a user file
		//
	     lRetVal =  sl_FsOpen((_u8 *)pFilename, FS_MODE_OPEN_CREATE(FirmwareLen, _FS_FILE_OPEN_FLAG_COMMIT|_FS_FILE_PUBLIC_WRITE), &ulToken, &lFileHandle);

		if(lRetVal < 0)
		{
			//
			// File may already be created
			//
			lRetVal = sl_FsClose(lFileHandle, 0, 0, 0);
                    GAgent_Printf( GAGENT_CRITICAL,"Line:%d ...\n", __LINE__);
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
			       GAgent_Printf( GAGENT_CRITICAL,"Line:%d ...\n", __LINE__);
				return lRetVal;
			}
		}

    }
    
   

    #if 1
    p_bin = buf;
     if( bin_len>0 )
    {  
       // status = cc3200_FlcWriteFile(lFileHandleTmp, 0, (uint8*)p_bin, bin_len);
        lRetVal = sl_FsWrite(lFileHandle, 0, (unsigned char *)p_bin, bin_len);
        if (lRetVal < 0)
        {
            GAgent_Printf(GAGENT_WARNING, "cc3200_FlcWriteFile first error\r\n");
             return RET_FAILED;
        }
        #if 1
        else
        {
             for( i=0;i<bin_len;i+=2 )
            {
                if( i%16 == 0 )
                    GAgent_Printf(15,"\r\n");
                    GAgent_Printf( 15,"%02x%02x ",p_bin[i],p_bin[i+1]);
            }
            GAgent_Printf(GAGENT_DEBUG, "\r\nWritten %d bytes\r\n", bin_len);
        }
        #endif
    }
    total_len -= bin_len;
    totalBytesReceived = bin_len;

    GAgent_MD5Update(&ctx, p_bin, bin_len);
    #endif
    
    resetPacket( pgc->rtinfo.Rxbuf );
    p_bin = pgc->rtinfo.Rxbuf->phead ;//( int8* )malloc(1024);
    if(p_bin == NULL )
    {
        GAgent_Printf( GAGENT_WARNING,"malloc p_bin fail!" );
        return RET_FAILED;
    }
    GAgent_Printf( GAGENT_INFO,"1.total_len = %d\r\n",total_len );

 
    while( total_len>0 )
    {                
        t.tv_sec = 1;
        t.tv_usec = 0;
        FD_ZERO(&readfds);
        FD_SET(socket_cloud, &readfds); 
        select(socket_cloud+1, &readfds, NULL, NULL, &t);
        if (FD_ISSET(socket_cloud, &readfds)) 
        {
            if( total_len>1024)
            {                
                bin_len = recv(socket_cloud, p_bin, 1024, 0);
            }
            else
            {
                bin_len = recv(socket_cloud, p_bin, total_len, 0);
            }
            
            if(bin_len == -1) 
            {
           
                close(socket_cloud);
                socket_cloud = -1;  
                //ERRORCODE
                GAgent_Printf(GAGENT_DEBUG, "\r\nsocket_cloud close bin_len1:%d\r\n",bin_len);
                return RET_FAILED; //Net error, donot need to send ack
            }
            //status = cc3200_FlcWriteFile(lFileHandleTmp, totalBytesReceived, (uint8*)p_bin, bin_len);
             lRetVal = sl_FsWrite(lFileHandle, totalBytesReceived, (unsigned char *)p_bin, bin_len);
            if (lRetVal < 0)
            {
                GAgent_Printf(GAGENT_WARNING, "cc3200_FlcWriteFile  error\r\n");
                cc3200_FlcAbortFile(lFileHandle);
                return RET_FAILED;
            }
            else
            {
               #if 0
                 for( i=0;i<bin_len;i+=2 )
                {
                    if( i%16 == 0 )
                        GAgent_Printf(15,"\r\n");
                        GAgent_Printf( 15,"%02x%02x ", p_bin[i],p_bin[i+1]);
                }
                GAgent_Printf(GAGENT_DEBUG, "\r\nWritten %d bytes\r\n", bin_len);
              #endif
            }
            totalBytesReceived += bin_len;
            total_len -= bin_len;
            in+=bin_len;

            GAgent_MD5Update(&ctx, p_bin, bin_len);

            GAgent_Printf( GAGENT_DEBUG,"file offset:%04x   %d%%",totalBytesReceived,( (in*100)/(all) ) );
            //GAgent_Printf( GAGENT_INFO," download firmware:%d%%",( (in*100)/(all) ));
        }
    }
     GAgent_MD5Final(&ctx, md5_calc);
    if(memcmp(MD5, md5_calc, 16) != 0)
    {
        GAgent_Printf(GAGENT_WARNING,"[CLOUD]md5 fail!");
        return RET_FAILED;
    }
    #if 0
    md5_hex((u8 *)UPDATE_START_ADDRESS, flash_addr - UPDATE_START_ADDRESS, md5V4_ret);
    GAgent_Printf( GAGENT_INFO,"firmware MD5 checking ......" );
    if(memcmp(md5V4_ret,MD5 ,16) != 0) 
    {
        GAgent_Printf( GAGENT_INFO,"bin source len=%d--------------\r\n\r\n",flash_addr - UPDATE_START_ADDRESS);
        for( i=0;i<(flash_addr - UPDATE_START_ADDRESS);i+=2 )
        {
            mxchipTick();
            if( i%16 == 0 )
                GAgent_Printf(15,"\r\n");
            GAgent_Printf( 15,"%02x%02x ",
                           pstart[i],pstart[i+1]);
        }
        ERRORCODE
            FLASH_Lock();

        sleep(1);
        GAgent_Printf(GAGENT_DEBUG,"GAgent md5:");
        for(i=0;i<16;i++)
            GAgent_Printf(GAGENT_DUMP,"%02x",md5V4_ret[i]);

        GAgent_Printf(GAGENT_INFO,"HTTP md5:");
        for(i=0;i<16;i++)
            GAgent_Printf(GAGENT_DUMP,"%02x",MD5[i]);

        GAgent_Printf(GAGENT_DEBUG, "\r\n MD5 ERROR OTA fail");
        return  RET_FAILED;
     }
    #endif
      //
    // close the user file
    //
    lRetVal = sl_FsClose(lFileHandle, 0, 0, 0);
    if (SL_RET_CODE_OK != lRetVal)
    {
         GAgent_Printf( GAGENT_INFO,"sl_FsClose error!!!" );
        return RET_FAILED;
    }
    GAgent_Printf( GAGENT_INFO,"firmware MD5 check OK !" );
    return RET_SUCCESS;

}
#endif

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
    uint32 ImageTestFlags = 0;
    uint32 flcStatus = 0;
    _i32 lFileHandle;
    int8 conFlag = 0;

    //sl_extLib_FlcInit();

OTA_START:
    ota_time++;
    OTA_socket=0;
    //GAgent_Printf( GAGENT_CRITICAL,"GAgent_WIFIOTAByUrl enter...\n");
    //GAgent_Printf( GAGENT_CRITICAL,"Line:%d ...\n", __LINE__);
    httpbuf = GAgent_BuildDownLoadFirmwareBuf( szdownloadUrl,httpbuf );
    //GAgent_Printf( GAGENT_CRITICAL,"Line:%d ...\n", __LINE__);
    if( httpbuf==NULL )
    {
        GAgent_Printf( GAGENT_INFO,"httpbuf ==NULL ");
        return RET_FAILED;
    }
    //GAgent_Printf( GAGENT_CRITICAL,"Line:%d ...\n", __LINE__);
    OTA_Host = GAgent_GetOtaHostFromUrl( szdownloadUrl,OTA_Host );
    if( OTA_Host==NULL )
    {
        GAgent_Printf( GAGENT_INFO,"OTA_Host ==NULL ");
        free( httpbuf );
        return NULL;
    }

    CC3200_DNSTaskDelete();
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
            GAgent_Printf( GAGENT_DEBUG,"MD5:%s",szMD5 );
            GAgent_Printf( GAGENT_DEBUG,"( response = %d)",OTA_Code );
            GAgent_Printf( GAGENT_DEBUG,"(binlen = %d )",firmwarelen );
            GAgent_Printf( GAGENT_DEBUG,"szMD5:%s",szMD5 );
            GAgent_Printf( GAGENT_DEBUG,"headlen = %d",ret );
            GAgent_Printf( GAGENT_DEBUG,"httpHeadlen = %d",httpHeadlen );
            GAgent_Printf( GAGENT_DEBUG,"filename:%s\n",filename );
            if( 200!=OTA_Code )
            {
                free(filename );
                free( OTA_Host );
                free( httpbuf );
                return RET_FAILED;
            }
#if 0  
            status = cc3200_setFactory();
            if (status == RET_SUCCESS)
            {
            GAgent_Printf( GAGENT_CRITICAL,"Line:%d ...\n", __LINE__); 
            NetStop();
            GAgent_DevReset();
            }
#endif

            #if 0
            status = cc3200_FlcOpenFile(NULL, firmwarelen, NULL, &lFileHandle, _FS_FILE_OPEN_FLAG_COMMIT|_FS_FILE_PUBLIC_WRITE);
            if( status < RET_SUCCESS)
            {
                GAgent_Printf( GAGENT_CRITICAL,"sl_openStorageFile failed!!!");
                return NULL;
            }
            #endif
            pgc->mcu.mcu_firmware_type = 0;
            ret = DRV_SaveOTApacket(pgc, buf+httpHeadlen,ret-httpHeadlen,firmwarelen,MD5,OTA_socket);
            //ret = g_flcSFlash.pFlcSflashCb->pWriteFile((_i32)g_flcSFlash.lFileHandle, 0, buf, httpHeadlen); 
           //ret = cc3200_FlcWriteFile((uint32)g_flcSFlash.lFileHandle, 0, buf, httpHeadlen);
            if( ret == RET_SUCCESS )
            {
                #if 0
                ret =cc3200_FlcCloseFile(lFileHandle, NULL, NULL, 0);
                   if( ret <0)
                    {
                      GAgent_Printf( GAGENT_CRITICAL,"close file failed!!!");
                       return (!RET_SUCCESS);
                    }
               #endif
                #if 0
                ret = cc3200_checkFile(firmwarelen, &lFileHandle);
                 if( RET_SUCCESS != ret)
                     return (!RET_SUCCESS);
                #endif
            
             

                // GAgent_DevReset( );
                #if 1
                 sBootInfo_t sBootInfo;
                flcStatus = cc3200_readBootInfo(&sBootInfo);
                GAgent_Printf( GAGENT_DEBUG,"flcStatus:%d \n", flcStatus); 
                #if 1  
                status = cc3200_setFactory();
               if (status == RET_SUCCESS)
                {
                    GAgent_Printf( GAGENT_DEBUG,"Line:%d ...\n", __LINE__); 
                    setconnFlag(0); 
                    NetStop();
                    GAgent_DevReset();
               }
               else
                    return RET_FAILED;
               #endif
                ImageTestFlags |= FLC_TEST_RESET_MCU;
                ImageTestFlags |= FLC_TEST_RESET_MCU_WITH_APP;
                //flcStatus = sl_extlib_FlcTest(ImageTestFlags);
                flcStatus = cc3200_FlcTest(ImageTestFlags);
                if ( flcStatus & FLC_TEST_RESET_MCU )
                {
                    GAgent_Printf( GAGENT_DEBUG,"Line:%d ...\n", __LINE__); 
                    status |= OTA_ACTION_RESET_MCU;
                    NetStop();
                    GAgent_DevReset( );
                }

                if ( flcStatus & FLC_TEST_RESET_NWP )
                {
                    GAgent_Printf( GAGENT_CRITICAL,"Line:%d ...\n", __LINE__);
                }

                if ( flcStatus & FLC_TEST_RESET_MCU_WITH_APP )
                {
                    GAgent_Printf( GAGENT_CRITICAL,"Line:%d ...\n", __LINE__);
                }

                flcStatus = sl_extlib_FlcCommit(FLC_COMMITED);
                if (flcStatus & FLC_TEST_RESET_MCU)
                {
                    GAgent_Printf( GAGENT_CRITICAL,"Line:%d ...\n", __LINE__);
                }

                if ( flcStatus & FLC_TEST_RESET_NWP )
                {
                    GAgent_Printf( GAGENT_CRITICAL,"Line:%d ...\n", __LINE__);
                }

                if ( flcStatus & FLC_TEST_RESET_MCU_WITH_APP )
                {
                    GAgent_Printf( GAGENT_CRITICAL,"Line:%d ...\n", __LINE__);
                }

                flcStatus = sl_extlib_FlcIsPendingCommit();
                GAgent_Printf( GAGENT_CRITICAL,"Line:%d, flcStatus:%d\n", __LINE__, flcStatus);

                GAgent_DevReset( );
                #else
                status = cc3200_FlcCommit(FLC_COMMITED);
                if (status == FLC_TEST_RESET_MCU)
                {
                    GAgent_Printf( GAGENT_CRITICAL,"Line:%d, flcStatus:%d\n", __LINE__, flcStatus);
                    GAgent_DevReset();
                }
                else
                 GAgent_Printf( GAGENT_CRITICAL,"Line:%d , status:%d...\n", __LINE__, status);
                GAgent_DevReset();
                #endif
                
            }
            else
            {
                 GAgent_Printf( GAGENT_CRITICAL,"OTA failed! Line:%d ...\n", __LINE__);
                 return RET_FAILED;
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
    int ret=0;
    uint8 *buf = NULL;
    int8 *filename = NULL;
    char MD5[16] = {0},szMD5[33];
    int32 OTA_Code=0,firmwarelen=0,httpHeadlen=0;
    int32 ota_WaitTimeMS=0;
    fd_set readfd;
    // MD5_CTX ctx;

    resetPacket( pgc->rtinfo.Rxbuf );
    buf = pgc->rtinfo.Rxbuf->phead ;

    while( 1 )
    {
        if( ota_WaitTimeMS>=10 )
        {
            ota_WaitTimeMS++;
            GAgent_Printf( GAGENT_DEBUG,"ota_WaitTimeMS timeout !!!" );
            break;
        }
        
        FD_ZERO( &readfd );
        FD_SET( socketid,&readfd );
        ret = GAgent_select(socketid+1,&readfd,NULL,NULL,1,0 );
        if( ret>0 && FD_ISSET( socketid,&readfd ) )
        {
            ret = recv(socketid, buf, 1024, 0 );
            OTA_Code = Http_Response_Code( (uint8*)buf );
            firmwarelen = GAgent_GetFirmwareLen( buf );
            Http_GetMD5( (uint8*)buf,MD5,szMD5 );
            httpHeadlen =  Http_HeadLen( (uint8*)buf );
            filename = GAgent_GetFirmwareName( buf,filename );
            pgc->mcu.mcu_firmware_type = Http_GetFileType(buf);
            pgc->rtinfo.filelen = firmwarelen;
            GAgent_Printf( GAGENT_DEBUG,"%s",szMD5 );
            GAgent_Printf( GAGENT_DEBUG,"( response = %d)",OTA_Code );
            GAgent_Printf( GAGENT_DEBUG,"(binlen = %d )",firmwarelen );
            GAgent_Printf( GAGENT_DEBUG,"%s",szMD5 );
            GAgent_Printf( GAGENT_DEBUG,"headlen = %d",ret );
            GAgent_Printf( GAGENT_DEBUG,"filename:%s\n",filename );
            GAgent_Printf( GAGENT_DEBUG,"firmware type :%d",pgc->mcu.mcu_firmware_type );
            if( 200!=OTA_Code )
            {
                free(filename );
                return (uint32)RET_FAILED;
            }
            GAgent_Printf( GAGENT_DEBUG,"%s %d save mcu ota bin start!",__FUNCTION__,__LINE__ ); 
            ret = DRV_SaveOTApacket(pgc, buf+httpHeadlen,ret-httpHeadlen,firmwarelen,MD5,socketid);
           
            if( ret ==RET_SUCCESS )
            {
                memcpy( pgc->mcu.MD5,szMD5,32 );
                GAgent_Printf( GAGENT_DEBUG,"Save mcu ota bin ok .");
                return (uint32)RET_SUCCESS;
            }
            else
            {
                return (uint32)RET_FAILED;
            }         
        }

    }
    if( ota_WaitTimeMS>=10 )
        return (uint32)RET_FAILED;
    else
        return (uint32)RET_SUCCESS;
    // ret = Http_ReadSocket( socketid, httpReceiveBuf, SOCKET_RECBUFFER_LEN );  
    // if(ret <=0 ) 
    // { 
    //     free(httpReceiveBuf);
    //     return RET_FAILED;
    // }
    
    // ret = Http_Response_Code( httpReceiveBuf );
    // if(200 != ret)
    // {
    //     free(httpReceiveBuf);
    //     return RET_FAILED;
    // }
    // headlen = Http_HeadLen( httpReceiveBuf );
    // pgc->rtinfo.filelen = Http_BodyLen( httpReceiveBuf );
    // //pgc->rtinfo.MD5 = (char *)malloc(32+1);
    // // if( pgc->rtinfo.MD5 == NULL )
    // // {
    // //     return RET_FAILED;
    // // }
    // Http_GetMD5( httpReceiveBuf,MD5,pgc->mcu.MD5 );
    // Http_GetSV( httpReceiveBuf,(char *)pgc->mcu.soft_ver);
  
    // offset = 0;
    // buf = httpReceiveBuf + headlen;
    // writelen = SOCKET_RECBUFFER_LEN - headlen;
    // GAgent_MD5Init(&ctx);
    // do
    // {
    //     ret = GAgent_SaveUpgradFirmware( offset, buf, writelen );
    //     if(ret < 0)
    //     {
    //         GAgent_Printf(GAGENT_INFO, "[CLOUD]%s OTA upgrad fail at off:0x%x", __func__, offset);
    //         free(httpReceiveBuf);
    //         return RET_FAILED;
    //     }
    //     offset += writelen;
    //     GAgent_MD5Update(&ctx, buf, writelen);
    //     writelen = pgc->rtinfo.filelen - offset;
    //     if(0 == writelen)
    //         break;
    //     if(writelen > SOCKET_RECBUFFER_LEN)
    //     {
    //         writelen = SOCKET_RECBUFFER_LEN;
    //     }
    //     writelen = Http_ReadSocket( socketid, httpReceiveBuf, writelen );    
    //     if(writelen <= 0 )
    //     {
    //         GAgent_Printf(GAGENT_INFO,"[CLOUD]%s, socket recv ota file fail!recived:0x%x", __func__, offset);
    //         free(httpReceiveBuf);
    //         return RET_FAILED;
    //     }
    //     buf = httpReceiveBuf;
    // }while(offset < pgc->rtinfo.filelen);
    // GAgent_MD5Final(&ctx, md5_calc);
    // if(memcmp(MD5, md5_calc, 16) != 0)
    // {
    //     GAgent_Printf(GAGENT_WARNING,"[CLOUD]md5 fail!");
    //     free(httpReceiveBuf);
    //     return RET_FAILED;
    // }
    // free(httpReceiveBuf);
    // return RET_SUCCESS;
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
