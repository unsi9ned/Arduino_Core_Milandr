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

#include "SPI.h"
#include "milandr/milandr_hal.h"

using namespace arduino;

//------------------------------------------------------------------------------
// Инициализация линий SPI
//------------------------------------------------------------------------------
SpiClass::SpiClass()
	: _mosiPin(0xFF),
	  _misoPin(0xFF),
	  _clkPin(0xFF),
	  _csPin(0xFF)
{

}

SpiClass::SpiClass(uint8_t mosiPin,
                   uint8_t misoPin,
                   uint8_t clkPin,
                   uint8_t csPin)
	: _mosiPin(mosiPin),
	  _misoPin(misoPin),
	  _clkPin(clkPin),
	  _csPin(csPin)
{

}

void SpiClass::begin() {}
void SpiClass::end() {}

void SpiClass::usingInterrupt(int interruptNumber) {}
void SpiClass::notUsingInterrupt(int interruptNumber) {}

void SpiClass::beginTransaction(SPISettings settings) {}
void SpiClass::endTransaction(void) {}

uint8_t SpiClass::transfer(uint8_t data) {
    return 0;
}
uint16_t SpiClass::transfer16(uint16_t data) {
    return 0;
}
void SpiClass::transfer(void *buf, size_t count) {}

void SpiClass::attachInterrupt() {}

void SpiClass::detachInterrupt() {}

SpiWrapper SPI1(milandr_spi_pin(SSP_1, SSP_TXD_LINE),
                milandr_spi_pin(SSP_1, SSP_RXD_LINE),
                milandr_spi_pin(SSP_1, SSP_CLK_LINE),
                milandr_spi_pin(SSP_1, SSP_FSS_LINE));

SpiWrapper SPI2(milandr_spi_pin(SSP_2, SSP_TXD_LINE),
                milandr_spi_pin(SSP_2, SSP_RXD_LINE),
                milandr_spi_pin(SSP_2, SSP_CLK_LINE),
                milandr_spi_pin(SSP_2, SSP_FSS_LINE));

HardwareSPI& SPI = *SPI1;
