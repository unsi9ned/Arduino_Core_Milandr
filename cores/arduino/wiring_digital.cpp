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

/**
 * @file wiring_digital.cpp
 * @brief Function definitions for setting pin mode, digital read and write.
 */
#include "api/Common.h"
#include "milandr/milandr_hal.h"

/**
 * Set a pin to a specific mode (input or output).
 *
 * Input pull mode can also be enabled using this function.
 *
 * @param pinNumber The Arduino pin to configure.
 * @param pinMode The mode to set the pin.
 */
void pinMode(pin_size_t pinNumber, PinMode pinMode)
{
	if(pinNumber >= milandr_gpio_count()) return;

	milandr_gpio_clock_enable(pinNumber, true);

	switch(pinMode)
	{
		// Floating input
		case INPUT:
			milandr_gpio_cfg_input(pinNumber);
		break;

		// Push-Pull output
		case OUTPUT:
			milandr_gpio_cfg_output_pp(pinNumber);
		break;

		case INPUT_PULLUP:
			milandr_gpio_cfg_input_pu(pinNumber);
		break;

		case INPUT_PULLDOWN:
			milandr_gpio_cfg_input_pd(pinNumber);
		break;

		case OUTPUT_OPENDRAIN:
			milandr_gpio_cfg_output_od(pinNumber);
		break;

		// Configure as floating input
		default:
			milandr_gpio_cfg_input(pinNumber);
		break;
	}
}

/**
 * Set a pin to a digital HIGH or LOW state.
 *
 * If the pin is configured as an input, this function will instead set the
 * pull up (HIGH) or down (LOW).
 *
 * @param pinNumber The Arduino pin number to write.
 * @param status The state to set the pin, either `HIGH` or `LOW`.
 */
void digitalWrite(pin_size_t pinNumber, PinStatus status)
{
	if(pinNumber >= milandr_gpio_count()) return;
	milandr_gpio_write(pinNumber, status);
}

/**
 * Read the digital state of a pin.
 *
 * @param pinNumber The Arduino pin number to read.
 * @return The digital state of the pin, either `HIGH` or `LOW`.
 */
PinStatus digitalRead(pin_size_t pinNumber)
{
	if(pinNumber >= milandr_gpio_count()) return LOW;
	return (PinStatus)milandr_gpio_read(pinNumber);
}
