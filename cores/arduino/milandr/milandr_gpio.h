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

#ifndef _MILANDR_GPIO_H
#define _MILANDR_GPIO_H

#include "MDR32FxQI_config.h"

//------------------------------------------------------------------------------
// Битовые маски для работы полями структуры tMilandrPin
//------------------------------------------------------------------------------
#define PIN_NUMBER_MASK   0xFUL
#define PIN_PORTNUM_MASK  0xF0UL
#define PIN_FUNC_MASK     0x700UL

//------------------------------------------------------------------------------
// Доступ к блоку регистров GPIO
//------------------------------------------------------------------------------
#define MDR_PORT(n)      (MDR_PORT_TypeDef    *)((uint32_t)MDR_PORTA + 0x8000UL * (n))


#endif //_MILANDR_GPIO_H
