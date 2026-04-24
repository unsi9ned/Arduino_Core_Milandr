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

#include "Uart.h"
#include "milandr/milandr_hal.h"

#define SERIAL_DATA_POS     (0x8)
#define SERIAL_DATA_BEGIN   (0x4)

#define SERIAL_WLEN_5        SERIAL_DATA_5
#define SERIAL_WLEN_6        SERIAL_DATA_6
#define SERIAL_WLEN_7        SERIAL_DATA_7
#define SERIAL_WLEN_8        SERIAL_DATA_8
#define SERIAL_WLEN_MASK     SERIAL_DATA_MASK
#define SERIAL_WLEN_POS      (0x8ul)
#define SERIAL_WLEN_OFFSET   (0x4ul)

#ifdef SERIAL_PARITY_MASK
#undef SERIAL_PARITY_MASK
#define SERIAL_PARITY_MASK   (0x7ul)
#endif

const static tMilandrParity parityTable[] =
{
	// Нумерация Arduino начинается с 0
	MILANDR_PARITY_NONE,

	MILANDR_PARITY_EVEN,
	MILANDR_PARITY_ODD,
	MILANDR_PARITY_NONE,
	MILANDR_PARITY_MARK,
	MILANDR_PARITY_SPACE,

	// Защита от ошибочного значение
	MILANDR_PARITY_NONE,
	MILANDR_PARITY_NONE,
};

namespace arduino {

//
// Конструктор
//
Uart::Uart(uint8_t rxPin, uint8_t txPin) :
	_rxPin(rxPin),
	_txPin(txPin),
	_uartN(UART_UNKNOWN)
{

}

//
// Инициализация
//
void Uart::begin(const unsigned long baudrate)
{
	begin(baudrate, SERIAL_8N1);
}

void Uart::begin(const unsigned long baudrate, const uint16_t config)
{
	uint8_t word_size = ((config & SERIAL_WLEN_MASK) >> SERIAL_WLEN_POS) + SERIAL_WLEN_OFFSET;
	uint8_t stop_bits = (config & SERIAL_STOP_BIT_1_5) ? 2 : 1;
	tMilandrParity parity = parityTable[config & SERIAL_PARITY_MASK];

	_uartN = milandr_uart_init(_rxPin, _txPin, baudrate, word_size, stop_bits, parity);
}

//
// Деинициализация
//
void Uart::end(void)
{
	if(_uartN == UART_UNKNOWN) return;
	milandr_uart_deinit(_rxPin, _txPin, _uartN);
	_uartN = UART_UNKNOWN;
}

//
// Количество доступных данных в приемном буфере
//
int Uart::available()
{
	return milandr_uart_available(_uartN);
}

//
// Чтение текущего байта в FIFO без удаления
//
int Uart::peek()
{
	return milandr_uart_peak(_uartN);
}

//
// Чтение текущего байта в FIFO с последующим удалением
//
int Uart::read()
{
	return milandr_uart_read(_uartN);
}

//
// Передача всех данных из буфера с ожиданием завершения
//
void Uart::flush()
{
	milandr_uart_flush(_uartN);
}

//
// Передача байта по UART
//
size_t Uart::write(const uint8_t c)
{
	return milandr_uart_write(_uartN, c);
}

//
// Передача массива по UART
//
size_t Uart::write(const uint8_t* buf, const size_t size)
{
	return milandr_uart_send(_uartN, buf, size);
}

//
// Проверка готовности модуля к работе
//
Uart::operator bool()
{
	return _uartN != UART_UNKNOWN;
}

}  // namespace arduino

// Instantiate user accessible Serial instance
arduino::UartWrapper Uart1 (milandr_uart_pin(UART_1, UART_RXD_LINE),
                            milandr_uart_pin(UART_1, UART_TXD_LINE));

arduino::UartWrapper Uart2 (milandr_uart_pin(UART_2, UART_RXD_LINE),
                            milandr_uart_pin(UART_2, UART_TXD_LINE));

arduino::UartWrapper Uart3 (milandr_uart_pin(UART_3, UART_RXD_LINE),
                            milandr_uart_pin(UART_3, UART_TXD_LINE));

arduino::HardwareSerial& Serial = *Uart1;

