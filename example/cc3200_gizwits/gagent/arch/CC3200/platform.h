#ifndef __PLATFORM_H_
#define __PLATFORM_H_

#include <stdlib.h>
#include <stdio.h>
#include <errno.h>

#include "simplelink.h"

#define _POSIX_C_SOURCE 200809L
#include "hal_uart.h"

#define WIFI_SOFTVAR    "04020006"
#define WIFI_HARDVER    "00CC3200"  //

#define UART_NAME       "/dev/ttyUSB0"
#define NET_ADAPTHER    "eth0"

#define RECONNHTTPGLAG1      "Gizwits" 
#define RECONNHTTPGLAG2      "TI-CC3200" 

extern void msleep(int m_seconds);

#define sockaddr_t sockaddr_in

#endif

