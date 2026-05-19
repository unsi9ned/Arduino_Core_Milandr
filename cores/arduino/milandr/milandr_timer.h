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

#ifndef _MILANDR_TIMER_H_
#define _MILANDR_TIMER_H_

#include <stdint.h>
#include <stddef.h>
#include "periph_definition.h"

#ifdef __cplusplus
extern "C" {
#endif

//------------------------------------------------------------------------------
// Экспортируемые функции
//------------------------------------------------------------------------------
extern bool milandr_pwm_set_value(uint8_t pin, int value);
extern void milandr_pwm_set_resolution(uint8_t resolution);

#ifdef __cplusplus
}
#endif

#endif  //_MILANDR_TIMER_H_
