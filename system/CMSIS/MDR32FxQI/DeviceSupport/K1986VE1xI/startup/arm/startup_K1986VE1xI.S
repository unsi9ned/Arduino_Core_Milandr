;/**
;  ******************************************************************************
;  * @file    startup_K1986VE1xI.S
;  * @author  Milandr Application Team
;  * @version V2.0
;  * @date    22/07/2024
;  * @brief   CMSIS core device startup file for K1986VE1xI.
;  ******************************************************************************
;  * <br><br>
;  *
;  * THE PRESENT FIRMWARE IS FOR GUIDANCE ONLY. IT AIMS AT PROVIDING CUSTOMERS
;  * WITH CODING INFORMATION REGARDING MILANDR'S PRODUCTS IN ORDER TO FACILITATE
;  * THE USE AND SAVE TIME. MILANDR SHALL NOT BE HELD LIABLE FOR ANY
;  * DIRECT, INDIRECT OR CONSEQUENTIAL DAMAGES RESULTING
;  * FROM THE CONTENT OF SUCH FIRMWARE AND/OR A USE MADE BY CUSTOMERS OF THE
;  * CODING INFORMATION CONTAINED HEREIN IN THEIR PRODUCTS.
;  *
;  * <h2><center>&copy; COPYRIGHT 2025 Milandr</center></h2>
;  ******************************************************************************
;  */

;//--------- <<< Use Configuration Wizard in Context Menu >>> ------------------

;<h> Stack Configuration
;  <o> Stack Size (in Bytes) <0x0-0xFFFFFFFF:8>
;</h>

Stack_Size      EQU     0x00001000

                AREA    STACK, NOINIT, READWRITE, ALIGN=3
__stack_limit
Stack_Mem       SPACE   Stack_Size
__initial_sp


;<h> Heap Configuration
;  <o>  Heap Size (in Bytes) <0x0-0xFFFFFFFF:8>
;</h>

Heap_Size       EQU     0x00001000

                AREA    HEAP, NOINIT, READWRITE, ALIGN=3
__heap_base
Heap_Mem        SPACE   Heap_Size
__heap_limit

                PRESERVE8
                THUMB


; Vector Table Mapped to Address 0 at Reset
                AREA    RESET, DATA, READONLY
                EXPORT  __Vectors
                EXPORT  __Vectors_End
                EXPORT  __Vectors_Size

; Attention for NMI_Handler:
; Called by software only (bit [31] of the ICSR register). Used for mission critical applications
__Vectors       DCD     __initial_sp                ;     Top of Stack
                DCD     Reset_Handler               ;     Reset Handler
                DCD     NMI_Handler                 ; -14 NMI Handler
                DCD     HardFault_Handler           ; -13 Hard Fault Handler
                DCD     0                           ;     Reserved
                DCD     0                           ;     Reserved
                DCD     0                           ;     Reserved
                DCD     0                           ;     Reserved
                DCD     0                           ;     Reserved
                DCD     0                           ;     Reserved
                DCD     0                           ;     Reserved
                DCD     SVC_Handler                 ;  -5 SVCall Handler
                DCD     0                           ;     Reserved
                DCD     0                           ;     Reserved
                DCD     PendSV_Handler              ;  -2 PendSV Handler
                DCD     SysTick_Handler             ;  -1 SysTick Handler

                ; External Interrupts
                DCD     MIL_STD_1553B2_IRQHandler   ;   0 MIL_STD_1553B2 Handler
                DCD     MIL_STD_1553B1_IRQHandler   ;   1 MIL_STD_1553B1 Handler
                DCD     USB_IRQHandler              ;   2 USB Host Handler
                DCD     CAN1_IRQHandler             ;   3 CAN1 Handler
                DCD     CAN2_IRQHandler             ;   4 CAN2 Handler
                DCD     DMA_IRQHandler              ;   5 DMA Handler
                DCD     UART1_IRQHandler            ;   6 UART1 Handler
                DCD     UART2_IRQHandler            ;   7 UART2 Handler
                DCD     SSP1_IRQHandler             ;   8 SSP1 Handler
                DCD     BUSY_IRQHandler             ;   9 NAND Flash Handler
                DCD     ARINC429R_IRQHandler        ;  10 ARINC429R1-ARINC429R8 Handler
                DCD     POWER_IRQHandler            ;  11 POWER Handler
                DCD     WWDG_IRQHandler             ;  12 WWDG Handler
                DCD     TIMER4_IRQHandler           ;  13 Timer4 Handler
                DCD     TIMER1_IRQHandler           ;  14 Timer1 Handler
                DCD     TIMER2_IRQHandler           ;  15 Timer2 Handler
                DCD     TIMER3_IRQHandler           ;  16 Timer3 Handler
                DCD     ADC_IRQHandler              ;  17 ADC Handler
                DCD     ETHERNET_IRQHandler         ;  18 Ethernet Handler
                DCD     SSP3_IRQHandler             ;  19 SSP3 Handler
                DCD     SSP2_IRQHandler             ;  20 SSP2 Handler
                DCD     ARINC429T1_IRQHandler       ;  21 ARINC429T1 Handler
                DCD     ARINC429T2_IRQHandler       ;  22 ARINC429T2 Handler
                DCD     ARINC429T3_IRQHandler       ;  23 ARINC429T3 Handler
                DCD     ARINC429T4_IRQHandler       ;  24 ARINC429T4 Handler
                DCD     0                           ;     Reserved
                DCD     0                           ;     Reserved
                DCD     BKP_IRQHandler              ;  27 BKP Handler
                DCD     EXT_INT1_IRQHandler         ;  28 EXT_INT1 Handler
                DCD     EXT_INT2_IRQHandler         ;  29 EXT_INT2 Handler
                DCD     EXT_INT3_IRQHandler         ;  30 EXT_INT3 Handler
                DCD     EXT_INT4_IRQHandler         ;  31 EXT_INT4 Handler
__Vectors_End

__Vectors_Size  EQU  __Vectors_End - __Vectors

                AREA    |.text|, CODE, READONLY

; Reset Handler
Reset_Handler   PROC
                EXPORT  Reset_Handler               [WEAK]
                IMPORT  SystemInit
                IMPORT  __main
                LDR     R0, =SystemInit
                BLX     R0
                LDR     R0,=__main
                BX      R0
                ENDP


; Dummy Exception Handlers (infinite loops which can be modified)

NMI_Handler     PROC
                EXPORT  NMI_Handler                 [WEAK]
                B       .
                ENDP

HardFault_Handler       PROC
                EXPORT  HardFault_Handler           [WEAK]
                B       .
                ENDP

SVC_Handler     PROC
                EXPORT  SVC_Handler                 [WEAK]
                B       .
                ENDP

PendSV_Handler  PROC
                EXPORT  PendSV_Handler              [WEAK]
                B       .
                ENDP

SysTick_Handler PROC
                EXPORT  SysTick_Handler             [WEAK]
                B       .
                ENDP


; External Interrupts

MIL_STD_1553B2_IRQHandler   PROC
                EXPORT  MIL_STD_1553B2_IRQHandler   [WEAK]
                B       .
                ENDP

MIL_STD_1553B1_IRQHandler   PROC
                EXPORT  MIL_STD_1553B1_IRQHandler   [WEAK]
                B       .
                ENDP

USB_IRQHandler          PROC
                EXPORT  USB_IRQHandler              [WEAK]
                B       .
                ENDP
                
CAN1_IRQHandler         PROC
                EXPORT  CAN1_IRQHandler             [WEAK]
                B       .
                ENDP

CAN2_IRQHandler         PROC
                EXPORT  CAN2_IRQHandler             [WEAK]
                B       .
                ENDP

DMA_IRQHandler          PROC
                EXPORT  DMA_IRQHandler              [WEAK]
                B       .
                ENDP

UART1_IRQHandler    PROC
                EXPORT  UART1_IRQHandler            [WEAK]
                B       .
                ENDP

UART2_IRQHandler        PROC
                EXPORT  UART2_IRQHandler            [WEAK]
                B       .
                ENDP

SSP1_IRQHandler         PROC
                EXPORT  SSP1_IRQHandler             [WEAK]
                B       .
                ENDP

BUSY_IRQHandler         PROC
                EXPORT  BUSY_IRQHandler             [WEAK]
                B       .
                ENDP

ARINC429R_IRQHandler    PROC
                EXPORT  ARINC429R_IRQHandler        [WEAK]
                B       .
                ENDP

POWER_IRQHandler        PROC
                EXPORT  POWER_IRQHandler            [WEAK]
                B       .
                ENDP

WWDG_IRQHandler         PROC
                EXPORT  WWDG_IRQHandler             [WEAK]
                B       .
                ENDP

TIMER4_IRQHandler       PROC
                EXPORT  TIMER4_IRQHandler           [WEAK]
                B       .
                ENDP

TIMER1_IRQHandler       PROC
                EXPORT  TIMER1_IRQHandler           [WEAK]
                B       .
                ENDP

TIMER2_IRQHandler       PROC
                EXPORT  TIMER2_IRQHandler           [WEAK]
                B       .
                ENDP

TIMER3_IRQHandler       PROC
                EXPORT  TIMER3_IRQHandler           [WEAK]
                B       .
                ENDP

ADC_IRQHandler          PROC
                EXPORT  ADC_IRQHandler              [WEAK]
                B       .
                ENDP

ETHERNET_IRQHandler     PROC
                EXPORT  ETHERNET_IRQHandler         [WEAK]
                B       .
                ENDP

SSP3_IRQHandler         PROC
                EXPORT  SSP3_IRQHandler             [WEAK]
                B       .
                ENDP

SSP2_IRQHandler         PROC
                EXPORT  SSP2_IRQHandler             [WEAK]
                B       .
                ENDP

ARINC429T1_IRQHandler   PROC
                EXPORT  ARINC429T1_IRQHandler       [WEAK]
                B       .
                ENDP
                
ARINC429T2_IRQHandler   PROC
                EXPORT  ARINC429T2_IRQHandler       [WEAK]
                B       .
                ENDP

ARINC429T3_IRQHandler   PROC
                EXPORT  ARINC429T3_IRQHandler       [WEAK]
                B       .
                ENDP

ARINC429T4_IRQHandler   PROC
                EXPORT  ARINC429T4_IRQHandler       [WEAK]
                B       .
                ENDP

BKP_IRQHandler          PROC
                EXPORT  BKP_IRQHandler              [WEAK]
                B       .
                ENDP

EXT_INT1_IRQHandler     PROC
                EXPORT  EXT_INT1_IRQHandler         [WEAK]
                B       .
                ENDP

EXT_INT2_IRQHandler     PROC
                EXPORT  EXT_INT2_IRQHandler         [WEAK]
                B       .
                ENDP

EXT_INT3_IRQHandler     PROC
                EXPORT  EXT_INT3_IRQHandler         [WEAK]
                B       .
                ENDP

EXT_INT4_IRQHandler     PROC
                EXPORT  EXT_INT4_IRQHandler         [WEAK]
                B       .
                ENDP

                ALIGN



; User Initial Stack & Heap

                IF      :DEF:__MICROLIB
                
                EXPORT  __initial_sp
                EXPORT  __stack_limit
                EXPORT  __heap_base
                EXPORT  __heap_limit
                
                ELSE
                
                IMPORT  __use_two_region_memory
                EXPORT  __user_initial_stackheap
__user_initial_stackheap

                LDR     R0, = Heap_Mem
                LDR     R1, = (Stack_Mem + Stack_Size)
                LDR     R2, = (Heap_Mem +  Heap_Size)
                LDR     R3, = Stack_Mem
                BX      LR

                ALIGN

                ENDIF


                END


