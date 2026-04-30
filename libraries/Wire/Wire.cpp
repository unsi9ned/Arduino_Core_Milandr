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
// Инициализация
//------------------------------------------------------------------------------
void TwoWire::begin()
{
	_ownAddress = WIRE_MASTER_ADDRESS << 1;
	_i2cN = milandr_i2c_init(_sdaPin, _sclPin, _ownAddress);
}

void TwoWire::begin(uint8_t address)

{
	peripheral_address = address;
	if(peripheral_address == 0)
	{
		// Implement: Configure I2C as a controller here.
	}
	else
	{
		// Implement: Configure I2C as a peripheral here.
	}
}

//------------------------------------------------------------------------------
// Деинициализация
//------------------------------------------------------------------------------
void TwoWire::end()
{
	if(isInit())
	{
		milandr_i2c_deinit(_sdaPin, _sclPin, _i2cN);
		_i2cN = I2C_UNKNOWN;
	}
}

size_t TwoWire::requestFrom(uint8_t address, size_t len) {
    return requestFrom(address, len, true);
}

size_t TwoWire::requestFrom(uint8_t address, size_t len, bool stopBit = true) {
    uint8_t read_buffer[WIRE_BUFFER_SIZE] = {0};

    // Implement: Fill read_buffer[] with I2C read data here, and capture into
    // number_of_bytes_received how many bytes were read in total
    uint8_t number_of_bytes_received = len;

    // Move the data from read_buffer into the RX ring buffer
    rx_buffer.clear();
    for (int i = 0; i < number_of_bytes_received; i++) {
        rx_buffer.store_char(read_buffer[i]);
    }

    return number_of_bytes_received;
}

void TwoWire::beginTransmission(uint8_t address) {
    memset(tx_buffer, 0, WIRE_BUFFER_SIZE);
    tx_buffer_i = 0;
}

uint8_t TwoWire::endTransmission() {
    return endTransmission(true);
}

uint8_t TwoWire::endTransmission(bool stopBit) {
    // Implement: Send the tx_buffer via I2C with or without the stop bit
    return 0;
}

size_t TwoWire::write(uint8_t value) {
    if (tx_buffer_i >= WIRE_BUFFER_SIZE) return 0;
    tx_buffer[tx_buffer_i++] = value;
    return 1;
}

size_t TwoWire::write(const uint8_t *buffer, size_t size) {
    if (tx_buffer_i + size >= WIRE_BUFFER_SIZE) {
        size = WIRE_BUFFER_SIZE - tx_buffer_i;
    }
    memcpy(tx_buffer + tx_buffer_i, buffer, size);
    return size;
}

int TwoWire::available() {
    return rx_buffer.available();
}

int TwoWire::peek() {
    return rx_buffer.peek();
}

int TwoWire::read() {
    return rx_buffer.read_char();
}

void TwoWire::setClock(uint32_t freq) {}

void TwoWire::onReceive(void (*handler)(int)) {
    // Implement: Configure the interrupts to run the onReceive handler
    onReceiveHandler = handler;
}

void TwoWire::onRequest(void (*handler)(void)) {
    // Implement: Configure the interrupts to run the onRequest handler
    onRequestHandler = handler;
}

void TwoWire::setWireTimeout(uint32_t timeout, bool reset_on_timeout) {
    timeout_us = timeout;
    timeout_reset = reset_on_timeout;
    // Implement: Configure the timer or I2C peripheral to timeout
}

void TwoWire::clearWireTimeoutFlag(void) {
    timeout_flag = false;
}

bool TwoWire::getWireTimeoutFlag(void) {
    return timeout_flag;
}

}; //namespace arduino

arduino::I2CWrapper I2C1(milandr_i2c_pin(I2C_1, I2C_SDA_LINE),
                         milandr_i2c_pin(I2C_1, I2C_SCL_LINE));
arduino::HardwareI2C& Wire = *I2C1;
