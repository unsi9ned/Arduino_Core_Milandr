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
// Битовые маски для работы полями структуры tMilandrPin
//------------------------------------------------------------------------------
#define PIN_NUMBER_MASK    0xFUL
#define PIN_NUMBER_POS     0xFUL
#define PIN_PORTNUM_MASK   0xF0UL
#define PIN_PORTNUM_POS    0x4UL
#define PIN_FUNC_MASK      0x700UL
#define PIN_FUNC_POS       0x8UL
#define PIN_PERIPH_MASK    0xF800UL
#define PIN_PERIPH_POS     0xBUL
#define PIN_PERIPH_N_MASK  0xF0000UL
#define PIN_PERIPH_N_POS   0x10UL
#define PIN_PERIPH_L_MASK  0xFF000000UL
#define PIN_PERIPH_L_POS   0x18UL

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
			struct
			{
				uint8_t pinFunc   :3;

				// Номер, идентифицирующий тип периферии (tPeriphVariant)
				uint8_t periph    :5;
			};

			struct
			{
				uint8_t mode      :2;
				uint8_t analog    :1;
				uint8_t           :5;
			};
		};

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
	CAN_1,
	CAN_2
}
tCanVariant;

typedef enum
{
	UART_1,
	UART_2,
	UART_3,
	UART_COUNT,
	UART_UNKNOWN = 0xFF
}
tUartVariant;

typedef enum
{
	UART_MODE_POLLING,
	UART_MODE_RING,
	UART_MODE_DMA
}
tUartDriverMode;

typedef enum
{
	SSP_1,
	SSP_2,
	SSP_COUNT,
	SSP_UNKNOWN = 0xFF
}
tSspVariant;

typedef enum
{
	I2C_1,
	I2C_COUNT,
	I2C_UNKNOWN = 0xFF
}
tI2cVariant;

typedef enum
{
	TIMER_1,
	TIMER_2,
	TIMER_3,
	TIMER_COUNT,
	TIMER_UNKNOWN = 0xFF
}
tTimerVariant;

typedef enum
{
	ADC_1,
	ADC_2,
	ADC_COUNT,
	ADC_UNKNOWN
}
tAdcVariant;

typedef enum
{
	DAC_1,
	DAC_2,
	DAC_COUNT,
	DAC_UNKNOWN
}
tDacVariant;

typedef enum
{
	COMP_1,
	COMP_COUNT,
	COMP_UNKNOWN
}
tComparatorVariant;

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

	/* Псевдо-периферия. Фактически PERIPH_TIMER. Указывая данный тип периферии
	 * мы сообщаем ядру, что данный выход таймера использутся для генерации ШИМ */
	PERIPH_PWM,

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
	DIO_EXT_INT1_LINE,
	DIO_EXT_INT2_LINE,
	DIO_EXT_INT3_LINE,
	DIO_EXT_INT4_LINE,
	DIO_EXT_INT5_LINE,
	DIO_EXT_INT6_LINE,
	DIO_EXT_INT7_LINE,
	DIO_EXT_INT8_LINE,
	ADC_CH0_LINE,
	ADC_CH1_LINE,
	ADC_CH2_LINE,
	ADC_CH3_LINE,
	ADC_CH4_LINE,
	ADC_CH5_LINE,
	ADC_CH6_LINE,
	ADC_CH7_LINE,
	ADC_CH8_LINE,
	ADC_CH9_LINE,
	ADC_CH10_LINE,
	ADC_CH11_LINE,
	ADC_CH12_LINE,
	ADC_CH13_LINE,
	ADC_CH14_LINE,
	ADC_CH15_LINE,
	ADC_CH31_LINE,
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
	TMR_ETR_LINE,
	TMR_BLK_LINE,
	TMR_CH1_LINE,
	TMR_CH2_LINE,
	TMR_CH3_LINE,
	TMR_CH4_LINE,
	TMR_CH1_N_LINE,
	TMR_CH2_N_LINE,
	TMR_CH3_N_LINE,
	TMR_CH4_N_LINE,
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
