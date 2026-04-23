/*
 * Arduino Core for Milandr MCUs
 * Copyright (c) 2026 Andrey Osipov
 *
 * This file is part of Arduino_Core_Milandr.
 * Project home: https://github.com/unsi9ned/Arduino_Core_Milandr
 * Author's website: https://hamlab.net
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 */

#include "systick.h"

volatile uint32_t mdr_ticks;

/*!
    \brief      configure systick
    \param[in]  none
    \param[out] none
    \retval     none
*/
void systick_config(void)
{
	/* setup systick timer for 1000Hz interrupts */
	if(SysTick_Config(SystemCoreClock / 1000U))
	{
		/* capture error */
		while (1)
		{
		}
	}
	/* configure the systick handler priority */
	NVIC_SetPriority(SysTick_IRQn, 0x00U);
}

void noOsSystickHandler() {}
void osSystickHandler() __attribute__((weak, alias("noOsSystickHandler")));

/*!
    \brief      this function handles SysTick exception
    \param[in]  none
    \param[out] none
    \retval     none
*/
void SysTick_Handler(void)
{
	mdr_ticks++;
	osSystickHandler();
}

/*!
    \brief      get current milliseconds
    \param[in]  none
    \param[out] none
    \retval     current milliseconds
*/
uint32_t getCurrentMillis(void)
{
	return mdr_ticks;
}

/*!
    \brief      get current microseconds
    \param[in]  none
    \param[out] none
    \retval     current microseconds
*/
uint32_t getCurrentMicros(void)
{
	uint32_t ms = mdr_ticks;
	uint32_t systick_value = SysTick->VAL;
	uint32_t systick_load = SysTick->LOAD + 1;
	uint32_t us = ((systick_load - systick_value) * 1000) / systick_load;
	return (ms * 1000 + us);
}
