
	.cdecls "FreeRTOSConfig.h"

	.thumb

	.def ulPortSetInterruptMask
	.def vPortClearInterruptMask
	.def vPortSVCHandler
	.def prvPortStartFirstTask
	.def xPortPendSVHandler
	;.def portDSB
	;.global portISB
	.global portCLZ

	.ref pxCurrentTCB          
    .ref vTaskSwitchContext

	.text

NVIC_VTABLE_R: .word  	0xE000ED08

;---------------------------------------------------------

ulPortSetInterruptMask:
	
	mrs r0,basepri
	mov r1, #configMAX_SYSCALL_INTERRUPT_PRIORITY                     
	msr basepri, r1     
	dsb
	isb
	bx r14
	
;---------------------------------------------------------

vPortClearInterruptMask:

	msr basepri, r0
	dsb
	isb
	bx r14
	

;---------------------------------------------------------

xPortPendSVHandler:

   mrs r0, psp                      

   ldr r3, pxCurrentTCBConst        ; Get the location of the current TCB.
   ldr r2, [r3]                     

   stmdb r0!, {r4-r11}              ; Save the remaining registers. 
   str r0, [r2]                     ; Save the new top of stack into the first member of the TCB. 

   stmdb sp!, {r3, r14}             
   mov r0, #configMAX_SYSCALL_INTERRUPT_PRIORITY                       
   msr basepri, r0                  
   dsb
   isb
   bl vTaskSwitchContext           
   mov r0, #0                       
   msr basepri, r0                  
   dsb
   isb
   ldmia sp!, {r3, r14}             														    
                                    ; Restore the context, including the critical nesting count.
   ldr r1, [r3]                     
   ldr r0, [r1]                     ; The first item in pxCurrentTCB is the task top of stack. 
   ldmia r0!, {r4-r11}              ; Pop the registers. 
   msr psp, r0                         
   bx r14                              

   .align 2                            
pxCurrentTCBConst: .word pxCurrentTCB 

;---------------------------------------------------------

vPortSVCHandler:
   	                                
   	ldr r3, pxCurrentTCBConst2      ; Restore the context.  
   	ldr r1, [r3]                    ; Use pxCurrentTCBConst to get the pxCurrentTCB address.  
   	ldr r0, [r1]                    ; The first item in pxCurrentTCB is the task top of stack. 
   	ldmia r0!, {r4-r11}             ; Pop the registers that are not automatically saved on exception entry and the critical nesting count.  
   	msr psp, r0                     ; Restore the task stack pointer. 
   	mov r0, #0                      
   	msr basepri, r0                 
   	dsb
   	isb
   	orr r14, #0xd                   
   	bx r14                          
   	                                
   	.align 2                        
pxCurrentTCBConst2: .word pxCurrentTCB


;---------------------------------------------------------

prvPortStartFirstTask:
                                       
 	ldr r0, NVIC_VTABLE_R			; Use the NVIC offset register to locate the stack. 
 	ldr r0, [r0]         
 	ldr r0, [r0]         
 	msr msp, r0          			; Set the msp back to the start of the stack.
 	cpsie i 						; System call to start first task.
 	dsb
 	isb
 	svc #0               			; System call to start first task. 

;---------------------------------------------------------

;portDSB:
;
;	dsb
;	bx r14
;
;---------------------------------------------------------
;
;portISB:
;
;	isb
;	bx r14
;
;---------------------------------------------------------

portCLZ:

	clz r0, r0
    bx r14

	.end
