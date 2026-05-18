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
#include "MDR32FxQI_timer.h"

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
static uint32_t  usedChannelMask;
static uint32_t  initChannelMask;

#define IS_CHAN_USED(tmr, ch) \
        (usedChannelMask & ((1UL << (ch & 0x7)) << (tmr * 8)))

#define SET_CHAN_USED(tmr, ch) \
        usedChannelMask |= ((1UL << (ch & 0x7)) << (tmr * 8))

#define IS_CHAN_INIT(tmr, ch) \
        (initChannelMask & ((1UL << (ch & 0x7)) << (tmr * 8)))

#define SET_CHAN_INIT(tmr, ch) \
		initChannelMask |= ((1UL << (ch & 0x7)) << (tmr * 8))

//------------------------------------------------------------------------------
// Обнуление карты каналов
//------------------------------------------------------------------------------
void milandr_timer_preinit(void)
{
	for(int i = 0; i < DMAX; i++)
	{
		channelMap[i].raw = NULL_CHANNEL;
	}

	usedChannelMask = 0;
	initChannelMask = 0;
}

//------------------------------------------------------------------------------
// Связать пин с конкретным таймером и каналом
//------------------------------------------------------------------------------
static bool occupy_channel(uint8_t pin)
{
	if(channelMap[pin].raw == NULL_CHANNEL)
	{
		const tMilandrPin * pins[2] =
		{
			&pinTable[pin][PIN_MUX_ALTER][0],
			&pinTable[pin][PIN_MUX_OVERRID][0],
		};

		//
		// На одном пине могут быть такие вариации:
		//   - один таймер, разные каналы (TMR1_CH1, TMR1_CH3)
		//   - разные таймеры, одинаковы каналы (TMR2_CH4, TMR3_CH4)
		//   - разные таймеры, разные каналы (TMR1_CH1, TMR2_CH2)
		//   - один таймер, один канал (TMR_CH1)
		//   - отсутствие каналов таймера

		for(int i = 0; i < 2; i++)
		{
			if(pins[i]->periph == PERIPH_TIMER)
			{
				uint8_t n = TIMER_GET_N(pins[i]->periphN);
				uint8_t c = TIMER_GET_CH(pins[i]->periphLine);
				uint8_t mux = (pins[i]->pinFunc == PIN_MUX_OVERRID) ? 1 : 0;

				if(!IS_CHAN_USED(n, c))
				{
					channelMap[pin].timer = n;
					channelMap[pin].channel = c;
					channelMap[pin].mux = mux;
					SET_CHAN_USED(n, c);
					return true;
				}
			}
		}

		return false;
	}

	return true;
}

//------------------------------------------------------------------------------
// Инициализация выхода таймера
//------------------------------------------------------------------------------
static bool init_pwm_out(uint8_t pin)
{
	if(channelMap[pin].raw == NULL_CHANNEL) return false;

	tTimerOut out = channelMap[pin];

	if(IS_CHAN_USED(out.timer, out.channel) &&
       IS_CHAN_INIT(out.timer, out.channel))
	{
		return true;
	}
	else if(IS_CHAN_USED(out.timer, out.channel))
	{
		//
		// Производим настройку GPIO
		//
		uint8_t portFunc = TIMER_PORT_MUX(out.mux);

		milandr_gpio_clock_enable(pin, true);
		milandr_gpio_cfg_output_pp(pin);

		if(portFunc == PIN_MUX_OVERRID)
			milandr_gpio_sel_alter_func(pin);
		else
			milandr_gpio_sel_override_func(pin);

		//
		// Производим настройку Timer
		//

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
	tTimerOut tmrChannel = { .raw = NULL_CHANNEL };

	if(pin > DMAX || !occupy_channel(pin) || !init_pwm_out(pin)) return tmrChannel;

	return channelMap[pin];

//	//
//	// Сначала выполняем поиск модуля TimerN, соответствующего номеру пина
//	// и его описание в таблице pinTable
//	//
//	tTimerVariant tmrN[2] = {TIMER_UNKNOWN, TIMER_UNKNOWN};
//	const tMilandrPin * lines[TIMER_CH_CNT] =
//	{
//		NULL, NULL, NULL, NULL,
//		NULL, NULL, NULL, NULL,
//	};
//	const tMilandrPin * pins[2] =
//	{
//		&pinTable[pin][PIN_MUX_ALTER][0],
//		&pinTable[pin][PIN_MUX_OVERRID][0],
//	};
//
//	//
//	// На одном пине могут быть такие вариации:
//	//   - один таймер, разные каналы (TMR1_CH1, TMR1_CH3)
//	//   - разные таймеры, одинаковы каналы (TMR2_CH4, TMR3_CH4)
//	//   - разные таймеры, разные каналы (TMR1_CH1, TMR2_CH2)
//	//   - один таймер, один канал (TMR_CH1)
//	//   - отсутствие каналов таймера
//	//
//	for(tTimerVariant tmrIdx = TIMER_1; tmrIdx < TIMER_COUNT; tmrIdx++)
//	{
//		for(tPeriphLineVariant ln = TMR_CH1_LINE; ln <= TMR_CH4_N_LINE; ln++)
//		{
//			const tMilandrPin * temp[2];
//			uint8_t cnt = milandr_find_pin(PERIPH_TIMER, ln, tmrIdx, pins, temp, 2);
//
//			if(cnt) lines[TIMER_GET_CH(ln)] = temp[0];
//			else    lines[TIMER_GET_CH(ln)] = NULL;
//		}

//		if(lines[UART_RX_LINE] && lines[UART_TX_LINE])
//		{
//			uartN = uartIdx;
//			break;
//		}
//	}

	//
	// Не удалось определить модуль UART. Неверная комбинация пинов rxPin/txPin
	//
	//if(uartN == UART_UNKNOWN) return uartN;
}
