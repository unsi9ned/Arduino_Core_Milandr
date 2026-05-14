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

#include "Wire.h"

namespace arduino
{

//------------------------------------------------------------------------------
// Конструкторы
//------------------------------------------------------------------------------
TwoWire::TwoWire()
	: _sdaPin(PIN_NC),
	  _sclPin(PIN_NC),
	  _ownAddress(WIRE_MASTER_ADDRESS << 1),
	  _i2cN(I2C_UNKNOWN)
{

}

TwoWire::TwoWire(uint8_t sdaPin, uint8_t sclPin)
	: _sdaPin(sdaPin),
	  _sclPin(sclPin),
	  _ownAddress(WIRE_MASTER_ADDRESS << 1),
	  _i2cN(I2C_UNKNOWN)
{
}

//------------------------------------------------------------------------------
// Инициализация в режиме Master
//------------------------------------------------------------------------------
void TwoWire::begin()
{
	_ownAddress = WIRE_MASTER_ADDRESS << 1;
	_i2cN = milandr_i2c_init(_sdaPin, _sclPin, _ownAddress);
}

//------------------------------------------------------------------------------
// Инициализация в режиме Slave (не поддерживается MDR32F92QI)
//------------------------------------------------------------------------------
void TwoWire::begin(uint8_t address)

{
	_ownAddress = address;
	_i2cN = I2C_UNKNOWN;
}

//------------------------------------------------------------------------------
// Завершение начатых транзакций и деинициализация
//------------------------------------------------------------------------------
void TwoWire::end()
{
	if(isInit())
	{
		milandr_i2c_deinit(_sdaPin, _sclPin, _i2cN);
		_i2cN = I2C_UNKNOWN;
	}
}

//------------------------------------------------------------------------------
// Чтение данных из Slave-устройства
//------------------------------------------------------------------------------
size_t TwoWire::requestFrom(uint8_t address, size_t len)
{
	return requestFrom(address, len, true);
}

size_t TwoWire::requestFrom(uint8_t address, size_t len, bool stopBit = true)
{
	if(!isInit()) return 0;
	milandr_i2c_master_receive(_i2cN, len, stopBit);
	return milandr_i2c_available(_i2cN);
}

//------------------------------------------------------------------------------
// Инициирование транзакции
//------------------------------------------------------------------------------
void TwoWire::beginTransmission(uint8_t address)
{
	if(isInit())
	{
		milandr_i2c_start_transaction(_i2cN, address);
	}
}

//------------------------------------------------------------------------------
// Завершение транзакции и передача данных из буфера на шину I2C
//------------------------------------------------------------------------------
uint8_t TwoWire::endTransmission()
{
	return endTransmission(true);
}

uint8_t TwoWire::endTransmission(bool stopBit)
{
	return milandr_i2c_end_transaction(_i2cN, stopBit);
}

//------------------------------------------------------------------------------
// Запись байта в кольцевой буфер для дальнейшей отправки
//------------------------------------------------------------------------------
size_t TwoWire::write(uint8_t value)
{
	return milandr_i2c_write_byte(_i2cN, value);
}

//------------------------------------------------------------------------------
// Запись массива байт в кольцевой буфер для дальнейшей отправки
//------------------------------------------------------------------------------
size_t TwoWire::write(const uint8_t *buffer, size_t size)
{
	size_t sent = 0;

	while(size > 0 && milandr_i2c_write_byte(_i2cN, *buffer++))
	{
		sent++;
		size--;
	}
	return sent;
}

//------------------------------------------------------------------------------
// Количество принятых в буфер байт
//------------------------------------------------------------------------------
int TwoWire::available()
{
	return milandr_i2c_available(_i2cN);
}

//------------------------------------------------------------------------------
// Чтение байта из буфера без удаления
//------------------------------------------------------------------------------
int TwoWire::peek()
{
	return milandr_i2c_peak(_i2cN);
}

//------------------------------------------------------------------------------
// Чтение байта из буфера с удалением
//------------------------------------------------------------------------------
int TwoWire::read()
{
	return milandr_i2c_read_byte(_i2cN);
}

//------------------------------------------------------------------------------
// Настройка скорости обмена данными
//------------------------------------------------------------------------------
void TwoWire::setClock(uint32_t freq)
{
	if(!isInit()) return;
	milandr_i2c_set_freq(_i2cN, freq);
}

//------------------------------------------------------------------------------
// Колбэк: вызывается, когда ведомый принял данные от мастера
// Чип не поддерживает режим Slave
//------------------------------------------------------------------------------
void TwoWire::onReceive(void (*handler)(int))
{
}

//------------------------------------------------------------------------------
// Колбэк: вызывается, когда ведомый должен отправить данные мастеру
// Чип не поддерживает режим Slave
//------------------------------------------------------------------------------
void TwoWire::onRequest(void (*handler)(void))
{
}

//------------------------------------------------------------------------------
// Неиспользуемые методы (были зачем-то добавлены автором шаблона, но не требуются
// API ядра. Защита от зависания есть в драйвере i2c
//------------------------------------------------------------------------------
void TwoWire::setWireTimeout(uint32_t timeout, bool reset_on_timeout)
{
}

void TwoWire::clearWireTimeoutFlag(void)
{
}

bool TwoWire::getWireTimeoutFlag(void)
{
	return false;
}

}; //namespace arduino

arduino::I2CWrapper I2C1(milandr_i2c_pin(I2C_1, I2C_SDA_LINE),
                         milandr_i2c_pin(I2C_1, I2C_SCL_LINE));
arduino::HardwareI2C& Wire = *I2C1;
