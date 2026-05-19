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
#include "MDR32FxQI_timer.h"

//------------------------------------------------------------------------------
// Константы
//------------------------------------------------------------------------------
#define TIMER_DEFAULT_RESOLUTION     8
#define TIMER_MAX_RESOLUTION         12
#define TIMER_PWD_FREQ_HZ            19531
#define TIMER_PERIOD                 4096

//------------------------------------------------------------------------------
// Полная таблица пинов с их функциональным назначением
//------------------------------------------------------------------------------
extern const tMilandrPin pinTable[][PIN_MUX_LINES_NUM][1];

//------------------------------------------------------------------------------
// Таблица пинов TMR по умолчанию
//------------------------------------------------------------------------------
#define TIMER_CH1_LINE    (TMR_CH1_LINE - TMR_CH1_LINE)
#define TIMER_CH2_LINE    (TMR_CH2_LINE - TMR_CH1_LINE)
#define TIMER_CH3_LINE    (TMR_CH3_LINE - TMR_CH1_LINE)
#define TIMER_CH4_LINE    (TMR_CH4_LINE - TMR_CH1_LINE)

#define TIMER_CH1N_LINE   (TMR_CH1_N_LINE - TMR_CH1_LINE)
#define TIMER_CH2N_LINE   (TMR_CH2_N_LINE - TMR_CH1_LINE)
#define TIMER_CH3N_LINE   (TMR_CH3_N_LINE - TMR_CH1_LINE)
#define TIMER_CH4N_LINE   (TMR_CH4_N_LINE - TMR_CH1_LINE)

#define TIMER_GET_CH(n)   ((n < TMR_CH1_LINE) ? PERIPH_UNKNOWN_LINE : (n - TMR_CH1_LINE))
#define TIMER_CH_CNT      ((TMR_CH4_N_LINE - TMR_CH1_LINE + 1) & 0xF)
#define TIMER_GET_N(n)    (n & 0x7)

#define TIMER_NULL            0x7
#define TIMER_CH_NULL         0xF
#define NULL_CHANNEL          0xFF
#define TIMER_PORT_MUX(mux)  ((mux) ? PIN_MUX_OVERRID : PIN_MUX_ALTER)

//------------------------------------------------------------------------------
// Карта используемых каналов и таймеров
//------------------------------------------------------------------------------
static tTimerOut channelMap[DMAX];
static uint32_t  initChannelMask;

#define IS_CHAN_INIT(tmr, ch) \
        (initChannelMask & ((1UL << (ch & 0x7)) << (tmr * 8)))

#define IS_TIMER_INIT(tmr) \
        (initChannelMask & (0xFF << (tmr * 8)))

#define SET_CHAN_INIT(tmr, ch) \
		initChannelMask |= ((1UL << (ch & 0x7)) << (tmr * 8))

//------------------------------------------------------------------------------
// Дескриптор периферии
//------------------------------------------------------------------------------
typedef struct
{
	uint32_t  clkMask;
	IRQn_Type irqNum;
	volatile MDR_TIMER_TypeDef * regs;
}
tPeriphDescriptor;

//------------------------------------------------------------------------------
// Таблица всех I2C-модулей
//------------------------------------------------------------------------------
static tPeriphDescriptor tmrTable[TIMER_COUNT] =
{
	[TIMER_1] =
	{
		.clkMask = RST_CLK_PCLK_TIMER1,
		.irqNum = Timer1_IRQn,
		.regs = MDR_TIMER1,
	},

	[TIMER_2] =
	{
		.clkMask = RST_CLK_PCLK_TIMER2,
		.irqNum = Timer2_IRQn,
		.regs = MDR_TIMER2,
	},

	[TIMER_3] =
	{
		.clkMask = RST_CLK_PCLK_TIMER3,
		.irqNum = Timer3_IRQn,
		.regs = MDR_TIMER3,
	},
};

//------------------------------------------------------------------------------
// Статические переменные
//------------------------------------------------------------------------------
static TIMER_CntInitTypeDef sTIM_CntInit;
static TIMER_ChnInitTypeDef sTIM_ChnInit;
static TIMER_ChnOutInitTypeDef sTIM_ChnOutInit;
static uint8_t tmrResolution = TIMER_DEFAULT_RESOLUTION;

//------------------------------------------------------------------------------
// Обнуление карты каналов
//------------------------------------------------------------------------------
void milandr_timer_preinit(void)
{
	for(int i = 0; i < DMAX; i++)
	{
		channelMap[i].raw = NULL_CHANNEL;

		for(int m = PIN_MUX_ALTER; m <= PIN_MUX_OVERRID; m++)
		{
			if(pinTable[i][m][0].periph == PERIPH_PWM)
			{
				channelMap[i].timer = pinTable[i][m][0].periphN;
				channelMap[i].channel = TIMER_GET_CH(pinTable[i][m][0].periphLine);
				channelMap[i].mux = (m == PIN_MUX_OVERRID) ? 1 : 0;
				break;
			}
		}
	}

	initChannelMask = 0;
}

//------------------------------------------------------------------------------
// Инициализация выхода таймера
//------------------------------------------------------------------------------
static bool init_pwm_out(uint8_t pin)
{
	if(channelMap[pin].raw == NULL_CHANNEL) return false;

	tTimerOut out = channelMap[pin];

	if(IS_CHAN_INIT(out.timer, out.channel))
	{
		return true;
	}
	else
	{
		//
		// Производим настройку GPIO
		//
		uint8_t portFunc = TIMER_PORT_MUX(out.mux);

		milandr_gpio_clock_enable(pin, true);
		milandr_gpio_cfg_output_pp(pin);

		if(portFunc == PIN_MUX_ALTER)
			milandr_gpio_sel_alter_func(pin);
		else
			milandr_gpio_sel_override_func(pin);

		//
		// Производим настройку Timer
		//
		volatile MDR_TIMER_TypeDef * TIMERx = tmrTable[out.timer].regs;

		if(!IS_TIMER_INIT(out.timer))
		{
			MDR_RST_CLK->PER_CLOCK |= tmrTable[out.timer].clkMask;

			/* Reset all TIMER1 settings */
			TIMER_DeInit((MDR_TIMER_TypeDef*)TIMERx);

			/* Initializes the TIMERx Counter                               */
			/* Входная частота 80МГц, полный период счета 4096 (12 бит)     */
			sTIM_CntInit.TIMER_Prescaler        = 0x0;
			sTIM_CntInit.TIMER_Period           = TIMER_PERIOD - 1;
			sTIM_CntInit.TIMER_CounterMode      = TIMER_CntMode_ClkFixedDir;
			sTIM_CntInit.TIMER_CounterDirection = TIMER_CntDir_Up;
			sTIM_CntInit.TIMER_EventSource      = TIMER_EvSrc_TIM_CLK;
			sTIM_CntInit.TIMER_FilterSampling   = TIMER_FDTS_TIMER_CLK_div_1;
			sTIM_CntInit.TIMER_ARR_UpdateMode   = TIMER_ARR_Update_Immediately;
			sTIM_CntInit.TIMER_ETR_FilterConf   = TIMER_Filter_1FF_at_TIMER_CLK;
			sTIM_CntInit.TIMER_ETR_Prescaler    = TIMER_ETR_Prescaler_None;
			sTIM_CntInit.TIMER_ETR_Polarity     = TIMER_ETRPolarity_NonInverted;
			sTIM_CntInit.TIMER_BRK_Polarity     = TIMER_BRKPolarity_NonInverted;
			TIMER_CntInit((MDR_TIMER_TypeDef*)TIMERx, &sTIM_CntInit);

			sTIM_ChnInit.TIMER_CH_Mode = TIMER_CH_MODE_PWM;
			sTIM_ChnInit.TIMER_CH_REF_Format = TIMER_CH_REF_Format6;
			sTIM_ChnInit.TIMER_CH_Number = out.channel % 4;
			TIMER_ChnInit((MDR_TIMER_TypeDef*)TIMERx, &sTIM_ChnInit);

			// Duty Cycle = 0 %
			TIMER_SetChnCompare((MDR_TIMER_TypeDef*)TIMERx, out.channel % 4, 0);

			/* Initializes the TIMER1 Channel Output */
			TIMER_ChnOutStructInit(&sTIM_ChnOutInit);
			sTIM_ChnOutInit.TIMER_CH_DirOut_Polarity = TIMER_CHOPolarity_NonInverted;
			sTIM_ChnOutInit.TIMER_CH_DirOut_Source   = TIMER_CH_OutSrc_REF;
			sTIM_ChnOutInit.TIMER_CH_DirOut_Mode     = TIMER_CH_OutMode_Output;
			sTIM_ChnOutInit.TIMER_CH_NegOut_Polarity = (out.channel < 4) ?
													   TIMER_CHOPolarity_NonInverted :
													   TIMER_CHOPolarity_Inverted;
			sTIM_ChnOutInit.TIMER_CH_NegOut_Source   = TIMER_CH_OutSrc_REF;
			sTIM_ChnOutInit.TIMER_CH_NegOut_Mode     = TIMER_CH_OutMode_Output;
			sTIM_ChnOutInit.TIMER_CH_Number          = out.channel % 4;
			TIMER_ChnOutInit((MDR_TIMER_TypeDef*)TIMERx, &sTIM_ChnOutInit);

			/* Enable TIMER1 clock */
			TIMER_BRGInit((MDR_TIMER_TypeDef*)TIMERx, TIMER_HCLKdiv1);

			/* Enable TIMER1 */
			TIMER_Cmd((MDR_TIMER_TypeDef*)TIMERx, ENABLE);
		}

		SET_CHAN_INIT(out.timer, out.channel);
		return true;
	}

	return false;
}

//------------------------------------------------------------------------------
// Инициализация таймера в режиме ШИМ
//------------------------------------------------------------------------------
tTimerOut milandr_pmw_init(uint8_t pin)
{
	init_pwm_out(pin % DMAX);
	return channelMap[pin];
}

//------------------------------------------------------------------------------
// Изменить разрядность таймера (виртуальную)
//------------------------------------------------------------------------------
void milandr_pwm_set_resolution(uint8_t resolution)
{
	if(resolution > TIMER_MAX_RESOLUTION)
		resolution = TIMER_MAX_RESOLUTION;
	else if(resolution < TIMER_DEFAULT_RESOLUTION)
		resolution = TIMER_DEFAULT_RESOLUTION;

	tmrResolution = resolution;
}

//------------------------------------------------------------------------------
// Масштабирование значения duty cycle в соответствии с 12-битным разрешением
//------------------------------------------------------------------------------
static uint16_t scale_resolution(int value)
{
	uint16_t scale = ((uint16_t)1 << tmrResolution);

	// Валидация значения
	value &= (scale - 1);
	return ((uint16_t)value * TIMER_PERIOD) / scale;
}

//------------------------------------------------------------------------------
// Установка коэффициента заполнения ШИМ
//------------------------------------------------------------------------------
void milandr_pwm_set_value(uint8_t pin, int value)
{
	tTimerOut out = channelMap[pin % DMAX];

	if(IS_CHAN_INIT(out.timer, out.channel))
	{
		volatile MDR_TIMER_TypeDef * TIMERx = tmrTable[out.timer].regs;
		TIMER_SetChnCompare((MDR_TIMER_TypeDef*)TIMERx, out.channel % 4, scale_resolution(value));
	}
}
