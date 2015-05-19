/*
 * Copyright (c) 2013-2014, Texas Instruments Incorporated
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

/*
 *  ======== Core_getId ========
 */
#ifdef ti_sysbios_hal_Core_delegate_getId
    extern UInt ti_sysbios_hal_Core_delegate_getId();
#define ti_sysbios_hal_Core_getId() (ti_sysbios_hal_Core_delegate_getId())
#else
#define ti_sysbios_hal_Core_getId() (ti_sysbios_hal_Core_CoreProxy_getId())
#endif

/*
 *  ======== Core_interruptCore ========
 */
#ifdef ti_sysbios_hal_Core_delegate_interruptCore
    extern Void ti_sysbios_hal_Core_delegate_interruptCore(UInt);
#define ti_sysbios_hal_Core_interruptCore(x) ti_sysbios_hal_Core_delegate_interruptCore(x)
#else
#define ti_sysbios_hal_Core_interruptCore(x) ti_sysbios_hal_Core_CoreProxy_interruptCore(x)
#endif

/*
 *  ======== Core_lock ========
 */
#ifdef ti_sysbios_hal_Core_delegate_lock
    extern Void ti_sysbios_hal_Core_delegate_lock();
#define ti_sysbios_hal_Core_lock() ti_sysbios_hal_Core_delegate_lock()
#else
#define ti_sysbios_hal_Core_lock() ti_sysbios_hal_Core_CoreProxy_lock()
#endif

/*
 *  ======== Core_unlock ========
 */
#ifdef ti_sysbios_hal_Core_delegate_unlock
    extern Void ti_sysbios_hal_Core_delegate_unlock();
#define ti_sysbios_hal_Core_unlock() ti_sysbios_hal_Core_delegate_unlock()
#else
#define ti_sysbios_hal_Core_unlock() ti_sysbios_hal_Core_CoreProxy_unlock()
#endif

#if defined(xdc_target__isaCompatible_v7M) || defined(xdc_target__isaCompatible_v7M4)

#include "ti/sysbios/family/arm/m3/Hwi.h"

#if defined(__ti__)

/*
 *  ======== Core_hwiDisable ========
 */
#define ti_sysbios_hal_Core_hwiDisable() _set_interrupt_priority(ti_sysbios_family_arm_m3_Hwi_disablePriority)

/*
 *  ======== Core_hwiEnable ========
 */
#define ti_sysbios_hal_Core_hwiEnable() _set_interrupt_priority(0)

/*
 *  ======== Core_hwiRestore ========
 */
#define ti_sysbios_hal_Core_hwiRestore(key) _set_interrupt_priority(key)

#else

/*
 *  ======== Core_hwiDisable ========
 */
static inline UInt ti_sysbios_hal_Core_hwiDisable()
{
    UInt key;
    asm volatile (
            "mrs %0, basepri\n\t"
            "msr basepri, %1"
            : "=&r" (key)
            : "r" (ti_sysbios_family_arm_m3_Hwi_disablePriority)
            );
    return key;
}

/*
 *  ======== Core_hwiEnable ========
 */
static inline UInt ti_sysbios_hal_Core_hwiEnable()
{
    UInt key;
    asm volatile (
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
static inline Void ti_sysbios_hal_Core_hwiRestore(UInt key)
{
    asm volatile (
            "msr basepri, %0"
            :: "r" (key)
            );
}

#endif

#else /* all other targets */

/*
 *  ======== Core_hwiDisable ========
 */
#define ti_sysbios_hal_Core_hwiDisable() (ti_sysbios_hal_Hwi_disable())

/*
 *  ======== Core_hwiEnable ========
 */
#define ti_sysbios_hal_Core_hwiEnable() (ti_sysbios_hal_Hwi_enable())

/*
 *  ======== Core_hwiRestore ========
 */
#define ti_sysbios_hal_Core_hwiRestore(key) (ti_sysbios_hal_Hwi_restore(key))

#endif

#ifdef __cplusplus
}
#endif
