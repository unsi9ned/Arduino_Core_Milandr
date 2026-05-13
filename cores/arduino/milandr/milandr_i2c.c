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
#include "MDR32FxQI_i2c.h"

//------------------------------------------------------------------------------
// Стандартная скорость I2C
//------------------------------------------------------------------------------
#define TWI_STANDARD_FREQ  100000UL

//------------------------------------------------------------------------------
// Таймаут операций на шине I2C
//------------------------------------------------------------------------------
#define TWI_TIMEOUT        0xFFFFUL

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

//------------------------------------------------------------------------------
// Ожидание окончания события на шине I2C
//------------------------------------------------------------------------------
#define milandr_i2c_while(cond, status) \
		{\
			volatile uint32_t timeout = TWI_TIMEOUT;\
			while((cond) && timeout)\
			{\
				timeout--;\
			}\
			status = (!(cond)) ? true : false;\
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
	I2C_DeInit();

	/* Configure I2C speed*/
	milandr_i2c_set_freq(i2cN, TWI_STANDARD_FREQ);

	/* Enable I2C */
	I2C_Cmd(ENABLE);

	return i2cN;
}

//------------------------------------------------------------------------------
// Принудительное завершение транзакции
//------------------------------------------------------------------------------
static inline void milandr_i2c_abort(MDR_I2C_TypeDef * I2Cx)
{
	bool status;

	if((I2Cx->STA & I2C_STA_BUSY) && (I2Cx->CTR & I2C_CTR_EN_I2C))
	{
		// Послать Stop
		I2Cx->CMD = I2C_CMD_STOP;

		// Дождать освобождения шины
		milandr_i2c_while(I2Cx->STA & I2C_STA_BUSY, status);
	}
}

//------------------------------------------------------------------------------
// Деинициализация I2C
//------------------------------------------------------------------------------
void milandr_i2c_deinit(uint8_t sdaPin, uint8_t sclPin, tI2cVariant i2cN)
{
	if(i2cN >= I2C_COUNT) return;

	volatile MDR_I2C_TypeDef * I2Cx = i2cTable[i2cN].regs;

	milandr_i2c_abort((MDR_I2C_TypeDef*)I2Cx);
	I2C_DeInit();

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

	i2cTable[i2cN].rxBuffer.buf_head = 0;
	i2cTable[i2cN].rxBuffer.buf_tail = 0;

	i2cTable[i2cN].txBuffer.buf_head = 0;
	i2cTable[i2cN].txBuffer.buf_tail = 0;

	i2cTable[i2cN].destAddr = address;
	i2cTable[i2cN].transmitting = true;
}

//------------------------------------------------------------------------------
// Запись данных
//------------------------------------------------------------------------------
static tMilandrI2cStatus milandr_i2c_master_transmit(tI2cVariant i2cN, bool stopBit)
{
	return MILANDR_I2C_ERROR;
}

//------------------------------------------------------------------------------
// Конец транзакции. Выгрузка данных из буфера и отправка по I2C
//------------------------------------------------------------------------------
tMilandrI2cStatus milandr_i2c_end_transaction(tI2cVariant i2cN, bool stopBit)
{
	tMilandrI2cStatus status = milandr_i2c_master_transmit(i2cN, stopBit);
	i2cTable[i2cN].transmitting = false;
	return status;
}
