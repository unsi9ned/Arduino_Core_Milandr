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
#include "periph_definition.h"
#include "variant.h"
#include "MDR32FxQI_config.h"
#include "MDR32FxQI_rst_clk.h"

//------------------------------------------------------------------------------
// Константы
//------------------------------------------------------------------------------
#define DAC_DEFAULT_RESOLUTION     8
#define DAC_MAX_RESOLUTION         12
#define DAC_MAX_VALUE              4095

//------------------------------------------------------------------------------
// Полная таблица пинов с их функциональным назначением
//------------------------------------------------------------------------------
extern const tMilandrPin   pinTable[][PIN_MUX_LINES_NUM][1];

//------------------------------------------------------------------------------
// Описание пинов
//------------------------------------------------------------------------------
#define DAC_OUTPUT_LINE      (DAC_OUT_LINE - DAC_OUT_LINE)
#define DAC_REFERENCE_LINE   (DAC_REF_LINE - DAC_OUT_LINE)
#define DAC_GET_LINE(n)      ((n < DAC_OUT_LINE) ? PERIPH_UNKNOWN_LINE : (n - DAC_OUT_LINE))
#define DAC_LINE_CNT         (DAC_REFERENCE_LINE - DAC_OUTPUT_LINE + 1)


#ifndef DAC1_REF
#define DAC1_REF             PIN_NC
#endif

#ifndef DAC1_OUT
#define DAC1_OUT             PIN_NC
#endif

#ifndef DAC2_REF
#define DAC2_REF             PIN_NC
#endif

#ifndef DAC2_OUT
#define DAC2_OUT             PIN_NC
#endif

//------------------------------------------------------------------------------
// Дескриптор периферии
//------------------------------------------------------------------------------
typedef struct
{
	uint8_t                    pins[DAC_LINE_CNT];
	uint32_t                   clkMask;
	uint8_t                    selRefBitPos;
	uint8_t                    enableBitPos;
	volatile MDR_DAC_TypeDef * regs;
	volatile uint32_t *        dataReg;
	bool                       isInit;
}
tPeriphDescriptor;

//------------------------------------------------------------------------------
// Таблица всех I2C-модулей
//------------------------------------------------------------------------------
static tPeriphDescriptor dacTable[DAC_COUNT] =
{
	[DAC_1] =
	{
		.pins =
		{
			[DAC_OUTPUT_LINE] = DAC1_OUT,
			[DAC_REFERENCE_LINE] = DAC1_REF,
		},
		.clkMask = RST_CLK_PCLK_DAC,
		.selRefBitPos = 0,
		.enableBitPos = 2,
		.regs = MDR_DAC,
		.dataReg = &MDR_DAC->DAC1_DATA,
	},

	[DAC_2] =
	{
		.pins =
		{
			[DAC_OUTPUT_LINE] = DAC2_OUT,
			[DAC_REFERENCE_LINE] = DAC2_REF,
		},
		.clkMask = RST_CLK_PCLK_DAC,
		.selRefBitPos = 1,
		.enableBitPos = 3,
		.regs = MDR_DAC,
		.dataReg = &MDR_DAC->DAC2_DATA,
	},
};

//------------------------------------------------------------------------------
// Статические переменные
//------------------------------------------------------------------------------
static uint8_t dacResolution = DAC_DEFAULT_RESOLUTION;

//------------------------------------------------------------------------------
// Сброс флагов инициализации
//------------------------------------------------------------------------------
void milandr_dac_preinit(void)
{
	for(int i = 0; i < DAC_COUNT; i++)
	{
		dacTable[i].isInit = false;
	}
}

//------------------------------------------------------------------------------
// Инициализация DAC
//------------------------------------------------------------------------------
static tDacVariant dac_init(uint8_t pin)
{
	const tMilandrPin * out = &pinTable[pin % DMAX][PIN_MUX_ANALOG][0];

	if(pin >= DMAX || out->periph != PERIPH_DAC || out->periphLine != DAC_OUT_LINE)
	{
		return DAC_UNKNOWN;
	}

	tDacVariant dacN = out->periphN % DAC_COUNT;

	if(dacTable[dacN].isInit) return dacN;

	//
	// Производим настройку GPIO
	//
	milandr_gpio_clock_enable(pin, true);
	milandr_gpio_cfg_output_analog(pin);

	//
	// Настройка DAC
	//
	MDR_RST_CLK->PER_CLOCK |= dacTable[dacN].clkMask;

	// DAC1 и DAC2 работают независимо друг от друга
	dacTable[dacN].regs->CFG &= ~DAC_CFG_SYNC_A;

	// В качестве опорного напряжения используется напряжение питания с вывода AUCC
	dacTable[dacN].regs->CFG &= ~(1UL << dacTable[dacN].selRefBitPos);

	// Включение DAC
	dacTable[dacN].regs->CFG |= (1UL << dacTable[dacN].enableBitPos);

	dacTable[dacN].isInit = true;

	return dacN;
}

//------------------------------------------------------------------------------
// Деинициализация выхода ЦАП
//------------------------------------------------------------------------------
void milandr_dac_deinit(uint8_t pin)
{
	const tMilandrPin * out = &pinTable[pin % DMAX][PIN_MUX_ANALOG][0];

	if(pin < DMAX && out->periph == PERIPH_DAC && out->periphLine == DAC_OUT_LINE)
	{
		tDacVariant dacN = out->periphN % DAC_COUNT;

		if(dacTable[dacN].isInit)
		{
			milandr_gpio_cfg_input(pin);
			dacTable[dacN].regs->CFG &= ~(1UL << dacTable[dacN].enableBitPos);
			dacTable[dacN].isInit = false;
		}
	}
}

//------------------------------------------------------------------------------
// Изменить разрядность ЦАП (виртуальную)
//------------------------------------------------------------------------------
void milandr_dac_set_resolution(uint8_t resolution)
{
	if(resolution > DAC_MAX_RESOLUTION)
		resolution = DAC_MAX_RESOLUTION;
	else if(resolution < DAC_DEFAULT_RESOLUTION)
		resolution = DAC_DEFAULT_RESOLUTION;

	dacResolution = resolution;
}

//------------------------------------------------------------------------------
// Масштабирование значения duty cycle в соответствии с 12-битным разрешением
//------------------------------------------------------------------------------
static uint16_t scale_resolution(int value)
{
	uint16_t scale = ((uint16_t)1 << dacResolution);

	// Валидация значения
	value &= (scale - 1);
	return ((uint16_t)value * (DAC_MAX_VALUE + 1)) / scale;
}

//------------------------------------------------------------------------------
// Установить значение DAC
//------------------------------------------------------------------------------
void milandr_dac_set_value(uint8_t pin, int value)
{
	tDacVariant dacN = dac_init(pin);

	if(dacN == DAC_UNKNOWN)
	{
		return;
	}
	else
	{
		*dacTable[dacN].dataReg = scale_resolution(value);
	}
}
