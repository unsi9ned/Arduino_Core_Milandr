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

#include "api/Common.h"
#include "milandr/milandr_hal.h"

void attachInterruptParam(pin_size_t interruptNumber, voidFuncPtrParam callback, PinStatus mode, void* param)
{
	tMilandrExtiTrig trig;

	switch(mode)
	{
		case CHANGE:
			trig = EXTI_TRIG_BOTH;
		break;
		case FALLING:
		case LOW:
			trig = EXTI_TRIG_FALLING;
		break;
		case RISING:
		case HIGH:
			trig = EXTI_TRIG_RISING;
		break;
		default:
			trig = EXTI_TRIG_BOTH;
		break;
	}

	if(!milandr_gpio_is_digital_input(interruptNumber))
	{
		pinMode(interruptNumber, INPUT);
	}
	milandr_gpio_interrup_enable(interruptNumber, trig, callback, param);
}

void attachInterrupt(pin_size_t pin, voidFuncPtr callback, PinStatus mode)
{
	attachInterruptParam(pin, (voidFuncPtrParam)callback, mode, NULL);
}

void detachInterrupt(pin_size_t pin)
{
	milandr_gpio_interrup_disable(pin);
}
