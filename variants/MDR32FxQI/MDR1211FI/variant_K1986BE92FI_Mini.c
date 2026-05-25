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

#include "system_K1986VE9xI.h"
#include "MDR32FxQI_rst_clk.h"
#include "MDR32FxQI_bkp.h"
#include "MDR32FxQI_eeprom.h"
#include "variant_K1986BE92FI_Mini.h"
#include "milandr/pin_names.h"
#include "milandr/periph_definition.h"

/*---------------------K1986BE92FI_Mini pins number-----------------------------
                                         ♫     ♫
          PWM8                       PWM7| PWM6| PWM5              PWM4
           |                          |  |  |  |  |                 |
           | P37      P35   P33   P31 | P29 | P27 | P25   P23   P21 |
          P38 | P36    | P34 | P32 | P30 | P28 | P26 | P24 | P22 | P20
           |  |  |     |  |  |  |  |  |  |  |  |  |  |  |  |  |  |  |
     G 5V F6 F5 F4 WP F3 F2 F1 F0 A0 A1 A2 A3 A4 A5 A6 A7 B10B9 B8 B7 3V3 G
     |  |  |  |  |  |  |  |  |  |  |  |  |  |  |  |  |  |  |  |  |  |  |  |
   +------------------------------------------------------------------------+
   |                                                                        |
  ┌-----┐                                                                   |+-- G
  | USB |                                                                   |+-- D1--P40--AN7
  └-----┘                                                                   |+-- D0--P39--AN6
   |                                                                        |+-- 3V
   +------------------------------------------------------------------------+
     |  |  |  |  |  |  |  |  |  |  |  |  |  |  |  |  |  |  |  |  |  |  |  |
    VB  R E3 E2 E1 E0 D7 D4 D2 D3 D5 D6 C2 C1 C0 B0 B1 B2 B3 B4 B5 B6 3V3 G
           |  |  |  |  |  |  |  |  |  |  |  |  |  |  |  |  |  |  |  |
          P0 P1 P2 P3 P4 P5 P6 P7 P8 P9 P10 | P12 | P14 | P16 | P18 |
           |  |        |  |  |  |  |  |  | P11   P13   P15   P17 | P19
           |  |       AN0 | AN2 | AN4 |  |              |        |
           |  |          AN1   AN3   AN5 |              |        |
           | PWM0                       PWM1           PWM2     PWM3
           ♫
------------------------------------------------------------------------------*/

//------------------------------------------------------------------------------
// Константы
//------------------------------------------------------------------------------
#define HSI_FREQ_HZ       8000000UL
#define HSE_FREQ_HZ       8000000UL
#define LSE_FREQ_HZ       32768UL
#define LSI_FREQ_HZ       40000UL

//------------------------------------------------------------------------------
// Глобальные переменные, которые должны быть заданы в variant.h
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
// Полная таблица пинов с их функциональным назначением
// Примеры:
//    1) Быстрый доступ к связке MDR_PORTx/PORT_Pin_y:
//       pinTable[D0][PIN_MODE_GPIO].pinName + битовые операции
//    2) Определение номера периферии пина MOSI = D5
//       pinTable[D5][PIN_MODE_ALTER].periphN или
//       pinTable[D5][PIN_MODE_OVERRID].periphN
// Примечание:
//    1) Eсли номер periphN = 0, то модуль в единственном эксземпляре и не нумеруется
//    2) Если pinTable[D5][PIN_MODE_ANALOG].periph = PERIPH_PORT, то пин не имеет аналоговой функции
//------------------------------------------------------------------------------
const tMilandrPin pinTable[DMAX][PIN_MUX_LINES_NUM][1] =
{
	[D0] =
	{
		[PIN_MUX_GPIO] = {{.pinName = PE_3, .pinFunc = PIN_MUX_GPIO, .periph = PERIPH_PORT, .periphN = 0, .periphLine = DIO_LINE}},
		[PIN_MUX_MAIN] = {{.pinName = PE_3, .pinFunc = PIN_MUX_MAIN, .periph = PERIPH_PORT, .periphN = 0, .periphLine = DIO_LINE}},
		[PIN_MUX_ALTER] = {{.pinName = PE_3, .pinFunc = PIN_MUX_ALTER, .periph = PERIPH_TONE, .periphN = TIMER_2, .periphLine = TMR_CH3_N_LINE}},
		[PIN_MUX_OVERRID] = {{.pinName = PE_3, .pinFunc = PIN_MUX_OVERRID, .periph = PERIPH_TIMER, .periphN = TIMER_3, .periphLine = TMR_CH1_N_LINE}},
		[PIN_MUX_ANALOG] = {{.pinName = PE_3, .pinFunc = PIN_MUX_ANALOG, .periph = PERIPH_COMP, .periphN = 0, .periphLine = COMP_IN2_LINE}},
	},

	[D1] =
	{
		[PIN_MUX_GPIO] = {{.pinName = PE_2, .pinFunc = PIN_MUX_GPIO, .periph = PERIPH_PORT, .periphN = 0, .periphLine = DIO_LINE}},
		[PIN_MUX_MAIN] = {{.pinName = PE_2, .pinFunc = PIN_MUX_MAIN, .periph = PERIPH_PORT, .periphN = 0, .periphLine = DIO_LINE}},
		[PIN_MUX_ALTER] = {{.pinName = PE_2, .pinFunc = PIN_MUX_ALTER, .periph = PERIPH_PWM, .periphN = TIMER_2, .periphLine = TMR_CH3_LINE}},
		[PIN_MUX_OVERRID] = {{.pinName = PE_2, .pinFunc = PIN_MUX_OVERRID, .periph = PERIPH_TIMER, .periphN = TIMER_3, .periphLine = TMR_CH1_LINE}},
		[PIN_MUX_ANALOG] = {{.pinName = PE_2, .pinFunc = PIN_MUX_ANALOG, .periph = PERIPH_COMP, .periphN = 0, .periphLine = COMP_IN1_LINE}},
	},

	[D2] =
	{
		[PIN_MUX_GPIO] = {{.pinName = PE_1, .pinFunc = PIN_MUX_GPIO, .periph = PERIPH_PORT, .periphN = 0, .periphLine = DIO_LINE}},
		[PIN_MUX_MAIN] = {{.pinName = PE_1, .pinFunc = PIN_MUX_MAIN, .periph = PERIPH_PORT, .periphN = 0, .periphLine = DIO_LINE}},
		[PIN_MUX_ALTER] = {{.pinName = PE_1, .pinFunc = PIN_MUX_ALTER, .periph = PERIPH_TIMER, .periphN = TIMER_2, .periphLine = TMR_CH1_N_LINE}},
		[PIN_MUX_OVERRID] = {{.pinName = PE_1, .pinFunc = PIN_MUX_OVERRID, .periph = PERIPH_CAN, .periphN = CAN_1, .periphLine = CAN_TXD_LINE}},
		[PIN_MUX_ANALOG] = {{.pinName = PE_1, .pinFunc = PIN_MUX_ANALOG, .periph = PERIPH_DAC, .periphN = DAC_2, .periphLine = DAC_REF_LINE}},
	},

	[D3] =
	{
		[PIN_MUX_GPIO] = {{.pinName = PE_0, .pinFunc = PIN_MUX_GPIO, .periph = PERIPH_PORT, .periphN = 0, .periphLine = DIO_LINE}},
		[PIN_MUX_MAIN] = {{.pinName = PE_0, .pinFunc = PIN_MUX_MAIN, .periph = PERIPH_PORT, .periphN = 0, .periphLine = DIO_LINE}},
		[PIN_MUX_ALTER] = {{.pinName = PE_0, .pinFunc = PIN_MUX_ALTER, .periph = PERIPH_TIMER, .periphN = TIMER_2, .periphLine = TMR_CH1_LINE}},
		[PIN_MUX_OVERRID] = {{.pinName = PE_0, .pinFunc = PIN_MUX_OVERRID, .periph = PERIPH_CAN, .periphN = CAN_1, .periphLine = CAN_RXD_LINE}},
		[PIN_MUX_ANALOG] = {{.pinName = PE_0, .pinFunc = PIN_MUX_ANALOG, .periph = PERIPH_DAC, .periphN = DAC_2, .periphLine = DAC_OUT_LINE}},
	},

	[D4] =
	{
		[PIN_MUX_GPIO] = {{.pinName = PD_7, .pinFunc = PIN_MUX_GPIO, .periph = PERIPH_PORT, .periphN = 0, .periphLine = DIO_LINE}},
		[PIN_MUX_MAIN] = {{.pinName = PD_7, .pinFunc = PIN_MUX_MAIN, .periph = PERIPH_TIMER, .periphN = TIMER_1, .periphLine = TMR_BLK_LINE}},
		[PIN_MUX_ALTER] = {{.pinName = PD_7, .pinFunc = PIN_MUX_ALTER, .periph = PERIPH_UART, .periphN = UART_2, .periphLine = UART_SIRIN_LINE}},
		[PIN_MUX_OVERRID] = {{.pinName = PD_7, .pinFunc = PIN_MUX_OVERRID, .periph = PERIPH_UART, .periphN = UART_1, .periphLine = UART_RXD_LINE}},
		[PIN_MUX_ANALOG] = {{.pinName = PD_7, .pinFunc = PIN_MUX_ANALOG, .periph = PERIPH_ADC, .periphN = ADC_2, .periphLine = ADC_CH7_LINE}},
	},

	[D5] =
	{
		[PIN_MUX_GPIO] = {{.pinName = PD_4, .pinFunc = PIN_MUX_GPIO, .periph = PERIPH_PORT, .periphN = 0, .periphLine = DIO_LINE}},
		[PIN_MUX_MAIN] = {{.pinName = PD_4, .pinFunc = PIN_MUX_MAIN, .periph = PERIPH_TIMER, .periphN = TIMER_1, .periphLine = TMR_ETR_LINE}},
		[PIN_MUX_ALTER] = {{.pinName = PD_4, .pinFunc = PIN_MUX_ALTER, .periph = PERIPH_UART, .periphN = UART_2, .periphLine = UART_SIROUT_LINE}},
		[PIN_MUX_OVERRID] = {{.pinName = PD_4, .pinFunc = PIN_MUX_OVERRID, .periph = PERIPH_TIMER, .periphN = TIMER_3, .periphLine = TMR_BLK_LINE}},
		[PIN_MUX_ANALOG] = {{.pinName = PD_4, .pinFunc = PIN_MUX_ANALOG, .periph = PERIPH_ADC, .periphN = ADC_2, .periphLine = ADC_CH4_LINE}},
	},

	[D6] =
	{
		[PIN_MUX_GPIO] = {{.pinName = PD_2, .pinFunc = PIN_MUX_GPIO, .periph = PERIPH_PORT, .periphN = 0, .periphLine = DIO_LINE}},
		[PIN_MUX_MAIN] = {{.pinName = PD_2, .pinFunc = PIN_MUX_MAIN, .periph = PERIPH_PORT, .periphN = 0, .periphLine = DIO_LINE}},
		[PIN_MUX_ALTER] = {{.pinName = PD_2, .pinFunc = PIN_MUX_ALTER, .periph = PERIPH_SSP, .periphN = SSP_2, .periphLine = SSP_RXD_LINE}},
		[PIN_MUX_OVERRID] = {{.pinName = PD_2, .pinFunc = PIN_MUX_OVERRID, .periph = PERIPH_TIMER, .periphN = TIMER_3, .periphLine = TMR_CH2_LINE}},
		[PIN_MUX_ANALOG] = {{.pinName = PD_2, .pinFunc = PIN_MUX_ANALOG, .periph = PERIPH_ADC, .periphN = ADC_2, .periphLine = ADC_CH2_LINE}},
	},

	[D7] =
	{
		[PIN_MUX_GPIO] = {{.pinName = PD_3, .pinFunc = PIN_MUX_GPIO, .periph = PERIPH_PORT, .periphN = 0, .periphLine = DIO_LINE}},
		[PIN_MUX_MAIN] = {{.pinName = PD_3, .pinFunc = PIN_MUX_MAIN, .periph = PERIPH_PORT, .periphN = 0, .periphLine = DIO_LINE}},
		[PIN_MUX_ALTER] = {{.pinName = PD_3, .pinFunc = PIN_MUX_ALTER, .periph = PERIPH_SSP, .periphN = SSP_2, .periphLine = SSP_FSS_LINE}},
		[PIN_MUX_OVERRID] = {{.pinName = PD_3, .pinFunc = PIN_MUX_OVERRID, .periph = PERIPH_TIMER, .periphN = TIMER_3, .periphLine = TMR_CH2_N_LINE}},
		[PIN_MUX_ANALOG] = {{.pinName = PD_3, .pinFunc = PIN_MUX_ANALOG, .periph = PERIPH_ADC, .periphN = ADC_2, .periphLine = ADC_CH3_LINE}},
	},

	[D8] =
	{
		[PIN_MUX_GPIO] = {{.pinName = PD_5, .pinFunc = PIN_MUX_GPIO, .periph = PERIPH_PORT, .periphN = 0, .periphLine = DIO_LINE}},
		[PIN_MUX_MAIN] = {{.pinName = PD_5, .pinFunc = PIN_MUX_MAIN, .periph = PERIPH_PORT, .periphN = 0, .periphLine = DIO_LINE}},
		[PIN_MUX_ALTER] = {{.pinName = PD_5, .pinFunc = PIN_MUX_ALTER, .periph = PERIPH_SSP, .periphN = SSP_2, .periphLine = SSP_CLK_LINE}},
		[PIN_MUX_OVERRID] = {{.pinName = PD_5, .pinFunc = PIN_MUX_OVERRID, .periph = PERIPH_TIMER, .periphN = TIMER_2, .periphLine = TMR_ETR_LINE}},
		[PIN_MUX_ANALOG] = {{.pinName = PD_5, .pinFunc = PIN_MUX_ANALOG, .periph = PERIPH_ADC, .periphN = ADC_2, .periphLine = ADC_CH5_LINE}},
	},

	[D9] =
	{
		[PIN_MUX_GPIO] = {{.pinName = PD_6, .pinFunc = PIN_MUX_GPIO, .periph = PERIPH_PORT, .periphN = 0, .periphLine = DIO_LINE}},
		[PIN_MUX_MAIN] = {{.pinName = PD_6, .pinFunc = PIN_MUX_MAIN, .periph = PERIPH_PORT, .periphN = 0, .periphLine = DIO_LINE}},
		[PIN_MUX_ALTER] = {{.pinName = PD_6, .pinFunc = PIN_MUX_ALTER, .periph = PERIPH_SSP, .periphN = SSP_2, .periphLine = SSP_TXD_LINE}},
		[PIN_MUX_OVERRID] = {{.pinName = PD_6, .pinFunc = PIN_MUX_OVERRID, .periph = PERIPH_TIMER, .periphN = TIMER_2, .periphLine = TMR_BLK_LINE}},
		[PIN_MUX_ANALOG] = {{.pinName = PD_6, .pinFunc = PIN_MUX_ANALOG, .periph = PERIPH_ADC, .periphN = ADC_2, .periphLine = ADC_CH6_LINE}},
	},

	[D10] =
	{
		[PIN_MUX_GPIO] = {{.pinName = PC_2, .pinFunc = PIN_MUX_GPIO, .periph = PERIPH_PORT, .periphN = 0, .periphLine = DIO_LINE}},
		[PIN_MUX_MAIN] = {{.pinName = PC_2, .pinFunc = PIN_MUX_MAIN, .periph = PERIPH_PORT, .periphN = 0, .periphLine = DIO_LINE}},
		[PIN_MUX_ALTER] = {{.pinName = PC_2, .pinFunc = PIN_MUX_ALTER, .periph = PERIPH_PWM, .periphN = TIMER_3, .periphLine = TMR_CH1_LINE}},
		[PIN_MUX_OVERRID] = {{.pinName = PC_2, .pinFunc = PIN_MUX_OVERRID, .periph = PERIPH_SSP, .periphN = SSP_2, .periphLine = SSP_RXD_LINE}},
		[PIN_MUX_ANALOG] = {{.pinName = PC_2, .pinFunc = PIN_MUX_ANALOG, .periph = PERIPH_PORT, .periphN = 0, .periphLine = DIO_LINE}},
	},

	[D11] =
	{
		[PIN_MUX_GPIO] = {{.pinName = PC_1, .pinFunc = PIN_MUX_GPIO, .periph = PERIPH_PORT, .periphN = 0, .periphLine = DIO_LINE}},
		[PIN_MUX_MAIN] = {{.pinName = PC_1, .pinFunc = PIN_MUX_MAIN, .periph = PERIPH_PORT, .periphN = 0, .periphLine = DIO_LINE}},
		[PIN_MUX_ALTER] = {{.pinName = PC_1, .pinFunc = PIN_MUX_ALTER, .periph = PERIPH_I2C, .periphN = I2C_1, .periphLine = I2C_SDA_LINE}},
		[PIN_MUX_OVERRID] = {{.pinName = PC_1, .pinFunc = PIN_MUX_OVERRID, .periph = PERIPH_SSP, .periphN = SSP_2, .periphLine = SSP_CLK_LINE}},
		[PIN_MUX_ANALOG] = {{.pinName = PC_1, .pinFunc = PIN_MUX_ANALOG, .periph = PERIPH_PORT, .periphN = 0, .periphLine = DIO_LINE}},
	},

	[D12] =
	{
		[PIN_MUX_GPIO] = {{.pinName = PC_0, .pinFunc = PIN_MUX_GPIO, .periph = PERIPH_PORT, .periphN = 0, .periphLine = DIO_LINE}},
		[PIN_MUX_MAIN] = {{.pinName = PC_0, .pinFunc = PIN_MUX_MAIN, .periph = PERIPH_PORT, .periphN = 0, .periphLine = DIO_LINE}},
		[PIN_MUX_ALTER] = {{.pinName = PC_0, .pinFunc = PIN_MUX_ALTER, .periph = PERIPH_I2C, .periphN = I2C_1, .periphLine = I2C_SCL_LINE}},
		[PIN_MUX_OVERRID] = {{.pinName = PC_0, .pinFunc = PIN_MUX_OVERRID, .periph = PERIPH_SSP, .periphN = SSP_2, .periphLine = SSP_FSS_LINE}},
		[PIN_MUX_ANALOG] = {{.pinName = PC_0, .pinFunc = PIN_MUX_ANALOG, .periph = PERIPH_PORT, .periphN = 0, .periphLine = DIO_LINE}},
	},

	[D13] =
	{
		[PIN_MUX_GPIO] = {{.pinName = PB_0, .pinFunc = PIN_MUX_GPIO, .periph = PERIPH_PORT, .periphN = 0, .periphLine = DIO_LINE}},
		[PIN_MUX_MAIN] = {{.pinName = PB_0, .pinFunc = PIN_MUX_MAIN, .periph = PERIPH_PORT, .periphN = 0, .periphLine = DIO_LINE}},
		[PIN_MUX_ALTER] = {{.pinName = PB_0, .pinFunc = PIN_MUX_ALTER, .periph = PERIPH_TIMER, .periphN = TIMER_3, .periphLine = TMR_CH1_LINE}},
		[PIN_MUX_OVERRID] = {{.pinName = PB_0, .pinFunc = PIN_MUX_OVERRID, .periph = PERIPH_UART, .periphN = UART_1, .periphLine = UART_TXD_LINE}},
		[PIN_MUX_ANALOG] = {{.pinName = PB_0, .pinFunc = PIN_MUX_ANALOG, .periph = PERIPH_PORT, .periphN = 0, .periphLine = DIO_LINE}},
	},

	[D14] =
	{
		[PIN_MUX_GPIO] = {{.pinName = PB_1, .pinFunc = PIN_MUX_GPIO, .periph = PERIPH_PORT, .periphN = 0, .periphLine = DIO_LINE}},
		[PIN_MUX_MAIN] = {{.pinName = PB_1, .pinFunc = PIN_MUX_MAIN, .periph = PERIPH_PORT, .periphN = 0, .periphLine = DIO_LINE}},
		[PIN_MUX_ALTER] = {{.pinName = PB_1, .pinFunc = PIN_MUX_ALTER, .periph = PERIPH_TIMER, .periphN = TIMER_3, .periphLine = TMR_CH1_N_LINE}},
		[PIN_MUX_OVERRID] = {{.pinName = PB_1, .pinFunc = PIN_MUX_OVERRID, .periph = PERIPH_UART, .periphN = UART_2, .periphLine = UART_RXD_LINE}},
		[PIN_MUX_ANALOG] = {{.pinName = PB_1, .pinFunc = PIN_MUX_ANALOG, .periph = PERIPH_PORT, .periphN = 0, .periphLine = DIO_LINE}},
	},

	[D15] =
	{
		[PIN_MUX_GPIO] = {{.pinName = PB_2, .pinFunc = PIN_MUX_GPIO, .periph = PERIPH_PORT, .periphN = 0, .periphLine = DIO_LINE}},
		[PIN_MUX_MAIN] = {{.pinName = PB_2, .pinFunc = PIN_MUX_MAIN, .periph = PERIPH_PORT, .periphN = 0, .periphLine = DIO_LINE}},
		[PIN_MUX_ALTER] = {{.pinName = PB_2, .pinFunc = PIN_MUX_ALTER, .periph = PERIPH_PWM, .periphN = TIMER_3, .periphLine = TMR_CH2_LINE}},
		[PIN_MUX_OVERRID] = {{.pinName = PB_2, .pinFunc = PIN_MUX_OVERRID, .periph = PERIPH_CAN, .periphN = CAN_1, .periphLine = CAN_TXD_LINE}},
		[PIN_MUX_ANALOG] = {{.pinName = PB_2, .pinFunc = PIN_MUX_ANALOG, .periph = PERIPH_PORT, .periphN = 0, .periphLine = DIO_LINE}},
	},

	[D16] =
	{
		[PIN_MUX_GPIO] = {{.pinName = PB_3, .pinFunc = PIN_MUX_GPIO, .periph = PERIPH_PORT, .periphN = 0, .periphLine = DIO_LINE}},
		[PIN_MUX_MAIN] = {{.pinName = PB_3, .pinFunc = PIN_MUX_MAIN, .periph = PERIPH_PORT, .periphN = 0, .periphLine = DIO_LINE}},
		[PIN_MUX_ALTER] = {{.pinName = PB_3, .pinFunc = PIN_MUX_ALTER, .periph = PERIPH_TIMER, .periphN = TIMER_3, .periphLine = TMR_CH2_N_LINE}},
		[PIN_MUX_OVERRID] = {{.pinName = PB_3, .pinFunc = PIN_MUX_OVERRID, .periph = PERIPH_CAN, .periphN = CAN_1, .periphLine = CAN_RXD_LINE}},
		[PIN_MUX_ANALOG] = {{.pinName = PB_3, .pinFunc = PIN_MUX_ANALOG, .periph = PERIPH_PORT, .periphN = 0, .periphLine = DIO_LINE}},
	},

	[D17] =
	{
		[PIN_MUX_GPIO] = {{.pinName = PB_4, .pinFunc = PIN_MUX_GPIO, .periph = PERIPH_PORT, .periphN = 0, .periphLine = DIO_LINE}},
		[PIN_MUX_MAIN] = {{.pinName = PB_4, .pinFunc = PIN_MUX_MAIN, .periph = PERIPH_PORT, .periphN = 0, .periphLine = DIO_LINE}},
		[PIN_MUX_ALTER] = {{.pinName = PB_4, .pinFunc = PIN_MUX_ALTER, .periph = PERIPH_TIMER, .periphN = TIMER_3, .periphLine = TMR_BLK_LINE}},
		[PIN_MUX_OVERRID] = {{.pinName = PB_4, .pinFunc = PIN_MUX_OVERRID, .periph = PERIPH_TIMER, .periphN = TIMER_3, .periphLine = TMR_ETR_LINE}},
		[PIN_MUX_ANALOG] = {{.pinName = PB_4, .pinFunc = PIN_MUX_ANALOG, .periph = PERIPH_PORT, .periphN = 0, .periphLine = DIO_LINE}},
	},

	[D18] =
	{
		[PIN_MUX_GPIO] = {{.pinName = PB_5, .pinFunc = PIN_MUX_GPIO, .periph = PERIPH_PORT, .periphN = 0, .periphLine = DIO_LINE}},
		[PIN_MUX_MAIN] = {{.pinName = PB_5, .pinFunc = PIN_MUX_MAIN, .periph = PERIPH_PORT, .periphN = 0, .periphLine = DIO_LINE}},
		[PIN_MUX_ALTER] = {{.pinName = PB_5, .pinFunc = PIN_MUX_ALTER, .periph = PERIPH_UART, .periphN = UART_1, .periphLine = UART_TXD_LINE}},
		[PIN_MUX_OVERRID] = {{.pinName = PB_5, .pinFunc = PIN_MUX_OVERRID, .periph = PERIPH_PWM, .periphN = TIMER_3, .periphLine = TMR_CH3_LINE}},
		[PIN_MUX_ANALOG] = {{.pinName = PB_5, .pinFunc = PIN_MUX_ANALOG, .periph = PERIPH_PORT, .periphN = 0, .periphLine = DIO_LINE}},
	},

	[D19] =
	{
		[PIN_MUX_GPIO] = {{.pinName = PB_6, .pinFunc = PIN_MUX_GPIO, .periph = PERIPH_PORT, .periphN = 0, .periphLine = DIO_LINE}},
		[PIN_MUX_MAIN] = {{.pinName = PB_6, .pinFunc = PIN_MUX_MAIN, .periph = PERIPH_PORT, .periphN = 0, .periphLine = DIO_LINE}},
		[PIN_MUX_ALTER] = {{.pinName = PB_6, .pinFunc = PIN_MUX_ALTER, .periph = PERIPH_UART, .periphN = UART_1, .periphLine = UART_RXD_LINE}},
		[PIN_MUX_OVERRID] = {{.pinName = PB_6, .pinFunc = PIN_MUX_OVERRID, .periph = PERIPH_TIMER, .periphN = TIMER_3, .periphLine = TMR_CH3_N_LINE}},
		[PIN_MUX_ANALOG] = {{.pinName = PB_6, .pinFunc = PIN_MUX_ANALOG, .periph = PERIPH_PORT, .periphN = 0, .periphLine = DIO_LINE}},
	},

	[D20] =
	{
		[PIN_MUX_GPIO] = {{.pinName = PB_7, .pinFunc = PIN_MUX_GPIO, .periph = PERIPH_PORT, .periphN = 0, .periphLine = DIO_LINE}},
		[PIN_MUX_MAIN] = {{.pinName = PB_7, .pinFunc = PIN_MUX_MAIN, .periph = PERIPH_PORT, .periphN = 0, .periphLine = DIO_LINE}},
		[PIN_MUX_ALTER] = {{.pinName = PB_7, .pinFunc = PIN_MUX_ALTER, .periph = PERIPH_UART, .periphN = UART_1, .periphLine = UART_SIROUT_LINE}},
		[PIN_MUX_OVERRID] = {{.pinName = PB_7, .pinFunc = PIN_MUX_OVERRID, .periph = PERIPH_PWM, .periphN = TIMER_3, .periphLine = TMR_CH4_LINE}},
		[PIN_MUX_ANALOG] = {{.pinName = PB_7, .pinFunc = PIN_MUX_ANALOG, .periph = PERIPH_PORT, .periphN = 0, .periphLine = DIO_LINE}},
	},

	[D21] =
	{
		[PIN_MUX_GPIO] = {{.pinName = PB_8, .pinFunc = PIN_MUX_GPIO, .periph = PERIPH_PORT, .periphN = 0, .periphLine = DIO_LINE}},
		[PIN_MUX_MAIN] = {{.pinName = PB_8, .pinFunc = PIN_MUX_MAIN, .periph = PERIPH_PORT, .periphN = 0, .periphLine = DIO_LINE}},
		[PIN_MUX_ALTER] = {{.pinName = PB_8, .pinFunc = PIN_MUX_ALTER, .periph = PERIPH_COMP, .periphN = COMP_1, .periphLine = COMP_OUT_LINE}},
		[PIN_MUX_OVERRID] = {{.pinName = PB_8, .pinFunc = PIN_MUX_OVERRID, .periph = PERIPH_TIMER, .periphN = TIMER_3, .periphLine = TMR_CH4_N_LINE}},
		[PIN_MUX_ANALOG] = {{.pinName = PB_8, .pinFunc = PIN_MUX_ANALOG, .periph = PERIPH_PORT, .periphN = 0, .periphLine = DIO_LINE}},
	},

	[D22] =
	{
		[PIN_MUX_GPIO] = {{.pinName = PB_9, .pinFunc = PIN_MUX_GPIO, .periph = PERIPH_PORT, .periphN = 0, .periphLine = DIO_LINE}},
		[PIN_MUX_MAIN] = {{.pinName = PB_9, .pinFunc = PIN_MUX_MAIN, .periph = PERIPH_PORT, .periphN = 0, .periphLine = DIO_LINE}},
		[PIN_MUX_ALTER] = {{.pinName = PB_9, .pinFunc = PIN_MUX_ALTER, .periph = PERIPH_UART, .periphN = UART_1, .periphLine = UART_SIRIN_LINE}},
		[PIN_MUX_OVERRID] = {{.pinName = PB_9, .pinFunc = PIN_MUX_OVERRID, .periph = PERIPH_PORT, .periphN = 0, .periphLine = DIO_EXT_INT4_LINE}},
		[PIN_MUX_ANALOG] = {{.pinName = PB_9, .pinFunc = PIN_MUX_ANALOG, .periph = PERIPH_PORT, .periphN = 0, .periphLine = DIO_LINE}},
	},

	[D23] =
	{
		[PIN_MUX_GPIO] = {{.pinName = PB_10, .pinFunc = PIN_MUX_GPIO, .periph = PERIPH_PORT, .periphN = 0, .periphLine = DIO_LINE}},
		[PIN_MUX_MAIN] = {{.pinName = PB_10, .pinFunc = PIN_MUX_MAIN, .periph = PERIPH_PORT, .periphN = 0, .periphLine = DIO_LINE}},
		[PIN_MUX_ALTER] = {{.pinName = PB_10, .pinFunc = PIN_MUX_ALTER, .periph = PERIPH_PORT, .periphN = 0, .periphLine = DIO_EXT_INT2_LINE}},
		[PIN_MUX_OVERRID] = {{.pinName = PB_10, .pinFunc = PIN_MUX_OVERRID, .periph = PERIPH_UART, .periphN = UART_1, .periphLine = UART_SIROUT_LINE}},
		[PIN_MUX_ANALOG] = {{.pinName = PB_10, .pinFunc = PIN_MUX_ANALOG, .periph = PERIPH_PORT, .periphN = 0, .periphLine = DIO_LINE}},
	},

	[D24] =
	{
		[PIN_MUX_GPIO] = {{.pinName = PA_7, .pinFunc = PIN_MUX_GPIO, .periph = PERIPH_PORT, .periphN = 0, .periphLine = DIO_LINE}},
		[PIN_MUX_MAIN] = {{.pinName = PA_7, .pinFunc = PIN_MUX_MAIN, .periph = PERIPH_PORT, .periphN = 0, .periphLine = DIO_LINE}},
		[PIN_MUX_ALTER] = {{.pinName = PA_7, .pinFunc = PIN_MUX_ALTER, .periph = PERIPH_CAN, .periphN = CAN_1, .periphLine = CAN_RXD_LINE}},
		[PIN_MUX_OVERRID] = {{.pinName = PA_7, .pinFunc = PIN_MUX_OVERRID, .periph = PERIPH_UART, .periphN = UART_1, .periphLine = UART_TXD_LINE}},
		[PIN_MUX_ANALOG] = {{.pinName = PA_7, .pinFunc = PIN_MUX_ANALOG, .periph = PERIPH_PORT, .periphN = 0, .periphLine = DIO_LINE}},
	},

	[D25] =
	{
		[PIN_MUX_GPIO] = {{.pinName = PA_6, .pinFunc = PIN_MUX_GPIO, .periph = PERIPH_PORT, .periphN = 0, .periphLine = DIO_LINE}},
		[PIN_MUX_MAIN] = {{.pinName = PA_6, .pinFunc = PIN_MUX_MAIN, .periph = PERIPH_PORT, .periphN = 0, .periphLine = DIO_LINE}},
		[PIN_MUX_ALTER] = {{.pinName = PA_6, .pinFunc = PIN_MUX_ALTER, .periph = PERIPH_CAN, .periphN = CAN_1, .periphLine = CAN_TXD_LINE}},
		[PIN_MUX_OVERRID] = {{.pinName = PA_6, .pinFunc = PIN_MUX_OVERRID, .periph = PERIPH_UART, .periphN = UART_1, .periphLine = UART_RXD_LINE}},
		[PIN_MUX_ANALOG] = {{.pinName = PA_6, .pinFunc = PIN_MUX_ANALOG, .periph = PERIPH_PORT, .periphN = 0, .periphLine = DIO_LINE}},
	},

	[D26] =
	{
		[PIN_MUX_GPIO] = {{.pinName = PA_5, .pinFunc = PIN_MUX_GPIO, .periph = PERIPH_PORT, .periphN = 0, .periphLine = DIO_LINE}},
		[PIN_MUX_MAIN] = {{.pinName = PA_5, .pinFunc = PIN_MUX_MAIN, .periph = PERIPH_PORT, .periphN = 0, .periphLine = DIO_LINE}},
		[PIN_MUX_ALTER] = {{.pinName = PA_5, .pinFunc = PIN_MUX_ALTER, .periph = PERIPH_PWM, .periphN = TIMER_1, .periphLine = TMR_CH3_LINE}},
		[PIN_MUX_OVERRID] = {{.pinName = PA_5, .pinFunc = PIN_MUX_OVERRID, .periph = PERIPH_TIMER, .periphN = TIMER_2, .periphLine = TMR_CH3_LINE}},
		[PIN_MUX_ANALOG] = {{.pinName = PA_5, .pinFunc = PIN_MUX_ANALOG, .periph = PERIPH_PORT, .periphN = 0, .periphLine = DIO_LINE}},
	},

	[D27] =
	{
		[PIN_MUX_GPIO] = {{.pinName = PA_4, .pinFunc = PIN_MUX_GPIO, .periph = PERIPH_PORT, .periphN = 0, .periphLine = DIO_LINE}},
		[PIN_MUX_MAIN] = {{.pinName = PA_4, .pinFunc = PIN_MUX_MAIN, .periph = PERIPH_PORT, .periphN = 0, .periphLine = DIO_LINE}},
		[PIN_MUX_ALTER] = {{.pinName = PA_4, .pinFunc = PIN_MUX_ALTER, .periph = PERIPH_TIMER, .periphN = TIMER_1, .periphLine = TMR_CH2_N_LINE}},
		[PIN_MUX_OVERRID] = {{.pinName = PA_4, .pinFunc = PIN_MUX_OVERRID, .periph = PERIPH_TONE, .periphN = TIMER_2, .periphLine = TMR_CH2_N_LINE}},
		[PIN_MUX_ANALOG] = {{.pinName = PA_4, .pinFunc = PIN_MUX_ANALOG, .periph = PERIPH_PORT, .periphN = 0, .periphLine = DIO_LINE}},
	},

	[D28] =
	{
		[PIN_MUX_GPIO] = {{.pinName = PA_3, .pinFunc = PIN_MUX_GPIO, .periph = PERIPH_PORT, .periphN = 0, .periphLine = DIO_LINE}},
		[PIN_MUX_MAIN] = {{.pinName = PA_3, .pinFunc = PIN_MUX_MAIN, .periph = PERIPH_PORT, .periphN = 0, .periphLine = DIO_LINE}},
		[PIN_MUX_ALTER] = {{.pinName = PA_3, .pinFunc = PIN_MUX_ALTER, .periph = PERIPH_PWM, .periphN = TIMER_1, .periphLine = TMR_CH2_LINE}},
		[PIN_MUX_OVERRID] = {{.pinName = PA_3, .pinFunc = PIN_MUX_OVERRID, .periph = PERIPH_TIMER, .periphN = TIMER_2, .periphLine = TMR_CH2_LINE}},
		[PIN_MUX_ANALOG] = {{.pinName = PA_3, .pinFunc = PIN_MUX_ANALOG, .periph = PERIPH_PORT, .periphN = 0, .periphLine = DIO_LINE}},
	},

	[D29] =
	{
		[PIN_MUX_GPIO] = {{.pinName = PA_2, .pinFunc = PIN_MUX_GPIO, .periph = PERIPH_PORT, .periphN = 0, .periphLine = DIO_LINE}},
		[PIN_MUX_MAIN] = {{.pinName = PA_2, .pinFunc = PIN_MUX_MAIN, .periph = PERIPH_PORT, .periphN = 0, .periphLine = DIO_LINE}},
		[PIN_MUX_ALTER] = {{.pinName = PA_2, .pinFunc = PIN_MUX_ALTER, .periph = PERIPH_TIMER, .periphN = TIMER_1, .periphLine = TMR_CH1_N_LINE}},
		[PIN_MUX_OVERRID] = {{.pinName = PA_2, .pinFunc = PIN_MUX_OVERRID, .periph = PERIPH_TONE, .periphN = TIMER_2, .periphLine = TMR_CH1_N_LINE}},
		[PIN_MUX_ANALOG] = {{.pinName = PA_2, .pinFunc = PIN_MUX_ANALOG, .periph = PERIPH_PORT, .periphN = 0, .periphLine = DIO_LINE}},
	},

	[D30] =
	{
		[PIN_MUX_GPIO] = {{.pinName = PA_1, .pinFunc = PIN_MUX_GPIO, .periph = PERIPH_PORT, .periphN = 0, .periphLine = DIO_LINE}},
		[PIN_MUX_MAIN] = {{.pinName = PA_1, .pinFunc = PIN_MUX_MAIN, .periph = PERIPH_PORT, .periphN = 0, .periphLine = DIO_LINE}},
		[PIN_MUX_ALTER] = {{.pinName = PA_1, .pinFunc = PIN_MUX_ALTER, .periph = PERIPH_TIMER, .periphN = TIMER_1, .periphLine = TMR_CH1_LINE}},
		[PIN_MUX_OVERRID] = {{.pinName = PA_1, .pinFunc = PIN_MUX_OVERRID, .periph = PERIPH_PWM, .periphN = TIMER_2, .periphLine = TMR_CH1_LINE}},
		[PIN_MUX_ANALOG] = {{.pinName = PA_1, .pinFunc = PIN_MUX_ANALOG, .periph = PERIPH_PORT, .periphN = 0, .periphLine = DIO_LINE}},
	},

	[D31] =
	{
		[PIN_MUX_GPIO] = {{.pinName = PA_0, .pinFunc = PIN_MUX_GPIO, .periph = PERIPH_PORT, .periphN = 0, .periphLine = DIO_LINE}},
		[PIN_MUX_MAIN] = {{.pinName = PA_0, .pinFunc = PIN_MUX_MAIN, .periph = PERIPH_PORT, .periphN = 0, .periphLine = DIO_LINE}},
		[PIN_MUX_ALTER] = {{.pinName = PA_0, .pinFunc = PIN_MUX_ALTER, .periph = PERIPH_PORT, .periphN = 0, .periphLine = DIO_EXT_INT1_LINE}},
		[PIN_MUX_OVERRID] = {{.pinName = PA_0, .pinFunc = PIN_MUX_OVERRID, .periph = PERIPH_PORT, .periphN = 0, .periphLine = DIO_LINE}},
		[PIN_MUX_ANALOG] = {{.pinName = PA_0, .pinFunc = PIN_MUX_ANALOG, .periph = PERIPH_PORT, .periphN = 0, .periphLine = DIO_LINE}},
	},

	[D32] =
	{
		[PIN_MUX_GPIO] = {{.pinName = PF_0, .pinFunc = PIN_MUX_GPIO, .periph = PERIPH_PORT, .periphN = 0, .periphLine = DIO_LINE}},
		[PIN_MUX_MAIN] = {{.pinName = PF_0, .pinFunc = PIN_MUX_MAIN, .periph = PERIPH_PORT, .periphN = 0, .periphLine = DIO_LINE}},
		[PIN_MUX_ALTER] = {{.pinName = PF_0, .pinFunc = PIN_MUX_ALTER, .periph = PERIPH_SSP, .periphN = SSP_1, .periphLine = SSP_TXD_LINE}},
		[PIN_MUX_OVERRID] = {{.pinName = PF_0, .pinFunc = PIN_MUX_OVERRID, .periph = PERIPH_UART, .periphN = UART_2, .periphLine = UART_RXD_LINE}},
		[PIN_MUX_ANALOG] = {{.pinName = PF_0, .pinFunc = PIN_MUX_ANALOG, .periph = PERIPH_PORT, .periphN = 0, .periphLine = DIO_LINE}},
	},

	[D33] =
	{
		[PIN_MUX_GPIO] = {{.pinName = PF_1, .pinFunc = PIN_MUX_GPIO, .periph = PERIPH_PORT, .periphN = 0, .periphLine = DIO_LINE}},
		[PIN_MUX_MAIN] = {{.pinName = PF_1, .pinFunc = PIN_MUX_MAIN, .periph = PERIPH_PORT, .periphN = 0, .periphLine = DIO_LINE}},
		[PIN_MUX_ALTER] = {{.pinName = PF_1, .pinFunc = PIN_MUX_ALTER, .periph = PERIPH_SSP, .periphN = SSP_1, .periphLine = SSP_CLK_LINE}},
		[PIN_MUX_OVERRID] = {{.pinName = PF_1, .pinFunc = PIN_MUX_OVERRID, .periph = PERIPH_UART, .periphN = UART_2, .periphLine = UART_TXD_LINE}},
		[PIN_MUX_ANALOG] = {{.pinName = PF_1, .pinFunc = PIN_MUX_ANALOG, .periph = PERIPH_PORT, .periphN = 0, .periphLine = DIO_LINE}},
	},

	[D34] =
	{
		[PIN_MUX_GPIO] = {{.pinName = PF_2, .pinFunc = PIN_MUX_GPIO, .periph = PERIPH_PORT, .periphN = 0, .periphLine = DIO_LINE}},
		[PIN_MUX_MAIN] = {{.pinName = PF_2, .pinFunc = PIN_MUX_MAIN, .periph = PERIPH_PORT, .periphN = 0, .periphLine = DIO_LINE}},
		[PIN_MUX_ALTER] = {{.pinName = PF_2, .pinFunc = PIN_MUX_ALTER, .periph = PERIPH_SSP, .periphN = SSP_1, .periphLine = SSP_FSS_LINE}},
		[PIN_MUX_OVERRID] = {{.pinName = PF_2, .pinFunc = PIN_MUX_OVERRID, .periph = PERIPH_CAN, .periphN = CAN_2, .periphLine = CAN_RXD_LINE}},
		[PIN_MUX_ANALOG] = {{.pinName = PF_2, .pinFunc = PIN_MUX_ANALOG, .periph = PERIPH_PORT, .periphN = 0, .periphLine = DIO_LINE}},
	},

	[D35] =
	{
		[PIN_MUX_GPIO] = {{.pinName = PF_3, .pinFunc = PIN_MUX_GPIO, .periph = PERIPH_PORT, .periphN = 0, .periphLine = DIO_LINE}},
		[PIN_MUX_MAIN] = {{.pinName = PF_3, .pinFunc = PIN_MUX_MAIN, .periph = PERIPH_PORT, .periphN = 0, .periphLine = DIO_LINE}},
		[PIN_MUX_ALTER] = {{.pinName = PF_3, .pinFunc = PIN_MUX_ALTER, .periph = PERIPH_SSP, .periphN = SSP_1, .periphLine = SSP_RXD_LINE}},
		[PIN_MUX_OVERRID] = {{.pinName = PF_3, .pinFunc = PIN_MUX_OVERRID, .periph = PERIPH_CAN, .periphN = CAN_2, .periphLine = CAN_TXD_LINE}},
		[PIN_MUX_ANALOG] = {{.pinName = PF_3, .pinFunc = PIN_MUX_ANALOG, .periph = PERIPH_PORT, .periphN = 0, .periphLine = DIO_LINE}},
	},

	[D36] =
	{
		[PIN_MUX_GPIO] = {{.pinName = PF_4, .pinFunc = PIN_MUX_GPIO, .periph = PERIPH_PORT, .periphN = 0, .periphLine = DIO_LINE}},
		[PIN_MUX_MAIN] = {{.pinName = PF_4, .pinFunc = PIN_MUX_MAIN, .periph = PERIPH_PORT, .periphN = 0, .periphLine = DIO_LINE}},
		[PIN_MUX_ALTER] = {{.pinName = PF_4, .pinFunc = PIN_MUX_ALTER, .periph = PERIPH_PORT, .periphN = 0, .periphLine = DIO_LINE}},
		[PIN_MUX_OVERRID] = {{.pinName = PF_4, .pinFunc = PIN_MUX_OVERRID, .periph = PERIPH_PORT, .periphN = 0, .periphLine = DIO_LINE}},
		[PIN_MUX_ANALOG] = {{.pinName = PF_4, .pinFunc = PIN_MUX_ANALOG, .periph = PERIPH_PORT, .periphN = 0, .periphLine = DIO_LINE}},
	},

	[D37] =
	{
		[PIN_MUX_GPIO] = {{.pinName = PF_5, .pinFunc = PIN_MUX_GPIO, .periph = PERIPH_PORT, .periphN = 0, .periphLine = DIO_LINE}},
		[PIN_MUX_MAIN] = {{.pinName = PF_5, .pinFunc = PIN_MUX_MAIN, .periph = PERIPH_PORT, .periphN = 0, .periphLine = DIO_LINE}},
		[PIN_MUX_ALTER] = {{.pinName = PF_5, .pinFunc = PIN_MUX_ALTER, .periph = PERIPH_PORT, .periphN = 0, .periphLine = DIO_LINE}},
		[PIN_MUX_OVERRID] = {{.pinName = PF_5, .pinFunc = PIN_MUX_OVERRID, .periph = PERIPH_PORT, .periphN = 0, .periphLine = DIO_LINE}},
		[PIN_MUX_ANALOG] = {{.pinName = PF_5, .pinFunc = PIN_MUX_ANALOG, .periph = PERIPH_PORT, .periphN = 0, .periphLine = DIO_LINE}},
	},

	[D38] =
	{
		[PIN_MUX_GPIO] = {{.pinName = PF_6, .pinFunc = PIN_MUX_GPIO, .periph = PERIPH_PORT, .periphN = 0, .periphLine = DIO_LINE}},
		[PIN_MUX_MAIN] = {{.pinName = PF_6, .pinFunc = PIN_MUX_MAIN, .periph = PERIPH_PORT, .periphN = 0, .periphLine = DIO_LINE}},
		[PIN_MUX_ALTER] = {{.pinName = PF_6, .pinFunc = PIN_MUX_ALTER, .periph = PERIPH_PWM, .periphN = TIMER_1, .periphLine = TMR_CH1_LINE}},
		[PIN_MUX_OVERRID] = {{.pinName = PF_6, .pinFunc = PIN_MUX_OVERRID, .periph = PERIPH_PORT, .periphN = 0, .periphLine = DIO_LINE}},
		[PIN_MUX_ANALOG] = {{.pinName = PF_6, .pinFunc = PIN_MUX_ANALOG, .periph = PERIPH_PORT, .periphN = 0, .periphLine = DIO_LINE}},
	},

#if USE_SWD_PINS
	[D39] =
	{
		[PIN_MUX_GPIO] = {{.pinName = PD_0, .pinFunc = PIN_MUX_GPIO, .periph = PERIPH_PORT, .periphN = 0, .periphLine = DIO_LINE}},
		[PIN_MUX_MAIN] = {{.pinName = PD_0, .pinFunc = PIN_MUX_MAIN, .periph = PERIPH_TIMER, .periphN = TIMER_1, .periphLine = TMR_CH1_N_LINE}},
		[PIN_MUX_ALTER] = {{.pinName = PD_0, .pinFunc = PIN_MUX_ALTER, .periph = PERIPH_UART, .periphN = UART_2, .periphLine = UART_RXD_LINE}},
		[PIN_MUX_OVERRID] = {{.pinName = PD_0, .pinFunc = PIN_MUX_OVERRID, .periph = PERIPH_TIMER, .periphN = TIMER_3, .periphLine = TMR_CH1_LINE}},
		[PIN_MUX_ANALOG] = {{.pinName = PD_0, .pinFunc = PIN_MUX_ANALOG, .periph = PERIPH_ADC, .periphN = ADC_2, .periphLine = ADC_CH0_LINE}},
	},

	[D40] =
	{
		[PIN_MUX_GPIO] = {{.pinName = PD_1, .pinFunc = PIN_MUX_GPIO, .periph = PERIPH_PORT, .periphN = 0, .periphLine = DIO_LINE}},
		[PIN_MUX_MAIN] = {{.pinName = PD_1, .pinFunc = PIN_MUX_MAIN, .periph = PERIPH_TIMER, .periphN = TIMER_1, .periphLine = TMR_CH1_LINE}},
		[PIN_MUX_ALTER] = {{.pinName = PD_1, .pinFunc = PIN_MUX_ALTER, .periph = PERIPH_UART, .periphN = UART_2, .periphLine = UART_TXD_LINE}},
		[PIN_MUX_OVERRID] = {{.pinName = PD_1, .pinFunc = PIN_MUX_OVERRID, .periph = PERIPH_TIMER, .periphN = TIMER_3, .periphLine = TMR_CH1_N_LINE}},
		[PIN_MUX_ANALOG] = {{.pinName = PD_1, .pinFunc = PIN_MUX_ANALOG, .periph = PERIPH_ADC, .periphN = ADC_2, .periphLine = ADC_CH1_LINE}},
	},
#endif
};

//------------------------------------------------------------------------------
// Повышение частоты шины HCLK до максимальной 80 МГц (по умолчанию 8 МГц)
//------------------------------------------------------------------------------
void initVariant()
{
	/* Set RST_CLK to default */
	/* Also resets DUcc voltage regulator work mode */
	RST_CLK_DeInit();
	SystemCoreClockUpdate();

	/* Включаем внешний источник тактирования - кварц 8 МГц */
	RST_CLK_HSEconfig(RST_CLK_HSE_ON);

	/* Good HSE clock */
	if (RST_CLK_HSEstatus() == SUCCESS)
	{
		RST_CLK_CPUclkSelectionC1(RST_CLK_CPU_C1srcHSEdiv1);

		/* Select HSE clock as CPU_PLL input clock source */
		/* Set PLL multiplier to 10                       */
		/* Увеличиваем частоту CPU_C1 до 80 МГц           */
		RST_CLK_CPU_PLLconfig(RST_CLK_CPU_PLLsrcHSEdiv1, RST_CLK_CPU_PLLmul10);
	}
	else
	{
		/* Работаем от внутреннего источника тактирования - HSI 8 МГц */
		RST_CLK_HSEconfig(RST_CLK_HSE_OFF);

		/* Select HSI clock as CPU_C1 input clock source */
		RST_CLK_CPUclkSelectionC1(RST_CLK_CPU_C1srcHSIdiv1);

		/* Select HSI clock as CPU_PLL input clock source */
		/* Set PLL multiplier to 10                       */
		/* Увеличиваем частоту CPU_C1 до 80 МГц           */
		RST_CLK_CPU_PLLconfig(RST_CLK_CPU_PLLsrcHSIdiv1, RST_CLK_CPU_PLLmul10);
	}

	/* Enable CPU_PLL */
	RST_CLK_CPU_PLLcmd(ENABLE);

	/* Good CPU PLL */
	if (RST_CLK_CPU_PLLstatus() == SUCCESS)
	{
		/* Set CPU_C3_prescaler to 2 */
		RST_CLK_CPUclkPrescaler(RST_CLK_CPUclkDIV1);
		/* Set CPU_C2_SEL to CPU_PLL output instead of CPU_C1 clock */
		RST_CLK_CPU_PLLuse(ENABLE);

		/* Setup internal DUcc voltage regulator work mode based on clock frequency */
		BKP_DUccMode(BKP_DUcc_upto_80MHz);
		/* Setup EEPROM access delay to 1: 10*HSE/1 = 80MHz > 75MHz */
		EEPROM_SetLatency(EEPROM_Latency_3);

		/* Select CPU_C3 clock on the CPU clock MUX */
		RST_CLK_CPUclkSelection(RST_CLK_CPUclkCPU_C3);
	}
	else
	{
		/* Работаем от внутреннего источника тактирования -
		 * HSI 8 МГц без умножения частоты                */

		/* Enable CPU_PLL */
		RST_CLK_CPU_PLLcmd(DISABLE);

		/* Setup internal DUcc voltage regulator work mode based on clock frequency */
		BKP_DUccMode(BKP_DUcc_upto_10MHz);
		/* Setup EEPROM access delay to 0: HSI = 8MHz < 25MHz */
		EEPROM_SetLatency(EEPROM_Latency_0);

		/* Select HSI clock on the CPU clock MUX */
		RST_CLK_CPUclkSelection(RST_CLK_CPUclkHSI);
	}

	SystemCoreClockUpdate();
}

