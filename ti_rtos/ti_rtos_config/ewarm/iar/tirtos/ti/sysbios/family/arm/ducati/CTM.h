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
 *     FUNCTION DECLARATIONS
 *     SYSTEM FUNCTIONS
 *     
 *     EPILOGUE
 *     STATE STRUCTURES
 *     PREFIX ALIASES
 */


/*
 * ======== PROLOGUE ========
 */

#ifndef ti_sysbios_family_arm_ducati_CTM__include
#define ti_sysbios_family_arm_ducati_CTM__include

#ifndef __nested__
#define __nested__
#define ti_sysbios_family_arm_ducati_CTM__top__
#endif

#ifdef __cplusplus
#define __extern extern "C"
#else
#define __extern extern
#endif

#define ti_sysbios_family_arm_ducati_CTM___VERS 160


/*
 * ======== INCLUDES ========
 */

#include <xdc/std.h>

#include <xdc/runtime/xdc.h>
#include <xdc/runtime/Types.h>
#include <ti/sysbios/family/arm/ducati/package/package.defs.h>

#include <xdc/runtime/IModule.h>


/*
 * ======== AUXILIARY DEFINITIONS ========
 */

/* CTM */
typedef xdc_UInt32 __T1_ti_sysbios_family_arm_ducati_CTM_CTM__RES_04;
typedef xdc_UInt32 __ARRAY1_ti_sysbios_family_arm_ducati_CTM_CTM__RES_04[7];
typedef __ARRAY1_ti_sysbios_family_arm_ducati_CTM_CTM__RES_04 __TA_ti_sysbios_family_arm_ducati_CTM_CTM__RES_04;
typedef xdc_UInt32 __T1_ti_sysbios_family_arm_ducati_CTM_CTM__RES_34;
typedef xdc_UInt32 __ARRAY1_ti_sysbios_family_arm_ducati_CTM_CTM__RES_34[3];
typedef __ARRAY1_ti_sysbios_family_arm_ducati_CTM_CTM__RES_34 __TA_ti_sysbios_family_arm_ducati_CTM_CTM__RES_34;
typedef xdc_UInt32 __T1_ti_sysbios_family_arm_ducati_CTM_CTM__TINTVLR;
typedef xdc_UInt32 __ARRAY1_ti_sysbios_family_arm_ducati_CTM_CTM__TINTVLR[8];
typedef __ARRAY1_ti_sysbios_family_arm_ducati_CTM_CTM__TINTVLR __TA_ti_sysbios_family_arm_ducati_CTM_CTM__TINTVLR;
typedef xdc_UInt32 __T1_ti_sysbios_family_arm_ducati_CTM_CTM__RES_60;
typedef xdc_UInt32 __ARRAY1_ti_sysbios_family_arm_ducati_CTM_CTM__RES_60[7];
typedef __ARRAY1_ti_sysbios_family_arm_ducati_CTM_CTM__RES_60 __TA_ti_sysbios_family_arm_ducati_CTM_CTM__RES_60;
typedef xdc_UInt32 __T1_ti_sysbios_family_arm_ducati_CTM_CTM__CTDBGSGL;
typedef xdc_UInt32 __ARRAY1_ti_sysbios_family_arm_ducati_CTM_CTM__CTDBGSGL[8];
typedef __ARRAY1_ti_sysbios_family_arm_ducati_CTM_CTM__CTDBGSGL __TA_ti_sysbios_family_arm_ducati_CTM_CTM__CTDBGSGL;
typedef xdc_UInt32 __T1_ti_sysbios_family_arm_ducati_CTM_CTM__RES_A0;
typedef xdc_UInt32 __ARRAY1_ti_sysbios_family_arm_ducati_CTM_CTM__RES_A0[20];
typedef __ARRAY1_ti_sysbios_family_arm_ducati_CTM_CTM__RES_A0 __TA_ti_sysbios_family_arm_ducati_CTM_CTM__RES_A0;
typedef xdc_UInt32 __T1_ti_sysbios_family_arm_ducati_CTM_CTM__CTGNBL;
typedef xdc_UInt32 __ARRAY1_ti_sysbios_family_arm_ducati_CTM_CTM__CTGNBL[2];
typedef __ARRAY1_ti_sysbios_family_arm_ducati_CTM_CTM__CTGNBL __TA_ti_sysbios_family_arm_ducati_CTM_CTM__CTGNBL;
typedef xdc_UInt32 __T1_ti_sysbios_family_arm_ducati_CTM_CTM__CTGRST;
typedef xdc_UInt32 __ARRAY1_ti_sysbios_family_arm_ducati_CTM_CTM__CTGRST[2];
typedef __ARRAY1_ti_sysbios_family_arm_ducati_CTM_CTM__CTGRST __TA_ti_sysbios_family_arm_ducati_CTM_CTM__CTGRST;
typedef xdc_UInt32 __T1_ti_sysbios_family_arm_ducati_CTM_CTM__CTCR;
typedef xdc_UInt32 __ARRAY1_ti_sysbios_family_arm_ducati_CTM_CTM__CTCR[32];
typedef __ARRAY1_ti_sysbios_family_arm_ducati_CTM_CTM__CTCR __TA_ti_sysbios_family_arm_ducati_CTM_CTM__CTCR;
typedef xdc_UInt32 __T1_ti_sysbios_family_arm_ducati_CTM_CTM__CTCNTR;
typedef xdc_UInt32 __ARRAY1_ti_sysbios_family_arm_ducati_CTM_CTM__CTCNTR[32];
typedef __ARRAY1_ti_sysbios_family_arm_ducati_CTM_CTM__CTCNTR __TA_ti_sysbios_family_arm_ducati_CTM_CTM__CTCNTR;
struct ti_sysbios_family_arm_ducati_CTM_CTM {
    xdc_UInt32 CTCNTL;
    __TA_ti_sysbios_family_arm_ducati_CTM_CTM__RES_04 RES_04;
    xdc_UInt32 CTSTMCNTL;
    xdc_UInt32 CTSTMMSTID;
    xdc_UInt32 CTSTMINTVL;
    xdc_UInt32 CTSTMSEL0;
    xdc_UInt32 CTSTMSEL1;
    __TA_ti_sysbios_family_arm_ducati_CTM_CTM__RES_34 RES_34;
    __TA_ti_sysbios_family_arm_ducati_CTM_CTM__TINTVLR TINTVLR;
    __TA_ti_sysbios_family_arm_ducati_CTM_CTM__RES_60 RES_60;
    xdc_UInt32 CTNUMDBG;
    __TA_ti_sysbios_family_arm_ducati_CTM_CTM__CTDBGSGL CTDBGSGL;
    __TA_ti_sysbios_family_arm_ducati_CTM_CTM__RES_A0 RES_A0;
    __TA_ti_sysbios_family_arm_ducati_CTM_CTM__CTGNBL CTGNBL;
    __TA_ti_sysbios_family_arm_ducati_CTM_CTM__CTGRST CTGRST;
    __TA_ti_sysbios_family_arm_ducati_CTM_CTM__CTCR CTCR;
    __TA_ti_sysbios_family_arm_ducati_CTM_CTM__CTCNTR CTCNTR;
};

/* ctm */
#define ti_sysbios_family_arm_ducati_CTM_ctm ti_sysbios_family_arm_ducati_CTM_ctm
__extern volatile ti_sysbios_family_arm_ducati_CTM_CTM ti_sysbios_family_arm_ducati_CTM_ctm;


/*
 * ======== INTERNAL DEFINITIONS ========
 */


/*
 * ======== MODULE-WIDE CONFIGS ========
 */

/* Module__diagsEnabled */
typedef xdc_Bits32 CT__ti_sysbios_family_arm_ducati_CTM_Module__diagsEnabled;
__extern __FAR__ const CT__ti_sysbios_family_arm_ducati_CTM_Module__diagsEnabled ti_sysbios_family_arm_ducati_CTM_Module__diagsEnabled__C;

/* Module__diagsIncluded */
typedef xdc_Bits32 CT__ti_sysbios_family_arm_ducati_CTM_Module__diagsIncluded;
__extern __FAR__ const CT__ti_sysbios_family_arm_ducati_CTM_Module__diagsIncluded ti_sysbios_family_arm_ducati_CTM_Module__diagsIncluded__C;

/* Module__diagsMask */
typedef xdc_Bits16 *CT__ti_sysbios_family_arm_ducati_CTM_Module__diagsMask;
__extern __FAR__ const CT__ti_sysbios_family_arm_ducati_CTM_Module__diagsMask ti_sysbios_family_arm_ducati_CTM_Module__diagsMask__C;

/* Module__gateObj */
typedef xdc_Ptr CT__ti_sysbios_family_arm_ducati_CTM_Module__gateObj;
__extern __FAR__ const CT__ti_sysbios_family_arm_ducati_CTM_Module__gateObj ti_sysbios_family_arm_ducati_CTM_Module__gateObj__C;

/* Module__gatePrms */
typedef xdc_Ptr CT__ti_sysbios_family_arm_ducati_CTM_Module__gatePrms;
__extern __FAR__ const CT__ti_sysbios_family_arm_ducati_CTM_Module__gatePrms ti_sysbios_family_arm_ducati_CTM_Module__gatePrms__C;

/* Module__id */
typedef xdc_runtime_Types_ModuleId CT__ti_sysbios_family_arm_ducati_CTM_Module__id;
__extern __FAR__ const CT__ti_sysbios_family_arm_ducati_CTM_Module__id ti_sysbios_family_arm_ducati_CTM_Module__id__C;

/* Module__loggerDefined */
typedef xdc_Bool CT__ti_sysbios_family_arm_ducati_CTM_Module__loggerDefined;
__extern __FAR__ const CT__ti_sysbios_family_arm_ducati_CTM_Module__loggerDefined ti_sysbios_family_arm_ducati_CTM_Module__loggerDefined__C;

/* Module__loggerObj */
typedef xdc_Ptr CT__ti_sysbios_family_arm_ducati_CTM_Module__loggerObj;
__extern __FAR__ const CT__ti_sysbios_family_arm_ducati_CTM_Module__loggerObj ti_sysbios_family_arm_ducati_CTM_Module__loggerObj__C;

/* Module__loggerFxn0 */
typedef xdc_runtime_Types_LoggerFxn0 CT__ti_sysbios_family_arm_ducati_CTM_Module__loggerFxn0;
__extern __FAR__ const CT__ti_sysbios_family_arm_ducati_CTM_Module__loggerFxn0 ti_sysbios_family_arm_ducati_CTM_Module__loggerFxn0__C;

/* Module__loggerFxn1 */
typedef xdc_runtime_Types_LoggerFxn1 CT__ti_sysbios_family_arm_ducati_CTM_Module__loggerFxn1;
__extern __FAR__ const CT__ti_sysbios_family_arm_ducati_CTM_Module__loggerFxn1 ti_sysbios_family_arm_ducati_CTM_Module__loggerFxn1__C;

/* Module__loggerFxn2 */
typedef xdc_runtime_Types_LoggerFxn2 CT__ti_sysbios_family_arm_ducati_CTM_Module__loggerFxn2;
__extern __FAR__ const CT__ti_sysbios_family_arm_ducati_CTM_Module__loggerFxn2 ti_sysbios_family_arm_ducati_CTM_Module__loggerFxn2__C;

/* Module__loggerFxn4 */
typedef xdc_runtime_Types_LoggerFxn4 CT__ti_sysbios_family_arm_ducati_CTM_Module__loggerFxn4;
__extern __FAR__ const CT__ti_sysbios_family_arm_ducati_CTM_Module__loggerFxn4 ti_sysbios_family_arm_ducati_CTM_Module__loggerFxn4__C;

/* Module__loggerFxn8 */
typedef xdc_runtime_Types_LoggerFxn8 CT__ti_sysbios_family_arm_ducati_CTM_Module__loggerFxn8;
__extern __FAR__ const CT__ti_sysbios_family_arm_ducati_CTM_Module__loggerFxn8 ti_sysbios_family_arm_ducati_CTM_Module__loggerFxn8__C;

/* Module__startupDoneFxn */
typedef xdc_Bool (*CT__ti_sysbios_family_arm_ducati_CTM_Module__startupDoneFxn)(void);
__extern __FAR__ const CT__ti_sysbios_family_arm_ducati_CTM_Module__startupDoneFxn ti_sysbios_family_arm_ducati_CTM_Module__startupDoneFxn__C;

/* Object__count */
typedef xdc_Int CT__ti_sysbios_family_arm_ducati_CTM_Object__count;
__extern __FAR__ const CT__ti_sysbios_family_arm_ducati_CTM_Object__count ti_sysbios_family_arm_ducati_CTM_Object__count__C;

/* Object__heap */
typedef xdc_runtime_IHeap_Handle CT__ti_sysbios_family_arm_ducati_CTM_Object__heap;
__extern __FAR__ const CT__ti_sysbios_family_arm_ducati_CTM_Object__heap ti_sysbios_family_arm_ducati_CTM_Object__heap__C;

/* Object__sizeof */
typedef xdc_SizeT CT__ti_sysbios_family_arm_ducati_CTM_Object__sizeof;
__extern __FAR__ const CT__ti_sysbios_family_arm_ducati_CTM_Object__sizeof ti_sysbios_family_arm_ducati_CTM_Object__sizeof__C;

/* Object__table */
typedef xdc_Ptr CT__ti_sysbios_family_arm_ducati_CTM_Object__table;
__extern __FAR__ const CT__ti_sysbios_family_arm_ducati_CTM_Object__table ti_sysbios_family_arm_ducati_CTM_Object__table__C;


/*
 * ======== FUNCTION DECLARATIONS ========
 */

/* Module_startup */
#define ti_sysbios_family_arm_ducati_CTM_Module_startup( state ) (-1)

/* Module__startupDone__S */
xdc__CODESECT(ti_sysbios_family_arm_ducati_CTM_Module__startupDone__S, "ti_sysbios_family_arm_ducati_CTM_Module__startupDone__S")
__extern xdc_Bool ti_sysbios_family_arm_ducati_CTM_Module__startupDone__S( void );


/*
 * ======== SYSTEM FUNCTIONS ========
 */

/* Module_startupDone */
#define ti_sysbios_family_arm_ducati_CTM_Module_startupDone() ti_sysbios_family_arm_ducati_CTM_Module__startupDone__S()

/* Object_heap */
#define ti_sysbios_family_arm_ducati_CTM_Object_heap() ti_sysbios_family_arm_ducati_CTM_Object__heap__C

/* Module_heap */
#define ti_sysbios_family_arm_ducati_CTM_Module_heap() ti_sysbios_family_arm_ducati_CTM_Object__heap__C

/* Module_id */
static inline CT__ti_sysbios_family_arm_ducati_CTM_Module__id ti_sysbios_family_arm_ducati_CTM_Module_id( void ) 
{
    return ti_sysbios_family_arm_ducati_CTM_Module__id__C;
}

/* Module_hasMask */
static inline xdc_Bool ti_sysbios_family_arm_ducati_CTM_Module_hasMask( void ) 
{
    return ti_sysbios_family_arm_ducati_CTM_Module__diagsMask__C != NULL;
}

/* Module_getMask */
static inline xdc_Bits16 ti_sysbios_family_arm_ducati_CTM_Module_getMask( void ) 
{
    return ti_sysbios_family_arm_ducati_CTM_Module__diagsMask__C != NULL ? *ti_sysbios_family_arm_ducati_CTM_Module__diagsMask__C : 0;
}

/* Module_setMask */
static inline xdc_Void ti_sysbios_family_arm_ducati_CTM_Module_setMask( xdc_Bits16 mask ) 
{
    if (ti_sysbios_family_arm_ducati_CTM_Module__diagsMask__C != NULL) *ti_sysbios_family_arm_ducati_CTM_Module__diagsMask__C = mask;
}


/*
 * ======== EPILOGUE ========
 */

#ifdef ti_sysbios_family_arm_ducati_CTM__top__
#undef __nested__
#endif

#endif /* ti_sysbios_family_arm_ducati_CTM__include */


/*
 * ======== STATE STRUCTURES ========
 */

#if defined(__config__) || (!defined(__nested__) && defined(ti_sysbios_family_arm_ducati_CTM__internalaccess))

#ifndef ti_sysbios_family_arm_ducati_CTM__include_state
#define ti_sysbios_family_arm_ducati_CTM__include_state


#endif /* ti_sysbios_family_arm_ducati_CTM__include_state */

#endif

/*
 * ======== PREFIX ALIASES ========
 */

#if !defined(__nested__) && !defined(ti_sysbios_family_arm_ducati_CTM__nolocalnames)

#ifndef ti_sysbios_family_arm_ducati_CTM__localnames__done
#define ti_sysbios_family_arm_ducati_CTM__localnames__done

/* module prefix */
#define CTM_CTM ti_sysbios_family_arm_ducati_CTM_CTM
#define CTM_ctm ti_sysbios_family_arm_ducati_CTM_ctm
#define CTM_Module_name ti_sysbios_family_arm_ducati_CTM_Module_name
#define CTM_Module_id ti_sysbios_family_arm_ducati_CTM_Module_id
#define CTM_Module_startup ti_sysbios_family_arm_ducati_CTM_Module_startup
#define CTM_Module_startupDone ti_sysbios_family_arm_ducati_CTM_Module_startupDone
#define CTM_Module_hasMask ti_sysbios_family_arm_ducati_CTM_Module_hasMask
#define CTM_Module_getMask ti_sysbios_family_arm_ducati_CTM_Module_getMask
#define CTM_Module_setMask ti_sysbios_family_arm_ducati_CTM_Module_setMask
#define CTM_Object_heap ti_sysbios_family_arm_ducati_CTM_Object_heap
#define CTM_Module_heap ti_sysbios_family_arm_ducati_CTM_Module_heap

#endif /* ti_sysbios_family_arm_ducati_CTM__localnames__done */
#endif
