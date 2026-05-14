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

typedef enum
{
	MILANDR_I2C_OK            = 0,
	MILANDR_I2C_DATA_TOO_LONG = 1,
	MILANDR_I2C_NACK_ADDR     = 2,
	MILANDR_I2C_NACK_DATA     = 3,
	MILANDR_I2C_ERROR         = 4,
	MILANDR_I2C_TIMEOUT       = 5,
	MILANDR_I2C_BUSY          = 6
}
tMilandrI2cStatus;

#ifdef __cplusplus
extern "C" {
#endif

//------------------------------------------------------------------------------
// Экспортируемые функции
//------------------------------------------------------------------------------
extern uint8_t milandr_i2c_pin(tI2cVariant i2c, tPeriphLineVariant line);
extern tI2cVariant milandr_i2c_init(uint8_t sdaPin, uint8_t sclPin, uint8_t ownAddress);
extern void milandr_i2c_deinit(uint8_t sdaPin, uint8_t sclPin, tI2cVariant i2cN);
extern void milandr_i2c_set_freq(tI2cVariant i2c, uint32_t freq);
extern void milandr_i2c_start_transaction(tI2cVariant i2cN, uint8_t address);
extern tMilandrI2cStatus milandr_i2c_end_transaction(tI2cVariant i2cN, bool stopBit);
extern size_t milandr_i2c_write_byte(tI2cVariant n, uint8_t data);
extern tMilandrI2cStatus milandr_i2c_master_receive(tI2cVariant n, size_t len, bool stopBit);
extern int milandr_i2c_available(tI2cVariant n);
extern int milandr_i2c_peak(tI2cVariant n);
extern int milandr_i2c_read_byte(tI2cVariant n);


#ifdef __cplusplus
}
#endif

#endif //_MILANDR_I2C_H_
