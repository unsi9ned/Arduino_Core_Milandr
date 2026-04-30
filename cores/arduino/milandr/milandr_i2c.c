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
// Полная таблица пинов с их функциональным назначением
//------------------------------------------------------------------------------
extern const tMilandrPin pinTable[][PIN_MUX_LINES_NUM][1];

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


typedef struct
{
	uint8_t   pins[TWI_LINE_CNT];
	uint32_t  clkMask;
	IRQn_Type irqNum;
	volatile MDR_I2C_TypeDef * regs;
}
tPeriphDescriptor;

//------------------------------------------------------------------------------
// Таблица всех I2C-модулей
//------------------------------------------------------------------------------
static const tPeriphDescriptor i2cTable[I2C_COUNT] =
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
		.regs = MDR_I2C
	}
};

//------------------------------------------------------------------------------
// Статические переменные
//------------------------------------------------------------------------------
I2C_InitTypeDef I2C_InitStruct;

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

	for(tI2cVariant i2cIdx = I2C_1; i2cIdx < UART_COUNT; i2cIdx++)
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
	if(i2cN == UART_UNKNOWN) return i2cN;

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

	I2C_DeInit();

	/* Initialize I2C_InitStruct */
	I2C_StructInit(&I2C_InitStruct);
	I2C_InitStruct.I2C_ClkDiv = 16;
	I2C_InitStruct.I2C_Speed  = I2C_SPEED_UP_TO_400KHz;

	/* Configure I2C parameters */
	I2C_Init(&I2C_InitStruct);

	return i2cN;
}

//------------------------------------------------------------------------------
// Деинициализация I2C
//------------------------------------------------------------------------------
tI2cVariant milandr_i2c_deinit(uint8_t sdaPin, uint8_t sclPin, tI2cVariant i2cN)
{
	if(i2cN >= I2C_COUNT) return I2C_UNKNOWN;

	volatile MDR_I2C_TypeDef * I2Cx = i2cTable[i2cN].regs;

	I2C_Cmd(DISABLE);
	I2C_DeInit();
	MDR_RST_CLK->PER_CLOCK &= ~i2cTable[i2cN].clkMask;

	milandr_gpio_sel_port_func(sdaPin);
	milandr_gpio_sel_port_func(sclPin);
	milandr_gpio_cfg_input(sdaPin);
	milandr_gpio_cfg_input(sclPin);
}
