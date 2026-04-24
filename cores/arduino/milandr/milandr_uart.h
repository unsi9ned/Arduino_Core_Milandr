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

#ifndef _MILANDR_UART_H_
#define _MILANDR_UART_H_

#include <stdint.h>
#include <stddef.h>
#include "periph_definition.h"

//------------------------------------------------------------------------------
// Конфигурация битов четности для UART
//------------------------------------------------------------------------------
typedef enum
{
	MILANDR_PARITY_NONE = 0x00,
	MILANDR_PARITY_EVEN = 0x06,
	MILANDR_PARITY_ODD = 0x02,
	MILANDR_PARITY_MARK = 0x82,
	MILANDR_PARITY_SPACE = 0x86
}
tMilandrParity;

#ifdef __cplusplus
extern "C" {
#endif


//------------------------------------------------------------------------------
// Экспортируемые функции
//------------------------------------------------------------------------------
extern uint8_t milandr_uart_pin(tUartVariant uart, tPeriphLineVariant line);
extern tUartVariant milandr_uart_init(uint8_t  rxPin,
                                      uint8_t  txPin,
                                      uint32_t baudRate,
                                      uint8_t  wordLen,
                                      uint8_t  stopBits,
                                      tMilandrParity  parity);

extern void milandr_uart_deinit(uint8_t  rxPin,
                                uint8_t  txPin,
                                tUartVariant uartN);

extern int milandr_uart_available(tUartVariant uartN);
extern int milandr_uart_peak(tUartVariant uartN);
extern int milandr_uart_read(tUartVariant uartN);
extern void milandr_uart_flush(tUartVariant uartN);
extern size_t milandr_uart_write(tUartVariant uartN, const uint8_t c);
extern size_t milandr_uart_send(tUartVariant uartN, const uint8_t* buf, const size_t size);


#ifdef __cplusplus
}
#endif

#endif //_MILANDR_UART_H_
