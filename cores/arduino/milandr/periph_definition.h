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

#include "pin_names.h"
#include "variant.h"
#include "MDR32FxQI_port.h"

//------------------------------------------------------------------------------
// Битовые маски для работы полями структуры tMilandrPin
//------------------------------------------------------------------------------
#define PIN_NUMBER_MASK 0xFUL
#define PIN_PORT_MASK   0xF0UL
#define PIN_FUNC_MASK   0x300UL
#define PIN_MODE_MASK   0x400UL
#define PIN_PERIPH_MASK 0xFF0000UL

//------------------------------------------------------------------------------
// Полное описание вывода с включением всей необходимой информации для его настройки
//------------------------------------------------------------------------------
typedef struct
{
	union
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
			uint8_t altFunc   :2;
			uint8_t remapFunc :2;

			// Определяет является ли вывод аналоговым (1) или цифровым (0)
			uint8_t pinMode   :1;
			uint8_t           :3;

			uint16_t reserved;
		};
	};

	volatile void * altPeriph;
	volatile void * remapPeriph;
}
tMilandrPin;

//------------------------------------------------------------------------------
// Нумерация всей имеющейся периферии
//------------------------------------------------------------------------------
#if defined(MDR_CAN1)
#define CAN1_FUNC        0x0001
#endif

#if defined(MDR_CAN2)
#define CAN2_FUNC        0x0002
#endif

#if defined(MDR_USB)
#define USB_FUNC         0x0001
#endif

#if defined(MDR_UART1)
#define UART1_FUNC       0x0001
#endif

#if defined(MDR_UART2)
#define UART2_FUNC       0x0002
#endif

#if defined(MDR_UART3)
#define UART3_FUNC       0x0004
#endif

#if defined(MDR_SSP1)
#define SSP1_FUNC        0x0001
#endif

#if defined(MDR_SSP2)
#define SSP2_FUNC        0x0002
#endif

#if defined(MDR_I2C)
#define I2C_FUNC         0x0001
#endif

#if defined(MDR_TIMER1)
#define TIMER1_FUNC      0x0001
#endif

#if defined(MDR_TIMER2)
#define TIMER2_FUNC      0x0002
#endif

#if defined(MDR_TIMER3)
#define TIMER3_FUNC      0x0004
#endif

#if defined(MDR_ADC)
#define ADC_FUNC         0x0001
#endif

#if defined(MDR_DAC)
#define DAC_FUNC         0x0001
#endif

#if defined(MDR_COMP)
#define COMP_FUNC        0x0001
#endif

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
	SSP_TXD_LINE,
	SSP_RXD_LINE,
	SSP_CLK_LINE,
	SSP_FSS_LINE,
	I2C_SCL_LINE,
	I2C_SDA_LINE,
	TMR_CH_LINE,
	TMR_CH_N_LINE,
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

//------------------------------------------------------------------------------
// Полная таблица пинов с их функциональным назначением
//------------------------------------------------------------------------------
extern const tMilandrPin pinTable[DMAX][PERIPH_LINE_VARIANTS_NUM][1];

#endif //_PERIPH_DEFINITION_H
