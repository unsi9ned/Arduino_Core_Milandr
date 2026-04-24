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

#include <stdbool.h>
#include "periph_definition.h"
#include "milandr_uart.h"
#include "MDR32FxQI_rst_clk.h"
#include "MDR32FxQI_uart.h"

//------------------------------------------------------------------------------
// Полная таблица пинов с их функциональным назначением
//------------------------------------------------------------------------------
extern const tMilandrPin pinTable[][PIN_MUX_LINES_NUM][1];

//------------------------------------------------------------------------------
// Включить тактирование USART
//------------------------------------------------------------------------------
void milandr_uart_init(uint8_t  rxPin,
                       uint8_t  txPin,
                       uint32_t baudRate,
                       uint8_t  wordLen,
                       uint8_t  stopBits,
                       tMilandrParity  parity)
{
	tMilandrPin mdrRxPin = pinTable[rxPin][PIN_MUX_ALTER][0];
	tMilandrPin mdrTxPin = pinTable[txPin][PIN_MUX_OVERRID][0];

	//MDR_RST_CLK->PER_CLOCK |= RST_CLK_PCLK_PORTA << mdrPin.port;
}

uint8_t milandr_uart_pin(tUartVariant uart, tPeriphLineVariant line)
{
	return line + 1;
}
