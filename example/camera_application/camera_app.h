//*****************************************************************************
// camera_app.h
//
// camera application macro & API's prototypes
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

#ifndef __CAMERA_APP_H__
#define __CAMERA_APP_H__

//*****************************************************************************
//
// If building with a C++ compiler, make all of the definitions in this header
// have a C binding.
//
//*****************************************************************************
#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */

#define UART_COMMAND_IMG_CAPTURE                    ('x')
#define LOWER_TO_UPPER_CASE                         (32)

#define DISABLE                           (0)
#define ENABLE                            (1)
#define SL_VERSION_LENGTH                 (11)

#ifdef ENABLE_JPEG
    // Capture is stopped at 'CAM_INT_FE' anyway 
    #ifdef XGA_FRAME
        #define PIXELS_IN_X_AXIS        (1024)
        #define PIXELS_IN_Y_AXIS        (768)

        #define NUM_OF_1KB_BUFFERS      120
    #elif VGA_FRAME
        #define PIXELS_IN_X_AXIS        (640)
        #define PIXELS_IN_Y_AXIS        (480)

        #define NUM_OF_1KB_BUFFERS      80
    #elif QVGA_FRAME
        #define PIXELS_IN_X_AXIS        (240)
        #define PIXELS_IN_Y_AXIS        (320)

        #define NUM_OF_1KB_BUFFERS      50
    #endif
#else   
    #ifdef QVGA_FRAME
        #define PIXELS_IN_X_AXIS        (240)
        #define PIXELS_IN_Y_AXIS        (256)

        #define NUM_OF_1KB_BUFFERS      120
    #endif
#endif  

#define BYTES_PER_PIXEL             (2)       // RGB 565 
#define FRAME_SIZE_IN_BYTES         \
(PIXELS_IN_X_AXIS * PIXELS_IN_Y_AXIS * BYTES_PER_PIXEL)

#define ONE_KB                      (1024)
#define IMAGE_BUF_SIZE              (ONE_KB * NUM_OF_1KB_BUFFERS)

#define NUM_OF_4B_CHUNKS            ((IMAGE_BUF_SIZE)/(sizeof(unsigned long)))
#define NUM_OF_1KB_CHUNKS           (IMAGE_BUF_SIZE/ONE_KB)
#define NUM_OF_4B_CHUNKS_IN_1KB     (ONE_KB/(sizeof(unsigned long)))

#define MAX_EMAIL_ID_LENGTH         34
#define SMTP_BUF_LEN                1024
  
//*****************************************************************************
//
// If building with a C++ compiler, make all of the definitions in this header
// have a C binding.
//
//*****************************************************************************
#ifdef __cplusplus
extern "C"
{
#endif


#define CAM_BT_CORRECT_EN   0x00001000

typedef enum opcd{
    START_CAPTURE = 1,
    STOP_CAPTURE,
    IMG_FMT,
    IMG_SIZE,
    EXIT
}e_opcode;

typedef struct cmd_struct{
    long    opcode;
    char    email_id[MAX_EMAIL_ID_LENGTH];
}s_cmd_struct;


//******************************************************************************
// APIs
//******************************************************************************

void StartCamera(void);

//*****************************************************************************
//
// Mark the end of the C bindings section for C++ compilers.
//
//*****************************************************************************
#ifdef __cplusplus
}
#endif /* __cplusplus */
#endif /* __CAMERA_APP_H__ */

//*****************************************************************************
//
// Close the Doxygen group.
//! @}
//
//*****************************************************************************
