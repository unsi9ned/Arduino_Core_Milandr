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

#ifndef _VARIANT_K1986BE92FI_MINI
#define _VARIANT_K1986BE92FI_MINI

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
// Arduino digital pins alias
//------------------------------------------------------------------------------
typedef enum
{
    D0 = 0, D1,   D2,   D3,   D4,   D5,   D6,   D7,   D8,   D9,
    D10,    D11,  D12,  D13,  D14,  D15,  D16,  D17,  D18,  D19,
    D20,    D21,  D22,  D23,  D24,  D25,  D26,  D27,  D28,  D29,
    D30,    D31,  D32,  D33,  D34,  D35,
    DMAX,
}
tArduinoPinAlias;

//------------------------------------------------------------------------------
// Нижний ряд пинов
//------------------------------------------------------------------------------
#define PE3   D0
#define PE2   D1
#define PE1   D2
#define PE0   D3
#define PD7   D4
#define PC2   D5
#define PC1   D6
#define PC0   D7
#define PB0   D8
#define PB1   D9
#define PB2   D10
#define PB3   D11
#define PB4   D12
#define PB5   D13
#define PB6   D14

//------------------------------------------------------------------------------
// Верхний ряд пинов
//------------------------------------------------------------------------------
#define PB7   D15
#define PB8   D16
#define PB9   D17
#define PB10  D18
#define PA7   D19
#define PA6   D20
#define PA5   D21
#define PA4   D22
#define PA3   D23
#define PA2   D24
#define PA1   D25
#define PA0   D26
#define PF0   D27
#define PF1   D28
#define PF2   D29
#define PF3   D30
#define PF4   D31
#define PF5   D32
#define PF6   D33

//------------------------------------------------------------------------------
// Разъем программирования
//------------------------------------------------------------------------------
#define PD0   D34
#define PD1   D35

//------------------------------------------------------------------------------
// User LED
//------------------------------------------------------------------------------
#define LED_BUILTIN                 PB7

//------------------------------------------------------------------------------
// User BUTTON
//------------------------------------------------------------------------------
#define USER_BUTTON                 PB6

//------------------------------------------------------------------------------
// UART modules
//------------------------------------------------------------------------------

/* UART1 */
#ifndef SERIAL1_RX
#define SERIAL1_RX                  PA6
#endif

#ifndef SERIAL1_TX
#define SERIAL1_TX                  PA7
#endif

/* UART2 */
#ifndef SERIAL2_RX
#define SERIAL2_RX                  PF0
#endif

#ifndef SERIAL2_TX
#define SERIAL2_TX                  PF1
#endif


#endif //_VARIANT_K1986BE92FI_MINI
