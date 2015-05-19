/*
 *  Do not modify this file; it is automatically 
 *  generated and any modifications will be overwritten.
 *
 * @(#) xdc-A32
 */

/*
 * ======== GENERATED SECTIONS ========
 *     
 *     PROLOGUE
 *     INCLUDES
 *     
 *     INTERNAL DEFINITIONS
 *     MODULE-WIDE CONFIGS
 *     VIRTUAL FUNCTIONS
 *     FUNCTION DECLARATIONS
 *     CONVERTORS
 *     SYSTEM FUNCTIONS
 *     
 *     EPILOGUE
 *     STATE STRUCTURES
 *     PREFIX ALIASES
 */


/*
 * ======== PROLOGUE ========
 */

#ifndef ti_sysbios_family_arm_ducati_Core__include
#define ti_sysbios_family_arm_ducati_Core__include

#ifndef __nested__
#define __nested__
#define ti_sysbios_family_arm_ducati_Core__top__
#endif

#ifdef __cplusplus
#define __extern extern "C"
#else
#define __extern extern
#endif

#define ti_sysbios_family_arm_ducati_Core___VERS 160


/*
 * ======== INCLUDES ========
 */

#include <xdc/std.h>

#include <xdc/runtime/xdc.h>
#include <xdc/runtime/Types.h>
#include <ti/sysbios/family/arm/ducati/Core__prologue.h>
#include <ti/sysbios/family/arm/ducati/package/package.defs.h>

#include <xdc/runtime/Error.h>
#include <ti/sysbios/family/arm/ducati/GateSmp.h>
#include <ti/sysbios/interfaces/ICore.h>


/*
 * ======== AUXILIARY DEFINITIONS ========
 */

/* HWI_SCHEDULER_LOCK */
#define ti_sysbios_family_arm_ducati_Core_HWI_SCHEDULER_LOCK (0x0001)

/* SWI_SCHEDULER_LOCK */
#define ti_sysbios_family_arm_ducati_Core_SWI_SCHEDULER_LOCK (0x0002)

/* TASK_SCHEDULER_LOCK */
#define ti_sysbios_family_arm_ducati_Core_TASK_SCHEDULER_LOCK (0x0004)


/*
 * ======== INTERNAL DEFINITIONS ========
 */

/* Module_State */
typedef xdc_Bool __T1_ti_sysbios_family_arm_ducati_Core_Module_State__gateEntered;
typedef xdc_Bool __ARRAY1_ti_sysbios_family_arm_ducati_Core_Module_State__gateEntered[2];
typedef __ARRAY1_ti_sysbios_family_arm_ducati_Core_Module_State__gateEntered __TA_ti_sysbios_family_arm_ducati_Core_Module_State__gateEntered;
typedef xdc_UInt __T1_ti_sysbios_family_arm_ducati_Core_Module_State__schedulerInts;
typedef xdc_UInt __ARRAY1_ti_sysbios_family_arm_ducati_Core_Module_State__schedulerInts[2];
typedef __ARRAY1_ti_sysbios_family_arm_ducati_Core_Module_State__schedulerInts __TA_ti_sysbios_family_arm_ducati_Core_Module_State__schedulerInts;
typedef xdc_Char __T1_ti_sysbios_family_arm_ducati_Core_Module_State__core1HwiStack;
typedef xdc_Char *__ARRAY1_ti_sysbios_family_arm_ducati_Core_Module_State__core1HwiStack;
typedef __ARRAY1_ti_sysbios_family_arm_ducati_Core_Module_State__core1HwiStack __TA_ti_sysbios_family_arm_ducati_Core_Module_State__core1HwiStack;


/*
 * ======== MODULE-WIDE CONFIGS ========
 */

/* Module__diagsEnabled */
typedef xdc_Bits32 CT__ti_sysbios_family_arm_ducati_Core_Module__diagsEnabled;
__extern __FAR__ const CT__ti_sysbios_family_arm_ducati_Core_Module__diagsEnabled ti_sysbios_family_arm_ducati_Core_Module__diagsEnabled__C;

/* Module__diagsIncluded */
typedef xdc_Bits32 CT__ti_sysbios_family_arm_ducati_Core_Module__diagsIncluded;
__extern __FAR__ const CT__ti_sysbios_family_arm_ducati_Core_Module__diagsIncluded ti_sysbios_family_arm_ducati_Core_Module__diagsIncluded__C;

/* Module__diagsMask */
typedef xdc_Bits16 *CT__ti_sysbios_family_arm_ducati_Core_Module__diagsMask;
__extern __FAR__ const CT__ti_sysbios_family_arm_ducati_Core_Module__diagsMask ti_sysbios_family_arm_ducati_Core_Module__diagsMask__C;

/* Module__gateObj */
typedef xdc_Ptr CT__ti_sysbios_family_arm_ducati_Core_Module__gateObj;
__extern __FAR__ const CT__ti_sysbios_family_arm_ducati_Core_Module__gateObj ti_sysbios_family_arm_ducati_Core_Module__gateObj__C;

/* Module__gatePrms */
typedef xdc_Ptr CT__ti_sysbios_family_arm_ducati_Core_Module__gatePrms;
__extern __FAR__ const CT__ti_sysbios_family_arm_ducati_Core_Module__gatePrms ti_sysbios_family_arm_ducati_Core_Module__gatePrms__C;

/* Module__id */
typedef xdc_runtime_Types_ModuleId CT__ti_sysbios_family_arm_ducati_Core_Module__id;
__extern __FAR__ const CT__ti_sysbios_family_arm_ducati_Core_Module__id ti_sysbios_family_arm_ducati_Core_Module__id__C;

/* Module__loggerDefined */
typedef xdc_Bool CT__ti_sysbios_family_arm_ducati_Core_Module__loggerDefined;
__extern __FAR__ const CT__ti_sysbios_family_arm_ducati_Core_Module__loggerDefined ti_sysbios_family_arm_ducati_Core_Module__loggerDefined__C;

/* Module__loggerObj */
typedef xdc_Ptr CT__ti_sysbios_family_arm_ducati_Core_Module__loggerObj;
__extern __FAR__ const CT__ti_sysbios_family_arm_ducati_Core_Module__loggerObj ti_sysbios_family_arm_ducati_Core_Module__loggerObj__C;

/* Module__loggerFxn0 */
typedef xdc_runtime_Types_LoggerFxn0 CT__ti_sysbios_family_arm_ducati_Core_Module__loggerFxn0;
__extern __FAR__ const CT__ti_sysbios_family_arm_ducati_Core_Module__loggerFxn0 ti_sysbios_family_arm_ducati_Core_Module__loggerFxn0__C;

/* Module__loggerFxn1 */
typedef xdc_runtime_Types_LoggerFxn1 CT__ti_sysbios_family_arm_ducati_Core_Module__loggerFxn1;
__extern __FAR__ const CT__ti_sysbios_family_arm_ducati_Core_Module__loggerFxn1 ti_sysbios_family_arm_ducati_Core_Module__loggerFxn1__C;

/* Module__loggerFxn2 */
typedef xdc_runtime_Types_LoggerFxn2 CT__ti_sysbios_family_arm_ducati_Core_Module__loggerFxn2;
__extern __FAR__ const CT__ti_sysbios_family_arm_ducati_Core_Module__loggerFxn2 ti_sysbios_family_arm_ducati_Core_Module__loggerFxn2__C;

/* Module__loggerFxn4 */
typedef xdc_runtime_Types_LoggerFxn4 CT__ti_sysbios_family_arm_ducati_Core_Module__loggerFxn4;
__extern __FAR__ const CT__ti_sysbios_family_arm_ducati_Core_Module__loggerFxn4 ti_sysbios_family_arm_ducati_Core_Module__loggerFxn4__C;

/* Module__loggerFxn8 */
typedef xdc_runtime_Types_LoggerFxn8 CT__ti_sysbios_family_arm_ducati_Core_Module__loggerFxn8;
__extern __FAR__ const CT__ti_sysbios_family_arm_ducati_Core_Module__loggerFxn8 ti_sysbios_family_arm_ducati_Core_Module__loggerFxn8__C;

/* Module__startupDoneFxn */
typedef xdc_Bool (*CT__ti_sysbios_family_arm_ducati_Core_Module__startupDoneFxn)(void);
__extern __FAR__ const CT__ti_sysbios_family_arm_ducati_Core_Module__startupDoneFxn ti_sysbios_family_arm_ducati_Core_Module__startupDoneFxn__C;

/* Object__count */
typedef xdc_Int CT__ti_sysbios_family_arm_ducati_Core_Object__count;
__extern __FAR__ const CT__ti_sysbios_family_arm_ducati_Core_Object__count ti_sysbios_family_arm_ducati_Core_Object__count__C;

/* Object__heap */
typedef xdc_runtime_IHeap_Handle CT__ti_sysbios_family_arm_ducati_Core_Object__heap;
__extern __FAR__ const CT__ti_sysbios_family_arm_ducati_Core_Object__heap ti_sysbios_family_arm_ducati_Core_Object__heap__C;

/* Object__sizeof */
typedef xdc_SizeT CT__ti_sysbios_family_arm_ducati_Core_Object__sizeof;
__extern __FAR__ const CT__ti_sysbios_family_arm_ducati_Core_Object__sizeof ti_sysbios_family_arm_ducati_Core_Object__sizeof__C;

/* Object__table */
typedef xdc_Ptr CT__ti_sysbios_family_arm_ducati_Core_Object__table;
__extern __FAR__ const CT__ti_sysbios_family_arm_ducati_Core_Object__table ti_sysbios_family_arm_ducati_Core_Object__table__C;

/* E_mismatchedIds */
#define ti_sysbios_family_arm_ducati_Core_E_mismatchedIds (ti_sysbios_family_arm_ducati_Core_E_mismatchedIds__C)
typedef xdc_runtime_Error_Id CT__ti_sysbios_family_arm_ducati_Core_E_mismatchedIds;
__extern __FAR__ const CT__ti_sysbios_family_arm_ducati_Core_E_mismatchedIds ti_sysbios_family_arm_ducati_Core_E_mismatchedIds__C;

/* numCores */
#ifdef ti_sysbios_family_arm_ducati_Core_numCores__D
#define ti_sysbios_family_arm_ducati_Core_numCores (ti_sysbios_family_arm_ducati_Core_numCores__D)
#else
#define ti_sysbios_family_arm_ducati_Core_numCores (ti_sysbios_family_arm_ducati_Core_numCores__C)
typedef xdc_UInt CT__ti_sysbios_family_arm_ducati_Core_numCores;
__extern __FAR__ const CT__ti_sysbios_family_arm_ducati_Core_numCores ti_sysbios_family_arm_ducati_Core_numCores__C;
#endif

/* id */
#ifdef ti_sysbios_family_arm_ducati_Core_id__D
#define ti_sysbios_family_arm_ducati_Core_id (ti_sysbios_family_arm_ducati_Core_id__D)
#else
#define ti_sysbios_family_arm_ducati_Core_id (ti_sysbios_family_arm_ducati_Core_id__C)
typedef xdc_UInt CT__ti_sysbios_family_arm_ducati_Core_id;
__extern __FAR__ const CT__ti_sysbios_family_arm_ducati_Core_id ti_sysbios_family_arm_ducati_Core_id__C;
#endif

/* ipuId */
#ifdef ti_sysbios_family_arm_ducati_Core_ipuId__D
#define ti_sysbios_family_arm_ducati_Core_ipuId (ti_sysbios_family_arm_ducati_Core_ipuId__D)
#else
#define ti_sysbios_family_arm_ducati_Core_ipuId (ti_sysbios_family_arm_ducati_Core_ipuId__C)
typedef xdc_UInt CT__ti_sysbios_family_arm_ducati_Core_ipuId;
__extern __FAR__ const CT__ti_sysbios_family_arm_ducati_Core_ipuId ti_sysbios_family_arm_ducati_Core_ipuId__C;
#endif

/* core1HwiStackSize */
#ifdef ti_sysbios_family_arm_ducati_Core_core1HwiStackSize__D
#define ti_sysbios_family_arm_ducati_Core_core1HwiStackSize (ti_sysbios_family_arm_ducati_Core_core1HwiStackSize__D)
#else
#define ti_sysbios_family_arm_ducati_Core_core1HwiStackSize (ti_sysbios_family_arm_ducati_Core_core1HwiStackSize__C)
typedef xdc_SizeT CT__ti_sysbios_family_arm_ducati_Core_core1HwiStackSize;
__extern __FAR__ const CT__ti_sysbios_family_arm_ducati_Core_core1HwiStackSize ti_sysbios_family_arm_ducati_Core_core1HwiStackSize__C;
#endif

/* syncExits */
#ifdef ti_sysbios_family_arm_ducati_Core_syncExits__D
#define ti_sysbios_family_arm_ducati_Core_syncExits (ti_sysbios_family_arm_ducati_Core_syncExits__D)
#else
#define ti_sysbios_family_arm_ducati_Core_syncExits (ti_sysbios_family_arm_ducati_Core_syncExits__C)
typedef xdc_Bool CT__ti_sysbios_family_arm_ducati_Core_syncExits;
__extern __FAR__ const CT__ti_sysbios_family_arm_ducati_Core_syncExits ti_sysbios_family_arm_ducati_Core_syncExits__C;
#endif

/* gate */
#define ti_sysbios_family_arm_ducati_Core_gate (ti_sysbios_family_arm_ducati_Core_gate__C)
typedef ti_sysbios_family_arm_ducati_GateSmp_Handle CT__ti_sysbios_family_arm_ducati_Core_gate;
__extern __FAR__ const CT__ti_sysbios_family_arm_ducati_Core_gate ti_sysbios_family_arm_ducati_Core_gate__C;


/*
 * ======== VIRTUAL FUNCTIONS ========
 */

/* Fxns__ */
struct ti_sysbios_family_arm_ducati_Core_Fxns__ {
    xdc_runtime_Types_Base* __base;
    const xdc_runtime_Types_SysFxns2 *__sysp;
    xdc_UInt (*getId)(void);
    xdc_Void (*interruptCore)(xdc_UInt);
    xdc_Void (*lock)(void);
    xdc_Void (*unlock)(void);
    xdc_UInt (*hwiDisable)(void);
    xdc_UInt (*hwiEnable)(void);
    xdc_Void (*hwiRestore)(xdc_UInt);
    xdc_runtime_Types_SysFxns2 __sfxns;
};

/* Module__FXNS__C */
__extern const ti_sysbios_family_arm_ducati_Core_Fxns__ ti_sysbios_family_arm_ducati_Core_Module__FXNS__C;


/*
 * ======== FUNCTION DECLARATIONS ========
 */

/* Module_startup */
#define ti_sysbios_family_arm_ducati_Core_Module_startup ti_sysbios_family_arm_ducati_Core_Module_startup__E
xdc__CODESECT(ti_sysbios_family_arm_ducati_Core_Module_startup__E, "ti_sysbios_family_arm_ducati_Core_Module_startup")
__extern xdc_Int ti_sysbios_family_arm_ducati_Core_Module_startup__E( xdc_Int state );
xdc__CODESECT(ti_sysbios_family_arm_ducati_Core_Module_startup__F, "ti_sysbios_family_arm_ducati_Core_Module_startup")
__extern xdc_Int ti_sysbios_family_arm_ducati_Core_Module_startup__F( xdc_Int state );
xdc__CODESECT(ti_sysbios_family_arm_ducati_Core_Module_startup__R, "ti_sysbios_family_arm_ducati_Core_Module_startup")
__extern xdc_Int ti_sysbios_family_arm_ducati_Core_Module_startup__R( xdc_Int state );

/* Module__startupDone__S */
xdc__CODESECT(ti_sysbios_family_arm_ducati_Core_Module__startupDone__S, "ti_sysbios_family_arm_ducati_Core_Module__startupDone__S")
__extern xdc_Bool ti_sysbios_family_arm_ducati_Core_Module__startupDone__S( void );

/* getId__E */
#define ti_sysbios_family_arm_ducati_Core_getId ti_sysbios_family_arm_ducati_Core_getId__E
xdc__CODESECT(ti_sysbios_family_arm_ducati_Core_getId__E, "ti_sysbios_family_arm_ducati_Core_getId")
__extern xdc_UInt ti_sysbios_family_arm_ducati_Core_getId__E( void );
xdc__CODESECT(ti_sysbios_family_arm_ducati_Core_getId__F, "ti_sysbios_family_arm_ducati_Core_getId")
__extern xdc_UInt ti_sysbios_family_arm_ducati_Core_getId__F( void );
__extern xdc_UInt ti_sysbios_family_arm_ducati_Core_getId__R( void );

/* interruptCore__E */
#define ti_sysbios_family_arm_ducati_Core_interruptCore ti_sysbios_family_arm_ducati_Core_interruptCore__E
xdc__CODESECT(ti_sysbios_family_arm_ducati_Core_interruptCore__E, "ti_sysbios_family_arm_ducati_Core_interruptCore")
__extern xdc_Void ti_sysbios_family_arm_ducati_Core_interruptCore__E( xdc_UInt coreId );
xdc__CODESECT(ti_sysbios_family_arm_ducati_Core_interruptCore__F, "ti_sysbios_family_arm_ducati_Core_interruptCore")
__extern xdc_Void ti_sysbios_family_arm_ducati_Core_interruptCore__F( xdc_UInt coreId );
__extern xdc_Void ti_sysbios_family_arm_ducati_Core_interruptCore__R( xdc_UInt coreId );

/* lock__E */
#define ti_sysbios_family_arm_ducati_Core_lock ti_sysbios_family_arm_ducati_Core_lock__E
xdc__CODESECT(ti_sysbios_family_arm_ducati_Core_lock__E, "ti_sysbios_family_arm_ducati_Core_lock")
__extern xdc_Void ti_sysbios_family_arm_ducati_Core_lock__E( void );
xdc__CODESECT(ti_sysbios_family_arm_ducati_Core_lock__F, "ti_sysbios_family_arm_ducati_Core_lock")
__extern xdc_Void ti_sysbios_family_arm_ducati_Core_lock__F( void );
__extern xdc_Void ti_sysbios_family_arm_ducati_Core_lock__R( void );

/* unlock__E */
#define ti_sysbios_family_arm_ducati_Core_unlock ti_sysbios_family_arm_ducati_Core_unlock__E
xdc__CODESECT(ti_sysbios_family_arm_ducati_Core_unlock__E, "ti_sysbios_family_arm_ducati_Core_unlock")
__extern xdc_Void ti_sysbios_family_arm_ducati_Core_unlock__E( void );
xdc__CODESECT(ti_sysbios_family_arm_ducati_Core_unlock__F, "ti_sysbios_family_arm_ducati_Core_unlock")
__extern xdc_Void ti_sysbios_family_arm_ducati_Core_unlock__F( void );
__extern xdc_Void ti_sysbios_family_arm_ducati_Core_unlock__R( void );

/* getIpuId__E */
#define ti_sysbios_family_arm_ducati_Core_getIpuId ti_sysbios_family_arm_ducati_Core_getIpuId__E
xdc__CODESECT(ti_sysbios_family_arm_ducati_Core_getIpuId__E, "ti_sysbios_family_arm_ducati_Core_getIpuId")
__extern xdc_UInt ti_sysbios_family_arm_ducati_Core_getIpuId__E( void );
xdc__CODESECT(ti_sysbios_family_arm_ducati_Core_getIpuId__F, "ti_sysbios_family_arm_ducati_Core_getIpuId")
__extern xdc_UInt ti_sysbios_family_arm_ducati_Core_getIpuId__F( void );
__extern xdc_UInt ti_sysbios_family_arm_ducati_Core_getIpuId__R( void );

/* hwiFunc__I */
#define ti_sysbios_family_arm_ducati_Core_hwiFunc ti_sysbios_family_arm_ducati_Core_hwiFunc__I
xdc__CODESECT(ti_sysbios_family_arm_ducati_Core_hwiFunc__I, "ti_sysbios_family_arm_ducati_Core_hwiFunc")
__extern xdc_Void ti_sysbios_family_arm_ducati_Core_hwiFunc__I( xdc_UArg arg );

/* startCore1__I */
#define ti_sysbios_family_arm_ducati_Core_startCore1 ti_sysbios_family_arm_ducati_Core_startCore1__I
xdc__CODESECT(ti_sysbios_family_arm_ducati_Core_startCore1__I, "ti_sysbios_family_arm_ducati_Core_startCore1")
__extern xdc_Void ti_sysbios_family_arm_ducati_Core_startCore1__I( void );

/* core1Startup__I */
#define ti_sysbios_family_arm_ducati_Core_core1Startup ti_sysbios_family_arm_ducati_Core_core1Startup__I
xdc__CODESECT(ti_sysbios_family_arm_ducati_Core_core1Startup__I, "ti_sysbios_family_arm_ducati_Core_core1Startup")
__extern xdc_Void ti_sysbios_family_arm_ducati_Core_core1Startup__I( void );

/* atexit__I */
#define ti_sysbios_family_arm_ducati_Core_atexit ti_sysbios_family_arm_ducati_Core_atexit__I
xdc__CODESECT(ti_sysbios_family_arm_ducati_Core_atexit__I, "ti_sysbios_family_arm_ducati_Core_atexit")
__extern xdc_Void ti_sysbios_family_arm_ducati_Core_atexit__I( xdc_Int arg );


/*
 * ======== CONVERTORS ========
 */

/* Module_upCast */
static inline ti_sysbios_interfaces_ICore_Module ti_sysbios_family_arm_ducati_Core_Module_upCast( void )
{
    return (ti_sysbios_interfaces_ICore_Module)&ti_sysbios_family_arm_ducati_Core_Module__FXNS__C;
}

/* Module_to_ti_sysbios_interfaces_ICore */
#define ti_sysbios_family_arm_ducati_Core_Module_to_ti_sysbios_interfaces_ICore ti_sysbios_family_arm_ducati_Core_Module_upCast


/*
 * ======== SYSTEM FUNCTIONS ========
 */

/* Module_startupDone */
#define ti_sysbios_family_arm_ducati_Core_Module_startupDone() ti_sysbios_family_arm_ducati_Core_Module__startupDone__S()

/* Object_heap */
#define ti_sysbios_family_arm_ducati_Core_Object_heap() ti_sysbios_family_arm_ducati_Core_Object__heap__C

/* Module_heap */
#define ti_sysbios_family_arm_ducati_Core_Module_heap() ti_sysbios_family_arm_ducati_Core_Object__heap__C

/* Module_id */
static inline CT__ti_sysbios_family_arm_ducati_Core_Module__id ti_sysbios_family_arm_ducati_Core_Module_id( void ) 
{
    return ti_sysbios_family_arm_ducati_Core_Module__id__C;
}

/* Module_hasMask */
static inline xdc_Bool ti_sysbios_family_arm_ducati_Core_Module_hasMask( void ) 
{
    return ti_sysbios_family_arm_ducati_Core_Module__diagsMask__C != NULL;
}

/* Module_getMask */
static inline xdc_Bits16 ti_sysbios_family_arm_ducati_Core_Module_getMask( void ) 
{
    return ti_sysbios_family_arm_ducati_Core_Module__diagsMask__C != NULL ? *ti_sysbios_family_arm_ducati_Core_Module__diagsMask__C : 0;
}

/* Module_setMask */
static inline xdc_Void ti_sysbios_family_arm_ducati_Core_Module_setMask( xdc_Bits16 mask ) 
{
    if (ti_sysbios_family_arm_ducati_Core_Module__diagsMask__C != NULL) *ti_sysbios_family_arm_ducati_Core_Module__diagsMask__C = mask;
}


/*
 * ======== EPILOGUE ========
 */

#include <ti/sysbios/family/arm/ducati/Core__epilogue.h>

#ifdef ti_sysbios_family_arm_ducati_Core__top__
#undef __nested__
#endif

#endif /* ti_sysbios_family_arm_ducati_Core__include */


/*
 * ======== STATE STRUCTURES ========
 */

#if defined(__config__) || (!defined(__nested__) && defined(ti_sysbios_family_arm_ducati_Core__internalaccess))

#ifndef ti_sysbios_family_arm_ducati_Core__include_state
#define ti_sysbios_family_arm_ducati_Core__include_state

/* Module_State */
struct ti_sysbios_family_arm_ducati_Core_Module_State {
    volatile xdc_Bool exitFlag;
    __TA_ti_sysbios_family_arm_ducati_Core_Module_State__gateEntered gateEntered;
    __TA_ti_sysbios_family_arm_ducati_Core_Module_State__schedulerInts schedulerInts;
    __TA_ti_sysbios_family_arm_ducati_Core_Module_State__core1HwiStack core1HwiStack;
};

/* Module__state__V */
extern struct ti_sysbios_family_arm_ducati_Core_Module_State__ ti_sysbios_family_arm_ducati_Core_Module__state__V;

#endif /* ti_sysbios_family_arm_ducati_Core__include_state */

#endif


/*
 * ======== PREFIX ALIASES ========
 */

#if !defined(__nested__) && !defined(ti_sysbios_family_arm_ducati_Core__nolocalnames)

#ifndef ti_sysbios_family_arm_ducati_Core__localnames__done
#define ti_sysbios_family_arm_ducati_Core__localnames__done

/* module prefix */
#define Core_HWI_SCHEDULER_LOCK ti_sysbios_family_arm_ducati_Core_HWI_SCHEDULER_LOCK
#define Core_SWI_SCHEDULER_LOCK ti_sysbios_family_arm_ducati_Core_SWI_SCHEDULER_LOCK
#define Core_TASK_SCHEDULER_LOCK ti_sysbios_family_arm_ducati_Core_TASK_SCHEDULER_LOCK
#define Core_Module_State ti_sysbios_family_arm_ducati_Core_Module_State
#define Core_E_mismatchedIds ti_sysbios_family_arm_ducati_Core_E_mismatchedIds
#define Core_numCores ti_sysbios_family_arm_ducati_Core_numCores
#define Core_id ti_sysbios_family_arm_ducati_Core_id
#define Core_ipuId ti_sysbios_family_arm_ducati_Core_ipuId
#define Core_core1HwiStackSize ti_sysbios_family_arm_ducati_Core_core1HwiStackSize
#define Core_syncExits ti_sysbios_family_arm_ducati_Core_syncExits
#define Core_gate ti_sysbios_family_arm_ducati_Core_gate
#define Core_getId ti_sysbios_family_arm_ducati_Core_getId
#define Core_interruptCore ti_sysbios_family_arm_ducati_Core_interruptCore
#define Core_lock ti_sysbios_family_arm_ducati_Core_lock
#define Core_unlock ti_sysbios_family_arm_ducati_Core_unlock
#define Core_hwiDisable ti_sysbios_family_arm_ducati_Core_hwiDisable
#define Core_hwiEnable ti_sysbios_family_arm_ducati_Core_hwiEnable
#define Core_hwiRestore ti_sysbios_family_arm_ducati_Core_hwiRestore
#define Core_getIpuId ti_sysbios_family_arm_ducati_Core_getIpuId
#define Core_Module_name ti_sysbios_family_arm_ducati_Core_Module_name
#define Core_Module_id ti_sysbios_family_arm_ducati_Core_Module_id
#define Core_Module_startup ti_sysbios_family_arm_ducati_Core_Module_startup
#define Core_Module_startupDone ti_sysbios_family_arm_ducati_Core_Module_startupDone
#define Core_Module_hasMask ti_sysbios_family_arm_ducati_Core_Module_hasMask
#define Core_Module_getMask ti_sysbios_family_arm_ducati_Core_Module_getMask
#define Core_Module_setMask ti_sysbios_family_arm_ducati_Core_Module_setMask
#define Core_Object_heap ti_sysbios_family_arm_ducati_Core_Object_heap
#define Core_Module_heap ti_sysbios_family_arm_ducati_Core_Module_heap
#define Core_Module_upCast ti_sysbios_family_arm_ducati_Core_Module_upCast
#define Core_Module_to_ti_sysbios_interfaces_ICore ti_sysbios_family_arm_ducati_Core_Module_to_ti_sysbios_interfaces_ICore

#endif /* ti_sysbios_family_arm_ducati_Core__localnames__done */
#endif
