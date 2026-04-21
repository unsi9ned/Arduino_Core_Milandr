/**
  ******************************************************************************
  * @file    startup_MDR32FG16S1QI.c
  * @author  Andrey Osipov
  * @version V1.0
  * @date    13/04/2026
  * @brief   CMSIS core device startup file for MDR32FG16S1QI (GCC)
  * @see     https://github.com/unsi9ned/Milandr.MDR32FxQI_DFP
  ******************************************************************************
  */

#include <MDR32FG16S1QI.h>

/*----------Stack Configuration-----------------------------------------------*/  
#define STACK_SIZE       0x00000400      /*!< Stack size (in Words)           */
__attribute__ ((section(".co_stack")))
unsigned long pulStack[STACK_SIZE];      

/*----------Macro definition--------------------------------------------------*/  
#define WEAK __attribute__ ((weak))           

/*----------Declaration of the default fault handlers-------------------------*/

__attribute__ ((used))

/* System exception vector handler */
void WEAK  Reset_Handler(void);   
void WEAK  NMI_Handler(void);       
void WEAK  HardFault_Handler(void); 
void WEAK  MemManage_Handler(void); 
void WEAK  BusFault_Handler(void);  
void WEAK  UsageFault_Handler(void);
void WEAK  SVC_Handler(void);       
void WEAK  PendSV_Handler(void);    
void WEAK  SysTick_Handler(void);   
   
void WEAK  SSP3_IRQHandler(void);                    /* 0: SSP3 Handler */
void WEAK  SSP4_IRQHandler(void);                    /* 1: SSP4 Handler */
void WEAK  USB_IRQHandler(void);                     /* 2: USB Host Handler */
void WEAK  MCBSP1_IRQHandler(void);                  /* 3: McBSP1 Handler */
void WEAK  MCBSP2_IRQHandler(void);                  /* 4: McBSP2 Handler */
void WEAK  DMA_IRQHandler(void);                     /* 5: DMA Handler */
void WEAK  UART1_IRQHandler(void);                   /* 6: UART1 Handler */
void WEAK  UART2_IRQHandler(void);                   /* 7: UART2 Handler */
void WEAK  SSP1_IRQHandler(void);                    /* 8: SSP1 Handler */
void WEAK  MCBSP3_IRQHandler(void);                  /* 9: MxBSP3 Handler */
void WEAK  I2C_IRQHandler(void);                     /* 10: I2C Handler */
void WEAK  POWER_IRQHandler(void);                   /* 11: POWER Detector Handler */
void WEAK  WWDG_IRQHandler(void);                    /* 12: WWDG Handler */
void WEAK  DMA_DSP_IRQHandler(void);                 /* 13: DMA DSP Handler */
void WEAK  TIMER1_IRQHandler(void);                  /* 14: Timer1 Handler */
void WEAK  TIMER2_IRQHandler(void);                  /* 15: Timer2 Handler */
void WEAK  TIMER3_IRQHandler(void);                  /* 16: Timer3 Handler */
void WEAK  ADC_IRQHandler(void);                     /* 17: ADC Handler */
void WEAK  SDIO_IRQHandler(void);                    /* 18: SDIO Handler */
void WEAK  COMPARATOR_IRQHandler(void);              /* 19: COMPARATOR Handler */
void WEAK  SSP2_IRQHandler(void);                    /* 20: SSP2 Handler */
void WEAK  AUDIO_IP_IRQHandler(void);                /* 21: Audio IP Handler */
void WEAK  DSP_CRPT_IRQHandler(void);                /* 22: DSP CRPT Handler */
void WEAK  DSP_TIMER_IRQHandler(void);               /* 23: DSP TIMER Handler */
void WEAK  DSP_CORE_IRQHandler(void);                /* 24: DSP CORE Handler */
void WEAK  DSP_STATE_IRQHandler(void);               /* 25: DSP STATE Handler */
void WEAK  UART3_IRQHandler(void);                   /* 26: UART3 Handler */
void WEAK  BACKUP_IRQHandler(void);                  /* 27: BACKUP Handler */
void WEAK  EXT_INT1_IRQHandler(void);                /* 28: EXT_INT1 Handler */
void WEAK  EXT_INT2_IRQHandler(void);                /* 29: EXT_INT2 Handler */
void WEAK  EXT_INT3_IRQHandler(void);                /* 30: EXT_INT3 Handler */
void WEAK  EXT_INT4_IRQHandler(void);                /* 31: EXT_INT4 Handler */



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
  *@brief The minimal vector table for a Cortex M3.  Note that the proper constructs
  *       must be placed on this to ensure that it ends up at physical address
  *       0x00000000.  
  */
__attribute__ ((used, section(".isr_vector")))
void (* const __Vectors[])(void) =
{	
  /*----------Core Exceptions------------------------------------------------ */
  (void *)&pulStack[STACK_SIZE-1],     /*!< The initial stack pointer         */
  Reset_Handler,             /*!< Reset Handler                               */
  NMI_Handler,               /*!< NMI Handler                                 */
  HardFault_Handler,         /*!< Hard Fault Handler                          */
  MemManage_Handler,         /*!< MPU Fault Handler                           */
  BusFault_Handler,          /*!< Bus Fault Handler                           */
  UsageFault_Handler,        /*!< Usage Fault Handler                         */
  0,0,0,0,                   /*!< Reserved                                    */
  SVC_Handler,               /*!< SVCall Handler                              */
  0,                         /*!< Debug Monitor Handler                       */
  0,                         /*!< Reserved                                    */
  PendSV_Handler,            /*!< PendSV Handler                              */
  SysTick_Handler,           /*!< SysTick Handler                             */
  
  /*----------External Exceptions---------------------------------------------*/
  SSP3_IRQHandler,                    /* 0: SSP3 Handler */
  SSP4_IRQHandler,                    /* 1: SSP4 Handler */
  USB_IRQHandler,                     /* 2: USB Host Handler */
  MCBSP1_IRQHandler,                  /* 3: McBSP1 Handler */
  MCBSP2_IRQHandler,                  /* 4: McBSP2 Handler */
  DMA_IRQHandler,                     /* 5: DMA Handler */
  UART1_IRQHandler,                   /* 6: UART1 Handler */
  UART2_IRQHandler,                   /* 7: UART2 Handler */
  SSP1_IRQHandler,                    /* 8: SSP1 Handler */
  MCBSP3_IRQHandler,                  /* 9: MxBSP3 Handler */
  I2C_IRQHandler,                     /* 10: I2C Handler */
  POWER_IRQHandler,                   /* 11: POWER Detector Handler */
  WWDG_IRQHandler,                    /* 12: WWDG Handler */
  DMA_DSP_IRQHandler,                 /* 13: DMA DSP Handler */
  TIMER1_IRQHandler,                  /* 14: Timer1 Handler */
  TIMER2_IRQHandler,                  /* 15: Timer2 Handler */
  TIMER3_IRQHandler,                  /* 16: Timer3 Handler */
  ADC_IRQHandler,                     /* 17: ADC Handler */
  SDIO_IRQHandler,                    /* 18: SDIO Handler */
  COMPARATOR_IRQHandler,              /* 19: COMPARATOR Handler */
  SSP2_IRQHandler,                    /* 20: SSP2 Handler */
  AUDIO_IP_IRQHandler,                /* 21: Audio IP Handler */
  DSP_CRPT_IRQHandler,                /* 22: DSP CRPT Handler */
  DSP_TIMER_IRQHandler,               /* 23: DSP TIMER Handler */
  DSP_CORE_IRQHandler,                /* 24: DSP CORE Handler */
  DSP_STATE_IRQHandler,               /* 25: DSP STATE Handler */
  UART3_IRQHandler,                   /* 26: UART3 Handler */
  BACKUP_IRQHandler,                  /* 27: BACKUP Handler */
  EXT_INT1_IRQHandler,                /* 28: EXT_INT1 Handler */
  EXT_INT2_IRQHandler,                /* 29: EXT_INT2 Handler */
  EXT_INT3_IRQHandler,                /* 30: EXT_INT3 Handler */
  EXT_INT4_IRQHandler,                /* 31: EXT_INT4 Handler */
};

/**
  * @brief  This is the code that gets called when the processor first
  *         starts execution following a reset event. Only the absolutely
  *         necessary set is performed, after which the application
  *         supplied main() routine is called. 
  * @param  None
  * @retval None
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

    /* If the code is running from RAM, update VTOR */
    if ((unsigned long)__Vectors >= 0x20000000)
    {
        SCB->VTOR = (uint32_t)__Vectors;
    }

    /* Setup the microcontroller system */
    SystemInit();

    /* Call the application's entry point */
    main();
}


/**
  *@brief Provide weak aliases for each Exception handler to the Default_Handler. 
  *       As they are weak aliases, any function with the same name will override 
  *       this definition.
  */
#pragma weak Reset_Handler = Default_Reset_Handler
#pragma weak NMI_Handler = Default_Handler     
#pragma weak HardFault_Handler = Default_Handler     
#pragma weak MemManage_Handler = Default_Handler     
#pragma weak BusFault_Handler = Default_Handler      
#pragma weak UsageFault_Handler = Default_Handler    
#pragma weak SVC_Handler = Default_Handler           
#pragma weak DebugMon_Handler = Default_Handler      
#pragma weak PendSV_Handler = Default_Handler        
#pragma weak SysTick_Handler = Default_Handler       
      
#pragma weak CAN1_IRQHandler = Default_Handler
#pragma weak CAN2_IRQHandler = Default_Handler
#pragma weak USB_IRQHandler = Default_Handler
#pragma weak DMA_IRQHandler = Default_Handler
#pragma weak UART1_IRQHandler = Default_Handler
#pragma weak UART2_IRQHandler = Default_Handler
#pragma weak SSP1_IRQHandler = Default_Handler
#pragma weak I2C_IRQHandler = Default_Handler
#pragma weak POWER_IRQHandler = Default_Handler
#pragma weak WWDG_IRQHandler = Default_Handler
#pragma weak Timer1_IRQHandler = Default_Handler
#pragma weak Timer2_IRQHandler = Default_Handler
#pragma weak Timer3_IRQHandler = Default_Handler
#pragma weak ADC_IRQHandler = Default_Handler
#pragma weak COMPARATOR_IRQHandler = Default_Handler
#pragma weak SSP2_IRQHandler = Default_Handler
#pragma weak BACKUP_IRQHandler = Default_Handler
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

