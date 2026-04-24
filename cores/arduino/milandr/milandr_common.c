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

#include <stdint.h>
#include <stddef.h>
#include "periph_definition.h"
#include "MDR32FxQI_config.h"

static volatile uint32_t mdr_ticks;

//------------------------------------------------------------------------------
// Функции предварительной инициализации
//------------------------------------------------------------------------------
void milandr_gpio_preinit() __attribute__((weak));
void milandr_uart_preinit() __attribute__((weak));

void milandr_hal_init(void)
{
	milandr_gpio_preinit();
	milandr_uart_preinit();
}

//------------------------------------------------------------------------------
// Поиск пина в наборе, удовлетворяющего заданым условиям:
//------------------------------------------------------------------------------
uint8_t milandr_find_pin(tPeriphVariant periph,
                         tPeriphLineVariant line,
                         uint8_t periphN,
                         const tMilandrPin ** set,
                         const tMilandrPin ** sorted,
                         int8_t variantNum)
{
	if(!set || !sorted) return 0;

	uint8_t n = 0;
	uint32_t mask = PIN_PERIPH_MASK |
					PIN_PERIPH_L_MASK |
					PIN_PERIPH_N_MASK;

	tMilandrPin target = {.raw = 0, .periph = periph, .periphLine = line, .periphN = periphN};

	for(int8_t i = 0; i < variantNum; i++)
	{
		sorted[i] = set[i];
	}

	for(int8_t i = 0; i <= variantNum - 1; i++)
	{
		if(sorted[i] && (sorted[i]->raw & mask) == target.raw)
		{
			n++;
			continue;
		}
		else
		{
			sorted[i] = NULL;
		}

		for(int8_t j = i + 1; j <= variantNum - 1; j++)
		{
			if(sorted[j] && (sorted[j]->raw & mask) == target.raw)
			{
				sorted[i] = sorted[j];
				i++;
				n++;
			}
			sorted[j] = NULL;
		}
	}

	return n;
}

/*!
    \brief      configure systick
    \param[in]  none
    \param[out] none
    \retval     none
*/
void milandr_systick_config(void)
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
uint32_t milandr_current_millis(void)
{
	return mdr_ticks;
}

/*!
    \brief      get current microseconds
    \param[in]  none
    \param[out] none
    \retval     current microseconds
*/
uint32_t milandr_current_micros(void)
{
	uint32_t ms = mdr_ticks;
	uint32_t systick_value = SysTick->VAL;
	uint32_t systick_load = SysTick->LOAD + 1;
	uint32_t us = ((systick_load - systick_value) * 1000) / systick_load;
	return (ms * 1000 + us);
}

void milandr_delay_microseconds(uint32_t us)
{
	__IO uint32_t currentTicks = SysTick->VAL;
	/* Number of ticks per millisecond */
	const uint32_t tickPerMs = SysTick->LOAD + 1;
	/* Number of ticks to count */
	const uint32_t nbTicks = ((us - ((us > 0) ? 1 : 0)) * tickPerMs) / 1000;
	/* Number of elapsed ticks */
	uint32_t elapsedTicks = 0;
	__IO uint32_t oldTicks = currentTicks;
	do
	{
		currentTicks = SysTick->VAL;
		elapsedTicks += (oldTicks < currentTicks) ?
		                tickPerMs + oldTicks - currentTicks :
		                oldTicks - currentTicks;
		oldTicks = currentTicks;
	}
	while (nbTicks > elapsedTicks);
}

void milandr_delay(uint32_t ms)
{
	if(ms != 0)
	{
		uint32_t start = milandr_current_millis();
		do
		{
			__NOP();
		}
		while (milandr_current_millis() - start < ms);
	}
}


