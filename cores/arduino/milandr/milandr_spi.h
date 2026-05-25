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

#ifndef _MILANDR_SPI_H_
#define _MILANDR_SPI_H_

#include <stdint.h>
#include <stddef.h>
#include "periph_definition.h"

//------------------------------------------------------------------------------
// Конфигурация режимов работы SPI
//------------------------------------------------------------------------------
typedef enum
{
	/* Захват данных по переднему фронту. CLK в 0 в холостом режиме  */
	MILANDR_SSP_MODE0 = 0x00,
	/* Захват данных по заднему фронту. CLK в 0 в холостом режиме  */
	MILANDR_SSP_MODE1 = 0x80,
	/* Захват данных по переднему фронту. CLK в 1 в холостом режиме  */
	MILANDR_SSP_MODE2 = 0x40,
	/* Захват данных по заднему фронту. CLK в 1 в холостом режиме  */
	MILANDR_SSP_MODE3 = 0xC0,
}
tMilandrSspMode;

//------------------------------------------------------------------------------
// Порядок бит
//------------------------------------------------------------------------------
typedef enum
{
	MILANDR_LSBFIRST = 0,
	MILANDR_MSBFIRST = 1,
}
tMilandrBitOrder;

#ifdef __cplusplus
extern "C" {
#endif


//------------------------------------------------------------------------------
// Экспортируемые функции
//------------------------------------------------------------------------------
extern uint8_t milandr_spi_pin(tSspVariant spi, tPeriphLineVariant line);

extern tSspVariant milandr_spi_master_init(uint8_t  mosiPin,
                                           uint8_t  misoPin,
                                           uint8_t  clkPin,
                                           uint8_t  csPin,
                                           uint32_t speedMax,
                                           tMilandrBitOrder bitOrder,
                                           tMilandrSspMode mode);

extern void milandr_spi_master_deinit(uint8_t  mosiPin,
                                      uint8_t  misoPin,
                                      uint8_t  clkPin,
                                      uint8_t  csPin,
                                      tSspVariant sspN);

extern void milandr_spi_master_set_cfg(tSspVariant sspN,
                                       uint32_t speedMax,
                                       tMilandrBitOrder bitOrder,
                                       tMilandrSspMode mode);

extern uint16_t milandr_spi_master_write(tSspVariant sspN,
                                         uint16_t data,
                                         uint8_t wordLen);

extern void milandr_spi_master_block_write(tSspVariant sspN,
                                           uint8_t * data,
                                           uint32_t dataLen);


#ifdef __cplusplus
}
#endif

#endif //_MILANDR_SPI_H_
