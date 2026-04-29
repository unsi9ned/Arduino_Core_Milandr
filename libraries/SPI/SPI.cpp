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

namespace arduino {

const tMilandrSspMode SpiClass::_mdr_modes[SPI_MODE3 + 1] =
{
	[SPI_MODE0] = MILANDR_SSP_MODE0,
	[SPI_MODE1] = MILANDR_SSP_MODE1,
	[SPI_MODE2] = MILANDR_SSP_MODE2,
	[SPI_MODE3] = MILANDR_SSP_MODE3,
};

//------------------------------------------------------------------------------
// Инициализация линий SPI
//------------------------------------------------------------------------------
SpiClass::SpiClass()
	: _mosiPin(0xFF),
	  _misoPin(0xFF),
	  _clkPin(0xFF),
	  _csPin(0xFF),
	  _sspN(SSP_UNKNOWN)
{

}

SpiClass::SpiClass(uint8_t mosiPin,
                   uint8_t misoPin,
                   uint8_t clkPin,
                   uint8_t csPin)
	: _mosiPin(mosiPin),
	  _misoPin(misoPin),
	  _clkPin(clkPin),
	  _csPin(csPin),
	  _sspN(SSP_UNKNOWN)
{

}

//
// Инициализация модуля SSP и связанных с ним портов В/В
// Драйвер только производит настройку пина CS на выход, но не берет на себя
// управление данной линией. Можно не указывать CS, но тогда пользователь
// должен настроить его сам
//
void SpiClass::begin()
{
	if(!isInit())
	{
		_sspN = milandr_spi_master_init(_mosiPin,
		                                _misoPin,
		                                _clkPin,
		                                _csPin,
		                                _spisettings.getClockFreq(),
		                                getMdrBitOrder(_spisettings),
		                                getMdrDateMode(_spisettings));
	}
}

//
// Деинициализация модуля SPP, перевод пинов в состояние Floating Input
//
void SpiClass::end()
{
	if(isInit())
	{
		milandr_spi_master_deinit(_mosiPin, _misoPin, _clkPin, _csPin, _sspN);
		_sspN = SSP_UNKNOWN;
	}
}

void SpiClass::usingInterrupt(int interruptNumber) {}
void SpiClass::notUsingInterrupt(int interruptNumber) {}

//
// Установка режима работы шины перед началом транзакции
//
void SpiClass::beginTransaction(SPISettings settings)
{
	milandr_spi_master_set_cfg(_sspN,
							   settings.getClockFreq(),
							   getMdrBitOrder(settings),
							   getMdrDateMode(settings));
}

//
// Ничего не делаем
//
void SpiClass::endTransaction(void)
{

}

//
// Send data to the SPI peripheral and read data back.
//
uint8_t SpiClass::transfer(uint8_t data)
{
	return (uint8_t)milandr_spi_master_write(_sspN, data, 8);
}

//
// Запись/чтение данных в 16-битном формате
//
uint16_t SpiClass::transfer16(uint16_t data)
{
	return milandr_spi_master_write(_sspN, data, 16);
}

//
// Чтение/запись массива данных
//
void SpiClass::transfer(void *buf, size_t count)
{
	milandr_spi_master_block_write(_sspN, (uint8_t*)buf, count);
}

void SpiClass::attachInterrupt() {}

void SpiClass::detachInterrupt() {}

//------------------------------------------------------------------------------
// Перевод Arduino режима SPI в режим Milandr
//------------------------------------------------------------------------------
tMilandrSspMode arduino::SpiClass::getMdrDateMode(const SPISettings& settings)
{
	SPIMode mode = settings.getDataMode();

	if(mode < SPI_MODE0 || mode > SPI_MODE3)
		return SpiClass::_mdr_modes[SPI_MODE0];
	else
		return SpiClass::_mdr_modes[mode];

}

//------------------------------------------------------------------------------
// Перевод определения порядка бит Arduino в определение порядка Milandr
//------------------------------------------------------------------------------
tMilandrBitOrder arduino::SpiClass::getMdrBitOrder(const SPISettings& settings)
{
	return settings.getBitOrder() == MSBFIRST ? MILANDR_MSBFIRST : MILANDR_LSBFIRST;
}

} //namesapce arduino

arduino::SpiWrapper SPI1(milandr_spi_pin(SSP_1, SSP_TXD_LINE),
                         milandr_spi_pin(SSP_1, SSP_RXD_LINE),
                         milandr_spi_pin(SSP_1, SSP_CLK_LINE),
                         milandr_spi_pin(SSP_1, SSP_FSS_LINE));

arduino::SpiWrapper SPI2(milandr_spi_pin(SSP_2, SSP_TXD_LINE),
                         milandr_spi_pin(SSP_2, SSP_RXD_LINE),
                         milandr_spi_pin(SSP_2, SSP_CLK_LINE),
                         milandr_spi_pin(SSP_2, SSP_FSS_LINE));

arduino::HardwareSPI &SPI = *SPI1;
