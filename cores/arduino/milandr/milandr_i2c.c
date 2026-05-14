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

#include "milandr_hal.h"
#include "variant.h"
#include "MDR32FxQI_config.h"
#include "MDR32FxQI_rst_clk.h"
#include "MDR32FxQI_port.h"

//------------------------------------------------------------------------------
// Стандартная скорость I2C
//------------------------------------------------------------------------------
#define TWI_STANDARD_FREQ  100000UL

//------------------------------------------------------------------------------
// Таймаут операций на шине I2C
//------------------------------------------------------------------------------
#ifndef TWI_TIMEOUT
#define TWI_TIMEOUT        0xFFFFUL
#endif

//------------------------------------------------------------------------------
// Тип операции чтение/запись
//------------------------------------------------------------------------------
#define TWI_READ           1
#define TWI_WRITE          0

//------------------------------------------------------------------------------
// Полная таблица пинов с их функциональным назначением
//------------------------------------------------------------------------------
extern const tMilandrPin   pinTable[][PIN_MUX_LINES_NUM][1];

//------------------------------------------------------------------------------
// Таблица пинов UART по умолчанию
//------------------------------------------------------------------------------
#define TWI_SCL_LINE      (I2C_SCL_LINE - I2C_SCL_LINE)
#define TWI_SDA_LINE      (I2C_SDA_LINE - I2C_SCL_LINE)
#define TWI_GET_LINE(n)   ((n < I2C_SCL_LINE) ? PERIPH_UNKNOWN_LINE : (n - I2C_SCL_LINE))
#define TWI_LINE_CNT      (I2C_SDA_LINE - I2C_SCL_LINE + 1)

#ifndef PIN_I2C1_SCL
#define PIN_I2C1_SCL      PIN_NC
#endif

#ifndef PIN_I2C1_SDA
#define PIN_I2C1_SDA      PIN_NC
#endif

//------------------------------------------------------------------------------
// Блок переменных кольцевого буфера
//------------------------------------------------------------------------------
#ifndef I2C1_RX_BUFFER_SIZE
#define I2C1_RX_BUFFER_SIZE      32
#endif

#ifndef I2C1_TX_BUFFER_SIZE
#define I2C1_TX_BUFFER_SIZE      32
#endif

typedef struct
{
	uint32_t buf_tail;           // Хвост приемного буффера
	uint32_t buf_head;           // Голова приемного буффера
	uint32_t buf_size;           // Общий объем буфера
	uint8_t* buf;                // Память, выделяемая под приемный буфер

}
tI2CRingBuf;

static uint8_t rx_memory_1[I2C1_RX_BUFFER_SIZE];
static uint8_t tx_memory_1[I2C1_TX_BUFFER_SIZE];

//------------------------------------------------------------------------------
// Дескриптор периферии
//------------------------------------------------------------------------------
typedef struct
{
	uint8_t   pins[TWI_LINE_CNT];
	uint32_t  clkMask;
	IRQn_Type irqNum;
	volatile MDR_I2C_TypeDef * regs;

	/* Буферы приема/передачи */
	tI2CRingBuf rxBuffer;
	tI2CRingBuf txBuffer;

	/* Флаг незавершенной транзакции */
	bool transmitting;

	/* Адрес Slave-устройства */
	uint8_t destAddr;
}
tPeriphDescriptor;

//------------------------------------------------------------------------------
// Таблица всех I2C-модулей
//------------------------------------------------------------------------------
static tPeriphDescriptor i2cTable[I2C_COUNT] =
{
	[I2C_1] =
	{
		.pins =
		{
			[TWI_SCL_LINE] = PIN_I2C1_SCL,
			[TWI_SDA_LINE] = PIN_I2C1_SDA,
		},

		.clkMask = RST_CLK_PCLK_I2C,
		.irqNum = I2C_IRQn,
		.regs = MDR_I2C,
		.rxBuffer =
		{
			.buf = rx_memory_1,
			.buf_size = sizeof(rx_memory_1)
		},
		.txBuffer =
		{
			.buf = tx_memory_1,
			.buf_size = sizeof(tx_memory_1)
		}
	}
};

#define rxb_tail(n)       i2cTable[n].rxBuffer.buf_tail
#define rxb_head(n)       i2cTable[n].rxBuffer.buf_head
#define rxb_size(n)       i2cTable[n].rxBuffer.buf_size
#define rxb_buf(n)        i2cTable[n].rxBuffer.buf

#define txb_tail(n)       i2cTable[n].txBuffer.buf_tail
#define txb_head(n)       i2cTable[n].txBuffer.buf_head
#define txb_size(n)       i2cTable[n].txBuffer.buf_size
#define txb_buf(n)        i2cTable[n].txBuffer.buf
#define txb_empty(n)      (txb_tail(n) == txb_head(n))
#define txb_full(n)       (((txb_head(n) + 1) % txb_size(n)) == txb_tail(n))
#define txb_available(n)  ((txb_size(n) + txb_head(n) - txb_tail(n)) % txb_size(n))
#define txb_free_space(n) (txb_size(n) - 1 - txb_available(n))

//------------------------------------------------------------------------------
// Ожидание окончания события на шине I2C
//------------------------------------------------------------------------------
#define milandr_i2c_wait(cond, status) \
		do {\
			volatile uint32_t timeout = TWI_TIMEOUT;\
			while((cond) && timeout)\
			{\
				timeout--;\
			}\
			status = (!(cond)) ? true : false;\
		} while(0)

#define milandr_i2c_wait_or_return(cond, status) \
		do {\
			volatile uint32_t timeout = TWI_TIMEOUT;\
			while((cond) && timeout)\
			{\
				timeout--;\
			}\
			if((cond)) return status;\
		} while(0)

#define milandr_i2c_wait_or_stop(i2c, cond, status) \
		do {\
			volatile uint32_t timeout = TWI_TIMEOUT;\
			while((cond) && timeout)\
			{\
				timeout--;\
			}\
			if((cond)) {\
				send_stop(i2c);\
				return status;\
			}\
		} while(0)

#define milandr_i2c_wait_or_cb(cond, status, callback) \
		do {\
			volatile uint32_t timeout = TWI_TIMEOUT;\
			while((cond) && timeout)\
			{\
				timeout--;\
			}\
			if((cond)) \
			{\
				do {\
					callback\
				} while(0);\
				return status;\
			}\
		} while(0)

//------------------------------------------------------------------------------
// Низкоуровневые функции
//------------------------------------------------------------------------------
static inline void deinit(tI2cVariant i2c)
{
	i2cTable[i2c].regs->CTR = 0;
	i2cTable[i2c].regs->STA = 0;
	i2cTable[i2c].regs->CMD = I2C_CMD_CLRINT;
	i2cTable[i2c].regs->PRL = 0;
	i2cTable[i2c].regs->PRH = 0;
}

static inline void i2c_enable(tI2cVariant i2c, bool state)
{
	volatile uint32_t ctr = i2cTable[i2c].regs->CTR;

	if(state)
		ctr |= I2C_CTR_EN_I2C;
	else
		ctr &= ~I2C_CTR_EN_I2C;

	i2cTable[i2c].regs->CTR = ctr;
}

static inline void send_stop(tI2cVariant i2c)
{
	i2cTable[i2c].regs->CMD = I2C_CMD_STOP;
}

static inline void send_address(tI2cVariant i2c, uint8_t WR)
{
	i2cTable[i2c].regs->TXD = i2cTable[i2c].destAddr | (WR & 0x1);
	i2cTable[i2c].regs->CMD = I2C_CMD_START | I2C_CMD_WR;
}

static inline void send_byte(tI2cVariant i2c, uint8_t byte)
{
	i2cTable[i2c].regs->TXD = byte;
	i2cTable[i2c].regs->CMD = I2C_CMD_WR;
}

static inline void request_byte(tI2cVariant i2c, bool nack)
{
	volatile uint32_t cmd = I2C_CMD_RD;

	if(nack)
		cmd |= I2C_CMD_ACK;

	i2cTable[i2c].regs->CMD = cmd;
}

static inline void read_byte(tI2cVariant i2c, uint8_t * data)
{
	*data = i2cTable[i2c].regs->RXD;
}

static void buffer_reset(tI2cVariant i2c)
{
	i2cTable[i2c].rxBuffer.buf_head = 0;
	i2cTable[i2c].rxBuffer.buf_tail = 0;

	i2cTable[i2c].txBuffer.buf_head = 0;
	i2cTable[i2c].txBuffer.buf_tail = 0;
}

//------------------------------------------------------------------------------
// Возвращает номера пинов в соответствии с variant платы
//------------------------------------------------------------------------------
uint8_t milandr_i2c_pin(tI2cVariant i2c, tPeriphLineVariant line)
{
	uint8_t index = TWI_GET_LINE(line);

	if(i2c >= I2C_COUNT || index >= TWI_LINE_CNT)
		return PIN_NC;
	else
		return i2cTable[i2c].pins[index];
}

//------------------------------------------------------------------------------
// Инициализация I2C
//------------------------------------------------------------------------------
tI2cVariant milandr_i2c_init(uint8_t sdaPin, uint8_t sclPin, uint8_t ownAddress)
{
	if(sdaPin > milandr_gpio_count() ||
	   sclPin > milandr_gpio_count() ||
	   sdaPin == sclPin)
	{
		return I2C_UNKNOWN;
	}

	//
	// Сначала выполняем поиск модуля UARTn, соответствующего пинам rxPin/txPin
	// и описание пинов RX/TX в таблице pinTable
	//
	tI2cVariant i2cN = I2C_UNKNOWN;
	const tMilandrPin * lines[TWI_LINE_CNT] = {NULL, NULL};
	const tMilandrPin * pins[4] =
	{
		&pinTable[sdaPin][PIN_MUX_ALTER][0],
		&pinTable[sdaPin][PIN_MUX_OVERRID][0],
		&pinTable[sclPin][PIN_MUX_ALTER][0],
		&pinTable[sclPin][PIN_MUX_OVERRID][0]
	};

	for(tI2cVariant i2cIdx = I2C_1; i2cIdx < I2C_COUNT; i2cIdx++)
	{
		for(tPeriphLineVariant ln = I2C_SCL_LINE; ln <= I2C_SDA_LINE; ln++)
		{
			const tMilandrPin * temp[4];
			uint8_t cnt = milandr_find_pin(PERIPH_I2C, ln, i2cIdx, pins, temp, 4);

			if(cnt) lines[TWI_GET_LINE(ln)] = temp[0];
			else    lines[TWI_GET_LINE(ln)] = NULL;
		}

		if(lines[TWI_SCL_LINE] && lines[TWI_SDA_LINE])
		{
			i2cN = i2cIdx;
			break;
		}
	}

	//
	// Не удалось определить модуль UART. Неверная комбинация пинов rxPin/txPin
	//
	if(i2cN == I2C_UNKNOWN) return i2cN;

	//
	// Инициализация дескриптора
	//
	i2cTable[i2cN].transmitting = false;

	//
	// Сброс приемного и передающего буферов
	//
	buffer_reset(i2cN);

	//
	// Производим настройку GPIO
	//
	const tMilandrPin * scl = lines[TWI_SCL_LINE];
	const tMilandrPin * sda = lines[TWI_SDA_LINE];

	milandr_gpio_clock_enable(sclPin, true);
	milandr_gpio_clock_enable(sdaPin, true);

	milandr_gpio_cfg_output_od_pu(sclPin);
	milandr_gpio_cfg_output_od_pu(sdaPin);

	if(scl->mode == PORT_FUNC_ALTER)
		milandr_gpio_sel_alter_func(sclPin);
	else
		milandr_gpio_sel_override_func(sclPin);

	if(sda->mode == PORT_FUNC_ALTER)
		milandr_gpio_sel_alter_func(sdaPin);
	else
		milandr_gpio_sel_override_func(sdaPin);

	//
	// Настройка I2C
	//
	volatile MDR_I2C_TypeDef * I2Cx = i2cTable[i2cN].regs;
	MDR_RST_CLK->PER_CLOCK |= i2cTable[i2cN].clkMask;

	/* Disable I2C */
	deinit(i2cN);

	/* Configure I2C speed*/
	milandr_i2c_set_freq(i2cN, TWI_STANDARD_FREQ);

	/* Enable I2C */
	i2c_enable(i2cN, true);

	return i2cN;
}

//------------------------------------------------------------------------------
// Принудительное завершение транзакции
//------------------------------------------------------------------------------
static inline tMilandrI2cStatus milandr_i2c_abort(MDR_I2C_TypeDef * I2Cx)
{
	if((I2Cx->STA & I2C_STA_BUSY) && (I2Cx->CTR & I2C_CTR_EN_I2C))
	{
		// Послать Stop
		I2Cx->CMD = I2C_CMD_STOP;

		// Дождать освобождения шины
		milandr_i2c_wait_or_return(I2Cx->STA & I2C_STA_BUSY, MILANDR_I2C_TIMEOUT);
	}

	return MILANDR_I2C_OK;
}

//------------------------------------------------------------------------------
// Деинициализация I2C
//------------------------------------------------------------------------------
void milandr_i2c_deinit(uint8_t sdaPin, uint8_t sclPin, tI2cVariant i2cN)
{
	if(i2cN >= I2C_COUNT) return;

	volatile MDR_I2C_TypeDef * I2Cx = i2cTable[i2cN].regs;

	milandr_i2c_abort((MDR_I2C_TypeDef*)I2Cx);
	deinit(i2cN);

	MDR_RST_CLK->PER_CLOCK &= ~i2cTable[i2cN].clkMask;

	milandr_gpio_sel_port_func(sdaPin);
	milandr_gpio_sel_port_func(sclPin);
	milandr_gpio_cfg_input(sdaPin);
	milandr_gpio_cfg_input(sclPin);
}

//------------------------------------------------------------------------------
// Настройка максимальной скорости передачи данных
//------------------------------------------------------------------------------
void milandr_i2c_set_freq(tI2cVariant i2cN, uint32_t freq)
{
	if(i2cN >= I2C_COUNT) return;

	volatile MDR_I2C_TypeDef * I2Cx = i2cTable[i2cN].regs;

	RST_CLK_FreqTypeDef RST_CLK_Clocks;
	RST_CLK_GetClocksFreq(&RST_CLK_Clocks);

	uint32_t enableState = I2Cx->CTR & I2C_CTR_EN_I2C;
	uint32_t div;

	/* Расчетная  скорость  обмена  данными  по  интерфейсу  I2C  определяется
	 * по формуле: Fscl = HCLK / (5 * (DIV + 1))
	 *
	 * Минимальное значение
	 * DIV равно 1. Если рассчитанное значение DIV не использует старшую часть, то регистр
	 * PRH  должен  быть  установлен  в  0.  Настройка  DIV  должна  выполняться  только  при
	 * выключенном контроллере I2C (EN_I2C = 0)
	 *
	 * Контроллер интерфейса I2C позволяет работать на типовых скоростях:
	 *    ─  нормальная:    100 кбит/с;
	 *    ─  быстрая:       400 кбит/с;
	 *    ─  очень быстрая: 1 Мбит/с
	*/
	if(freq <= 100000UL)
	{
		div = RST_CLK_Clocks.CPU_CLK_Frequency / 500000UL;
	}
	else if(freq > 100000UL && freq <= 400000UL)
	{
		div = RST_CLK_Clocks.CPU_CLK_Frequency / 2000000UL;
	}
	else
	{
		// 1 Мбит/с
		div = RST_CLK_Clocks.CPU_CLK_Frequency / 5000000UL;
	}

	div = (div > 1) ? div - 1 : 1;

	I2Cx->CTR &= ~I2C_CTR_EN_I2C;
	I2Cx->PRH = (div >> 8) & 0xFFul;
	I2Cx->PRL = div & 0xFFul;
	I2Cx->CTR |= enableState;
}

//------------------------------------------------------------------------------
// Начало транзакции
//------------------------------------------------------------------------------
void milandr_i2c_start_transaction(tI2cVariant i2cN, uint8_t address)
{
	if(i2cN >= I2C_COUNT) return;

	buffer_reset(i2cN);

	i2cTable[i2cN].destAddr = (address & 0x7F) << 1;
	i2cTable[i2cN].transmitting = true;
}

//------------------------------------------------------------------------------
// Запись данных
//------------------------------------------------------------------------------
static tMilandrI2cStatus milandr_i2c_master_transmit(tI2cVariant n, bool stopBit)
{
	if(n >= I2C_COUNT) return MILANDR_I2C_ERROR;

	bool status;
	volatile MDR_I2C_TypeDef * I2Cx = i2cTable[n].regs;

	/* Checking if the I2C bus is free */
	if((I2Cx->STA & I2C_STA_BUSY)) return MILANDR_I2C_BUSY;

	/* Generate a START condition and send address */
	send_address(n, TWI_WRITE);

	/* Wait end of transfer */
	milandr_i2c_wait_or_stop(n, I2Cx->STA & I2C_STA_TR_PROG, MILANDR_I2C_TIMEOUT);

	/* Received a NACK */
	if(I2Cx->STA & I2C_STA_RX_ACK)
	{
		send_stop(n);
		return MILANDR_I2C_NACK_ADDR;
	}

	while(!txb_empty(n))
	{
		/* Extract byte from ring buffer */
		uint8_t c = txb_buf(n)[txb_tail(n)];
		txb_tail(n) = (txb_tail(n) + 1) % txb_size(n);
		send_byte(n, c);

		/* Wait end of transfer */
		milandr_i2c_wait_or_stop(n, I2Cx->STA & I2C_STA_TR_PROG, MILANDR_I2C_TIMEOUT);

		/* Received a NACK */
		if(I2Cx->STA & I2C_STA_RX_ACK)
		{
			send_stop(n);
			return MILANDR_I2C_NACK_DATA;
		}
	}

	if(stopBit) send_stop(n);

	return MILANDR_I2C_OK;
}

//------------------------------------------------------------------------------
// Конец транзакции. Выгрузка данных из буфера и отправка по I2C
//------------------------------------------------------------------------------
tMilandrI2cStatus milandr_i2c_end_transaction(tI2cVariant i2cN, bool stopBit)
{
	if(i2cN >= I2C_COUNT || !i2cTable[i2cN].transmitting) return MILANDR_I2C_ERROR;

	tMilandrI2cStatus status = milandr_i2c_master_transmit(i2cN, stopBit);
	i2cTable[i2cN].transmitting = false;
	return status;
}

//------------------------------------------------------------------------------
// Добавление байта в передающий буфер
//------------------------------------------------------------------------------
size_t milandr_i2c_write_byte(tI2cVariant n, uint8_t data)
{
	if(n == I2C_UNKNOWN) return 0;

	// Буфер полностью заполнен. Сообщаем вызвавшему уровню, что не отправили
	// ни одного байта
	if(txb_full(n))
	{
		return 0;
	}
	// Помещаем один символ в буфер
	else
	{
		uint8_t next_head = (txb_head(n) + 1) % txb_size(n);
		txb_buf(n)[txb_head(n)] = data;
		txb_head(n) = next_head;
	}

	return 1;
}

//------------------------------------------------------------------------------
// Чтение данных
//------------------------------------------------------------------------------
tMilandrI2cStatus milandr_i2c_master_receive(tI2cVariant n, size_t len, bool stopBit)
{
	if(n >= I2C_COUNT) return MILANDR_I2C_ERROR;

	bool status;
	volatile MDR_I2C_TypeDef * I2Cx = i2cTable[n].regs;

	/* Reset ring buffer */
	buffer_reset(n);

	/* Checking if the I2C bus is free */
	if((I2Cx->STA & I2C_STA_BUSY)) return MILANDR_I2C_BUSY;

	/* Generate a START condition and send address */
	send_address(n, TWI_READ);

	/* Wait end of transfer */
	milandr_i2c_wait_or_stop(n, I2Cx->STA & I2C_STA_TR_PROG, MILANDR_I2C_TIMEOUT);

	/* Received a NACK */
	if(I2Cx->STA & I2C_STA_RX_ACK)
	{
		send_stop(n);
		return MILANDR_I2C_NACK_ADDR;
	}

	while(len > 0)
	{
		// В последнем байте отправляем NACK
		if(len > 1)
			request_byte(n, false);
		else
			request_byte(n, true);

		/* Wait end of transfer */
		milandr_i2c_wait_or_stop(n, I2Cx->STA & I2C_STA_TR_PROG, MILANDR_I2C_TIMEOUT);

		/* Get data from I2C RXD register */
		uint8_t byte;
		read_byte(n, &byte);

		// Чтение в буфер
		uint32_t next_head = (rxb_head(n) + 1) % rxb_size(n);

		if(next_head == rxb_tail(n))
		{
			// Буфер полон — сдвигаем tail (теряем старый байт)
			rxb_tail(n) = (rxb_tail(n) + 1) % rxb_size(n);
		}

		rxb_buf(n)[rxb_head(n)] = byte;
		rxb_head(n) = next_head;

		len--;
	}

	if(stopBit) send_stop(n);

	return MILANDR_I2C_OK;
}

//------------------------------------------------------------------------------
// Возвращает количество принятых данных
//------------------------------------------------------------------------------
int milandr_i2c_available(tI2cVariant n)
{
	if(n >= I2C_COUNT) return 0;

	return (rxb_size(n) + rxb_head(n) - rxb_tail(n)) % rxb_size(n);
}

//------------------------------------------------------------------------------
// Чтение байта из буфера без удаления
//------------------------------------------------------------------------------
int milandr_i2c_peak(tI2cVariant n)
{
	if(n >= I2C_COUNT) return -1;

	uint8_t ch;
	ch = rxb_buf(n)[rxb_tail(n)];
	return (int)ch;
}

//------------------------------------------------------------------------------
// Чтение байта из буфера с удалением
//------------------------------------------------------------------------------
int milandr_i2c_read_byte(tI2cVariant n)
{
	if(n >= I2C_COUNT) return -1;

	int ch = -1;

	if(rxb_tail(n) != rxb_head(n))
	{
		ch = rxb_buf(n)[rxb_tail(n)];
		rxb_tail(n) = (rxb_tail(n) + 1) % rxb_size(n);
	}

	return (int)ch;
}
