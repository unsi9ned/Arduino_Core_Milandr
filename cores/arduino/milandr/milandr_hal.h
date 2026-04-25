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

#ifndef _MILANDR_HAL_H_
#define _MILANDR_HAL_H_

#include <stdint.h>
#include <stdbool.h>

#include "periph_definition.h"
#include "milandr_gpio.h"
#include "milandr_uart.h"

#ifdef __cplusplus
extern "C" {
#endif

//------------------------------------------------------------------------------
// Глобальные переменные, которые должны быть заданы в variant.h
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
// Экспортируемые функции
//------------------------------------------------------------------------------
extern void milandr_hal_init(void);
extern void milandr_enter_critical(void);
extern void milandr_exit_critical(void);
extern void milandr_systick_config(void);
extern uint32_t milandr_current_millis(void);
extern uint32_t milandr_current_micros(void);
extern void milandr_delay_microseconds(uint32_t us);
extern void milandr_delay(uint32_t ms);
extern uint8_t milandr_find_pin(tPeriphVariant periph,
                                tPeriphLineVariant line,
                                uint8_t periphN,
                                const tMilandrPin ** set,
                                const tMilandrPin ** sorted,
                                int8_t variantNum);

#ifdef __cplusplus
}
#endif

#endif //_MILANDR_HAL_H_
