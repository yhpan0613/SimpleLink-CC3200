/*
 * Copyright (c) 2012-2014, Texas Instruments Incorporated
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * *  Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *
 * *  Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * *  Neither the name of Texas Instruments Incorporated nor the names of
 *    its contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#ifdef __cplusplus
extern "C" {
#endif

#include "ti/sysbios/family/arm/m3/Hwi.h"

#if defined(__ti__)

/*
 *  ======== Core_hwiDisable ========
 */
#define ti_sysbios_family_arm_ducati_Core_hwiDisable() _set_interrupt_priority(ti_sysbios_family_arm_m3_Hwi_disablePriority)

/*
 *  ======== Core_hwiEnable ========
 */
#define ti_sysbios_family_arm_ducati_Core_hwiEnable() _set_interrupt_priority(0)

/*
 *  ======== Core_hwiRestore ========
 */
#define ti_sysbios_family_arm_ducati_Core_hwiRestore(key) _set_interrupt_priority(key)

#else

/*
 *  ======== Core_hwiDisable ========
 */
static inline UInt ti_sysbios_family_arm_ducati_Core_hwiDisable()
{
    UInt key;
    //UInt disablePri = ti_sysbios_family_arm_m3_Hwi_disablePriority;
    __asm__ __volatile__ (
            "mrs %0, basepri\n\t"
            "msr basepri, %1"
            : "=r" (key)
            : "r" (ti_sysbios_family_arm_m3_Hwi_disablePriority)
            );
    return key;
}

/*
 *  ======== Core_hwiEnable ========
 */
static inline UInt ti_sysbios_family_arm_ducati_Core_hwiEnable()
{
    UInt key;
    __asm__ __volatile__ (
            "movw r12, #0\n\t"
            "mrs %0, basepri\n\t"
            "msr basepri, r12"
            : "=r" (key)
            :: "r12"
            );
    return key;
}


/*
 *  ======== Core_hwiRestore ========
 */
static inline Void ti_sysbios_family_arm_ducati_Core_hwiRestore(UInt key)
{
    __asm__ __volatile__ (
            "msr basepri, %0"
            :: "r" (key)
            );
}

#endif

#ifdef __cplusplus
}
#endif
