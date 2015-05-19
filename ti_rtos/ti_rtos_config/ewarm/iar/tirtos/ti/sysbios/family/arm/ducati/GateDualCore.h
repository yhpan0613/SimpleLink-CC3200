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
 *     PER-INSTANCE TYPES
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

#ifndef ti_sysbios_family_arm_ducati_GateDualCore__include
#define ti_sysbios_family_arm_ducati_GateDualCore__include

#ifndef __nested__
#define __nested__
#define ti_sysbios_family_arm_ducati_GateDualCore__top__
#endif

#ifdef __cplusplus
#define __extern extern "C"
#else
#define __extern extern
#endif

#define ti_sysbios_family_arm_ducati_GateDualCore___VERS 160


/*
 * ======== INCLUDES ========
 */

#include <xdc/std.h>

#include <xdc/runtime/xdc.h>
#include <xdc/runtime/Types.h>
#include <xdc/runtime/IInstance.h>
#include <ti/sysbios/family/arm/ducati/package/package.defs.h>

#include <xdc/runtime/Assert.h>
#include <xdc/runtime/Error.h>
#include <xdc/runtime/IGateProvider.h>


/*
 * ======== AUXILIARY DEFINITIONS ========
 */

/* Q_BLOCKING */
#define ti_sysbios_family_arm_ducati_GateDualCore_Q_BLOCKING (1)

/* Q_PREEMPTING */
#define ti_sysbios_family_arm_ducati_GateDualCore_Q_PREEMPTING (2)

/* gateArray */
#define ti_sysbios_family_arm_ducati_GateDualCore_gateArray ti_sysbios_family_arm_ducati_GateDualCore_gateArray
__extern xdc_UInt32 ti_sysbios_family_arm_ducati_GateDualCore_gateArray[4];


/*
 * ======== INTERNAL DEFINITIONS ========
 */

/* Module_State */
typedef xdc_UInt8 __T1_ti_sysbios_family_arm_ducati_GateDualCore_Module_State__usedGates;
typedef xdc_UInt8 __ARRAY1_ti_sysbios_family_arm_ducati_GateDualCore_Module_State__usedGates[4];
typedef __ARRAY1_ti_sysbios_family_arm_ducati_GateDualCore_Module_State__usedGates __TA_ti_sysbios_family_arm_ducati_GateDualCore_Module_State__usedGates;


/*
 * ======== MODULE-WIDE CONFIGS ========
 */

/* Module__diagsEnabled */
typedef xdc_Bits32 CT__ti_sysbios_family_arm_ducati_GateDualCore_Module__diagsEnabled;
__extern __FAR__ const CT__ti_sysbios_family_arm_ducati_GateDualCore_Module__diagsEnabled ti_sysbios_family_arm_ducati_GateDualCore_Module__diagsEnabled__C;

/* Module__diagsIncluded */
typedef xdc_Bits32 CT__ti_sysbios_family_arm_ducati_GateDualCore_Module__diagsIncluded;
__extern __FAR__ const CT__ti_sysbios_family_arm_ducati_GateDualCore_Module__diagsIncluded ti_sysbios_family_arm_ducati_GateDualCore_Module__diagsIncluded__C;

/* Module__diagsMask */
typedef xdc_Bits16 *CT__ti_sysbios_family_arm_ducati_GateDualCore_Module__diagsMask;
__extern __FAR__ const CT__ti_sysbios_family_arm_ducati_GateDualCore_Module__diagsMask ti_sysbios_family_arm_ducati_GateDualCore_Module__diagsMask__C;

/* Module__gateObj */
typedef xdc_Ptr CT__ti_sysbios_family_arm_ducati_GateDualCore_Module__gateObj;
__extern __FAR__ const CT__ti_sysbios_family_arm_ducati_GateDualCore_Module__gateObj ti_sysbios_family_arm_ducati_GateDualCore_Module__gateObj__C;

/* Module__gatePrms */
typedef xdc_Ptr CT__ti_sysbios_family_arm_ducati_GateDualCore_Module__gatePrms;
__extern __FAR__ const CT__ti_sysbios_family_arm_ducati_GateDualCore_Module__gatePrms ti_sysbios_family_arm_ducati_GateDualCore_Module__gatePrms__C;

/* Module__id */
typedef xdc_runtime_Types_ModuleId CT__ti_sysbios_family_arm_ducati_GateDualCore_Module__id;
__extern __FAR__ const CT__ti_sysbios_family_arm_ducati_GateDualCore_Module__id ti_sysbios_family_arm_ducati_GateDualCore_Module__id__C;

/* Module__loggerDefined */
typedef xdc_Bool CT__ti_sysbios_family_arm_ducati_GateDualCore_Module__loggerDefined;
__extern __FAR__ const CT__ti_sysbios_family_arm_ducati_GateDualCore_Module__loggerDefined ti_sysbios_family_arm_ducati_GateDualCore_Module__loggerDefined__C;

/* Module__loggerObj */
typedef xdc_Ptr CT__ti_sysbios_family_arm_ducati_GateDualCore_Module__loggerObj;
__extern __FAR__ const CT__ti_sysbios_family_arm_ducati_GateDualCore_Module__loggerObj ti_sysbios_family_arm_ducati_GateDualCore_Module__loggerObj__C;

/* Module__loggerFxn0 */
typedef xdc_runtime_Types_LoggerFxn0 CT__ti_sysbios_family_arm_ducati_GateDualCore_Module__loggerFxn0;
__extern __FAR__ const CT__ti_sysbios_family_arm_ducati_GateDualCore_Module__loggerFxn0 ti_sysbios_family_arm_ducati_GateDualCore_Module__loggerFxn0__C;

/* Module__loggerFxn1 */
typedef xdc_runtime_Types_LoggerFxn1 CT__ti_sysbios_family_arm_ducati_GateDualCore_Module__loggerFxn1;
__extern __FAR__ const CT__ti_sysbios_family_arm_ducati_GateDualCore_Module__loggerFxn1 ti_sysbios_family_arm_ducati_GateDualCore_Module__loggerFxn1__C;

/* Module__loggerFxn2 */
typedef xdc_runtime_Types_LoggerFxn2 CT__ti_sysbios_family_arm_ducati_GateDualCore_Module__loggerFxn2;
__extern __FAR__ const CT__ti_sysbios_family_arm_ducati_GateDualCore_Module__loggerFxn2 ti_sysbios_family_arm_ducati_GateDualCore_Module__loggerFxn2__C;

/* Module__loggerFxn4 */
typedef xdc_runtime_Types_LoggerFxn4 CT__ti_sysbios_family_arm_ducati_GateDualCore_Module__loggerFxn4;
__extern __FAR__ const CT__ti_sysbios_family_arm_ducati_GateDualCore_Module__loggerFxn4 ti_sysbios_family_arm_ducati_GateDualCore_Module__loggerFxn4__C;

/* Module__loggerFxn8 */
typedef xdc_runtime_Types_LoggerFxn8 CT__ti_sysbios_family_arm_ducati_GateDualCore_Module__loggerFxn8;
__extern __FAR__ const CT__ti_sysbios_family_arm_ducati_GateDualCore_Module__loggerFxn8 ti_sysbios_family_arm_ducati_GateDualCore_Module__loggerFxn8__C;

/* Module__startupDoneFxn */
typedef xdc_Bool (*CT__ti_sysbios_family_arm_ducati_GateDualCore_Module__startupDoneFxn)(void);
__extern __FAR__ const CT__ti_sysbios_family_arm_ducati_GateDualCore_Module__startupDoneFxn ti_sysbios_family_arm_ducati_GateDualCore_Module__startupDoneFxn__C;

/* Object__count */
typedef xdc_Int CT__ti_sysbios_family_arm_ducati_GateDualCore_Object__count;
__extern __FAR__ const CT__ti_sysbios_family_arm_ducati_GateDualCore_Object__count ti_sysbios_family_arm_ducati_GateDualCore_Object__count__C;

/* Object__heap */
typedef xdc_runtime_IHeap_Handle CT__ti_sysbios_family_arm_ducati_GateDualCore_Object__heap;
__extern __FAR__ const CT__ti_sysbios_family_arm_ducati_GateDualCore_Object__heap ti_sysbios_family_arm_ducati_GateDualCore_Object__heap__C;

/* Object__sizeof */
typedef xdc_SizeT CT__ti_sysbios_family_arm_ducati_GateDualCore_Object__sizeof;
__extern __FAR__ const CT__ti_sysbios_family_arm_ducati_GateDualCore_Object__sizeof ti_sysbios_family_arm_ducati_GateDualCore_Object__sizeof__C;

/* Object__table */
typedef xdc_Ptr CT__ti_sysbios_family_arm_ducati_GateDualCore_Object__table;
__extern __FAR__ const CT__ti_sysbios_family_arm_ducati_GateDualCore_Object__table ti_sysbios_family_arm_ducati_GateDualCore_Object__table__C;

/* A_nestedEnter */
#define ti_sysbios_family_arm_ducati_GateDualCore_A_nestedEnter (ti_sysbios_family_arm_ducati_GateDualCore_A_nestedEnter__C)
typedef xdc_runtime_Assert_Id CT__ti_sysbios_family_arm_ducati_GateDualCore_A_nestedEnter;
__extern __FAR__ const CT__ti_sysbios_family_arm_ducati_GateDualCore_A_nestedEnter ti_sysbios_family_arm_ducati_GateDualCore_A_nestedEnter__C;

/* E_invalidIndex */
#define ti_sysbios_family_arm_ducati_GateDualCore_E_invalidIndex (ti_sysbios_family_arm_ducati_GateDualCore_E_invalidIndex__C)
typedef xdc_runtime_Error_Id CT__ti_sysbios_family_arm_ducati_GateDualCore_E_invalidIndex;
__extern __FAR__ const CT__ti_sysbios_family_arm_ducati_GateDualCore_E_invalidIndex ti_sysbios_family_arm_ducati_GateDualCore_E_invalidIndex__C;

/* E_gateInUse */
#define ti_sysbios_family_arm_ducati_GateDualCore_E_gateInUse (ti_sysbios_family_arm_ducati_GateDualCore_E_gateInUse__C)
typedef xdc_runtime_Error_Id CT__ti_sysbios_family_arm_ducati_GateDualCore_E_gateInUse;
__extern __FAR__ const CT__ti_sysbios_family_arm_ducati_GateDualCore_E_gateInUse ti_sysbios_family_arm_ducati_GateDualCore_E_gateInUse__C;

/* gateArrayAddress */
#define ti_sysbios_family_arm_ducati_GateDualCore_gateArrayAddress (ti_sysbios_family_arm_ducati_GateDualCore_gateArrayAddress__C)
typedef xdc_Ptr CT__ti_sysbios_family_arm_ducati_GateDualCore_gateArrayAddress;
__extern __FAR__ const CT__ti_sysbios_family_arm_ducati_GateDualCore_gateArrayAddress ti_sysbios_family_arm_ducati_GateDualCore_gateArrayAddress__C;

/* numGates */
#ifdef ti_sysbios_family_arm_ducati_GateDualCore_numGates__D
#define ti_sysbios_family_arm_ducati_GateDualCore_numGates (ti_sysbios_family_arm_ducati_GateDualCore_numGates__D)
#else
#define ti_sysbios_family_arm_ducati_GateDualCore_numGates (ti_sysbios_family_arm_ducati_GateDualCore_numGates__C)
typedef xdc_UInt CT__ti_sysbios_family_arm_ducati_GateDualCore_numGates;
__extern __FAR__ const CT__ti_sysbios_family_arm_ducati_GateDualCore_numGates ti_sysbios_family_arm_ducati_GateDualCore_numGates__C;
#endif

/* initGates */
#ifdef ti_sysbios_family_arm_ducati_GateDualCore_initGates__D
#define ti_sysbios_family_arm_ducati_GateDualCore_initGates (ti_sysbios_family_arm_ducati_GateDualCore_initGates__D)
#else
#define ti_sysbios_family_arm_ducati_GateDualCore_initGates (ti_sysbios_family_arm_ducati_GateDualCore_initGates__C)
typedef xdc_Bool CT__ti_sysbios_family_arm_ducati_GateDualCore_initGates;
__extern __FAR__ const CT__ti_sysbios_family_arm_ducati_GateDualCore_initGates ti_sysbios_family_arm_ducati_GateDualCore_initGates__C;
#endif

/* enableStats */
#ifdef ti_sysbios_family_arm_ducati_GateDualCore_enableStats__D
#define ti_sysbios_family_arm_ducati_GateDualCore_enableStats (ti_sysbios_family_arm_ducati_GateDualCore_enableStats__D)
#else
#define ti_sysbios_family_arm_ducati_GateDualCore_enableStats (ti_sysbios_family_arm_ducati_GateDualCore_enableStats__C)
typedef xdc_Bool CT__ti_sysbios_family_arm_ducati_GateDualCore_enableStats;
__extern __FAR__ const CT__ti_sysbios_family_arm_ducati_GateDualCore_enableStats ti_sysbios_family_arm_ducati_GateDualCore_enableStats__C;
#endif


/*
 * ======== PER-INSTANCE TYPES ========
 */

/* Params */
struct ti_sysbios_family_arm_ducati_GateDualCore_Params {
    size_t __size;
    const void *__self;
    void *__fxns;
    xdc_runtime_IInstance_Params *instance;
    xdc_UInt index;
    xdc_runtime_IInstance_Params __iprms;
};

/* Struct */
struct ti_sysbios_family_arm_ducati_GateDualCore_Struct {
    const ti_sysbios_family_arm_ducati_GateDualCore_Fxns__ *__fxns;
    xdc_UInt __f0;
    volatile xdc_UInt32 *__f1;
    volatile xdc_UInt8 *__f2;
    xdc_UInt __f3;
    xdc_UInt __f4;
    xdc_UInt __f5;
    xdc_UInt __f6;
    xdc_runtime_Types_CordAddr __name;
};


/*
 * ======== VIRTUAL FUNCTIONS ========
 */

/* Fxns__ */
struct ti_sysbios_family_arm_ducati_GateDualCore_Fxns__ {
    xdc_runtime_Types_Base* __base;
    const xdc_runtime_Types_SysFxns2 *__sysp;
    xdc_Bool (*query)(xdc_Int);
    xdc_IArg (*enter)(ti_sysbios_family_arm_ducati_GateDualCore_Handle);
    xdc_Void (*leave)(ti_sysbios_family_arm_ducati_GateDualCore_Handle, xdc_IArg);
    xdc_runtime_Types_SysFxns2 __sfxns;
};

/* Module__FXNS__C */
__extern const ti_sysbios_family_arm_ducati_GateDualCore_Fxns__ ti_sysbios_family_arm_ducati_GateDualCore_Module__FXNS__C;


/*
 * ======== FUNCTION DECLARATIONS ========
 */

/* Module_startup */
#define ti_sysbios_family_arm_ducati_GateDualCore_Module_startup ti_sysbios_family_arm_ducati_GateDualCore_Module_startup__E
xdc__CODESECT(ti_sysbios_family_arm_ducati_GateDualCore_Module_startup__E, "ti_sysbios_family_arm_ducati_GateDualCore_Module_startup")
__extern xdc_Int ti_sysbios_family_arm_ducati_GateDualCore_Module_startup__E( xdc_Int state );
xdc__CODESECT(ti_sysbios_family_arm_ducati_GateDualCore_Module_startup__F, "ti_sysbios_family_arm_ducati_GateDualCore_Module_startup")
__extern xdc_Int ti_sysbios_family_arm_ducati_GateDualCore_Module_startup__F( xdc_Int state );
xdc__CODESECT(ti_sysbios_family_arm_ducati_GateDualCore_Module_startup__R, "ti_sysbios_family_arm_ducati_GateDualCore_Module_startup")
__extern xdc_Int ti_sysbios_family_arm_ducati_GateDualCore_Module_startup__R( xdc_Int state );

/* Instance_init__E */
xdc__CODESECT(ti_sysbios_family_arm_ducati_GateDualCore_Instance_init__E, "ti_sysbios_family_arm_ducati_GateDualCore_Instance_init")
__extern xdc_Int ti_sysbios_family_arm_ducati_GateDualCore_Instance_init__E(ti_sysbios_family_arm_ducati_GateDualCore_Object *, const ti_sysbios_family_arm_ducati_GateDualCore_Params *, xdc_runtime_Error_Block *);

/* Instance_init__R */
xdc__CODESECT(ti_sysbios_family_arm_ducati_GateDualCore_Instance_init__R, "ti_sysbios_family_arm_ducati_GateDualCore_Instance_init")
__extern xdc_Int ti_sysbios_family_arm_ducati_GateDualCore_Instance_init__R(ti_sysbios_family_arm_ducati_GateDualCore_Object *, const ti_sysbios_family_arm_ducati_GateDualCore_Params *, xdc_runtime_Error_Block *);

/* Handle__label__S */
xdc__CODESECT(ti_sysbios_family_arm_ducati_GateDualCore_Handle__label__S, "ti_sysbios_family_arm_ducati_GateDualCore_Handle__label__S")
__extern xdc_runtime_Types_Label *ti_sysbios_family_arm_ducati_GateDualCore_Handle__label__S( xdc_Ptr obj, xdc_runtime_Types_Label *lab );

/* Module__startupDone__S */
xdc__CODESECT(ti_sysbios_family_arm_ducati_GateDualCore_Module__startupDone__S, "ti_sysbios_family_arm_ducati_GateDualCore_Module__startupDone__S")
__extern xdc_Bool ti_sysbios_family_arm_ducati_GateDualCore_Module__startupDone__S( void );

/* Object__create__S */
xdc__CODESECT(ti_sysbios_family_arm_ducati_GateDualCore_Object__create__S, "ti_sysbios_family_arm_ducati_GateDualCore_Object__create__S")
__extern xdc_Ptr ti_sysbios_family_arm_ducati_GateDualCore_Object__create__S( xdc_Ptr __oa, xdc_SizeT __osz, xdc_Ptr __aa, const xdc_UChar *__pa, xdc_SizeT __psz, xdc_runtime_Error_Block *__eb );

/* create */
xdc__CODESECT(ti_sysbios_family_arm_ducati_GateDualCore_create, "ti_sysbios_family_arm_ducati_GateDualCore_create")
__extern ti_sysbios_family_arm_ducati_GateDualCore_Handle ti_sysbios_family_arm_ducati_GateDualCore_create( const ti_sysbios_family_arm_ducati_GateDualCore_Params *__prms, xdc_runtime_Error_Block *__eb );

/* construct */
xdc__CODESECT(ti_sysbios_family_arm_ducati_GateDualCore_construct, "ti_sysbios_family_arm_ducati_GateDualCore_construct")
__extern void ti_sysbios_family_arm_ducati_GateDualCore_construct( ti_sysbios_family_arm_ducati_GateDualCore_Struct *__obj, const ti_sysbios_family_arm_ducati_GateDualCore_Params *__prms, xdc_runtime_Error_Block *__eb );

/* Object__delete__S */
xdc__CODESECT(ti_sysbios_family_arm_ducati_GateDualCore_Object__delete__S, "ti_sysbios_family_arm_ducati_GateDualCore_Object__delete__S")
__extern xdc_Void ti_sysbios_family_arm_ducati_GateDualCore_Object__delete__S( xdc_Ptr instp );

/* delete */
xdc__CODESECT(ti_sysbios_family_arm_ducati_GateDualCore_delete, "ti_sysbios_family_arm_ducati_GateDualCore_delete")
__extern Void ti_sysbios_family_arm_ducati_GateDualCore_delete(ti_sysbios_family_arm_ducati_GateDualCore_Handle *instp);

/* Object__destruct__S */
xdc__CODESECT(ti_sysbios_family_arm_ducati_GateDualCore_Object__destruct__S, "ti_sysbios_family_arm_ducati_GateDualCore_Object__destruct__S")
__extern xdc_Void ti_sysbios_family_arm_ducati_GateDualCore_Object__destruct__S( xdc_Ptr objp );

/* destruct */
xdc__CODESECT(ti_sysbios_family_arm_ducati_GateDualCore_destruct, "ti_sysbios_family_arm_ducati_GateDualCore_destruct")
__extern void ti_sysbios_family_arm_ducati_GateDualCore_destruct(ti_sysbios_family_arm_ducati_GateDualCore_Struct *obj);

/* Object__get__S */
xdc__CODESECT(ti_sysbios_family_arm_ducati_GateDualCore_Object__get__S, "ti_sysbios_family_arm_ducati_GateDualCore_Object__get__S")
__extern xdc_Ptr ti_sysbios_family_arm_ducati_GateDualCore_Object__get__S( xdc_Ptr oarr, xdc_Int i );

/* Object__first__S */
xdc__CODESECT(ti_sysbios_family_arm_ducati_GateDualCore_Object__first__S, "ti_sysbios_family_arm_ducati_GateDualCore_Object__first__S")
__extern xdc_Ptr ti_sysbios_family_arm_ducati_GateDualCore_Object__first__S( void );

/* Object__next__S */
xdc__CODESECT(ti_sysbios_family_arm_ducati_GateDualCore_Object__next__S, "ti_sysbios_family_arm_ducati_GateDualCore_Object__next__S")
__extern xdc_Ptr ti_sysbios_family_arm_ducati_GateDualCore_Object__next__S( xdc_Ptr obj );

/* Params__init__S */
xdc__CODESECT(ti_sysbios_family_arm_ducati_GateDualCore_Params__init__S, "ti_sysbios_family_arm_ducati_GateDualCore_Params__init__S")
__extern xdc_Void ti_sysbios_family_arm_ducati_GateDualCore_Params__init__S( xdc_Ptr dst, const xdc_Void *src, xdc_SizeT psz, xdc_SizeT isz );

/* query__E */
#define ti_sysbios_family_arm_ducati_GateDualCore_query ti_sysbios_family_arm_ducati_GateDualCore_query__E
xdc__CODESECT(ti_sysbios_family_arm_ducati_GateDualCore_query__E, "ti_sysbios_family_arm_ducati_GateDualCore_query")
__extern xdc_Bool ti_sysbios_family_arm_ducati_GateDualCore_query__E( xdc_Int qual );
xdc__CODESECT(ti_sysbios_family_arm_ducati_GateDualCore_query__F, "ti_sysbios_family_arm_ducati_GateDualCore_query")
__extern xdc_Bool ti_sysbios_family_arm_ducati_GateDualCore_query__F( xdc_Int qual );
__extern xdc_Bool ti_sysbios_family_arm_ducati_GateDualCore_query__R( xdc_Int qual );

/* enter__E */
#define ti_sysbios_family_arm_ducati_GateDualCore_enter ti_sysbios_family_arm_ducati_GateDualCore_enter__E
xdc__CODESECT(ti_sysbios_family_arm_ducati_GateDualCore_enter__E, "ti_sysbios_family_arm_ducati_GateDualCore_enter")
__extern xdc_IArg ti_sysbios_family_arm_ducati_GateDualCore_enter__E( ti_sysbios_family_arm_ducati_GateDualCore_Handle __inst );
xdc__CODESECT(ti_sysbios_family_arm_ducati_GateDualCore_enter__F, "ti_sysbios_family_arm_ducati_GateDualCore_enter")
__extern xdc_IArg ti_sysbios_family_arm_ducati_GateDualCore_enter__F( ti_sysbios_family_arm_ducati_GateDualCore_Object *__inst );
__extern xdc_IArg ti_sysbios_family_arm_ducati_GateDualCore_enter__R( ti_sysbios_family_arm_ducati_GateDualCore_Handle __inst );

/* leave__E */
#define ti_sysbios_family_arm_ducati_GateDualCore_leave ti_sysbios_family_arm_ducati_GateDualCore_leave__E
xdc__CODESECT(ti_sysbios_family_arm_ducati_GateDualCore_leave__E, "ti_sysbios_family_arm_ducati_GateDualCore_leave")
__extern xdc_Void ti_sysbios_family_arm_ducati_GateDualCore_leave__E( ti_sysbios_family_arm_ducati_GateDualCore_Handle __inst, xdc_IArg key );
xdc__CODESECT(ti_sysbios_family_arm_ducati_GateDualCore_leave__F, "ti_sysbios_family_arm_ducati_GateDualCore_leave")
__extern xdc_Void ti_sysbios_family_arm_ducati_GateDualCore_leave__F( ti_sysbios_family_arm_ducati_GateDualCore_Object *__inst, xdc_IArg key );
__extern xdc_Void ti_sysbios_family_arm_ducati_GateDualCore_leave__R( ti_sysbios_family_arm_ducati_GateDualCore_Handle __inst, xdc_IArg key );

/* sync__E */
#define ti_sysbios_family_arm_ducati_GateDualCore_sync ti_sysbios_family_arm_ducati_GateDualCore_sync__E
xdc__CODESECT(ti_sysbios_family_arm_ducati_GateDualCore_sync__E, "ti_sysbios_family_arm_ducati_GateDualCore_sync")
__extern xdc_Void ti_sysbios_family_arm_ducati_GateDualCore_sync__E( ti_sysbios_family_arm_ducati_GateDualCore_Handle __inst );
xdc__CODESECT(ti_sysbios_family_arm_ducati_GateDualCore_sync__F, "ti_sysbios_family_arm_ducati_GateDualCore_sync")
__extern xdc_Void ti_sysbios_family_arm_ducati_GateDualCore_sync__F( ti_sysbios_family_arm_ducati_GateDualCore_Object *__inst );
__extern xdc_Void ti_sysbios_family_arm_ducati_GateDualCore_sync__R( ti_sysbios_family_arm_ducati_GateDualCore_Handle __inst );

/* postInit__I */
#define ti_sysbios_family_arm_ducati_GateDualCore_postInit ti_sysbios_family_arm_ducati_GateDualCore_postInit__I
xdc__CODESECT(ti_sysbios_family_arm_ducati_GateDualCore_postInit__I, "ti_sysbios_family_arm_ducati_GateDualCore_postInit")
__extern xdc_Void ti_sysbios_family_arm_ducati_GateDualCore_postInit__I( ti_sysbios_family_arm_ducati_GateDualCore_Object *gate );


/*
 * ======== CONVERTORS ========
 */

/* Module_upCast */
static inline xdc_runtime_IGateProvider_Module ti_sysbios_family_arm_ducati_GateDualCore_Module_upCast( void )
{
    return (xdc_runtime_IGateProvider_Module)&ti_sysbios_family_arm_ducati_GateDualCore_Module__FXNS__C;
}

/* Module_to_xdc_runtime_IGateProvider */
#define ti_sysbios_family_arm_ducati_GateDualCore_Module_to_xdc_runtime_IGateProvider ti_sysbios_family_arm_ducati_GateDualCore_Module_upCast

/* Handle_upCast */
static inline xdc_runtime_IGateProvider_Handle ti_sysbios_family_arm_ducati_GateDualCore_Handle_upCast( ti_sysbios_family_arm_ducati_GateDualCore_Handle i )
{
    return (xdc_runtime_IGateProvider_Handle)i;
}

/* Handle_to_xdc_runtime_IGateProvider */
#define ti_sysbios_family_arm_ducati_GateDualCore_Handle_to_xdc_runtime_IGateProvider ti_sysbios_family_arm_ducati_GateDualCore_Handle_upCast

/* Handle_downCast */
static inline ti_sysbios_family_arm_ducati_GateDualCore_Handle ti_sysbios_family_arm_ducati_GateDualCore_Handle_downCast( xdc_runtime_IGateProvider_Handle i )
{
    xdc_runtime_IGateProvider_Handle i2 = (xdc_runtime_IGateProvider_Handle)i;
    return (void*)i2->__fxns == (void*)&ti_sysbios_family_arm_ducati_GateDualCore_Module__FXNS__C ? (ti_sysbios_family_arm_ducati_GateDualCore_Handle)i : 0;
}

/* Handle_from_xdc_runtime_IGateProvider */
#define ti_sysbios_family_arm_ducati_GateDualCore_Handle_from_xdc_runtime_IGateProvider ti_sysbios_family_arm_ducati_GateDualCore_Handle_downCast


/*
 * ======== SYSTEM FUNCTIONS ========
 */

/* Module_startupDone */
#define ti_sysbios_family_arm_ducati_GateDualCore_Module_startupDone() ti_sysbios_family_arm_ducati_GateDualCore_Module__startupDone__S()

/* Object_heap */
#define ti_sysbios_family_arm_ducati_GateDualCore_Object_heap() ti_sysbios_family_arm_ducati_GateDualCore_Object__heap__C

/* Module_heap */
#define ti_sysbios_family_arm_ducati_GateDualCore_Module_heap() ti_sysbios_family_arm_ducati_GateDualCore_Object__heap__C

/* Module_id */
static inline CT__ti_sysbios_family_arm_ducati_GateDualCore_Module__id ti_sysbios_family_arm_ducati_GateDualCore_Module_id( void ) 
{
    return ti_sysbios_family_arm_ducati_GateDualCore_Module__id__C;
}

/* Module_hasMask */
static inline xdc_Bool ti_sysbios_family_arm_ducati_GateDualCore_Module_hasMask( void ) 
{
    return ti_sysbios_family_arm_ducati_GateDualCore_Module__diagsMask__C != NULL;
}

/* Module_getMask */
static inline xdc_Bits16 ti_sysbios_family_arm_ducati_GateDualCore_Module_getMask( void ) 
{
    return ti_sysbios_family_arm_ducati_GateDualCore_Module__diagsMask__C != NULL ? *ti_sysbios_family_arm_ducati_GateDualCore_Module__diagsMask__C : 0;
}

/* Module_setMask */
static inline xdc_Void ti_sysbios_family_arm_ducati_GateDualCore_Module_setMask( xdc_Bits16 mask ) 
{
    if (ti_sysbios_family_arm_ducati_GateDualCore_Module__diagsMask__C != NULL) *ti_sysbios_family_arm_ducati_GateDualCore_Module__diagsMask__C = mask;
}

/* Params_init */
static inline void ti_sysbios_family_arm_ducati_GateDualCore_Params_init( ti_sysbios_family_arm_ducati_GateDualCore_Params *prms ) 
{
    if (prms) {
        ti_sysbios_family_arm_ducati_GateDualCore_Params__init__S(prms, 0, sizeof(ti_sysbios_family_arm_ducati_GateDualCore_Params), sizeof(xdc_runtime_IInstance_Params));
    }
}

/* Params_copy */
static inline void ti_sysbios_family_arm_ducati_GateDualCore_Params_copy(ti_sysbios_family_arm_ducati_GateDualCore_Params *dst, const ti_sysbios_family_arm_ducati_GateDualCore_Params *src) 
{
    if (dst) {
        ti_sysbios_family_arm_ducati_GateDualCore_Params__init__S(dst, (const void *)src, sizeof(ti_sysbios_family_arm_ducati_GateDualCore_Params), sizeof(xdc_runtime_IInstance_Params));
    }
}

/* Object_count */
#define ti_sysbios_family_arm_ducati_GateDualCore_Object_count() ti_sysbios_family_arm_ducati_GateDualCore_Object__count__C

/* Object_sizeof */
#define ti_sysbios_family_arm_ducati_GateDualCore_Object_sizeof() ti_sysbios_family_arm_ducati_GateDualCore_Object__sizeof__C

/* Object_get */
static inline ti_sysbios_family_arm_ducati_GateDualCore_Handle ti_sysbios_family_arm_ducati_GateDualCore_Object_get(ti_sysbios_family_arm_ducati_GateDualCore_Instance_State *oarr, int i) 
{
    return (ti_sysbios_family_arm_ducati_GateDualCore_Handle)ti_sysbios_family_arm_ducati_GateDualCore_Object__get__S(oarr, i);
}

/* Object_first */
static inline ti_sysbios_family_arm_ducati_GateDualCore_Handle ti_sysbios_family_arm_ducati_GateDualCore_Object_first( void )
{
    return (ti_sysbios_family_arm_ducati_GateDualCore_Handle)ti_sysbios_family_arm_ducati_GateDualCore_Object__first__S();
}

/* Object_next */
static inline ti_sysbios_family_arm_ducati_GateDualCore_Handle ti_sysbios_family_arm_ducati_GateDualCore_Object_next( ti_sysbios_family_arm_ducati_GateDualCore_Object *obj )
{
    return (ti_sysbios_family_arm_ducati_GateDualCore_Handle)ti_sysbios_family_arm_ducati_GateDualCore_Object__next__S(obj);
}

/* Handle_label */
static inline xdc_runtime_Types_Label *ti_sysbios_family_arm_ducati_GateDualCore_Handle_label( ti_sysbios_family_arm_ducati_GateDualCore_Handle inst, xdc_runtime_Types_Label *lab )
{
    return ti_sysbios_family_arm_ducati_GateDualCore_Handle__label__S(inst, lab);
}

/* Handle_name */
static inline String ti_sysbios_family_arm_ducati_GateDualCore_Handle_name( ti_sysbios_family_arm_ducati_GateDualCore_Handle inst )
{
    xdc_runtime_Types_Label lab;
    return ti_sysbios_family_arm_ducati_GateDualCore_Handle__label__S(inst, &lab)->iname;
}

/* handle */
static inline ti_sysbios_family_arm_ducati_GateDualCore_Handle ti_sysbios_family_arm_ducati_GateDualCore_handle( ti_sysbios_family_arm_ducati_GateDualCore_Struct *str )
{
    return (ti_sysbios_family_arm_ducati_GateDualCore_Handle)str;
}

/* struct */
static inline ti_sysbios_family_arm_ducati_GateDualCore_Struct *ti_sysbios_family_arm_ducati_GateDualCore_struct( ti_sysbios_family_arm_ducati_GateDualCore_Handle inst )
{
    return (ti_sysbios_family_arm_ducati_GateDualCore_Struct*)inst;
}


/*
 * ======== EPILOGUE ========
 */

#ifdef ti_sysbios_family_arm_ducati_GateDualCore__top__
#undef __nested__
#endif

#endif /* ti_sysbios_family_arm_ducati_GateDualCore__include */


/*
 * ======== STATE STRUCTURES ========
 */

#if defined(__config__) || (!defined(__nested__) && defined(ti_sysbios_family_arm_ducati_GateDualCore__internalaccess))

#ifndef ti_sysbios_family_arm_ducati_GateDualCore__include_state
#define ti_sysbios_family_arm_ducati_GateDualCore__include_state

/* Module_State */
struct ti_sysbios_family_arm_ducati_GateDualCore_Module_State {
    __TA_ti_sysbios_family_arm_ducati_GateDualCore_Module_State__usedGates usedGates;
};

/* Module__state__V */
extern struct ti_sysbios_family_arm_ducati_GateDualCore_Module_State__ ti_sysbios_family_arm_ducati_GateDualCore_Module__state__V;

/* Object */
struct ti_sysbios_family_arm_ducati_GateDualCore_Object {
    const ti_sysbios_family_arm_ducati_GateDualCore_Fxns__ *__fxns;
    xdc_UInt index;
    volatile xdc_UInt32 *gatePtr;
    volatile xdc_UInt8 *gateBytePtr;
    xdc_UInt stalls;
    xdc_UInt noStalls;
    xdc_UInt totalStalls;
    xdc_UInt maxStall;
};

#endif /* ti_sysbios_family_arm_ducati_GateDualCore__include_state */

#endif


/*
 * ======== PREFIX ALIASES ========
 */

#if !defined(__nested__) && !defined(ti_sysbios_family_arm_ducati_GateDualCore__nolocalnames)

#ifndef ti_sysbios_family_arm_ducati_GateDualCore__localnames__done
#define ti_sysbios_family_arm_ducati_GateDualCore__localnames__done

/* module prefix */
#define GateDualCore_Instance ti_sysbios_family_arm_ducati_GateDualCore_Instance
#define GateDualCore_Handle ti_sysbios_family_arm_ducati_GateDualCore_Handle
#define GateDualCore_Module ti_sysbios_family_arm_ducati_GateDualCore_Module
#define GateDualCore_Object ti_sysbios_family_arm_ducati_GateDualCore_Object
#define GateDualCore_Struct ti_sysbios_family_arm_ducati_GateDualCore_Struct
#define GateDualCore_Q_BLOCKING ti_sysbios_family_arm_ducati_GateDualCore_Q_BLOCKING
#define GateDualCore_Q_PREEMPTING ti_sysbios_family_arm_ducati_GateDualCore_Q_PREEMPTING
#define GateDualCore_gateArray ti_sysbios_family_arm_ducati_GateDualCore_gateArray
#define GateDualCore_Instance_State ti_sysbios_family_arm_ducati_GateDualCore_Instance_State
#define GateDualCore_Module_State ti_sysbios_family_arm_ducati_GateDualCore_Module_State
#define GateDualCore_A_nestedEnter ti_sysbios_family_arm_ducati_GateDualCore_A_nestedEnter
#define GateDualCore_E_invalidIndex ti_sysbios_family_arm_ducati_GateDualCore_E_invalidIndex
#define GateDualCore_E_gateInUse ti_sysbios_family_arm_ducati_GateDualCore_E_gateInUse
#define GateDualCore_gateArrayAddress ti_sysbios_family_arm_ducati_GateDualCore_gateArrayAddress
#define GateDualCore_numGates ti_sysbios_family_arm_ducati_GateDualCore_numGates
#define GateDualCore_initGates ti_sysbios_family_arm_ducati_GateDualCore_initGates
#define GateDualCore_enableStats ti_sysbios_family_arm_ducati_GateDualCore_enableStats
#define GateDualCore_Params ti_sysbios_family_arm_ducati_GateDualCore_Params
#define GateDualCore_query ti_sysbios_family_arm_ducati_GateDualCore_query
#define GateDualCore_enter ti_sysbios_family_arm_ducati_GateDualCore_enter
#define GateDualCore_leave ti_sysbios_family_arm_ducati_GateDualCore_leave
#define GateDualCore_sync ti_sysbios_family_arm_ducati_GateDualCore_sync
#define GateDualCore_Module_name ti_sysbios_family_arm_ducati_GateDualCore_Module_name
#define GateDualCore_Module_id ti_sysbios_family_arm_ducati_GateDualCore_Module_id
#define GateDualCore_Module_startup ti_sysbios_family_arm_ducati_GateDualCore_Module_startup
#define GateDualCore_Module_startupDone ti_sysbios_family_arm_ducati_GateDualCore_Module_startupDone
#define GateDualCore_Module_hasMask ti_sysbios_family_arm_ducati_GateDualCore_Module_hasMask
#define GateDualCore_Module_getMask ti_sysbios_family_arm_ducati_GateDualCore_Module_getMask
#define GateDualCore_Module_setMask ti_sysbios_family_arm_ducati_GateDualCore_Module_setMask
#define GateDualCore_Object_heap ti_sysbios_family_arm_ducati_GateDualCore_Object_heap
#define GateDualCore_Module_heap ti_sysbios_family_arm_ducati_GateDualCore_Module_heap
#define GateDualCore_construct ti_sysbios_family_arm_ducati_GateDualCore_construct
#define GateDualCore_create ti_sysbios_family_arm_ducati_GateDualCore_create
#define GateDualCore_handle ti_sysbios_family_arm_ducati_GateDualCore_handle
#define GateDualCore_struct ti_sysbios_family_arm_ducati_GateDualCore_struct
#define GateDualCore_Handle_label ti_sysbios_family_arm_ducati_GateDualCore_Handle_label
#define GateDualCore_Handle_name ti_sysbios_family_arm_ducati_GateDualCore_Handle_name
#define GateDualCore_Instance_init ti_sysbios_family_arm_ducati_GateDualCore_Instance_init
#define GateDualCore_Object_count ti_sysbios_family_arm_ducati_GateDualCore_Object_count
#define GateDualCore_Object_get ti_sysbios_family_arm_ducati_GateDualCore_Object_get
#define GateDualCore_Object_first ti_sysbios_family_arm_ducati_GateDualCore_Object_first
#define GateDualCore_Object_next ti_sysbios_family_arm_ducati_GateDualCore_Object_next
#define GateDualCore_Object_sizeof ti_sysbios_family_arm_ducati_GateDualCore_Object_sizeof
#define GateDualCore_Params_copy ti_sysbios_family_arm_ducati_GateDualCore_Params_copy
#define GateDualCore_Params_init ti_sysbios_family_arm_ducati_GateDualCore_Params_init
#define GateDualCore_delete ti_sysbios_family_arm_ducati_GateDualCore_delete
#define GateDualCore_destruct ti_sysbios_family_arm_ducati_GateDualCore_destruct
#define GateDualCore_Module_upCast ti_sysbios_family_arm_ducati_GateDualCore_Module_upCast
#define GateDualCore_Module_to_xdc_runtime_IGateProvider ti_sysbios_family_arm_ducati_GateDualCore_Module_to_xdc_runtime_IGateProvider
#define GateDualCore_Handle_upCast ti_sysbios_family_arm_ducati_GateDualCore_Handle_upCast
#define GateDualCore_Handle_to_xdc_runtime_IGateProvider ti_sysbios_family_arm_ducati_GateDualCore_Handle_to_xdc_runtime_IGateProvider
#define GateDualCore_Handle_downCast ti_sysbios_family_arm_ducati_GateDualCore_Handle_downCast
#define GateDualCore_Handle_from_xdc_runtime_IGateProvider ti_sysbios_family_arm_ducati_GateDualCore_Handle_from_xdc_runtime_IGateProvider

#endif /* ti_sysbios_family_arm_ducati_GateDualCore__localnames__done */
#endif
