#include "netevent.h"
#include "gagent.h"

int32 GAgent_select(int32 nfds, fd_set *readfds, fd_set *writefds,
           fd_set *exceptfds,int32 sec,int32 usec )
{
    struct timeval t;

    t.tv_sec = sec;// 秒
    t.tv_usec = usec;// 微秒
    return select( nfds,readfds,writefds,exceptfds,&t );
}
/****************************************************************
Function    :   GAgent_CreateTcpServer
Description :   creat TCP server.
tcp_port    :   server port.
return      :   0> the socket id .
                other error.
Add by Alex.lin     --2015-04-24.
****************************************************************/
int32 GAgent_CreateTcpServer( uint16 tcp_port )
{
    struct sockaddr_t addr;
    //int32 bufferSize=0;
    int32 serversocketid=0;
    int32 ret =0;

    serversocketid = socket( AF_INET, SOCK_STREAM, IPPROTO_TCP );
    if( serversocketid < 0 )
    {
        serversocketid = INVALID_SOCKET;
        GAgent_Printf(GAGENT_INFO, "TCPServer socket create error");
        return RET_FAILED;
    }
    //bufferSize = SOCKET_TCPSOCKET_BUFFERSIZE;
    //setsockopt( serversocketid, SOL_SOCKET, SO_RCVBUF, &bufferSize, 4 );
    //setsockopt( serversocketid, SOL_SOCKET, SO_SNDBUF, &bufferSize, 4 );

    GAgent_Printf(GAGENT_INFO, "GAgent_CreateTcpServer ,serversocketid=%d", serversocketid);
    Gagent_setsocketnonblock(serversocketid);  //Nik.chen for test
    

   //Gagent_setsocketrectime(serversocketid);
  
    memset(&addr, 0x0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_port = htons(tcp_port);
    addr.sin_addr.s_addr = INADDR_ANY;
    if( bind( serversocketid, (struct sockaddr *)&addr, sizeof(addr)) != 0 )
    {
        GAgent_Printf(GAGENT_ERROR, "TCPSrever socket bind error");
        close(serversocketid);
        serversocketid = INVALID_SOCKET;
        return RET_FAILED;
    }

    if(tcp_port == 80)
    {
    ret = listen( serversocketid, 0 );
    if(ret != 0 )
    {
        GAgent_Printf( GAGENT_ERROR, "TCPServer 80 socket listen error 111, ret=%d!", ret);
         ret = listen( serversocketid, 4 );
          if(ret != 0 )
             GAgent_Printf( GAGENT_ERROR, "TCPServer 80 socket listen error 222, ret=%d!", ret);
        close( serversocketid );
        serversocketid = INVALID_SOCKET;
        return RET_FAILED;
    }
    }else
    {
    if(listen( serversocketid, LAN_TCPCLIENT_MAX ) != 0 )
    {
        GAgent_Printf( GAGENT_ERROR, "TCPServer socket listen error!");
        close( serversocketid );
        serversocketid = INVALID_SOCKET;
        return RET_FAILED;
    }
    }
    //Gagent_setsocketnonblock(serversocketid);  
    return serversocketid;
}

int32 GAgent_CreateUDPServer( uint16 udp_port )
{
    int32 serversocketid = 0;
    struct sockaddr_t addr;
    
    serversocketid = socket( AF_INET, SOCK_DGRAM, IPPROTO_UDP );
    if(serversocketid < 0)
    {
        GAgent_Printf(GAGENT_ERROR, "UDPServer socket create error");
        serversocketid = INVALID_SOCKET;
        return RET_FAILED;
    }
    
  Gagent_setsocketnonblock(serversocketid);
   //Gagent_setsocketrectime(serversocketid);
    
    
    memset(&addr, 0x0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_port = htons(udp_port);
    addr.sin_addr.s_addr = INADDR_ANY;
    if( bind( serversocketid, (struct sockaddr *)&addr, sizeof(addr)) != 0 )
    {
        GAgent_Printf(GAGENT_ERROR, "UDPServer socket bind error");
        close(serversocketid);
        serversocketid = INVALID_SOCKET;
        return RET_FAILED;
    }

    GAgent_Printf(GAGENT_CRITICAL,"UDP Server socketid:%d on port:%d", serversocketid, udp_port);
    return serversocketid;
}
int32 GAgent_CreateUDPBroadCastServer( uint16 udpbroadcast_port, struct sockaddr_t *sockaddr)
{
    //int udpbufsize=2;
    int32 serversocketid = 0;
    struct sockaddr_t addr;
    memset( &addr, 0, sizeof(addr) );

    serversocketid = socket( AF_INET, SOCK_DGRAM, IPPROTO_UDP );
    if(serversocketid < 0) 
    {
        GAgent_Printf(GAGENT_DEBUG, "UDP BC socket create error");
        serversocketid = INVALID_SOCKET;
        return RET_FAILED;
    }

    Gagent_setsocketnonblock(serversocketid);
    // Gagent_setsocketrectime(serversocketid);

    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = htonl(INADDR_BROADCAST);
    addr.sin_port = htons(udpbroadcast_port);

#if 0
    if( setsockopt(serversocketid, SOL_SOCKET, SO_BROADCAST, &udpbufsize,sizeof(int)) != 0 )
    {
        GAgent_Printf(GAGENT_DEBUG,"UDP BC Server setsockopt error!");
    }
    
    if(bind(serversocketid, (struct sockaddr *)&addr, sizeof(addr)) != 0)
    {
        GAgent_Printf(GAGENT_DEBUG,"UDP BC Server bind error!");
        close(serversocketid);
        serversocketid = INVALID_SOCKET;
    }
#endif

    GAgent_Printf(GAGENT_DEBUG,"UDP BC Server socketid:%d on port:%d", serversocketid, udpbroadcast_port);
    *sockaddr = addr;
    return serversocketid;
}

