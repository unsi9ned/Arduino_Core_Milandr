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

#ifndef _PERIPH_DEFINITION_H
#define _PERIPH_DEFINITION_H

#include <stdint.h>

//------------------------------------------------------------------------------
// Расширение набора значений MODE
//------------------------------------------------------------------------------
typedef enum
{
	PIN_MUX_GPIO = 0,   // Режим цифрового порта (в SPL это PORT)
	PIN_MUX_MAIN,       // Основная функция (SPI, UART, ...)
	PIN_MUX_ALTER,      // Альтернативная функция
	PIN_MUX_OVERRID,    // Переопределённая функция
	PIN_MUX_ANALOG,     // Аналоговый режим
	PIN_MUX_LINES_NUM
}
tPinMode;

//------------------------------------------------------------------------------
// Полное описание вывода с включением всей необходимой информации для его настройки
//------------------------------------------------------------------------------
typedef union
{
	uint32_t raw;

	struct
	{
		// Псевдоним вывода порта. Фактически одно из значений tPinName
		union
		{
			uint8_t pinName;

			struct
			{
				// Номер пина
				uint8_t pin  :4;

				// Номер порта
				uint8_t port :4;
			};
		};

		// Содержит функцию работы вывода. Фактически это поле MODE регистра FUNC
		// c добавленным битом 2, являющийся инвертированным ANALOG_EN
		union
		{
			uint8_t pinFunc   :3;

			struct
			{
				uint8_t func  :2;
				uint8_t analog:1;
			};
		};

		// Номер, идентифицирующий тип периферии (tPeriphVariant)
		uint8_t periph    :5;

		// Номер, идентифицирующий номер периферийного модуля SPI1, SPI2, SPI3 и т.д.
		uint8_t periphN   :4;
		uint8_t           :4;

		// Номер, идентифицирующий тип линии периферийного модуля (MOSI, MISO, TMR_CH и т.д.)
		uint8_t periphLine;
	};
}
tMilandrPin;

//------------------------------------------------------------------------------
// Нумерация всей имеющейся периферии
//------------------------------------------------------------------------------
typedef enum
{
	CAN_1 = 1,
	CAN_2
}
tAdcVariant;

typedef enum
{
	UART_1 = 1,
	UART_2,
}
tUartVariant;

typedef enum
{
	SSP_1 = 1,
	SSP_2,
}
tSspVariant;

typedef enum
{
	TIMER_1 = 1,
	TIMER_2,
	TIMER_3,
}
tTimerVariant;

typedef enum
{
	PERIPH_PORT,
	PERIPH_CAN,
	PERIPH_USB,
	PERIPH_UART,
	PERIPH_SSP,
	PERIPH_I2C,
	PERIPH_TIMER,
	PERIPH_ADC,
	PERIPH_DAC,
	PERIPH_COMP,

	PERIPH_VARIANTS_NUM,
	PERIPH_UNKNOWN = 0x1F
}
tPeriphVariant;

//------------------------------------------------------------------------------
// Все возможные варианты линий периферии
//------------------------------------------------------------------------------
typedef enum
{
	DIO_LINE = 0,
	ADC_IN_LINE,
	ADC_REF_P_LINE,
	ADC_REF_N_LINE,
	DAC_OUT_LINE,
	DAC_REF_LINE,
	CAN_RXD_LINE,
	CAN_TXD_LINE,
	UART_RXD_LINE,
	UART_TXD_LINE,
	UART_SIROUT_LINE,
	UART_SIRIN_LINE,
	SSP_TXD_LINE,
	SSP_RXD_LINE,
	SSP_CLK_LINE,
	SSP_FSS_LINE,
	I2C_SCL_LINE,
	I2C_SDA_LINE,
	TMR_CH1_LINE,
	TMR_CH2_LINE,
	TMR_CH3_LINE,
	TMR_CH4_LINE,
	TMR_CH1_N_LINE,
	TMR_CH2_N_LINE,
	TMR_CH3_N_LINE,
	TMR_CH4_N_LINE,
	TMR_BLK_LINE,
	TMR_ETR_LINE,
	COMP_IN1_LINE,
	COMP_IN2_LINE,
	COMP_REF_P_LINE,
	COMP_REF_N_LINE,
	COMP_OUT_LINE,

	PERIPH_LINE_VARIANTS_NUM,
	PERIPH_UNKNOWN_LINE = 0xFF
}
tPeriphLineVariant;

#endif //_PERIPH_DEFINITION_H
