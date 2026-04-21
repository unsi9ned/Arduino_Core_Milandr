/**
  ******************************************************************************
  * @file    MDR32FxQI_asm.c
  * @author  Andrey Osipov
  * @version V1.0
  * @date    13/04/2026
  * @brief   Assembler functions for GCC (CoIDE)
  * @note    Adapted from Milandr original for GCC/CoIDE compatibility
  * @see     https://github.com/unsi9ned/Milandr.MDR32FxQI_DFP
  ******************************************************************************
  */

#include <stdint.h>
#include "MDR32FxQI_config.h"

/**
  * @brief   Updates data cache.
  * @warning This function can be used only for MCU MDR32F1QI, K1986VE1xI (errata 0007).
  * @note    Registers r0-r1 are not preserved.
  * @param   None
  * @retval  None
  */
__RAMFUNC
__attribute__ ((naked))
void EEPROM_UpdateDCacheAsm(void)
{
    __asm volatile (
        "movs   r0, #0x50        \n"  /* r0 = 0x00000050 */
        "nop                      \n"  /* Align next LDR instruction */
        "EEPROM_ReadFlash:        \n"
        "ldr    r1, [r0, #0x00]   \n"  /* Read EEPROM */
        "subs   r0, r0, #0x10     \n"  /* r0 -= 0x10 */
        "cmp    r0, #0x00         \n"
        "bne    EEPROM_ReadFlash  \n"
        "bx     lr                \n"
    );
}

/**
  * @brief   Performs a blocking program delay in loops.
  * @param   loops: specifies the delay time in loops.
  * @retval  None
  */
__RAMFUNC
__attribute__ ((naked))
void DELAY_PROGRAM_WaitLoopsAsm(uint32_t loops)
{
    __asm volatile (
        "cmp    r0, #0x00         \n"
        "beq    DELAY_PROGRAM_Exit\n"
        "DELAY_PROGRAM_Loop:      \n"
        "subs   r0, r0, #1        \n"
        "bne    DELAY_PROGRAM_Loop\n"
        "DELAY_PROGRAM_Exit:      \n"
        "bx     lr                \n"
    );
}
