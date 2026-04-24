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
#include <stddef.h>
#include "pin_names.h"
#include "periph_definition.h"
#include "variant.h"
#include "milandr_hal.h"
#include "MDR32FxQI_rst_clk.h"
#include "MDR32FxQI_port.h"
#include "MDR32FxQI_uart.h"

//------------------------------------------------------------------------------
// Доступ к блоку регистров UART
//------------------------------------------------------------------------------
#define MDR_UART(n)      (MDR_UART_TypeDef    *)((uint32_t)MDR_UART1 + 0x8000UL * (n))

//------------------------------------------------------------------------------
// Полная таблица пинов с их функциональным назначением
//------------------------------------------------------------------------------
extern const tMilandrPin pinTable[][PIN_MUX_LINES_NUM][1];

//------------------------------------------------------------------------------
// Таблица пинов UART по умолчанию
//------------------------------------------------------------------------------
#define UART_RX_LINE     (UART_RXD_LINE - UART_RXD_LINE)
#define UART_TX_LINE     (UART_TXD_LINE - UART_RXD_LINE)
#define UART_GET_LINE(n) ((n < UART_RXD_LINE) ? PERIPH_UNKNOWN_LINE : (n - UART_RXD_LINE))
#define UART_LINE_NUM    (UART_TX_LINE - UART_RX_LINE + 1)

#ifndef SERIAL1_RX
#define SERIAL1_RX       0xFF
#endif

#ifndef SERIAL1_TX
#define SERIAL1_TX       0xFF
#endif

#ifndef SERIAL2_RX
#define SERIAL2_RX       0xFF
#endif

#ifndef SERIAL2_TX
#define SERIAL2_TX       0xFF
#endif

#ifndef SERIAL3_RX
#define SERIAL3_RX       0xFF
#endif

#ifndef SERIAL3_TX
#define SERIAL3_TX       0xFF
#endif

static const uint8_t uartDefaultPins[UART_COUNT][UART_LINE_NUM] =
{
	[UART_1] =
	{
		[UART_RX_LINE] = SERIAL1_RX,
		[UART_TX_LINE] = SERIAL1_TX,
	},
	[UART_2] =
	{
		[UART_RX_LINE] = SERIAL2_RX,
		[UART_TX_LINE] = SERIAL2_TX,
	},
	[UART_3] =
	{
		[UART_RX_LINE] = SERIAL3_RX,
		[UART_TX_LINE] = SERIAL3_TX,
	},
};

enum UartWlen
{
	WLEN5 = 0,
	WLEN6,
	WLEN7,
	WLEN8,
	WLEN_SIZE
};

static const uint8_t uartWlenTable[WLEN_SIZE] =
{
	[WLEN5] = UART_WordLength5b,
	[WLEN6] = UART_WordLength6b,
	[WLEN7] = UART_WordLength7b,
	[WLEN8] = UART_WordLength8b,
};

//------------------------------------------------------------------------------
// Статические переменные
//------------------------------------------------------------------------------
static UART_InitTypeDef UART_InitStructure;

//------------------------------------------------------------------------------
// Возвращает номера пинов RX/TX в соответствии с variant платы
//------------------------------------------------------------------------------
uint8_t milandr_uart_pin(tUartVariant uart, tPeriphLineVariant line)
{
	uint8_t index = UART_GET_LINE(line);

	if(uart >= UART_COUNT || index >= UART_LINE_NUM)
		return 0xFF;
	else
		return uartDefaultPins[uart][index];
}

//------------------------------------------------------------------------------
// Инициализация USART
//------------------------------------------------------------------------------
tUartVariant milandr_uart_init(uint8_t  rxPin,
                               uint8_t  txPin,
                               uint32_t baudRate,
                               uint8_t  wordLen,
                               uint8_t  stopBits,
                               tMilandrParity  parity)
{
	if(rxPin > milandr_gpio_count() ||
	   txPin > milandr_gpio_count() ||
	   rxPin == txPin)
	{
		return UART_UNKNOWN;
	}

	//
	// Сначала выполняем поиск модуля UARTn, соответствующего пинам rxPin/txPin
	// и описание пинов RX/TX в таблице pinTable
	//
	tUartVariant uartN = UART_UNKNOWN;
	const tMilandrPin * lines[UART_LINE_NUM] = {NULL, NULL};
	const tMilandrPin * pins[4] =
	{
		&pinTable[rxPin][PIN_MUX_ALTER][0],
		&pinTable[rxPin][PIN_MUX_OVERRID][0],
		&pinTable[txPin][PIN_MUX_ALTER][0],
		&pinTable[txPin][PIN_MUX_OVERRID][0]
	};

	for(tUartVariant uartIdx = UART_1; uartIdx < UART_COUNT; uartIdx++)
	{
		for(tPeriphLineVariant ln = UART_RXD_LINE; ln <= UART_TXD_LINE; ln++)
		{
			const tMilandrPin * temp[4];
			uint8_t cnt = milandr_find_pin(PERIPH_UART, ln, uartIdx, pins, temp, 4);

			if(cnt) lines[UART_GET_LINE(ln)] = temp[0];
			else    lines[UART_GET_LINE(ln)] = NULL;
		}

		if(lines[UART_RX_LINE] && lines[UART_TX_LINE])
		{
			uartN = uartIdx;
			break;
		}
	}

	//
	// Не удалось определить модуль UART. Неверная комбинация пинов rxPin/txPin
	//
	if(uartN == UART_UNKNOWN) return uartN;

	//
	// Производим настройку GPIO
	//
	const tMilandrPin * rx = lines[UART_RX_LINE];
	const tMilandrPin * tx = lines[UART_TX_LINE];

	milandr_gpio_clock_enable(rxPin, true);
	milandr_gpio_clock_enable(txPin, true);

	milandr_gpio_cfg_input(rxPin);
	milandr_gpio_cfg_output_pp(txPin);

	if(rx->mode == PORT_FUNC_ALTER)
		milandr_gpio_sel_alter_func(rxPin);
	else
		milandr_gpio_sel_override_func(rxPin);

	if(tx->mode == PORT_FUNC_ALTER)
		milandr_gpio_sel_alter_func(txPin);
	else
		milandr_gpio_sel_override_func(txPin);

	//
	// Настройка UART
	//
	volatile MDR_UART_TypeDef * UARTx = MDR_UART(uartN);
	MDR_RST_CLK->PER_CLOCK |= (RST_CLK_PCLK_UART1 << uartN);

	UART_DeInit((MDR_UART_TypeDef*)UARTx);

	/* Set the HCLK division factor = 1 for UART1,UART2*/
	UART_BRGInit((MDR_UART_TypeDef*)UARTx, UART_HCLKdiv1);

	/* Initialize UART_InitStructure */
	UART_InitStructure.UART_BaudRate = baudRate;
	UART_InitStructure.UART_WordLength = uartWlenTable[(wordLen - 5) & 0x3];
	UART_InitStructure.UART_StopBits = (stopBits == 2) ?
										UART_StopBits2 :
										UART_StopBits1;
	UART_InitStructure.UART_Parity = (UART_Parity_TypeDef)parity;
	UART_InitStructure.UART_FIFOMode = UART_FIFO_OFF;
	UART_InitStructure.UART_HardwareFlowControl = UART_HardwareFlowControl_RXE |
												  UART_HardwareFlowControl_TXE;

	/* Configure UART1 parameters */
	UART_Init((MDR_UART_TypeDef*)UARTx, &UART_InitStructure);

	/* Enables UART1 peripheral */
	UART_Cmd((MDR_UART_TypeDef*)UARTx, ENABLE);

	return uartN;
}

//------------------------------------------------------------------------------
// Деинициализация USART
//------------------------------------------------------------------------------
void milandr_uart_deinit(uint8_t  rxPin,
                         uint8_t  txPin,
                         tUartVariant uartN)
{
	volatile MDR_UART_TypeDef * UARTx = MDR_UART(uartN);

	UART_Cmd((MDR_UART_TypeDef*)UARTx, DISABLE);
	UART_DeInit((MDR_UART_TypeDef*)UARTx);
	MDR_RST_CLK->PER_CLOCK &= ~(RST_CLK_PCLK_UART1 << uartN);

	milandr_gpio_sel_port_func(rxPin);
	milandr_gpio_sel_port_func(txPin);
	milandr_gpio_cfg_input(rxPin);
	milandr_gpio_cfg_input(txPin);
	milandr_gpio_clock_enable(rxPin, false);
	milandr_gpio_clock_enable(txPin, false);
}

//------------------------------------------------------------------------------
// Возвращает количество принятых данных
//------------------------------------------------------------------------------
int milandr_uart_available(tUartVariant uartN)
{
	if(uartN == UART_UNKNOWN) return 0;

	volatile MDR_UART_TypeDef * UARTx = MDR_UART(uartN);
	return (UARTx->FR & UART_FLAG_RXFF) ? 1 : 0;
}

//------------------------------------------------------------------------------
// Чтение байта из буфера без удаления
//------------------------------------------------------------------------------
int milandr_uart_peak(tUartVariant uartN)
{
	if(uartN == UART_UNKNOWN) return -1;

	return -1;
}

//------------------------------------------------------------------------------
// Чтение байта из буфера с удалением
//------------------------------------------------------------------------------
int milandr_uart_read(tUartVariant uartN)
{
	if(uartN == UART_UNKNOWN) return -1;

	volatile MDR_UART_TypeDef * UARTx = MDR_UART(uartN);

	if((UARTx->FR & UART_FLAG_RXFF))
		return UARTx->DR;
	else
		return -1;
}

//------------------------------------------------------------------------------
// Передача всех данных из буфера с ожиданием завершения
//------------------------------------------------------------------------------
void milandr_uart_flush(tUartVariant uartN)
{
	if(uartN == UART_UNKNOWN) return;

	volatile MDR_UART_TypeDef * UARTx = MDR_UART(uartN);
	while (UARTx->FR & UART_FLAG_TXFF);
}

//------------------------------------------------------------------------------
// Передача байта по uart
//------------------------------------------------------------------------------
size_t milandr_uart_write(tUartVariant uartN, const uint8_t c)
{
	// Все равно сообщаем, что отправили байт, чтобы программа не зависала
	if(uartN == UART_UNKNOWN) return 1;

	volatile MDR_UART_TypeDef * UARTx = MDR_UART(uartN);
	while ((UARTx->FR & UART_FLAG_TXFE) == RESET);
	UARTx->DR = c;

	return 1;
}

//------------------------------------------------------------------------------
// Передача массива по uart
//------------------------------------------------------------------------------
size_t milandr_uart_send(tUartVariant uartN, const uint8_t* buf, const size_t size)
{
	// Все равно сообщаем, что отправили байт, чтобы программа не зависала
	if(uartN == UART_UNKNOWN || !buf || size <= 0) return 0;

	uint8_t n = size;
	const uint8_t * ptr = buf;
	volatile MDR_UART_TypeDef * UARTx = MDR_UART(uartN);

	while(n--)
	{
		while ((UARTx->FR & UART_FLAG_TXFE) == RESET);
		UARTx->DR = *ptr++;
	}

	return size;
}
