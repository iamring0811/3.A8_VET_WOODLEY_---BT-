;/****************************************Copyright (c)**************************************************
;********************************************************************************************************/

;define the stack size
SVC_STACK_LEGTH         EQU         16
FIQ_STACK_LEGTH         EQU         16
IRQ_STACK_LEGTH         EQU         16
ABT_STACK_LEGTH         EQU         256
UND_STACK_LEGTH         EQU         16

NoInt       EQU 0x80
NoFIQ		EQU	0x40

USR32Mode   EQU 0x10
SVC32Mode   EQU 0x13
SYS32Mode   EQU 0x1f
IRQ32Mode   EQU 0x12
FIQ32Mode   EQU 0x11


;    IMPORT __use_no_semihosting_swi

;The imported labels
;    IMPORT  FIQ_Exception                   ;Fast interrupt exceptions handler 快速中断异常处理程序
    IMPORT  Main                          ;The entry point to the main function C语言主程序入口 
;    IMPORT  TargetResetInit                 ;initialize the target board 目标板基本初始化
	
;The emported labels
    EXPORT  bottom_of_heap
    EXPORT  StackUsr
    
    EXPORT  Reset
    EXPORT __user_initial_stackheap

    EXPORT  Restart

    CODE32

    AREA    Init,CODE,READONLY
    ENTRY

;interrupt vectors
Reset
        LDR     PC, ResetAddr
        LDR     PC, UndefinedAddr
        LDR     PC, SWI_Addr
        LDR     PC, PrefetchAddr
        LDR     PC, DataAbortAddr
        DCD     0xb9206e50
        LDR     PC, [PC, #-0x120]
        LDR     PC, FIQ_Addr

ResetAddr           DCD     ResetInit
UndefinedAddr       DCD     Undefined
SWI_Addr            DCD     SoftwareInterrupt
PrefetchAddr        DCD     PrefetchAbort
DataAbortAddr       DCD     DataAbort
Nouse               DCD     0
IRQ_Addr            DCD     0
FIQ_Addr            DCD     FIQ_Handler


Undefined
        B       Undefined

SoftwareInterrupt			
;        B       SoftwareInterrupt
        CMP     R0, #4
        LDRLO   PC, [PC, R0, LSL #2]
        MOVS    PC, LR

SwiFunction
        DCD     IRQDisable       ;0
        DCD     IRQEnable        ;1
        DCD		FIQDisable		 ;2
        DCD		FIQEnable		 ;3

IRQDisable
        MRS     R0, SPSR
        ORR     R0, R0, #NoInt
        MSR     SPSR_c, R0
        MOVS    PC, LR

IRQEnable
        MRS   	R0, SPSR
        BIC   	R0, R0, #NoInt
        MSR   	SPSR_c, R0
        MOVS    PC, LR
        
FIQDisable
        MSR		CPSR_c, #(SVC32Mode|NoFIQ)
        MRS     R0, SPSR
        ORR     R0, R0, #NoFIQ
        MSR     SPSR_c, R0
        MOVS    PC, LR

FIQEnable
        MSR		CPSR_c, #(SVC32Mode|NoFIQ)
        MRS  	R0, SPSR
        BIC   	R0, R0, #NoFIQ
        MSR   	SPSR_c, R0
        MOVS  	PC, LR
;// Changed 2004-12-09
      
PrefetchAbort
        B       PrefetchAbort

DataAbort
        B       DataAbort

FIQ_Handler
        STMFD   SP!, {R0-R3, LR}
;        BL      FIQ_Exception
        LDMFD   SP!, {R0-R3, LR}
        SUBS    PC,  LR,  #4

            
InitStack    
        MOV     R0, LR
;Build the SVC stack
        MSR     CPSR_c, #0xd3		
        LDR     SP, StackSvc	
;Build the IRQ stack	
        MSR     CPSR_c, #0xd2
        LDR     SP, StackIrq
;Build the FIQ stack
        MSR     CPSR_c, #0xd1
        LDR     SP, StackFiq
;Build the DATAABORT stack
        MSR     CPSR_c, #0xd7
        LDR     SP, StackAbt
;Build the UDF stack
        MSR     CPSR_c, #0xdb
        LDR     SP, StackUnd
;Build the SYS stack
        MSR     CPSR_c, #0xdf
        LDR     SP, =StackUsr

        MOV     PC, R0

ResetInit
                
        BL      InitStack               ;Initialize the stack
;;      BL      TargetResetInit         ;Initialize the target board
                                        ;Jump to the entry point of C program
        B       Main


Restart
        LDR     PC, RestartAddr

RestartAddr           DCD     0xA0000000



__user_initial_stackheap    
    LDR   R0, =bottom_of_heap		
   ; LDR   R1, =StackUsr			
    LDR   R2, =top_of_heap		
    LDR   R3, =bottom_of_Stacks		
    MOV   PC, LR    
    
StackSvc           DCD     SvcStackSpace + (SVC_STACK_LEGTH - 1)* 4
StackIrq           DCD     IrqStackSpace + (IRQ_STACK_LEGTH - 1)* 4
StackFiq           DCD     FiqStackSpace + (FIQ_STACK_LEGTH - 1)* 4
StackAbt           DCD     AbtStackSpace + (ABT_STACK_LEGTH - 1)* 4
StackUnd           DCD     UndtStackSpace + (UND_STACK_LEGTH - 1)* 4

    IF :DEF: EN_CRP
        IF  . >= 0x1fc
        INFO    1,"\nThe data at 0x000001fc must be 0x87654321.\nPlease delete some source before this line."
        ENDIF
CrpData
    WHILE . < 0x1fc
    NOP
    WEND
CrpData1
    DCD     0x87654321          ;/*When the Data is 为0x87654321,user code be protected. 当此数为0x87654321时，用户程序被保护 */
    ENDIF
    
        AREA    MyStacks, DATA, NOINIT, ALIGN=2
SvcStackSpace      SPACE   SVC_STACK_LEGTH * 4  ;Stack spaces for Administration Mode 管理模式堆栈空间
IrqStackSpace      SPACE   IRQ_STACK_LEGTH * 4  ;Stack spaces for Interrupt ReQuest Mode 中断模式堆栈空间
FiqStackSpace      SPACE   FIQ_STACK_LEGTH * 4  ;Stack spaces for Fast Interrupt reQuest Mode 快速中断模式堆栈空间
AbtStackSpace      SPACE   ABT_STACK_LEGTH * 4  ;Stack spaces for Suspend Mode 中止义模式堆栈空间
UndtStackSpace     SPACE   UND_STACK_LEGTH * 4  ;Stack spaces for Undefined Mode 未定义模式堆栈


        AREA    Heap, DATA, NOINIT        
bottom_of_heap    SPACE   0x10000

        AREA    StackBottom, DATA, NOINIT
bottom_of_Stacks   SPACE  0x10000

        AREA    HeapTop, DATA, NOINIT
top_of_heap     SPACE  0x10000

        AREA    Stacks, DATA, NOINIT
StackUsr      	SPACE  0x10000
    END

;/*********************************************************************************************************
;**                            End Of File
;********************************************************************************************************/
