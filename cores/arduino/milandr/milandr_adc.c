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
#include "MDR32FxQI_adc.h"

//------------------------------------------------------------------------------
// Константы
//------------------------------------------------------------------------------
#define ADC_DEFAULT_RESOLUTION     8
#define ADC_MAX_RESOLUTION         12
#define ADC_MAX_VALUE              4095

//------------------------------------------------------------------------------
// Полная таблица пинов с их функциональным назначением
//------------------------------------------------------------------------------
extern const tMilandrPin   pinTable[][PIN_MUX_LINES_NUM][1];

//------------------------------------------------------------------------------
// Описание пинов
//------------------------------------------------------------------------------
#define ADC_GET_CH(n) (((n) < ADC_CH0_LINE) ? PERIPH_UNKNOWN_LINE : ((n) - ADC_CH0_LINE))

#define ADC_NULL              0x3
#define ADC_CH_NULL           0x3F
#define NULL_CHANNEL          0xFF

#define TEMP_SENSOR_PIN       DMAX
#define TEMP_SENSOR_CHAN      ADC_CH31_LINE

//------------------------------------------------------------------------------
// Канал АЦП
//------------------------------------------------------------------------------
typedef union
{
	struct
	{
		uint8_t adc     :2;
		uint8_t channel :6;
	};

	uint8_t raw;
}
tAdcInput;

//------------------------------------------------------------------------------
// Карта используемых каналов и таймеров
//------------------------------------------------------------------------------
static tAdcInput channelMap[DMAX + 1];
static uint32_t  initChannelMask[ADC_COUNT];

#define IS_CHAN_INIT(adc, ch) \
        (initChannelMask[adc % ADC_COUNT] & (1UL << (ch & 0x1F)))

#define IS_ADC_INIT(adc) \
        (initChannelMask[adc % ADC_COUNT] & 0xFFFFFFFFul)

#define SET_CHAN_INIT(adc, ch) \
        initChannelMask[adc % ADC_COUNT] |= (1UL << (ch & 0x1F))

#define RST_CHAN_INIT(adc, ch) \
        initChannelMask[adc % ADC_COUNT] &= ~(1UL << (ch & 0x1F))

//------------------------------------------------------------------------------
// Дескриптор периферии
//------------------------------------------------------------------------------
typedef struct
{
	uint32_t  clkMask;
	IRQn_Type irqNum;
	volatile MDR_ADC_TypeDef * regs;
	volatile uint32_t * ADC_H_LEVEL;
	volatile uint32_t * ADC_L_LEVEL;
	volatile uint32_t * ADC_RESULT;
	volatile uint32_t * ADC_STATUS;
	volatile uint32_t * ADC_CHSEL;
}
tPeriphDescriptor;

//------------------------------------------------------------------------------
// Таблица всех ADC-модулей
//------------------------------------------------------------------------------
static tPeriphDescriptor adcTable[ADC_COUNT] =
{
	[ADC_1] =
	{
		.clkMask = RST_CLK_PCLK_ADC,
		.irqNum = ADC_IRQn,
		.regs = MDR_ADC,
		.ADC_H_LEVEL = &MDR_ADC->ADC1_H_LEVEL,
		.ADC_L_LEVEL = &MDR_ADC->ADC1_L_LEVEL,
		.ADC_RESULT = &MDR_ADC->ADC1_RESULT,
		.ADC_STATUS = &MDR_ADC->ADC1_STATUS,
		.ADC_CHSEL = &MDR_ADC->ADC1_CHSEL,
	},

	[ADC_2] =
	{
		.clkMask = RST_CLK_PCLK_ADC,
		.irqNum = ADC_IRQn,
		.regs = MDR_ADC,
		.ADC_H_LEVEL = &MDR_ADC->ADC2_H_LEVEL,
		.ADC_L_LEVEL = &MDR_ADC->ADC2_L_LEVEL,
		.ADC_RESULT = &MDR_ADC->ADC2_RESULT,
		.ADC_STATUS = &MDR_ADC->ADC2_STATUS,
		.ADC_CHSEL = &MDR_ADC->ADC2_CHSEL,
	},
};

//------------------------------------------------------------------------------
// Статические переменные
//------------------------------------------------------------------------------
static ADC_InitTypeDef sADC;
static ADCx_InitTypeDef sADCx;
static uint8_t adcResolution = ADC_DEFAULT_RESOLUTION;

//------------------------------------------------------------------------------
// Сброс флагов инициализации
//------------------------------------------------------------------------------
void milandr_adc_preinit(void)
{
	for(int i = 0; i < DMAX; i++)
	{
		if(pinTable[i][PIN_MUX_ANALOG][0].periph == PERIPH_ADC)
		{
			channelMap[i].adc = pinTable[i][PIN_MUX_ANALOG][0].periphN;
			channelMap[i].channel = ADC_GET_CH(pinTable[i][PIN_MUX_ANALOG][0].periphLine);
		}
		else
			channelMap[i].raw = NULL_CHANNEL;
	}

	channelMap[TEMP_SENSOR_PIN].adc = ADC_1;
	channelMap[TEMP_SENSOR_PIN].channel = ADC_GET_CH(TEMP_SENSOR_CHAN);

	for(int i = 0; i < ADC_COUNT; i++)
	{
		initChannelMask[i] = 0;
	}
}

//------------------------------------------------------------------------------
// Инициализация входа АЦП
//------------------------------------------------------------------------------
static bool init_adc_input(uint8_t pin)
{
	if(channelMap[pin].raw == NULL_CHANNEL) return false;

	tAdcInput input = channelMap[pin];

	if(IS_CHAN_INIT(input.adc, input.channel))
	{
		return true;
	}
	else
	{
		//
		// Производим настройку GPIO. Канал AIN_TEMP является виртуальным
		//
		if(pin < TEMP_SENSOR_PIN)
		{
			milandr_gpio_clock_enable(pin, true);
			milandr_gpio_cfg_input_analog(pin);
		}

		//
		// Производим настройку ADC
		//
		if(!IS_ADC_INIT(input.adc))
		{
			MDR_RST_CLK->PER_CLOCK |= adcTable[input.adc].clkMask;

			/* ADC Configuration */
			/* Reset all ADC settings */
			ADC_DeInit();

			ADC_StructInit(&sADC);
			sADC.ADC_SynchronousMode      = ADC_SyncMode_Independent;
			sADC.ADC_StartDelay           = 0;
			sADC.ADC_TempSensor           = ADC_TEMP_SENSOR_Enable;
			sADC.ADC_TempSensorAmplifier  = ADC_TEMP_SENSOR_AMPLIFIER_Enable;
			sADC.ADC_TempSensorConversion = ADC_TEMP_SENSOR_CONVERSION_Enable;
			sADC.ADC_IntVRefConversion    = ADC_VREF_CONVERSION_Disable;
			sADC.ADC_IntVRefTrimming      = 1;
			ADC_Init(&sADC);

			ADCx_StructInit(&sADCx);
			sADCx.ADC_ClockSource      = ADC_CLOCK_SOURCE_CPU;
			sADCx.ADC_SamplingMode     = ADC_SAMPLING_MODE_SINGLE_CONV;
			sADCx.ADC_ChannelSwitching = ADC_CH_SWITCHING_Disable;
			sADCx.ADC_ChannelNumber    = ADC_CH_TEMP_SENSOR;
			sADCx.ADC_Channels         = 0;
			sADCx.ADC_LevelControl     = ADC_LEVEL_CONTROL_Disable;
			sADCx.ADC_LowLevel         = 0;
			sADCx.ADC_HighLevel        = 0;
			sADCx.ADC_VRefSource       = ADC_VREF_SOURCE_INTERNAL;
			sADCx.ADC_IntVRefSource    = ADC_INT_VREF_SOURCE_INEXACT;
			sADCx.ADC_Prescaler        = ADC_CLK_div_2048;
			sADCx.ADC_DelayGo          = 0xF;
			ADC1_Init(&sADCx);

			/* ADC2 Configuration */
			ADCx_StructInit(&sADCx);
			sADCx.ADC_ClockSource      = ADC_CLOCK_SOURCE_CPU;
			sADCx.ADC_SamplingMode     = ADC_SAMPLING_MODE_SINGLE_CONV;
			sADCx.ADC_ChannelSwitching = ADC_CH_SWITCHING_Disable;
			sADCx.ADC_ChannelNumber    = ADC_CH_TEMP_SENSOR;
			sADCx.ADC_LevelControl     = ADC_LEVEL_CONTROL_Disable;
			sADCx.ADC_LowLevel         = 0;
			sADCx.ADC_HighLevel        = 0;
			sADCx.ADC_VRefSource       = ADC_VREF_SOURCE_INTERNAL;
			sADCx.ADC_IntVRefSource    = ADC_INT_VREF_SOURCE_INEXACT;
			sADCx.ADC_Prescaler        = ADC_CLK_div_256;
			sADCx.ADC_DelayGo          = 3;
			ADC2_Init(&sADCx);

			/* ADC1 enable */
			ADC1_Cmd(ENABLE);

			/* ADC2 enable */
			ADC2_Cmd(ENABLE);

			/* Канал 31 используется датчиков темепературы */
			SET_CHAN_INIT(ADC_1, ADC_GET_CH(TEMP_SENSOR_CHAN));
			SET_CHAN_INIT(ADC_2, ADC_GET_CH(TEMP_SENSOR_CHAN));
		}

		SET_CHAN_INIT(ADC_1, input.channel);
		SET_CHAN_INIT(ADC_2, input.channel);
		return true;
	}

	return false;
}

//------------------------------------------------------------------------------
// Деинициализация входа АЦП
//------------------------------------------------------------------------------
void milandr_adc_deinit(uint8_t pin)
{
	tAdcInput in = channelMap[pin % DMAX];
	if(in.raw == NULL_CHANNEL || !IS_CHAN_INIT(in.adc, in.channel)) return;

	// Перевод пина в режим входа
	milandr_gpio_cfg_input(pin);

	// Сброс флага инициализации канала
	RST_CHAN_INIT(ADC_1, in.channel);
	RST_CHAN_INIT(ADC_2, in.channel);
}

//------------------------------------------------------------------------------
// Изменить разрядность ЦАП (виртуальную)
//------------------------------------------------------------------------------
void milandr_adc_set_resolution(uint8_t resolution)
{
	if(resolution > ADC_MAX_RESOLUTION)
		resolution = ADC_MAX_RESOLUTION;
	else if(resolution < ADC_DEFAULT_RESOLUTION)
		resolution = ADC_DEFAULT_RESOLUTION;

	adcResolution = resolution;
}

//------------------------------------------------------------------------------
// Масштабирование значения duty cycle в соответствии с 12-битным разрешением
//------------------------------------------------------------------------------
static uint16_t scale_resolution(uint16_t value)
{
	return (value >> (ADC_MAX_RESOLUTION - adcResolution));
}

//------------------------------------------------------------------------------
// Оцифровка напряжения на входе
//------------------------------------------------------------------------------
uint16_t milandr_adc_read_value(uint8_t pin)
{
	if(!init_adc_input(pin % (DMAX + 1)))
	{
		return 0;
	}
	else
	{
		uint16_t result = 0;
		ADCx_Channel_Number ch = ADC_GET_CH(TEMP_SENSOR_CHAN);

		if(pin < TEMP_SENSOR_PIN)
		{
			const tMilandrPin * ain = &pinTable[pin % DMAX][PIN_MUX_ANALOG][0];
			ch = ADC_GET_CH(ain->periphLine & 0x1F);

			ADC2_SetChannel(ch);
			ADC2_Start();

			// Ждем окончания преобразований
			while(MDR_ADC->ADC2_CFG & ADC2_CFG_REG_GO);

			result = scale_resolution((uint16_t)(ADC2_GetResult() & 0xFFF));
		}
		else
		{
			ADC1_SetChannel(ch);
			ADC1_Start();

			// Ждем окончания преобразований
			while(MDR_ADC->ADC1_CFG & ADC1_CFG_REG_GO);

			result = scale_resolution((uint16_t)(ADC1_GetResult() & 0xFFF));
		}

		return result;
	}
}
