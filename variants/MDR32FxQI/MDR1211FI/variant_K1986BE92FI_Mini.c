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

#include "variant_K1986BE92FI_Mini.h"
#include "milandr/pin_names.h"
#include "milandr/periph_definition.h"

/*---------------------K1986BE92FI_Mini pins number-----------------------------

             P32      P30   P28   P26   P24   P22   P20   P18   P16
          P33 | P31    | P29 | P27 | P25 | P23 | P21 | P19 | P17 | P15
           |  |  |     |  |  |  |  |  |  |  |  |  |  |  |  |  |  |  |
          86 85 84    83 82 81 80  0  1  2  3  4  5  6  7 26 25 24 23
           |  |  |     |  |  |  |  |  |  |  |  |  |  |  |  |  |  |  |
     G 5V F6 F5 F4 WP F3 F2 F1 F0 A0 A1 A2 A3 A4 A5 A6 A7 B10B9 B8 B7 3V3 G
     |  |  |  |  |  |  |  |  |  |  |  |  |  |  |  |  |  |  |  |  |  |  |  |
   +------------------------------------------------------------------------+
   |                                                                        |
  ┌-----┐                                                                   |+-- G
  | USB |                                                                   |+-- D1--49--P35--AN1
  └-----┘                                                                   |+-- D0--48--P34--AN0
   |                                                                        |+-- 3V
   +------------------------------------------------------------------------+
     |  |  |  |  |  |  |  |  |  |  |  |  |  |  |  |  |  |  |  |  |  |  |  |
    VB  R E3 E2 E1 E0 D7 D4 D2 D3 D5 D6 C2 C1 C0 B0 B1 B2 B3 B4 B5 B6 3V3 G
           |  |  |  |  |  |  |  |  |  |  |  |  |  |  |  |  |  |  |  |
          67 66 65 64 55 52 50 51 53 54 34 33 32 16 17 18 19 20 21 22
           |  |  |  |  |  |  |  |  |  |  |  |  |  |  |  |  |  |  |  |
          P0 P1 P2 P3 P4  |  |  |  |  | P5 P6 P7 P8 P9 P10 | P12 | P14
                          |  |  |  |  |                   P11   P13
                          | AN2 | AN5 |
                         AN4   AN3   AN6


------------------------------------------------------------------------------*/

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
		[PIN_MUX_ALTER] = {{.pinName = PE_3, .pinFunc = PIN_MUX_ALTER, .periph = PERIPH_TIMER, .periphN = TIMER_2, .periphLine = TMR_CH3_N_LINE}},
		[PIN_MUX_OVERRID] = {{.pinName = PE_3, .pinFunc = PIN_MUX_OVERRID, .periph = PERIPH_TIMER, .periphN = TIMER_3, .periphLine = TMR_CH1_N_LINE}},
		[PIN_MUX_ANALOG] = {{.pinName = PE_3, .pinFunc = PIN_MUX_ANALOG, .periph = PERIPH_COMP, .periphN = 0, .periphLine = COMP_IN2_LINE}},
	},

	[D1] =
	{
		[PIN_MUX_GPIO] = {{.pinName = PE_2, .pinFunc = PIN_MUX_GPIO, .periph = PERIPH_PORT, .periphN = 0, .periphLine = DIO_LINE}},
		[PIN_MUX_MAIN] = {{.pinName = PE_2, .pinFunc = PIN_MUX_MAIN, .periph = PERIPH_PORT, .periphN = 0, .periphLine = DIO_LINE}},
		[PIN_MUX_ALTER] = {{.pinName = PE_2, .pinFunc = PIN_MUX_ALTER, .periph = PERIPH_TIMER, .periphN = TIMER_2, .periphLine = TMR_CH3_LINE}},
		[PIN_MUX_OVERRID] = {{.pinName = PE_2, .pinFunc = PIN_MUX_OVERRID, .periph = PERIPH_TIMER, .periphN = TIMER_3, .periphLine = TMR_CH1_LINE}},
		[PIN_MUX_ANALOG] = {{.pinName = PE_2, .pinFunc = PIN_MUX_ANALOG, .periph = PERIPH_COMP, .periphN = 0, .periphLine = COMP_IN1_LINE}},
	},

	//...

	[D14] =
	{
		[PIN_MUX_GPIO] = {{.pinName = PB_6, .pinFunc = PIN_MUX_GPIO, .periph = PERIPH_PORT, .periphN = 0, .periphLine = DIO_LINE}},
		[PIN_MUX_MAIN] = {{.pinName = PB_6, .pinFunc = PIN_MUX_MAIN, .periph = PERIPH_PORT, .periphN = 0, .periphLine = DIO_LINE}},
		[PIN_MUX_ALTER] = {{.pinName = PB_6, .pinFunc = PIN_MUX_ALTER, .periph = PERIPH_UART, .periphN = UART_1, .periphLine = UART_RXD_LINE}},
		[PIN_MUX_OVERRID] = {{.pinName = PB_6, .pinFunc = PIN_MUX_OVERRID, .periph = PERIPH_TIMER, .periphN = TIMER_3, .periphLine = TMR_CH3_N_LINE}},
		[PIN_MUX_ANALOG] = {{.pinName = PB_6, .pinFunc = PIN_MUX_ANALOG, .periph = PERIPH_PORT, .periphN = 0, .periphLine = DIO_LINE}},
	},

	[D15] =
	{
		[PIN_MUX_GPIO] = {{.pinName = PB_7, .pinFunc = PIN_MUX_GPIO, .periph = PERIPH_PORT, .periphN = 0, .periphLine = DIO_LINE}},
		[PIN_MUX_MAIN] = {{.pinName = PB_7, .pinFunc = PIN_MUX_MAIN, .periph = PERIPH_PORT, .periphN = 0, .periphLine = DIO_LINE}},
		[PIN_MUX_ALTER] = {{.pinName = PB_7, .pinFunc = PIN_MUX_ALTER, .periph = PERIPH_UART, .periphN = UART_1, .periphLine = UART_SIROUT_LINE}},
		[PIN_MUX_OVERRID] = {{.pinName = PB_7, .pinFunc = PIN_MUX_OVERRID, .periph = PERIPH_TIMER, .periphN = TIMER_3, .periphLine = TMR_CH4_LINE}},
		[PIN_MUX_ANALOG] = {{.pinName = PB_7, .pinFunc = PIN_MUX_ANALOG, .periph = PERIPH_PORT, .periphN = 0, .periphLine = DIO_LINE}},
	},

	//...

	[D19] =
	{
		[PIN_MUX_GPIO] = {{.pinName = PA_7, .pinFunc = PIN_MUX_GPIO, .periph = PERIPH_PORT, .periphN = 0, .periphLine = DIO_LINE}},
		[PIN_MUX_MAIN] = {{.pinName = PA_7, .pinFunc = PIN_MUX_MAIN, .periph = PERIPH_PORT, .periphN = 0, .periphLine = DIO_LINE}},
		[PIN_MUX_ALTER] = {{.pinName = PA_7, .pinFunc = PIN_MUX_ALTER, .periph = PERIPH_CAN, .periphN = CAN_1, .periphLine = CAN_RXD_LINE}},
		[PIN_MUX_OVERRID] = {{.pinName = PA_7, .pinFunc = PIN_MUX_OVERRID, .periph = PERIPH_UART, .periphN = UART_1, .periphLine = UART_TXD_LINE}},
		[PIN_MUX_ANALOG] = {{.pinName = PA_7, .pinFunc = PIN_MUX_ANALOG, .periph = PERIPH_PORT, .periphN = 0, .periphLine = DIO_LINE}},
	},

	[D20] =
	{
		[PIN_MUX_GPIO] = {{.pinName = PA_6, .pinFunc = PIN_MUX_GPIO, .periph = PERIPH_PORT, .periphN = 0, .periphLine = DIO_LINE}},
		[PIN_MUX_MAIN] = {{.pinName = PA_6, .pinFunc = PIN_MUX_MAIN, .periph = PERIPH_PORT, .periphN = 0, .periphLine = DIO_LINE}},
		[PIN_MUX_ALTER] = {{.pinName = PA_6, .pinFunc = PIN_MUX_ALTER, .periph = PERIPH_CAN, .periphN = CAN_1, .periphLine = CAN_TXD_LINE}},
		[PIN_MUX_OVERRID] = {{.pinName = PA_6, .pinFunc = PIN_MUX_OVERRID, .periph = PERIPH_UART, .periphN = UART_1, .periphLine = UART_RXD_LINE}},
		[PIN_MUX_ANALOG] = {{.pinName = PA_6, .pinFunc = PIN_MUX_ANALOG, .periph = PERIPH_PORT, .periphN = 0, .periphLine = DIO_LINE}},
	},

	// ...

	[D27] =
	{
		[PIN_MUX_GPIO] = {{.pinName = PF_0, .pinFunc = PIN_MUX_GPIO, .periph = PERIPH_PORT, .periphN = 0, .periphLine = DIO_LINE}},
		[PIN_MUX_MAIN] = {{.pinName = PF_0, .pinFunc = PIN_MUX_MAIN, .periph = PERIPH_PORT, .periphN = 0, .periphLine = DIO_LINE}},
		[PIN_MUX_ALTER] = {{.pinName = PF_0, .pinFunc = PIN_MUX_ALTER, .periph = PERIPH_SSP, .periphN = SSP_1, .periphLine = SSP_TXD_LINE}},
		[PIN_MUX_OVERRID] = {{.pinName = PF_0, .pinFunc = PIN_MUX_OVERRID, .periph = PERIPH_UART, .periphN = UART_2, .periphLine = UART_RXD_LINE}},
		[PIN_MUX_ANALOG] = {{.pinName = PF_0, .pinFunc = PIN_MUX_ANALOG, .periph = PERIPH_PORT, .periphN = 0, .periphLine = DIO_LINE}},
	},

	[D28] =
	{
		[PIN_MUX_GPIO] = {{.pinName = PF_1, .pinFunc = PIN_MUX_GPIO, .periph = PERIPH_PORT, .periphN = 0, .periphLine = DIO_LINE}},
		[PIN_MUX_MAIN] = {{.pinName = PF_1, .pinFunc = PIN_MUX_MAIN, .periph = PERIPH_PORT, .periphN = 0, .periphLine = DIO_LINE}},
		[PIN_MUX_ALTER] = {{.pinName = PF_1, .pinFunc = PIN_MUX_ALTER, .periph = PERIPH_SSP, .periphN = SSP_1, .periphLine = SSP_CLK_LINE}},
		[PIN_MUX_OVERRID] = {{.pinName = PF_1, .pinFunc = PIN_MUX_OVERRID, .periph = PERIPH_UART, .periphN = UART_2, .periphLine = UART_TXD_LINE}},
		[PIN_MUX_ANALOG] = {{.pinName = PF_1, .pinFunc = PIN_MUX_ANALOG, .periph = PERIPH_PORT, .periphN = 0, .periphLine = DIO_LINE}},
	},
};

void initVariant()
{

}

