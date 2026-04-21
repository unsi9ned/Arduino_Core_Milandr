/**
  ******************************************************************************
  * @file    startup_MDR32F1QI.c
  * @author  Andrey Osipov
  * @version V1.0
  * @date    13/04/2026
  * @brief   CMSIS core device startup file for MDR32F1QI (Cortex-M1, GCC)
  * @see     https://github.com/unsi9ned/Milandr.MDR32FxQI_DFP
  ******************************************************************************
  */

#include <MDR32F1QI.h>

/*----------Stack Configuration-----------------------------------------------*/  
#define STACK_SIZE       0x00001000      /*!< Stack size (in Words)           */
__attribute__ ((section(".co_stack")))
unsigned long pulStack[STACK_SIZE];      

/*----------Macro definition--------------------------------------------------*/  
#define WEAK __attribute__ ((weak))           

/*----------Declaration of the default fault handlers-------------------------*/

__attribute__ ((used))

/* System exception vector handler (Cortex-M1 has fewer exceptions) */
void WEAK  Reset_Handler(void);   
void WEAK  NMI_Handler(void);       
void WEAK  HardFault_Handler(void); 
void WEAK  SVC_Handler(void);       
void WEAK  PendSV_Handler(void);    
void WEAK  SysTick_Handler(void);   
   
/* Peripheral interrupt handlers */
void WEAK  MIL_STD_1553B2_IRQHandler(void);
void WEAK  MIL_STD_1553B1_IRQHandler(void);
void WEAK  USB_IRQHandler(void);
void WEAK  CAN1_IRQHandler(void);
void WEAK  CAN2_IRQHandler(void);
void WEAK  DMA_IRQHandler(void);
void WEAK  UART1_IRQHandler(void);
void WEAK  UART2_IRQHandler(void);
void WEAK  SSP1_IRQHandler(void);
void WEAK  BUSY_IRQHandler(void);
void WEAK  ARINC429R_IRQHandler(void);
void WEAK  POWER_IRQHandler(void);
void WEAK  WWDG_IRQHandler(void);
void WEAK  TIMER4_IRQHandler(void);
void WEAK  TIMER1_IRQHandler(void);
void WEAK  TIMER2_IRQHandler(void);
void WEAK  TIMER3_IRQHandler(void);
void WEAK  ADC_IRQHandler(void);
void WEAK  ETHERNET_IRQHandler(void);
void WEAK  SSP3_IRQHandler(void);
void WEAK  SSP2_IRQHandler(void);
void WEAK  ARINC429T1_IRQHandler(void);
void WEAK  ARINC429T2_IRQHandler(void);
void WEAK  ARINC429T3_IRQHandler(void);
void WEAK  ARINC429T4_IRQHandler(void);
void WEAK  BKP_IRQHandler(void);
void WEAK  EXT_INT1_IRQHandler(void);
void WEAK  EXT_INT2_IRQHandler(void);
void WEAK  EXT_INT3_IRQHandler(void);
void WEAK  EXT_INT4_IRQHandler(void);


/*----------Symbols defined in linker script----------------------------------*/  
extern unsigned long _sidata;    /*!< Start address for the initialization 
                                      values of the .data section.            */
extern unsigned long _sdata;     /*!< Start address for the .data section     */    
extern unsigned long _edata;     /*!< End address for the .data section       */    
extern unsigned long _sbss;      /*!< Start address for the .bss section      */
extern unsigned long _ebss;      /*!< End address for the .bss section        */      
extern void _eram;               /*!< End address for ram                     */


/*----------Function prototypes-----------------------------------------------*/  
extern void SystemInit(void);
extern int main(void);           /*!< The entry point for the application.    */
void Default_Reset_Handler(void);   /*!< Default reset handler                */
static void Default_Handler(void);  /*!< Default exception handler            */


/**
  * @brief The vector table for Cortex-M1 (no MemManage, BusFault, UsageFault, DebugMon)
  *        Note: VTOR is not supported on Cortex-M1, vector table is fixed at 0x00000000
  */
__attribute__ ((used, section(".isr_vector")))
void (* const __Vectors[])(void) =
{	
  /*----------Core Exceptions (Cortex-M1 specific)----------------------------*/
   (void *)&pulStack[STACK_SIZE-1],     /*!< The initial stack pointer         */
  Reset_Handler,                       /*!< Reset Handler                     */
  NMI_Handler,                         /*!< NMI Handler                       */
  HardFault_Handler,                   /*!< Hard Fault Handler                */
  0,                                   /*!< Reserved                          */
  0,                                   /*!< Reserved                          */
  0,                                   /*!< Reserved                          */
  0,                                   /*!< Reserved                          */
  0,                                   /*!< Reserved                          */
  0,                                   /*!< Reserved                          */
  0,                                   /*!< Reserved                          */
  SVC_Handler,                         /*!< SVCall Handler                    */
  0,                                   /*!< Reserved                          */
  0,                                   /*!< Reserved                          */
  PendSV_Handler,                      /*!< PendSV Handler                    */
  SysTick_Handler,                     /*!< SysTick Handler                   */
  
  /*----------External Exceptions---------------------------------------------*/
  MIL_STD_1553B2_IRQHandler,		// 0
  MIL_STD_1553B1_IRQHandler,		// 1
  USB_IRQHandler,			// 2
  CAN1_IRQHandler,			// 3
  CAN2_IRQHandler,			// 4
  DMA_IRQHandler,			// 5
  UART1_IRQHandler,			// 6
  UART2_IRQHandler,			// 7
  SSP1_IRQHandler,			// 8
  BUSY_IRQHandler,			// 9
  ARINC429R_IRQHandler,		// 10
  POWER_IRQHandler,			// 11
  WWDG_IRQHandler,			// 12
  TIMER4_IRQHandler,		// 13
  TIMER1_IRQHandler,		// 14
  TIMER2_IRQHandler,		// 15
  TIMER3_IRQHandler,		// 16
  ADC_IRQHandler,			// 17
  ETHERNET_IRQHandler,		// 18
  SSP3_IRQHandler,			// 19
  SSP2_IRQHandler,			// 20
  ARINC429T1_IRQHandler,		// 21
  ARINC429T2_IRQHandler,		// 22
  ARINC429T3_IRQHandler,		// 23
  ARINC429T4_IRQHandler,		// 24
  0,					// 25
  0,					// 26
  BKP_IRQHandler,			// 27
  EXT_INT1_IRQHandler,		// 28
  EXT_INT2_IRQHandler,		// 29
  EXT_INT3_IRQHandler,		// 30
  EXT_INT4_IRQHandler		// 31
};

/**
  * @brief  This is the code that gets called when the processor first
  *         starts execution following a reset event. Only the absolutely
  *         necessary set is performed, after which the application
  *         supplied main() routine is called. 
  * @param  None
  * @retval None
  * @note   Cortex-M1 does NOT support VTOR, vector table is always at 0x00000000
  */
void Default_Reset_Handler(void)
{
    unsigned long *pulSrc, *pulDest;

    /* Copy the data segment initializers from flash to SRAM */
    pulSrc = &_sidata;
    for(pulDest = &_sdata; pulDest < &_edata; )
    {
        *(pulDest++) = *(pulSrc++);
    }

    /* Zero fill the bss segment */
    for(pulDest = &_sbss; pulDest < &_ebss; )
    {
        *(pulDest++) = 0;
    }

    /* Cortex-M1 does NOT have VTOR (Vector Table Offset Register)
       Vector table is fixed at address 0x00000000, so no relocation needed */

    /* Setup the microcontroller system */
    SystemInit();

    /* Call the application's entry point */
    main();
}


/**
  * @brief Provide weak aliases for each Exception handler to the Default_Handler. 
  *        As they are weak aliases, any function with the same name will override 
  *        this definition.
  */
#pragma weak Reset_Handler = Default_Reset_Handler
#pragma weak NMI_Handler = Default_Handler     
#pragma weak HardFault_Handler = Default_Handler     
#pragma weak SVC_Handler = Default_Handler           
#pragma weak PendSV_Handler = Default_Handler        
#pragma weak SysTick_Handler = Default_Handler       

#pragma weak MIL_STD_1553B2_IRQHandler = Default_Handler
#pragma weak MIL_STD_1553B1_IRQHandler = Default_Handler
#pragma weak USB_IRQHandler = Default_Handler
#pragma weak CAN1_IRQHandler = Default_Handler
#pragma weak CAN2_IRQHandler = Default_Handler
#pragma weak DMA_IRQHandler = Default_Handler
#pragma weak UART1_IRQHandler = Default_Handler
#pragma weak UART2_IRQHandler = Default_Handler
#pragma weak SSP1_IRQHandler = Default_Handler
#pragma weak BUSY_IRQHandler = Default_Handler
#pragma weak ARINC429R_IRQHandler = Default_Handler
#pragma weak POWER_IRQHandler = Default_Handler
#pragma weak WWDG_IRQHandler = Default_Handler
#pragma weak TIMER4_IRQHandler = Default_Handler
#pragma weak TIMER1_IRQHandler = Default_Handler
#pragma weak TIMER2_IRQHandler = Default_Handler
#pragma weak TIMER3_IRQHandler = Default_Handler
#pragma weak ADC_IRQHandler = Default_Handler
#pragma weak ETHERNET_IRQHandler = Default_Handler
#pragma weak SSP3_IRQHandler = Default_Handler
#pragma weak SSP2_IRQHandler = Default_Handler
#pragma weak ARINC429T1_IRQHandler = Default_Handler
#pragma weak ARINC429T2_IRQHandler = Default_Handler
#pragma weak ARINC429T3_IRQHandler = Default_Handler
#pragma weak ARINC429T4_IRQHandler = Default_Handler
#pragma weak BKP_IRQHandler = Default_Handler
#pragma weak EXT_INT1_IRQHandler = Default_Handler
#pragma weak EXT_INT2_IRQHandler = Default_Handler
#pragma weak EXT_INT3_IRQHandler = Default_Handler
#pragma weak EXT_INT4_IRQHandler = Default_Handler

/**
  * @brief  This is the code that gets called when the processor receives an 
  *         unexpected interrupt.  This simply enters an infinite loop, 
  *         preserving the system state for examination by a debugger.
  * @param  None
  * @retval None  
  */
static void Default_Handler(void) 
{
	/* Go into an infinite loop. */
	while (1) 
	{
	}
}