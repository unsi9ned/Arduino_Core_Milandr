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

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif


//------------------------------------------------------------------------------
// Экспортируемые функции
//------------------------------------------------------------------------------
extern void milandr_systick_config(void);
extern uint32_t milandr_current_millis(void);
extern uint32_t milandr_current_micros(void);
extern void milandr_delay_microseconds(uint32_t us);
extern void milandr_delay(uint32_t ms);

extern uint8_t milandr_max_dio(void);

extern void milandr_gpio_clock_enable(uint8_t arduinoPin);
extern void milandr_gpio_cfg_input(uint8_t arduinoPin);
extern void milandr_gpio_cfg_input_pu(uint8_t arduinoPin);
extern void milandr_gpio_cfg_input_pd(uint8_t arduinoPin);
extern void milandr_gpio_cfg_output_pp(uint8_t arduinoPin);
extern void milandr_gpio_cfg_output_od(uint8_t arduinoPin);
extern void milandr_gpio_write(uint8_t arduinoPin, uint8_t level);
extern uint8_t milandr_gpio_read(uint8_t arduinoPin);


#ifdef __cplusplus
}
#endif

#endif //_MILANDR_GPIO_H
