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
#include <stdlib.h>
#include "pin_names.h"
#include "periph_definition.h"
#include "variant.h"
#include "milandr_hal.h"
#include "MDR32FxQI_rst_clk.h"
#include "MDR32FxQI_port.h"
#include "MDR32FxQI_uart.h"

#define UART_BUF_STATIC_ALLOC 1

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

//------------------------------------------------------------------------------
// Таблица длин символа
//------------------------------------------------------------------------------
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
// Блок переменных кольцевого буфера
//------------------------------------------------------------------------------
#ifndef SERIAL1_RX_BUFFER_SIZE
#define SERIAL1_RX_BUFFER_SIZE       64
#endif

#ifndef SERIAL1_TX_BUFFER_SIZE
#define SERIAL1_TX_BUFFER_SIZE       64
#endif

#ifndef SERIAL2_RX_BUFFER_SIZE
#define SERIAL2_RX_BUFFER_SIZE       64
#endif

#ifndef SERIAL2_TX_BUFFER_SIZE
#define SERIAL2_TX_BUFFER_SIZE       64
#endif

#if UART_BUF_STATIC_ALLOC
#ifndef SERIAL3_RX_BUFFER_SIZE
#define SERIAL3_RX_BUFFER_SIZE       8
#endif

#ifndef SERIAL3_TX_BUFFER_SIZE
#define SERIAL3_TX_BUFFER_SIZE       8
#endif
#else
#ifndef SERIAL3_RX_BUFFER_SIZE
#define SERIAL3_RX_BUFFER_SIZE       64
#endif

#ifndef SERIAL3_TX_BUFFER_SIZE
#define SERIAL3_TX_BUFFER_SIZE       64
#endif
#endif

typedef struct
{
	uint32_t buf_tail;           // Хвост приемного буффера
	uint32_t buf_head;           // Голова приемного буффера
	uint32_t buf_size;           // Общий объем буфера
	uint8_t* buf;                // Память, выделяемая под приемный буфер

}
tUartRingBuf;

//------------------------------------------------------------------------------
// Статические переменные
//------------------------------------------------------------------------------
static UART_InitTypeDef UART_InitStructure;
static tUartRingBuf rxRingBuffer[UART_COUNT];
static tUartRingBuf txRingBuffer[UART_COUNT];

#if UART_BUF_STATIC_ALLOC

static uint8_t rx_memory_1[SERIAL1_RX_BUFFER_SIZE];
static uint8_t tx_memory_1[SERIAL1_TX_BUFFER_SIZE];

static uint8_t rx_memory_2[SERIAL2_RX_BUFFER_SIZE];
static uint8_t tx_memory_2[SERIAL2_TX_BUFFER_SIZE];

static uint8_t rx_memory_3[SERIAL3_RX_BUFFER_SIZE];
static uint8_t tx_memory_3[SERIAL3_TX_BUFFER_SIZE];

static uint8_t * uartAllocTable[UART_COUNT][UART_LINE_NUM] =
{
	[UART_1] =
	{
		[UART_RX_LINE] = rx_memory_1,
		[UART_TX_LINE] = tx_memory_1,
	},
	[UART_2] =
	{
		[UART_RX_LINE] = rx_memory_2,
		[UART_TX_LINE] = tx_memory_2,
	},
	[UART_3] =
	{
		[UART_RX_LINE] = rx_memory_3,
		[UART_TX_LINE] = tx_memory_3,
	},
};

#endif

#define rxb_tail(n)       rxRingBuffer[n].buf_tail
#define rxb_head(n)       rxRingBuffer[n].buf_head
#define rxb_size(n)       rxRingBuffer[n].buf_size
#define rxb_buf(n)        rxRingBuffer[n].buf

#define txb_tail(n)       txRingBuffer[n].buf_tail
#define txb_head(n)       txRingBuffer[n].buf_head
#define txb_size(n)       txRingBuffer[n].buf_size
#define txb_buf(n)        txRingBuffer[n].buf
#define txb_empty(n)      (txb_tail(n) == txb_head(n))
#define txb_full(n)       (((txb_head(n) + 1) % txb_size(n)) == txb_tail(n))
#define txb_available(n)  ((txb_size(n) + txb_head(n) - txb_tail(n)) % txb_size(n))
#define txb_free_space(n) (txb_size(n) - 1 - txb_available(n))

// Размеры приемных буферов
static uint16_t rxBufSize[UART_COUNT] =
{
	[UART_1] = SERIAL1_RX_BUFFER_SIZE,
	[UART_2] = SERIAL2_RX_BUFFER_SIZE,
	[UART_3] = SERIAL3_RX_BUFFER_SIZE,
};

// Размеры приемных буферов
static uint16_t txBufSize[UART_COUNT] =
{
	[UART_1] = SERIAL1_TX_BUFFER_SIZE,
	[UART_2] = SERIAL2_TX_BUFFER_SIZE,
	[UART_3] = SERIAL3_TX_BUFFER_SIZE,
};

// Таблица номеров прерываний
static IRQn_Type irqTable[UART_COUNT] =
{
#if defined(MDR_UART1)
	[UART_1] = UART1_IRQn,
#endif

#if defined(MDR_UART2)
	[UART_2] = UART2_IRQn,
#endif

#if defined(MDR_UART3)
	[UART_3] = UART3_IRQn,
#endif
};

//------------------------------------------------------------------------------
// Прототипы
//------------------------------------------------------------------------------
static void ring_buffer_send(volatile uint32_t * DR, tUartVariant n);

//------------------------------------------------------------------------------
// Предварительная инициализация драйвера
//------------------------------------------------------------------------------
void milandr_uart_preinit()
{
	for (int i = 0; i < UART_COUNT; i++)
	{
		rxRingBuffer[i].buf_tail = 0;
		rxRingBuffer[i].buf_head = 0;
		rxRingBuffer[i].buf_size = rxBufSize[i];
		rxRingBuffer[i].buf = NULL;

		txRingBuffer[i].buf_tail = 0;
		txRingBuffer[i].buf_head = 0;
		txRingBuffer[i].buf_size = txBufSize[i];
		txRingBuffer[i].buf = NULL;
	}
}

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
// Инициализация кольцевого буфера
//------------------------------------------------------------------------------
static bool rx_buffer_init(tUartVariant uartN)
{
	if(uartN >= UART_COUNT) return false;

	rxRingBuffer[uartN].buf_tail = 0;
	rxRingBuffer[uartN].buf_head = 0;

#if UART_BUF_STATIC_ALLOC
	rxRingBuffer[uartN].buf = uartAllocTable[uartN][UART_RX_LINE];
#else
	rxRingBuffer[uartN].buf = malloc(rxRingBuffer[uartN].buf_size);
#endif

	return rxRingBuffer[uartN].buf != NULL;
}

static bool tx_buffer_init(tUartVariant uartN)
{
	if(uartN >= UART_COUNT) return false;

	txRingBuffer[uartN].buf_tail = 0;
	txRingBuffer[uartN].buf_head = 0;

#if UART_BUF_STATIC_ALLOC
	txRingBuffer[uartN].buf = uartAllocTable[uartN][UART_TX_LINE];
#else
	txRingBuffer[uartN].buf = malloc(txRingBuffer[uartN].buf_size);
#endif

	return txRingBuffer[uartN].buf != NULL;
}

//------------------------------------------------------------------------------
// Деинициализация кольцевого буфера
//------------------------------------------------------------------------------
static void rx_buffer_deinit(tUartVariant uartN)
{
	if(uartN >= UART_COUNT) return;

#if !UART_BUF_STATIC_ALLOC
	free(rxRingBuffer[uartN].buf);
#endif
	rxRingBuffer[uartN].buf_tail = 0;
	rxRingBuffer[uartN].buf_head = 0;
	rxRingBuffer[uartN].buf = NULL;
}

static void tx_buffer_deinit(tUartVariant uartN)
{
	if(uartN >= UART_COUNT) return;

#if !UART_BUF_STATIC_ALLOC
	free(txRingBuffer[uartN].buf);
#endif
	txRingBuffer[uartN].buf_tail = 0;
	txRingBuffer[uartN].buf_head = 0;
	txRingBuffer[uartN].buf = NULL;
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
	if(rxPin >= milandr_gpio_count() ||
	   txPin >= milandr_gpio_count() ||
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
	// Инициализация кольцевого буфера
	//
	if(!rx_buffer_init(uartN))
	{
		return UART_UNKNOWN;
	}

	if(!tx_buffer_init(uartN))
	{
		rx_buffer_deinit(uartN);
		return UART_UNKNOWN;
	}

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

	/* Interrupts Enable */
	NVIC_EnableIRQ(irqTable[uartN]);

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

	/* Enable Receiver & Transmitter interrupt*/
	//UART_ITConfig((MDR_UART_TypeDef*)UARTx, UART_IT_RX | UART_IT_TX, ENABLE);
	UART_ITConfig((MDR_UART_TypeDef*)UARTx, UART_IT_RX | UART_IT_TX | UART_IT_OE, ENABLE);

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

	rx_buffer_deinit(uartN);
	tx_buffer_deinit(uartN);
}

//------------------------------------------------------------------------------
// Возвращает количество принятых данных
//------------------------------------------------------------------------------
int milandr_uart_available(tUartVariant n)
{
	if(n == UART_UNKNOWN) return 0;

	return (rxb_size(n) + rxb_head(n) - rxb_tail(n)) % rxb_size(n);
}

//------------------------------------------------------------------------------
// Проверка приемного FIFO на занятость
// (Возможно, пригодится для синхронных операций)
//------------------------------------------------------------------------------
bool milandr_uart_rxfifo_is_empty(tUartVariant n)
{
	volatile MDR_UART_TypeDef * UARTx = MDR_UART(n);
	return ((UARTx->FR & UART_FLAG_RXFF) == RESET);
}

//------------------------------------------------------------------------------
// Чтение байта из буфера без удаления
//------------------------------------------------------------------------------
int milandr_uart_peak(tUartVariant n)
{
	if(n == UART_UNKNOWN) return -1;

	uint8_t ch;
	ch = rxb_buf(n)[rxb_tail(n)];
	return (int)ch;
}

//------------------------------------------------------------------------------
// Чтение байта из буфера с удалением
//------------------------------------------------------------------------------
int milandr_uart_read(tUartVariant n)
{
	if(n == UART_UNKNOWN) return -1;

	int ch = -1;

	if(rxb_tail(n) != rxb_head(n))
	{
		ch = rxb_buf(n)[rxb_tail(n)];
		rxb_tail(n) = (rxb_tail(n) + 1) % rxb_size(n);
	}

	return (int)ch;
}

//------------------------------------------------------------------------------
// Синхронное чтение байта из буфера FIFO
//------------------------------------------------------------------------------
int milandr_uart_read_blocking(tUartVariant n)
{
	volatile MDR_UART_TypeDef * UARTx = MDR_UART(n);
	while((UARTx->FR & UART_FLAG_RXFF) == RESET);
	return (int)UARTx->DR;
}

//------------------------------------------------------------------------------
// Передача всех данных из буфера с ожиданием завершения
//------------------------------------------------------------------------------
void milandr_uart_flush(tUartVariant n)
{
	if(n == UART_UNKNOWN) return;

	volatile MDR_UART_TypeDef * UARTx = MDR_UART(n);

	// Ждём, пока кольцевой буфер передачи опустеет
	while (!txb_empty(n))
	{
		// Пинаем передатчик, если он простаивает
		if(UARTx->FR & UART_FR_TXFE)
		{
			ring_buffer_send(&UARTx->DR, n);
		}
	}

	// Ждём, пока аппаратный TX FIFO опустеет
	while (!(UARTx->FR & UART_FR_TXFE))
	{

	}

	// Ждём, пока сдвиговый регистр закончит передачу
	while (UARTx->FR & UART_FR_BUSY)
	{

	}
}

//------------------------------------------------------------------------------
// Передача байта по uart
//------------------------------------------------------------------------------
size_t milandr_uart_write(tUartVariant n, const uint8_t c)
{
	if(n == UART_UNKNOWN) return 0;

	volatile MDR_UART_TypeDef * UARTx = MDR_UART(n);

	//Если данные пока не попадали в буфер и передатчик готов к отправке
	if(txb_empty(n) && (UARTx->FR & UART_FR_TXFE))
	{
		UARTx->DR = c;
	}
	// Буфер полностью заполнен. Сообщаем вызвавшему уровню, что не отправили
	// ни одного байта
	else if(txb_full(n))
	{
		return 0;
	}
	// Помещаем один символ в буфер
	else
	{
		uint8_t next_head = (txb_head(n) + 1) % txb_size(n);
		txb_buf(n)[txb_head(n)] = c;
		txb_head(n) = next_head;

		// Если конвеер отправки поломался, пинаем его вручную
		if(UARTx->FR & UART_FR_TXFE)
		{
			ring_buffer_send(&UARTx->DR, n);
		}
	}

	return 1;
}

//------------------------------------------------------------------------------
// Синхронная запись байта в FIFO
//------------------------------------------------------------------------------
void milandr_uart_write_blocking(tUartVariant n, const uint8_t c)
{
	if(n == UART_UNKNOWN) return;

	volatile MDR_UART_TypeDef * UARTx = MDR_UART(n);
	while ((UARTx->FR & UART_FLAG_TXFE) == RESET);
	UARTx->DR = c;
}

//------------------------------------------------------------------------------
// Передача массива по uart
//------------------------------------------------------------------------------
size_t milandr_uart_send(tUartVariant n, const uint8_t* buf, const size_t size)
{
	if(n == UART_UNKNOWN || !buf || size <= 0) return 0;

	uint8_t sz = size;
	const uint8_t * ptr = buf;
	volatile MDR_UART_TypeDef * UARTx = MDR_UART(n);

	uint32_t free_space = txb_free_space(n);

	for(uint32_t i = 0; i < free_space && i < size; i++)
	{
		milandr_uart_write(n, *ptr++);
	}

	return (size < free_space) ? size : free_space;
}

//------------------------------------------------------------------------------
// Передача данных по UART в синхронном режиме
//------------------------------------------------------------------------------
size_t milandr_uart_send_blocking(tUartVariant n, const uint8_t* buf, const size_t size)
{
	if(n == UART_UNKNOWN || !buf || size <= 0) return 0;

	uint8_t sz = size;
	const uint8_t * ptr = buf;
	volatile MDR_UART_TypeDef * UARTx = MDR_UART(n);

	while(sz--)
	{
		while ((UARTx->FR & UART_FLAG_TXFE) == RESET);
		UARTx->DR = *ptr++;
	}

	return size;
}

//------------------------------------------------------------------------------
// Прием данных в кольцевой буфер
//------------------------------------------------------------------------------
static void ring_buffer_receive(tUartVariant n, uint8_t chr)
{
	uint32_t next_head = (rxb_head(n) + 1) % rxb_size(n);

	if(next_head == rxb_tail(n))
	{
		// Буфер полон — сдвигаем tail (теряем старый байт)
		// Здесь возможна гонка, но это безопасно:
		// - Если основной код читал tail в этот момент, он прочитает либо
		//   старое, либо новое значение — оба валидны
		rxb_tail(n) = (rxb_tail(n) + 1) % rxb_size(n);
	}

	rxb_buf(n)[rxb_head(n)] = chr;
	rxb_head(n) = next_head;
}

//------------------------------------------------------------------------------
// Отправка данных из кольцевого буфера
//------------------------------------------------------------------------------
static void ring_buffer_send(volatile uint32_t * DR, tUartVariant n)
{
	if(!txb_empty(n))
	{
		uint8_t c = txb_buf(n)[txb_tail(n)];
		txb_tail(n) = (txb_tail(n) + 1) % txb_size(n);
		*DR = c;
	}
}

//------------------------------------------------------------------------------
// Обработчики прерываний UART1
//------------------------------------------------------------------------------
#ifdef MDR_UART1
void UART1_IRQHandler(void)
{
	volatile uint32_t status = MDR_UART1->RIS;
	status &= ~UART_MIS_TXMIS;

	if(MDR_UART1->MIS & UART_MIS_OEMIS)
	{
		//TODO RX Overflow
		MDR_UART1->ICR = UART_MIS_OEMIS;
	}

	if(MDR_UART1->MIS & UART_MIS_RXMIS)
	{
		uint8_t chr = MDR_UART1->DR;
		ring_buffer_receive(UART_1, chr);
	}

	//Прерывание по передаче
	if(MDR_UART1->MIS & UART_MIS_TXMIS)
	{
		MDR_UART1->ICR = UART_ICR_TXIC;
		ring_buffer_send(&MDR_UART1->DR, UART_1);
	}

	MDR_UART1->ICR = status;
}
#endif

//------------------------------------------------------------------------------
// Обработчики прерываний UART2
//------------------------------------------------------------------------------
#ifdef MDR_UART2
void UART2_IRQHandler(void)
{
	volatile uint32_t status = MDR_UART2->RIS;
	status &= ~UART_MIS_TXMIS;

	if(MDR_UART2->MIS & UART_MIS_OEMIS)
	{
		//TODO RX Overflow
		MDR_UART2->ICR = UART_MIS_OEMIS;
	}

	if(MDR_UART2->MIS & UART_MIS_RXMIS)
	{
		uint8_t chr = MDR_UART2->DR;
		ring_buffer_receive(UART_2, chr);
	}

	//Прерывание по передаче
	if(MDR_UART2->MIS & UART_MIS_TXMIS)
	{
		MDR_UART2->ICR = UART_ICR_TXIC;
		ring_buffer_send(&MDR_UART2->DR, UART_2);
	}

	MDR_UART2->ICR = status;
}
#endif

//------------------------------------------------------------------------------
// Обработчики прерываний UART3
//------------------------------------------------------------------------------
#ifdef MDR_UART3
void UART3_IRQHandler(void)
{
	volatile uint32_t status = MDR_UART3->RIS;
	status &= ~UART_MIS_TXMIS;

	if(MDR_UART3->MIS & UART_MIS_OEMIS)
	{
		//TODO RX Overflow
		MDR_UART3->ICR = UART_MIS_OEMIS;
	}

	if(MDR_UART3->MIS & UART_MIS_RXMIS)
	{
		uint8_t chr = MDR_UART3->DR;
		ring_buffer_receive(UART_3, chr);
	}

	//Прерывание по передаче
	if(MDR_UART3->MIS & UART_MIS_TXMIS)
	{
		MDR_UART3->ICR = UART_ICR_TXIC;
		ring_buffer_send(&MDR_UART3->DR, UART_3);
	}

	MDR_UART3->ICR = status;
}
#endif
