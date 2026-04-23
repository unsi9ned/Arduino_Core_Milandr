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
#include "milandr/periph_definition.h"
#include "MDR32FxQI_rst_clk.h"

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
	if(pinNumber >= DMAX) return;

	const tMilandrPin * mdrPin = &pinTable[pinNumber][PORT_FUNC_PORT][0];
	volatile MDR_PORT_TypeDef * port = MDR_PORT(mdrPin->port);
	uint16_t portPin = (1 << mdrPin->pin);

	// Сначала включаем тактирование порта
	MDR_RST_CLK->PER_CLOCK |= RST_CLK_PCLK_PORTA << mdrPin->port;

	// MODE = 0x0 (Digital Port)
	port->FUNC &= ~(PORT_FUNC_MODE0_Msk << (mdrPin->pin * 2));
	port->ANALOG |= ((uint32_t)PORT_MODE_DIGITAL << mdrPin->pin);

	// Schmitt Trigger Disabled
	port->PD &= ~(((uint32_t)PORT_PD_SHM_ON << mdrPin->pin) << PORT_PD_SHM_Pos);

	// Fast port
	port->PWR |= (PORT_PWR0_Msk << (mdrPin->pin * 2));

	//Filter Disabled
	port->GFEN &= ~((uint32_t)PORT_GFEN_ON << mdrPin->pin);

	switch(pinMode)
	{
		// Floating input
		case INPUT:
			port->OE &= ~portPin;
			port->PULL &= ~(((uint32_t)PORT_PULL_UP_ON << mdrPin->pin) << PORT_PULL_UP_Pos);
			port->PULL &= ~(((uint32_t)PORT_PULL_DOWN_ON << mdrPin->pin) << PORT_PULL_DOWN_Pos);
		break;

		// Push-Pull output
		case OUTPUT:
			port->OE |= portPin;
			port->PD &= ~(((uint32_t)PORT_PD_OPEN << mdrPin->pin) << PORT_PD_Pos);
		break;

		case INPUT_PULLUP:
			port->OE &= ~portPin;
			port->PULL |= (((uint32_t)PORT_PULL_UP_ON << mdrPin->pin) << PORT_PULL_UP_Pos);
			port->PULL &= ~(((uint32_t)PORT_PULL_DOWN_ON << mdrPin->pin) << PORT_PULL_DOWN_Pos);
		break;

		case INPUT_PULLDOWN:
			port->OE &= ~portPin;
			port->PULL &= ~(((uint32_t)PORT_PULL_UP_ON << mdrPin->pin) << PORT_PULL_UP_Pos);
			port->PULL |= (((uint32_t)PORT_PULL_DOWN_ON << mdrPin->pin) << PORT_PULL_DOWN_Pos);
		break;

		case OUTPUT_OPENDRAIN:
			port->OE |= portPin;
			port->PULL &= ~(((uint32_t)PORT_PULL_UP_ON << mdrPin->pin) << PORT_PULL_UP_Pos);
			port->PULL &= ~(((uint32_t)PORT_PULL_DOWN_ON << mdrPin->pin) << PORT_PULL_DOWN_Pos);
			port->PD |= (((uint32_t)PORT_PD_OPEN << mdrPin->pin) << PORT_PD_Pos);
		break;

		// Configure as floating input
		default:
			port->OE &= ~portPin;
			port->PULL &= ~(((uint32_t)PORT_PULL_UP_ON << mdrPin->pin) << PORT_PULL_UP_Pos);
			port->PULL &= ~(((uint32_t)PORT_PULL_DOWN_ON << mdrPin->pin) << PORT_PULL_DOWN_Pos);
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
	if(pinNumber >= DMAX) return;

	const tMilandrPin * mdrPin = &pinTable[pinNumber][PORT_FUNC_PORT][0];
	volatile MDR_PORT_TypeDef * port = MDR_PORT(mdrPin->port);
	uint16_t portPin = (1 << mdrPin->pin);

	if(status == HIGH)
		port->RXTX |= portPin;
	else
		port->RXTX &= ~portPin;
}

/**
 * Read the digital state of a pin.
 *
 * @param pinNumber The Arduino pin number to read.
 * @return The digital state of the pin, either `HIGH` or `LOW`.
 */
PinStatus digitalRead(pin_size_t pinNumber)
{
	if(pinNumber >= DMAX) return LOW;

	const tMilandrPin * mdrPin = &pinTable[pinNumber][PORT_FUNC_PORT][0];
	volatile MDR_PORT_TypeDef * port = MDR_PORT(mdrPin->port);

	return (PinStatus)((port->RXTX >> mdrPin->pin) & 1UL);
}
