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

#include <stddef.h>
#include "periph_definition.h"
#include "milandr_gpio.h"
#include "MDR32FxQI_port.h"
#include "MDR32FxQI_rst_clk.h"

//------------------------------------------------------------------------------
// Включить тактирование порта
//------------------------------------------------------------------------------
void milandr_gpio_clock_enable(uint8_t arduinoPin)
{
	const tMilandrPin mdrPin = pinTable[arduinoPin][PIN_MUX_GPIO][0];
	MDR_RST_CLK->PER_CLOCK |= RST_CLK_PCLK_PORTA << mdrPin.port;
}

//------------------------------------------------------------------------------
// Общая для всех режимов конфигурации пина
//------------------------------------------------------------------------------
static void milandr_gpio_cfg_common(uint8_t arduinoPin, volatile MDR_PORT_TypeDef ** portN, tMilandrPin * mdrPin)
{
	if(!mdrPin) return;

	*mdrPin = pinTable[arduinoPin][PIN_MUX_GPIO][0];
	volatile MDR_PORT_TypeDef *port = MDR_PORT(mdrPin->port & 0xFul);
	*portN = port;

	// MODE = 0x0 (Digital Port)
	port->FUNC &= ~(PORT_FUNC_MODE0_Msk << (mdrPin->pin * 2));
	port->ANALOG |= ((uint32_t)PORT_MODE_DIGITAL << mdrPin->pin);

	// Schmitt Trigger Disabled
	port->PD &= ~(((uint32_t)PORT_PD_SHM_ON << mdrPin->pin) << PORT_PD_SHM_Pos);

	// Fast port
	port->PWR |= (PORT_PWR0_Msk << (mdrPin->pin * 2));

	//Filter Disabled
	port->GFEN &= ~((uint32_t)PORT_GFEN_ON << mdrPin->pin);
}

//------------------------------------------------------------------------------
// Конфигурации пина как плавающий вход
//------------------------------------------------------------------------------
void milandr_gpio_cfg_input(uint8_t arduinoPin)
{
	volatile MDR_PORT_TypeDef * port = NULL;
	tMilandrPin mdrPin;
	milandr_gpio_cfg_common(arduinoPin, &port, &mdrPin);
	uint16_t pinMask = (1 << mdrPin.pin);

	port->OE &= ~pinMask;
	port->PULL &= ~(((uint32_t)PORT_PULL_UP_ON << mdrPin.pin) << PORT_PULL_UP_Pos);
	port->PULL &= ~(((uint32_t)PORT_PULL_DOWN_ON << mdrPin.pin) << PORT_PULL_DOWN_Pos);
}

//------------------------------------------------------------------------------
// Конфигурации пина как вход с подтяжкой к питанию
//------------------------------------------------------------------------------
void milandr_gpio_cfg_input_pu(uint8_t arduinoPin)
{
	volatile MDR_PORT_TypeDef * port = NULL;
	tMilandrPin mdrPin;
	milandr_gpio_cfg_common(arduinoPin, &port, &mdrPin);
	uint16_t pinMask = (1 << mdrPin.pin);

	port->OE &= ~pinMask;
	port->PULL |= (((uint32_t)PORT_PULL_UP_ON << mdrPin.pin) << PORT_PULL_UP_Pos);
	port->PULL &= ~(((uint32_t)PORT_PULL_DOWN_ON << mdrPin.pin) << PORT_PULL_DOWN_Pos);
}

//------------------------------------------------------------------------------
// Конфигурации пина как вход с подтяжкой к земле
//------------------------------------------------------------------------------
void milandr_gpio_cfg_input_pd(uint8_t arduinoPin)
{
	volatile MDR_PORT_TypeDef * port = NULL;
	tMilandrPin mdrPin;
	milandr_gpio_cfg_common(arduinoPin, &port, &mdrPin);
	uint16_t pinMask = (1 << mdrPin.pin);

	port->OE &= ~pinMask;
	port->PULL &= ~(((uint32_t)PORT_PULL_UP_ON << mdrPin.pin) << PORT_PULL_UP_Pos);
	port->PULL |= (((uint32_t)PORT_PULL_DOWN_ON << mdrPin.pin) << PORT_PULL_DOWN_Pos);
}

//------------------------------------------------------------------------------
// Конфигурации пина как выход
//------------------------------------------------------------------------------
void milandr_gpio_cfg_output_pp(uint8_t arduinoPin)
{
	volatile MDR_PORT_TypeDef * port = NULL;
	tMilandrPin mdrPin;
	milandr_gpio_cfg_common(arduinoPin, &port, &mdrPin);
	uint16_t pinMask = (1 << mdrPin.pin);

	port->OE |= pinMask;
	port->PD &= ~(((uint32_t)PORT_PD_OPEN << mdrPin.pin) << PORT_PD_Pos);
}

//------------------------------------------------------------------------------
// Конфигурации пина как выход c открытым коллектором
//------------------------------------------------------------------------------
void milandr_gpio_cfg_output_od(uint8_t arduinoPin)
{
	volatile MDR_PORT_TypeDef * port = NULL;
	tMilandrPin mdrPin;
	milandr_gpio_cfg_common(arduinoPin, &port, &mdrPin);
	uint16_t pinMask = (1 << mdrPin.pin);

	port->OE |= pinMask;
	port->PULL &= ~(((uint32_t)PORT_PULL_UP_ON << mdrPin.pin) << PORT_PULL_UP_Pos);
	port->PULL &= ~(((uint32_t)PORT_PULL_DOWN_ON << mdrPin.pin) << PORT_PULL_DOWN_Pos);
	port->PD |= (((uint32_t)PORT_PD_OPEN << mdrPin.pin) << PORT_PD_Pos);
}

//------------------------------------------------------------------------------
// Установка пина в 0/1
//------------------------------------------------------------------------------
void milandr_gpio_write(uint8_t arduinoPin, uint8_t level)
{
	const tMilandrPin mdrPin = pinTable[arduinoPin][PIN_MUX_GPIO][0];
	volatile MDR_PORT_TypeDef * port = MDR_PORT(mdrPin.port);
	uint16_t portMask = (1 << mdrPin.pin);

	if(level & 1)
		port->RXTX |= portMask;
	else
		port->RXTX &= ~portMask;
}

//------------------------------------------------------------------------------
// Чтение пина в 0/1
//------------------------------------------------------------------------------
uint8_t milandr_gpio_read(uint8_t arduinoPin)
{
	const tMilandrPin mdrPin = pinTable[arduinoPin][PIN_MUX_GPIO][0];
	volatile MDR_PORT_TypeDef * port = MDR_PORT(mdrPin.port);
	return (uint8_t)((port->RXTX >> mdrPin.pin) & 1UL);
}
