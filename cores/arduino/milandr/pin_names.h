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

#ifndef _PIN_MAMES_H_
#define _PIN_MAMES_H_

#include "MDR32FxQI_config.h"

//------------------------------------------------------------------------------
// Псевдонимы выводов порта, по которым можно идетифицировать номер порта и пина:
// Биты <7..4> - идентифицируют номер порта PORTA = 0, PORTB = 1 и т.д.
// Биты <3..0> - идентифицируют номер пина внутри порта
//------------------------------------------------------------------------------
typedef enum
{
#if defined(MDR_PORTA)
	PA_0  = 0x00,
	PA_1  = 0x01,
	PA_2  = 0x02,
	PA_3  = 0x03,
	PA_4  = 0x04,
	PA_5  = 0x05,
	PA_6  = 0x06,
	PA_7  = 0x07,
	PA_8  = 0x08,
	PA_9  = 0x09,
	PA_10 = 0x0A,
	PA_11 = 0x0B,
	PA_12 = 0x0C,
	PA_13 = 0x0D,
	PA_14 = 0x0E,
	PA_15 = 0x0F,
#endif

#if defined(MDR_PORTB)
	PB_0  = 0x10,
	PB_1  = 0x11,
	PB_2  = 0x12,
	PB_3  = 0x13,
	PB_4  = 0x14,
	PB_5  = 0x15,
	PB_6  = 0x16,
	PB_7  = 0x17,
	PB_8  = 0x18,
	PB_9  = 0x19,
	PB_10 = 0x1A,
	PB_11 = 0x1B,
	PB_12 = 0x1C,
	PB_13 = 0x1D,
	PB_14 = 0x1E,
	PB_15 = 0x1F,
#endif

#if defined(MDR_PORTC)
	PC_0  = 0x20,
	PC_1  = 0x21,
	PC_2  = 0x22,
	PC_3  = 0x23,
	PC_4  = 0x24,
	PC_5  = 0x25,
	PC_6  = 0x26,
	PC_7  = 0x27,
	PC_8  = 0x28,
	PC_9  = 0x29,
	PC_10 = 0x2A,
	PC_11 = 0x2B,
	PC_12 = 0x2C,
	PC_13 = 0x2D,
	PC_14 = 0x2E,
	PC_15 = 0x2F,
#endif

#if defined(MDR_PORTD)
	PD_0  = 0x30,
	PD_1  = 0x31,
	PD_2  = 0x32,
	PD_3  = 0x33,
	PD_4  = 0x34,
	PD_5  = 0x35,
	PD_6  = 0x36,
	PD_7  = 0x37,
	PD_8  = 0x38,
	PD_9  = 0x39,
	PD_10 = 0x3A,
	PD_11 = 0x3B,
	PD_12 = 0x3C,
	PD_13 = 0x3D,
	PD_14 = 0x3E,
	PD_15 = 0x3F,
#endif

#if defined(MDR_PORTE)
	PE_0  = 0x40,
	PE_1  = 0x41,
	PE_2  = 0x42,
	PE_3  = 0x43,
	PE_4  = 0x44,
	PE_5  = 0x45,
	PE_6  = 0x46,
	PE_7  = 0x47,
	PE_8  = 0x48,
	PE_9  = 0x49,
	PE_10 = 0x4A,
	PE_11 = 0x4B,
	PE_12 = 0x4C,
	PE_13 = 0x4D,
	PE_14 = 0x4E,
	PE_15 = 0x4F,
#endif

#if defined(MDR_PORTF)
	PF_0  = 0x50,
	PF_1  = 0x51,
	PF_2  = 0x52,
	PF_3  = 0x53,
	PF_4  = 0x54,
	PF_5  = 0x55,
	PF_6  = 0x56,
	PF_7  = 0x57,
	PF_8  = 0x58,
	PF_9  = 0x59,
	PF_10 = 0x5A,
	PF_11 = 0x5B,
	PF_12 = 0x5C,
	PF_13 = 0x5D,
	PF_14 = 0x5E,
	PF_15 = 0x5F,
#endif

#if defined(MDR_PORTG)
	PG_0  = 0x60,
	PG_1  = 0x61,
	PG_2  = 0x62,
	PG_3  = 0x63,
	PG_4  = 0x64,
	PG_5  = 0x65,
	PG_6  = 0x66,
	PG_7  = 0x67,
	PG_8  = 0x68,
	PG_9  = 0x69,
	PG_10 = 0x6A,
	PG_11 = 0x6B,
	PG_12 = 0x6C,
	PG_13 = 0x6D,
	PG_14 = 0x6E,
	PG_15 = 0x6F,
#endif
	PIN_NAMES_NUM,
	NC = 0xFF
}
tPinName;

//------------------------------------------------------------------------------
// Нумерация портов
//------------------------------------------------------------------------------
typedef enum
{
#if defined(MDR_PORTA)
	MDR_PORT_A = 0,
#endif

#if defined(MDR_PORTB)
	MDR_PORT_B = 1,
#endif

#if defined(MDR_PORTC)
	MDR_PORT_C = 2,
#endif

#if defined(MDR_PORTD)
	MDR_PORT_D = 3,
#endif

#if defined(MDR_PORTE)
	MDR_PORT_E = 4,
#endif

#if defined(MDR_PORTF)
	MDR_PORT_F = 5,
#endif

#if defined(MDR_PORTG)
	MDR_PORT_G = 6,
#endif
}
tPortNumber;

#endif //_PIN_MAMES_H_
