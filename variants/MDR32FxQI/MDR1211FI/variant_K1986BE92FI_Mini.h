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

          PWM8                       PWM7  PWM6  PWM5              PWM4
           |                          |     |     |                 |
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
              |        |  |  |  |  |  |  | P11   P13   P15   P17 | P19
              |       AN0 | AN2 | AN4 |  |              |        |
              |          AN1   AN3   AN5 |              |        |
             PWM0                       PWM1           PWM2     PWM3

------------------------------------------------------------------------------*/

#ifndef USE_SWD_PINS
#define USE_SWD_PINS   0
#endif

#ifndef PIN_NC
#define PIN_NC         0xFF
#endif

//------------------------------------------------------------------------------
// Arduino digital pins alias
//------------------------------------------------------------------------------
typedef enum
{
    D0 = 0, D1,   D2,   D3,   D4,   D5,   D6,   D7,   D8,   D9,
    D10,    D11,  D12,  D13,  D14,  D15,  D16,  D17,  D18,  D19,
    D20,    D21,  D22,  D23,  D24,  D25,  D26,  D27,  D28,  D29,
    D30,    D31,  D32,  D33,  D34,  D35,  D36,  D37,  D38,
#if USE_SWD_PINS
    D39,    D40,
#endif
    DMAX
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
#define PD4   D5
#define PD2   D6
#define PD3   D7
#define PD5   D8
#define PD6   D9
#define PC2   D10
#define PC1   D11
#define PC0   D12
#define PB0   D13
#define PB1   D14
#define PB2   D15
#define PB3   D16
#define PB4   D17
#define PB5   D18
#define PB6   D19

//------------------------------------------------------------------------------
// Верхний ряд пинов
//------------------------------------------------------------------------------
#define PB7   D20
#define PB8   D21
#define PB9   D22
#define PB10  D23
#define PA7   D24
#define PA6   D25
#define PA5   D26
#define PA4   D27
#define PA3   D28
#define PA2   D29
#define PA1   D30
#define PA0   D31
#define PF0   D32
#define PF1   D33
#define PF2   D34
#define PF3   D35
#define PF4   D36
#define PF5   D37
#define PF6   D38

//------------------------------------------------------------------------------
// Разъем программирования
//------------------------------------------------------------------------------
#if USE_SWD_PINS

#define PD0   D39
#define PD1   D40

#endif

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

#ifndef SERIAL1_RX_BUFFER_SIZE
#define SERIAL1_RX_BUFFER_SIZE      128
#endif

#ifndef SERIAL1_TX_BUFFER_SIZE
#define SERIAL1_TX_BUFFER_SIZE      128
#endif

/* UART2 */
#ifndef SERIAL2_RX
#define SERIAL2_RX                  PF0
#endif

#ifndef SERIAL2_TX
#define SERIAL2_TX                  PF1
#endif

#ifndef SERIAL2_RX_BUFFER_SIZE
#define SERIAL2_RX_BUFFER_SIZE      128
#endif

#ifndef SERIAL2_TX_BUFFER_SIZE
#define SERIAL2_TX_BUFFER_SIZE      128
#endif

//------------------------------------------------------------------------------
// SPI modules
//------------------------------------------------------------------------------

/* SSP1 definitions */
#define PIN_SPI1_SS                 PF2
#define PIN_SPI1_MOSI               PF0
#define PIN_SPI1_MISO               PF3
#define PIN_SPI1_SCK                PF1

/* SSP2 definitions */
#define PIN_SPI2_SS                 PD3
#define PIN_SPI2_MOSI               PD6
#define PIN_SPI2_MISO               PD2
#define PIN_SPI2_SCK                PD5

//------------------------------------------------------------------------------
// I2C modules
//------------------------------------------------------------------------------

#define PIN_I2C1_SCL                PC0
#define PIN_I2C1_SDA                PC1

#ifndef I2C1_RX_BUFFER_SIZE
#define I2C1_RX_BUFFER_SIZE         32
#endif

#ifndef I2C1_TX_BUFFER_SIZE
#define I2C1_TX_BUFFER_SIZE         32
#endif

/* Таймаут операции на шине I2C. Задает количество циклов while */
#ifndef TWI_TIMEOUT
#define TWI_TIMEOUT                 0xFFFFUL
#endif

//------------------------------------------------------------------------------
// PWM Outputs
//------------------------------------------------------------------------------

#define PWM0                        PE2     //TMR2_CH3
#define PWM1                        PC2     //TMR3_CH1
#define PWM2                        PB2     //TMR3_CH2
#define PWM3                        PB5     //TMR3_CH3
#define PWM4                        PB7     //TMR3_CH4
#define PWM5                        PA5     //TMR1_CH3
#define PWM6                        PA3     //TMR1_CH2
#define PWM7                        PA1     //TMR2_CH1
#define PWM8                        PF6     //TMR1_CH1

//------------------------------------------------------------------------------
// DAC Outputs
//------------------------------------------------------------------------------

#define PIN_DAC2_OUT                PE0

//------------------------------------------------------------------------------
// ADC Inputs
//------------------------------------------------------------------------------

#define AIN0                        PD7
#define AIN1                        PD4
#define AIN2                        PD2
#define AIN3                        PD3
#define AIN4                        PD5
#define AIN5                        PD6

#if USE_SWD_PINS

#define AIN6                        PD0
#define AIN7                        PD1

#endif

#define AIN_TEMP                    PIN_NC

#endif //_VARIANT_K1986BE92FI_MINI
