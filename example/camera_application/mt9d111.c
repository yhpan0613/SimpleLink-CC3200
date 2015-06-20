//*****************************************************************************
//  MT9D111.c
//
// Micron MT9D111 camera sensor driver
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
//! \addtogroup mt9d111
//! @{
//
//*****************************************************************************
#include <stdio.h>
#include <stdbool.h>
#include <stdint.h>
#include "mt9d111.h"
#include "hw_types.h"
#include "rom.h"
#include "rom_map.h"
#include "hw_memmap.h"
#include "i2c.h"
#include "i2cconfig.h"
#include "common.h"

#define RET_OK                  0
#define RET_ERROR               -1
#define SENSOR_PAGE_REG         0xF0
#define CAM_I2C_SLAVE_ADDR      ((0xBA >> 1))

typedef struct MT9D111RegLst
{
    unsigned char ucPageAddr;
    unsigned char ucRegAddr;
    unsigned short usValue;
} s_RegList;

#ifndef ENABLE_JPEG
static const s_RegList preview_on_cmd_list[]= {
    {1, 0xC6, 0xA103    },  // SEQ_CMD
    {1, 0xC8, 0x0001    },  // SEQ_CMD, Do Preview
    {1, 0xC6, 0xA104    },  // SEQ_CMD
    {111, 0xC8, 0x0003  },  // SEQ_CMD, Do Preview
    {1, 0xC6, 0xA103    },  // SEQ_CMD-refresh
    {1, 0xC8, 0x0005    },  // SEQ_CMD-refresh
    {1, 0xC6, 0xA103    },  // SEQ_CMD-refresh
    {1, 0xC8, 0x0006    },  // SEQ_CMD-refresh
    {1, 0xC6, 0xA104    },  // SEQ_CMD
    {111, 0xC8, 0x0003  },  // SEQ_CMD, Do Preview
    {100, 0x00, 0x01E0  },  // Delay = 500ms
};

static  const s_RegList freq_setup_cmd_List[]= {
    {1, 0xC6, 0x276D    },  // MODE_FIFO_CONF1_A
    {1, 0xC8, 0xE4E2    },  // MODE_FIFO_CONF1_A
    {1, 0xC6, 0xA76F    },  // MODE_FIFO_CONF2_A
    {1, 0xC8, 0x00E8    },  // MODE_FIFO_CONF2_A
    {1, 0xC6, 0xA103    },  // SEQ_CMD
    {1, 0xC8, 0x0005    },  // SEQ_CMD (Refresh)
   // Set maximum integration time to get a minimum of 15 fps at 45MHz
    {1, 0xC6, 0xA20E    },  // AE_MAX_INDEX
    {1, 0xC8, 0x0004},      // AE_MAX_INDEX
    {1, 0xC6, 0xA102    },  // SEQ_MODE
    {1, 0xC8, 0x0001    },  // SEQ_MODE
    {1, 0xC6, 0xA102    },  // SEQ_MODE
    {1, 0xC8, 0x0005    },  // SEQ_MODE
   // Set minimum integration time to get a maximum of 15 fps at 45MHz
    {1, 0xC6, 0xA20D    },  // AE_MAX_INDEX
    {1, 0xC8, 0x0004    },  // AE_MAX_INDEX
    {1, 0xC6, 0xA103    },  // SEQ_CMD
    {1, 0xC8, 0x0005    },  // SEQ_CMD (Refresh)
};

static  const s_RegList image_size_240_320_preview_cmds_list[]=
{
    {0, 0x07, 0x00FE    },  // HORZ_BLANK_A
    {0, 0x08, 0x02A0    },  // VERT_BLANK_A
    {0, 0x20, 0x0303    },  // READ_MODE_B (Image flip settings)
    {0, 0x21, 0x8400    },  // READ_MODE_A (1ADC)
    {1, 0xC6, 0x2703    },  // MODE_OUTPUT_WIDTH_A
    {1, 0xC8, 0x00F0    },  // MODE_OUTPUT_WIDTH_A
    {1, 0xC6, 0x2705    },  // MODE_OUTPUT_HEIGHT_A
    {1, 0xC8, 0x0140    },  // MODE_OUTPUT_HEIGHT_A
    {1, 0xC6, 0x2727    },  // MODE_CROP_X0_A
    {1, 0xC8, 0x0000    },  // MODE_CROP_X0_A
    {1, 0xC6, 0x2729    },  // MODE_CROP_X1_A
    {1, 0xC8, 0x00F0    },  // MODE_CROP_X1_A
    {1, 0xC6, 0x272B    },  // MODE_CROP_Y0_A
    {1, 0xC8, 0x0000    },  // MODE_CROP_Y0_A
    {1, 0xC6, 0x272D    },  // MODE_CROP_Y1_A
    {1, 0xC8, 0x0140    },  // MODE_CROP_Y1_A
    {1, 0xC6, 0x270F    },  // MODE_SENSOR_ROW_START_A
    {1, 0xC8, 0x001C    },  // MODE_SENSOR_ROW_START_A
    {1, 0xC6, 0x2711    },  // MODE_SENSOR_COL_START_A
    {1, 0xC8, 0x003C    },  // MODE_SENSOR_COL_START_A
    {1, 0xC6, 0x2713    },  // MODE_SENSOR_ROW_HEIGHT_A
    {1, 0xC8, 0x0280    },  // MODE_SENSOR_ROW_HEIGHT_A
    {1, 0xC6, 0x2715    },  // MODE_SENSOR_COL_WIDTH_A
    {1, 0xC8, 0x03C0    },  // MODE_SENSOR_COL_WIDTH_A
    {1, 0xC6, 0x2717    },  // MODE_SENSOR_X_DELAY_A
    {1, 0xC8, 0x0088    },  // MODE_SENSOR_X_DELAY_A
    {1, 0xC6, 0x2719    },  // MODE_SENSOR_ROW_SPEED_A
    {1, 0xC8, 0x0011    },  // MODE_SENSOR_ROW_SPEED_A
    {1, 0xC6, 0xA103    },  // SEQ_CMD
    {1, 0xC8, 0x0005    },  // SEQ_CMD
    {1, 0xC6, 0xA103    },  // SEQ_CMD
    {1, 0xC8, 0x0006    },  // SEQ_CMD
};

static  const s_RegList preview_cmds_list[]= {

    {1, 0xC6, 0xA77D    },  // MODE_OUTPUT_FORMAT_A
    {1, 0xC8, 0x0020    },  // MODE_OUTPUT_FORMAT_A; RGB565
    {1, 0xC6, 0x270B    },  // MODE_CONFIG
    {1, 0xC8, 0x0030    },  // MODE_CONFIG, JPEG disabled for A and B
    {1, 0xC6, 0xA103    },  // SEQ_CMD
    {1, 0xC8, 0x0005    }   // SEQ_CMD, refresh
};
#else 
static  const s_RegList capture_cmds_list[]= {
    {0, 0x65, 0xA000    },  // Disable PLL
    {0, 0x65, 0xE000    },  // Power DOWN PLL
    {100, 0x00, 0x01F4  },  // Delay =500ms
    {0,  0x66,  0x1E03  },
    {0,  0x67,  0x0501  },
    {0, 0x65,   0xA000  },  // Disable PLL
    {0,  0x65,  0x2000  },  // Enable PLL
    {0, 0x20, 0x0000    },  // READ_MODE_B (Image flip settings)
    {100, 0x00, 0x01F4  },  // Delay =500ms
    {100, 0x00, 0x01F4  },  // Delay =500ms
    {100, 0x00, 0x01F4  },  // Delay =500ms
    {1, 0xC6, 0xA102    },  // SEQ_MODE
    {1, 0xC8, 0x0001    },  // SEQ_MODE
    {1, 0xC6, 0xA102    },  // SEQ_MODE
    {1, 0xC8, 0x0005    },  // SEQ_MODE
    {1,  0xC6, 0xA120   },  // Enable Capture video
    {1,  0xC8, 0x0002   },
    {1,  0xC6, 0x270B   },  // Mode config, disable JPEG bypass
    {1,  0xC8, 0x0000   },
    {1,  0xC6, 0x2702   },  // FIFO_config0b, no spoof, adaptive clock
    {1,  0xC8, 0x001E   },
    {1,  0xC6, 0xA907   },  // JPEG mode config, video
    {1,  0xC8, 0x0035   },
    {1,  0xC6, 0xA906   },  // Format YCbCr422
    {1,  0xC8, 0x0000   },
    {1,  0xC6, 0xA90A   },  // Set the qscale1
    {1,  0xC8, 0x0089   },
    {1,  0xC6, 0x2908   },  // Set the restartInt
    {1,  0xC8, 0x0020   },
    {100, 0x00, 0x01F4  },  // Delay =500ms
    {1, 0xC6, 0x2707    },  // MODE_OUTPUT_WIDTH_B
#ifdef XGA_FRAME
    {1, 0xC8, 1024      },
#elif VGA_FRAME
    {1, 0xC8, 640       },
#elif QVGA_FRAME
    {1, 0xC8, 240       },
#endif
    {1, 0xC6, 0x2709    },  // MODE_OUTPUT_HEIGHT_B
#ifdef XGA_FRAME
    {1, 0xC8, 768       },
#elif VGA_FRAME
    {1, 0xC8, 480       },
#elif QVGA_FRAME
    {1, 0xC8, 320       },
#endif
    {1, 0xC6, 0x2735    },  // MODE_CROP_X0_B
    {1, 0xC8, 0x0000    },
    {1, 0xC6, 0x2737    },  // MODE_CROP_X1_B
    {1, 0xC8, 1600  },
    {1, 0xC6, 0x2739    },  // MODE_CROP_Y0_B
    {1, 0xC8, 0x0000    },
    {1, 0xC6, 0x273B    },  // MODE_CROP_Y1_B
    {1, 0xC8, 1200      },
    {1, 0xC6, 0xA103    },  // SEQ_CMD, Do capture
    {1, 0xC8, 0x0002    },
    {100, 0x00, 0x01F4  },  // Delay =500ms
};
#endif 

static  const s_RegList init_cmds_list[]= {
    {100,0x00,0x01F4    },
    {0, 0x33, 0x0343    }, // RESERVED_CORE_33
    {1, 0xC6, 0xA115    }, // SEQ_LLMODE
    {1, 0xC8, 0x0020    }, // SEQ_LLMODE
    {0, 0x38, 0x0866    }, // RESERVED_CORE_38
    {2, 0x80, 0x0168    }, // LENS_CORRECTION_CONTROL
    {2, 0x81, 0x6432    }, // ZONE_BOUNDS_X1_X2
    {2, 0x82, 0x3296    }, // ZONE_BOUNDS_X0_X3
    {2, 0x83, 0x9664    }, // ZONE_BOUNDS_X4_X5
    {2, 0x84, 0x5028    }, // ZONE_BOUNDS_Y1_Y2
    {2, 0x85, 0x2878    }, // ZONE_BOUNDS_Y0_Y3
    {2, 0x86, 0x7850    }, // ZONE_BOUNDS_Y4_Y5
    {2, 0x87, 0x0000    }, // CENTER_OFFSET
    {2, 0x88, 0x0152    }, // FX_RED
    {2, 0x89, 0x015C    }, // FX_GREEN
    {2, 0x8A, 0x00F4    }, // FX_BLUE
    {2, 0x8B, 0x0108    }, // FY_RED
    {2, 0x8C, 0x00FA    }, // FY_GREEN
    {2, 0x8D, 0x00CF    }, // FY_BLUE
    {2, 0x8E, 0x09AD    }, // DF_DX_RED
    {2, 0x8F, 0x091E    }, // DF_DX_GREEN
    {2, 0x90, 0x0B3F    }, // DF_DX_BLUE
    {2, 0x91, 0x0C85    }, // DF_DY_RED
    {2, 0x92, 0x0CFF    }, // DF_DY_GREEN
    {2, 0x93, 0x0D86    }, // DF_DY_BLUE
    {2, 0x94, 0x163A    }, // SECOND_DERIV_ZONE_0_RED
    {2, 0x95, 0x0E47    }, // SECOND_DERIV_ZONE_0_GREEN
    {2, 0x96, 0x103C    }, // SECOND_DERIV_ZONE_0_BLUE
    {2, 0x97, 0x1D35    }, // SECOND_DERIV_ZONE_1_RED
    {2, 0x98, 0x173E    }, // SECOND_DERIV_ZONE_1_GREEN
    {2, 0x99, 0x1119    }, // SECOND_DERIV_ZONE_1_BLUE
    {2, 0x9A, 0x1663    }, // SECOND_DERIV_ZONE_2_RED
    {2, 0x9B, 0x1569    }, // SECOND_DERIV_ZONE_2_GREEN
    {2, 0x9C, 0x104C    }, // SECOND_DERIV_ZONE_2_BLUE
    {2, 0x9D, 0x1015    }, // SECOND_DERIV_ZONE_3_RED
    {2, 0x9E, 0x1010    }, // SECOND_DERIV_ZONE_3_GREEN
    {2, 0x9F, 0x0B0A    }, // SECOND_DERIV_ZONE_3_BLUE
    {2, 0xA0, 0x0D53    }, // SECOND_DERIV_ZONE_4_RED
    {2, 0xA1, 0x0D51    }, // SECOND_DERIV_ZONE_4_GREEN
    {2, 0xA2, 0x0A44    }, // SECOND_DERIV_ZONE_4_BLUE
    {2, 0xA3, 0x1545    }, // SECOND_DERIV_ZONE_5_RED
    {2, 0xA4, 0x1643    }, // SECOND_DERIV_ZONE_5_GREEN
    {2, 0xA5, 0x1231    }, // SECOND_DERIV_ZONE_5_BLUE
    {2, 0xA6, 0x0047    }, // SECOND_DERIV_ZONE_6_RED
    {2, 0xA7, 0x035C    }, // SECOND_DERIV_ZONE_6_GREEN
    {2, 0xA8, 0xFE30    }, // SECOND_DERIV_ZONE_6_BLUE
    {2, 0xA9, 0x4625    }, // SECOND_DERIV_ZONE_7_RED
    {2, 0xAA, 0x47F3    }, // SECOND_DERIV_ZONE_7_GREEN
    {2, 0xAB, 0x5859    }, // SECOND_DERIV_ZONE_7_BLUE
    {2, 0xAC, 0x0000    }, // X2_FACTORS
    {2, 0xAD, 0x0000    }, // GLOBAL_OFFSET_FXY_FUNCTION
    {2, 0xAE, 0x0000    }, // K_FACTOR_IN_K_FX_FY
    {1, 0x08, 0x01FC    }, // COLOR_PIPELINE_CONTROL
    {1, 0xC6, 0x2003    }, // MON_ARG1
    {1, 0xC8, 0x0748    }, // MON_ARG1
    {1, 0xC6, 0xA002    }, // MON_CMD
    {1, 0xC8, 0x0001    }, // MON_CMD
    {111, 0xC8,0x0000 },
    {1, 0xC6, 0xA361    }, // AWB_TG_MIN0
    {1, 0xC8, 0x00E2    }, // AWB_TG_MIN0
    {1, 0x1F, 0x0018    }, // RESERVED_SOC1_1F
    {1, 0x51, 0x7F40    }, // RESERVED_SOC1_51
    {0, 0x33, 0x0343    }, // RESERVED_CORE_33
    {0, 0x38, 0x0868    }, // RESERVED_CORE_38
    {1, 0xC6, 0xA10F    }, // SEQ_RESET_LEVEL_TH
    {1, 0xC8, 0x0042    }, // SEQ_RESET_LEVEL_TH
    {1, 0x1F, 0x0020    }, // RESERVED_SOC1_1F
    {1, 0xC6, 0xAB04    }, // HG_MAX_DLEVEL
    {1, 0xC8, 0x0008    }, // HG_MAX_DLEVEL
    {1, 0xC6, 0xA103    }, // SEQ_CMD
    {1, 0xC8, 0x0005    }, // SEQ_CMD
    {1, 0xC6, 0xA104    }, // SEQ_CMD
    {111, 0xC8,0x0003   },
    {1, 0x08, 0x01FC    }, // COLOR_PIPELINE_CONTROL
    {1, 0x08, 0x01EC    }, // COLOR_PIPELINE_CONTROL
    {1, 0x08, 0x01FC    }, // COLOR_PIPELINE_CONTROL
    {1, 0x36, 0x0F08    }, // APERTURE_PARAMETERS
    {1, 0xC6, 0xA103    }, // SEQ_CMD
    {1, 0xC8, 0x0005    }, // SEQ_CMD
};

//*****************************************************************************
// Static Function Declarations
//*****************************************************************************
static long RegLstWrite(s_RegList *pRegLst, unsigned long ulNofItems);
extern void MT9D111Delay(unsigned long ucDelay);


//*****************************************************************************
//
//! This function initilizes the camera sensor
//!
//! \param                      None
//!
//! \return                     0 - Success
//!                             -1 - Error
//
//*****************************************************************************

long CameraSensorInit()
{
    long lRetVal = -1;
    
    lRetVal = RegLstWrite((s_RegList *)init_cmds_list, \
                                    sizeof(init_cmds_list)/sizeof(s_RegList));
    ASSERT_ON_ERROR(lRetVal);

#ifndef ENABLE_JPEG
    lRetVal = RegLstWrite((s_RegList *)preview_cmds_list,
                      sizeof(preview_cmds_list)/sizeof(s_RegList));
    ASSERT_ON_ERROR(lRetVal);
    lRetVal = RegLstWrite((s_RegList *)image_size_240_320_preview_cmds_list, \
                    sizeof(image_size_240_320_preview_cmds_list)/ \
                    sizeof(s_RegList));
    ASSERT_ON_ERROR(lRetVal);
    lRetVal = RegLstWrite((s_RegList *)freq_setup_cmd_List,
                    sizeof(freq_setup_cmd_List)/sizeof(s_RegList));
    ASSERT_ON_ERROR(lRetVal);
    lRetVal = RegLstWrite((s_RegList *)preview_on_cmd_list,
                    sizeof(preview_on_cmd_list)/sizeof(s_RegList));
    ASSERT_ON_ERROR(lRetVal);
#endif 
    return 0;
}

//*****************************************************************************
//
//! This function configures the sensor in JPEG mode
//!
//! \param                      None
//!
//! \return                     0 - Success
//!                             -1 - Error
//
//*****************************************************************************
long StartSensorInJpegMode()
{
#ifdef ENABLE_JPEG
    long lRetVal = -1;

    lRetVal = RegLstWrite((s_RegList *)capture_cmds_list,
                        sizeof(capture_cmds_list)/sizeof(s_RegList));
    ASSERT_ON_ERROR(lRetVal);    
#endif 
    return 0;
}

//*****************************************************************************
//
//! This function implements the Register Write in MT9D111 sensor
//!
//! \param1                     Register List
//! \param2                     No. Of Items
//!
//! \return                     0 - Success
//!                             -1 - Error
//
//*****************************************************************************
static long RegLstWrite(s_RegList *pRegLst, unsigned long ulNofItems)
{
    unsigned long       ulNdx;
    unsigned short      usTemp;
    unsigned char       i;
    unsigned char       ucBuffer[20];
    unsigned long       ulSize;
    long lRetVal = -1;

    if(pRegLst == NULL)
    {
        return RET_ERROR;
    }
    
    for(ulNdx = 0; ulNdx < ulNofItems; ulNdx++)
    {
        if(pRegLst->ucPageAddr == 100)
        {
            // PageAddr == 100, insret a delay equal to reg value 
            MT9D111Delay(pRegLst->usValue * 80000/3);
        }
        else if(pRegLst->ucPageAddr == 111)
        {
            // PageAddr == 111, wait for specified register value 
            do
            {
                ucBuffer[0] = pRegLst->ucRegAddr;
                lRetVal = I2CBufferWrite(CAM_I2C_SLAVE_ADDR,ucBuffer,1,1);
                ASSERT_ON_ERROR(lRetVal);
                if(I2CBufferRead(CAM_I2C_SLAVE_ADDR,ucBuffer,2,1))
                {
                    return RET_ERROR;
                }

                usTemp = ucBuffer[0] << 8;
                usTemp |= ucBuffer[1];
            }while(usTemp != pRegLst->usValue);
        }
        else
        {
            // Set the page 
            ucBuffer[0] = SENSOR_PAGE_REG;
            ucBuffer[1] = 0x00;
            ucBuffer[2] = (unsigned char)(pRegLst->ucPageAddr);
            if(0 != I2CBufferWrite(CAM_I2C_SLAVE_ADDR,ucBuffer,3,I2C_SEND_STOP))
            {
                return RET_ERROR;
            }

            ucBuffer[0] = SENSOR_PAGE_REG;
            lRetVal = I2CBufferWrite(CAM_I2C_SLAVE_ADDR,ucBuffer,1,I2C_SEND_STOP);
            ASSERT_ON_ERROR(lRetVal);
            lRetVal = I2CBufferRead(CAM_I2C_SLAVE_ADDR,ucBuffer,2,I2C_SEND_STOP);
            ASSERT_ON_ERROR(lRetVal);

            ucBuffer[0] = pRegLst->ucRegAddr;

            if(pRegLst->ucPageAddr  == 0x1 && pRegLst->ucRegAddr == 0xC8)
            {
                usTemp = 0xC8;
                i=1;
                while(pRegLst->ucRegAddr == usTemp)
                {
                    ucBuffer[i] = (unsigned char)(pRegLst->usValue >> 8);
                    ucBuffer[i+1] = (unsigned char)(pRegLst->usValue & 0xFF);
                    i += 2;
                    usTemp++;
                    pRegLst++;
                    ulNdx++;
                }

                ulSize = (i-2)*2 + 1;
                ulNdx--;
                pRegLst--;
            }
            else
            {
                ulSize = 3;
                ucBuffer[1] = (unsigned char)(pRegLst->usValue >> 8);
                ucBuffer[2] = (unsigned char)(pRegLst->usValue & 0xFF);
            }

            if(0 != I2CBufferWrite(CAM_I2C_SLAVE_ADDR,ucBuffer,
                                                      ulSize,I2C_SEND_STOP))
            {
                return RET_ERROR;
            }
        }

        pRegLst++;
        MT9D111Delay(10);
    }

    return RET_OK;
}

//*****************************************************************************
//
// Close the Doxygen group.
//! @}
//
//*****************************************************************************
