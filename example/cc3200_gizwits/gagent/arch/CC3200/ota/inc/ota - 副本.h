#ifndef _OTA_H_
#define _OTA_H_
#include "gagent.h"

static int32 DRV_OTAPacketHandle_V4( pgcontext pgc,int8 *buf,int32 Len,int32 FirmwareLen,
                                    int8 *FV,uint8 * MD5,int32 socket_cloud);
static int32 GAgent_GetFirmwareLen( int8 *buf );
static int8 *GAgent_GetFirmwareName( int8* buf,int8* filename );
static int8* GAgent_GetOtaHostFromUrl( int8 *szdownloadUrl,int8 *OTA_Host );
static int8* GAgent_BuildDownLoadFirmwareBuf( int8 *szdownloadUrl,int8 *httpbuf );
static int32 GAgent_OTAResponse_Code( int8* buf );
int32 GAgent_WIFIOTAByUrl( pgcontext pgc,int8 *szdownloadUrl );
uint32 GAgent_DeleteFirmware( int32 offset,int32 filelen );
#endif