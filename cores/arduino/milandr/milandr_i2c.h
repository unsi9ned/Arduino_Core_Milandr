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

#ifndef _MILANDR_I2C_H_
#define _MILANDR_I2C_H_

#include <stdint.h>
#include <stddef.h>
#include "periph_definition.h"

#ifdef __cplusplus
extern "C" {
#endif

//------------------------------------------------------------------------------
// Экспортируемые функции
//------------------------------------------------------------------------------
extern uint8_t milandr_i2c_pin(tI2cVariant i2c, tPeriphLineVariant line);
extern tI2cVariant milandr_i2c_init(uint8_t sdaPin, uint8_t sclPin, uint8_t ownAddress);
extern tI2cVariant milandr_i2c_deinit(uint8_t sdaPin, uint8_t sclPin, tI2cVariant i2cN);


#ifdef __cplusplus
}
#endif

#endif //_MILANDR_I2C_H_
