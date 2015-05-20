#include "hal_uart.h"

/****************************************************************
Function        :   open_serial
Description     :   open serial 
comport         :   serial comport number
bandrate        :   serial bandrate 
nBits           :   serial data Bit
return          :   >0 the serial fd 
                    other fail.
Add by Alex.lin     --2015-03-31
****************************************************************/
int serial_open(char *comport, int bandrate,int nBits,char nEvent,int nStop )
{
  return 0;
}
/****************************************************************
Function        :   serial_read
Description     :   read data form serial fd 
buf             :   data form serial pointer.
buflen          :   data want to read.
return          :   >0 the realy data length form serial 
Add by Alex.lin     --2015-03-31
****************************************************************/
int serial_read( int serial_fd, unsigned char *buf,int buflen )
{
  return 0;
}

/****************************************************************
Function        :   serial_write
Description     :   write data to serial fd 
buf             :   data need to serial pointer.
buflen          :   data want to write.
return          :   >0 the number of bytes written is returned
                    other error.
Add by Alex.lin     --2015-03-31
****************************************************************/
int serial_write( int serial_fd,unsigned char *buf,int buflen )
{
  return 0;
}
