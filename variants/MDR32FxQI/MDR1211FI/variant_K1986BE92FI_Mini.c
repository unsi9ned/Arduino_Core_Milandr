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

#include "milandr/periph_definition.h"
#include "variant_K1986BE92FI_Mini.h"

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
// Полная таблица пинов с их функциональным назначением
//------------------------------------------------------------------------------
const tMilandrPin pinTable[DMAX][PERIPH_LINE_VARIANTS_NUM][1] =
{
	[D0] =
	{
		[DIO_LINE] = {{.pinName = PE_3, .altFunc = PORT_FUNC_PORT, .pinMode = PORT_MODE_DIGITAL}},
		[COMP_IN2_LINE] = {{.pinName = PE_3, .altFunc = PORT_FUNC_PORT, .pinMode = PORT_MODE_ANALOG}},
		[TMR_CH_N_LINE] = {{.pinName = PE_3, .altFunc = PORT_FUNC_ALTER, .pinMode = PORT_MODE_DIGITAL, .altPeriph = MDR_TIMER2}},
	}
};
