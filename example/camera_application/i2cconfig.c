//*****************************************************************************
// i2cconfig.c
//
// I2C features APIs
//
// Copyright (C) 2014 Texas Instruments Incorporated - http://www.ti.com/ 
// 
// 
//  Redistribution and use in source and binary forms, with or without 
//  modification, are permitted provided that the following conditions 
//  are met:
//
//    Redistributions of source code must retain the above copyright 
//    notice, this list of conditions and the following disclaimer.
//
//    Redistributions in binary form must reproduce the above copyright
//    notice, this list of conditions and the following disclaimer in the 
//    documentation and/or other materials provided with the   
//    distribution.
//
//    Neither the name of Texas Instruments Incorporated nor the names of
//    its contributors may be used to endorse or promote products derived
//    from this software without specific prior written permission.
//
//  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS 
//  "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT 
//  LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
//  A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT 
//  OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, 
//  SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT 
//  LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
//  DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
//  THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT 
//  (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE 
//  OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//
//*****************************************************************************
//*****************************************************************************
//
//! \addtogroup i2cconfig
//! @{
//
//*****************************************************************************
#include <stdbool.h>
#include <stdint.h>
#include "hw_types.h"
#include "rom.h"
#include "rom_map.h"
#include "hw_memmap.h"
#include "i2c.h"
#include "prcm.h"
#include "i2cconfig.h"


void MT9D111Delay(unsigned long ucDelay);

//*****************************************************************************
//
//! This function implements delay in the camera sensor
//!
//! \param                      delay value
//!
//! \return                     None
//
//*****************************************************************************
#if defined(ewarm)
    void MT9D111Delay(unsigned long ucDelay)
    {
    __asm("    subs    r0, #1\n"
          "    bne.n   MT9D111Delay\n"
          "    bx      lr");
    }
#endif
#if defined(ccs)

    __asm("    .sect \".text:MT9D111Delay\"\n"
          "    .clink\n"
          "    .thumbfunc MT9D111Delay\n"
          "    .thumb\n"
          "    .global MT9D111Delay\n"
          "MT9D111Delay:\n"
          "    subs r0, #1\n"
          "    bne.n MT9D111Delay\n"
          "    bx lr\n");

#endif

//*****************************************************************************
//
//!     I2CInit
//!    
//!    \param                      Delay  
//!     \return                     None or error code                            
//
//*****************************************************************************
unsigned long I2CInit()
{
    // Enable I2C Peripheral 
    MAP_PRCMPeripheralClkEnable(PRCM_I2CA0, PRCM_RUN_MODE_CLK);
    MAP_PRCMPeripheralReset(PRCM_I2CA0);

    // Configure I2C module, 400 Kbps fast mode 
    MAP_I2CMasterInitExpClk(I2CA0_BASE,80000000,false);
    //  MAP_I2CMasterDisable(I2CA0_BASE);

    return 0;
}

//****************************************************************************
//
//! Invokes the I2C driver APIs to read from the device. This assumes the 
//! device local address to read from is set using the I2CWrite API.
//!
//! \param      ucDevAddr is the device I2C slave address
//! \param      ucBuffer is the pointer to the read data to be placed
//! \param      ulSize is the length of data to be read
//! \param      ucFlags Flag
//! 
//! This function works in a polling mode,
//!    1. Writes the device register address to be written to.
//!    2. In a loop, reads all the bytes over I2C
//!
//! \return 0: Success, < 0: Failure.
//
//****************************************************************************
unsigned long I2CBufferRead(unsigned char ucDevAddr, unsigned char *ucBuffer,
                            unsigned long ulSize,unsigned char ucFlags)
{
    unsigned long ulNdx;

    // Set I2C codec slave address 
    MAP_I2CMasterSlaveAddrSet(I2CA0_BASE,ucDevAddr, true);
    MAP_I2CMasterIntClearEx(I2CA0_BASE, I2C_INT_MASTER);

    if(ulSize == 1)
    {
        // Start single transfer. 
        MAP_I2CMasterControl(I2CA0_BASE, I2C_MASTER_CMD_SINGLE_RECEIVE);
    }
    else
    {
        // Start the transfer. 
        MAP_I2CMasterControl(I2CA0_BASE, I2C_MASTER_CMD_BURST_RECEIVE_START);

        // Wait for transfer completion. 
        while((MAP_I2CMasterIntStatusEx(I2CA0_BASE, false) &
                                                   I2C_INT_MASTER) == 0)
        {
        }

        // Read first byte from the controller. 
        ucBuffer[0] = MAP_I2CMasterDataGet(I2CA0_BASE);

        for(ulNdx=1; ulNdx < ulSize-1; ulNdx++)
        {
            MT9D111Delay(10);
            MAP_I2CMasterIntClearEx(I2CA0_BASE, I2C_INT_MASTER);

            // continue the transfer. 
            MAP_I2CMasterControl(I2CA0_BASE, I2C_MASTER_CMD_BURST_RECEIVE_CONT);

            // Wait for transfer completion. 
            while((MAP_I2CMasterIntStatusEx(I2CA0_BASE, false) & 
                                                         I2C_INT_MASTER) == 0)
            {
            }

            // Read next byte from the controller. 
            ucBuffer[ulNdx] = MAP_I2CMasterDataGet(I2CA0_BASE);
        }

        MAP_I2CMasterIntClearEx(I2CA0_BASE, I2C_INT_MASTER);
        MAP_I2CMasterControl(I2CA0_BASE,I2C_MASTER_CMD_BURST_RECEIVE_FINISH);
    }

    // Wait for transfer completion. 
    while((MAP_I2CMasterIntStatusEx(I2CA0_BASE, false) & I2C_INT_MASTER) == 0)
    {
    }

    // Read the last byte from the controller. 
    ucBuffer[ulSize-1] = MAP_I2CMasterDataGet(I2CA0_BASE);

    return 0;
}
//****************************************************************************
//
//! Invokes the I2C driver APIs to write to the specified address
//!
//! \param ucDevAddr is the device I2C slave address
//! \param ucBuffer is the pointer to the data to be written
//! \param ulSize is the length of data to be written
//! \param ucFlags
//! 
//! This function works in a polling mode,
//!    1. Writes the device register address to be written to.
//!    2. In a loop, writes all the bytes over I2C
//!
//! \return 0: Success, < 0: Failure.
//
//****************************************************************************

unsigned long I2CBufferWrite(unsigned char ucDevAddr, unsigned char *ucBuffer,
                             unsigned long ulSize,unsigned char ucFlags)
{
    unsigned long ulNdx;

   // Set I2C codec slave address 
    MAP_I2CMasterSlaveAddrSet(I2CA0_BASE,ucDevAddr, false);

   // Write the first byte to the controller. 
    MAP_I2CMasterDataPut(I2CA0_BASE,ucBuffer[0]);
    MAP_I2CMasterIntClearEx(I2CA0_BASE, I2C_INT_MASTER);

    if( ulSize == 1)
    {
        MAP_I2CMasterControl(I2CA0_BASE, I2C_MASTER_CMD_SINGLE_SEND);
    }
    else
    {
       // Continue the transfer. 
        MAP_I2CMasterControl(I2CA0_BASE, I2C_MASTER_CMD_BURST_SEND_START);

       // Wait until the current byte has been transferred. 
     while((MAP_I2CMasterIntStatusEx(I2CA0_BASE, false) & I2C_INT_MASTER) == 0)
        {
        }
        for(ulNdx=1; ulNdx < ulSize-1; ulNdx++)
        {
           // Write the next byte to the controller. 
            MAP_I2CMasterDataPut(I2CA0_BASE,ucBuffer[ulNdx]);

           // Clear Master Interrupt 
            MAP_I2CMasterIntClearEx(I2CA0_BASE, I2C_INT_MASTER);

           // Continue the transfer. 
            MAP_I2CMasterControl(I2CA0_BASE, I2C_MASTER_CMD_BURST_SEND_CONT);

           // Wait until the current byte has been transferred. 
            while((MAP_I2CMasterIntStatusEx(I2CA0_BASE, false) & I2C_INT_MASTER) 
                                                                 == 0)
            {
            }
        }

       // Write the last byte to the controller. 
        MAP_I2CMasterDataPut(I2CA0_BASE, ucBuffer[ulSize-1]);
        MAP_I2CMasterIntClearEx(I2CA0_BASE, I2C_INT_MASTER);

       // End the transfer. 
        MAP_I2CMasterControl(I2CA0_BASE,I2C_MASTER_CMD_BURST_SEND_FINISH);
    }

   // Wait until the current byte has been transferred. 
    while((MAP_I2CMasterIntStatusEx(I2CA0_BASE, false) & I2C_INT_MASTER) == 0)
    {
    }

    return 0;
}

//*****************************************************************************
//
// Close the Doxygen group.
//! @}
//
//*****************************************************************************
