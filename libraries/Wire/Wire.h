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

/**
 * I2C/TwoWire library.
 *
 * There are two operating modes: Controller or Peripheral.
 *
 * The official default read/write buffer sizes are 32 bytes and Arduino user
 * sketches should perform individual I2C transactions within these bounds.
 * However, this example Wire library has been configured so that larger
 * buffers can be created by modifying the WIRE_BUFFER_SIZE definition.
 *
 * Example sketch as an I2C controller:
 *
 * @code
 * #include <Wire.h>
 *
 * void setup() {
 *     Wire.begin();
 * }
 *
 * void loop() {
 *     // Write three bytes to a peripheral with address 0x90
 *     Wire.beginTransmission(0x90);
 *     Wire.write(0x01);
 *     Wire.write({0x02, 0x03 }, 2);
 *     Wire.endTransmission();
 *     // Read two bytes from a peripheral with address 0x90
 *     Wire.requestFrom(0x90, 2);
 *     while(Wire.available()) {
 *         char c = Wire.read();  // Get the data byte by byte
 *     }
 * }
 * @endcode
 *
 * Example sketch as an I2C peripheral:
 *
 * @code
 * #include <Wire.h>
 *
 * void peripheralReceiveHandler(int count) {
 *     while(Wire.available()) {
 *         char c = Wire.read();  // Get the data byte by byte
 *     }
 * }
 *
 * void peripheralSendHandler() {
 *     Wire.write("my data to send to controller");
 * }
 *
 * void setup() {
 *     Wire.begin(0x24);  // Set up peripheral mode with address 0x24
 *     Wire.onReceive(peripheralReceiveHandler);
 *     Wire.onRequest(peripheralSendHandler);
 * }
 *
 * void loop() {
 *     delay(50);
 * }
 * @endcode
 */
#pragma once

#include "api/HardwareI2C.h"
#include "api/RingBuffer.h"
#include "milandr/milandr_hal.h"

// Wire.end() is not always available and this macro indicates it is present
#define WIRE_HAS_END (1)
// The timeout methods Wire.setWireTimeout(), Wire.getWireTimeoutFlag() and
// Wire.clearWireTimeout() are not always available either
#define WIRE_HAS_TIMEOUT (1)

// This is not part of the official Arduino Core API, but it might be useful
// to easily change the tx/rx buffer sizes
#ifndef WIRE_BUFFER_SIZE
#define WIRE_BUFFER_SIZE (32)
#endif

#define WIRE_MASTER_ADDRESS (0x33)

namespace arduino {

class FakeI2C : public HardwareI2C
{
public:
	FakeI2C(){}

	void begin() {}
	void begin(uint8_t address) {}
	void end() {}

	void setClock(uint32_t freq) {}

	void beginTransmission(uint8_t address) {}
	uint8_t endTransmission(bool stopBit) {return 0;}
	uint8_t endTransmission(void) {return endTransmission(true);}

	size_t requestFrom(uint8_t address, size_t len, bool stopBit) {return 0;}
	size_t requestFrom(uint8_t address, size_t len) {return requestFrom(address, len, true);}

	void onReceive(void(*)(int)) {}
	void onRequest(void(*)(void)) {}

	int available() {return 0;}
	int read() {return 0;}
	int peek() {return 0;}
	size_t write(uint8_t) {return 0;}
};

class TwoWire : public HardwareI2C
{
private:
	// Timeout related state
	uint32_t timeout_us = 0;
	bool timeout_reset = false;
	bool timeout_flag = false;
	void (*onReceiveHandler)(int);
	void (*onRequestHandler)(void);
	// Receive and transmit buffers
	RingBufferN<WIRE_BUFFER_SIZE> rx_buffer;
	uint8_t tx_buffer[WIRE_BUFFER_SIZE] = {0};
	uint8_t tx_buffer_i = 0;

	uint8_t     _sdaPin;
	uint8_t     _sclPin;
	uint8_t     _ownAddress;
	uint8_t     _targetAddress;
	tI2cVariant _i2cN;

public:
	/**
	 * Constructor for this I2C class.
	 *
	 * Don't initialise the I2C hardware here, that should be done in begin(),
	 * the constructor can be used to prepare the instance before using it.
	 *
	 * The main "Wire" instance accessed by the users is declared in this
	 * header file and instantiated in Wire.cpp. So, the user is not expected
	 * to use the class constructor, its signature is not standarised, and it
	 * can be adapted to requirements of your Arduino core.
	 */
	TwoWire();
	TwoWire(uint8_t sdaPin, uint8_t sclPin);

	/**
	 * Initialise the Wire instance.
	 *
	 * By default the I2C is configured as a controller, but if the address
	 * argument is used it is configured as an I2C peripheral.
	 *
	 * @param address If the address argument is specified, it initialise the
	 *                instance as an I2C peripheral with the given address.
	 */
	void begin();
	void begin(uint8_t address);

	/**
	 * Disable the Wire instance, undoing all the effects of begin().
	 *
	 * This method was not in the original Wire API and might not present in
	 * early Arduino Cores, but should be included in new ones.
	 */
	void end();

	/**
	 * As the controller, request a number of bytes from a peripheral.
	 *
	 * Only available in controller mode.
	 *
	 * @param address 7-bit address of the I2C peripheral.
	 * @param len Number of bytes to request.
	 * @param stopBit True sends the a stop bit at the end of the transmission.
	 *                False sends a repeated start bit.
	 *
	 * @return Number of bytes sent by the peripheral from the request.
	 */
	size_t requestFrom(uint8_t address, size_t len);
	size_t requestFrom(uint8_t address, size_t len, bool stopBit);

	/**
	 * Start the preparation for an I2C controller transmission.
	 *
	 * To be followed by calls to the write() methods and finished with
	 * a called with the endTransmission() method.
	 *
	 * This method is not used in peripheral mode.
	 *
	 * @param address 7-bit address of the I2C peripheral.
	 */
	void beginTransmission(uint8_t address);

	/**
	 * Send the I2C controller transmission started by beginTransmission().
	 *
	 * In controller mode the calls to the write() methods only queue the data
	 * to be sent, and a call to this method sends all the queued data.
	 *
	 * This method is not used in peripheral mode.
	 *
	 * @param stopBit True sends the a stop bit at the end of the transmission.
	 *                False sends a repeated start bit.
	 *
	 * @return One of these values:
	 *         0 = Success
	 *         1 = Data too long
	 *         2 = NACK on transmit of address
	 *         3 = NACK on transmit of data
	 *         4 = Other error
	 */
	uint8_t endTransmission();
	uint8_t endTransmission(bool stopBit);

	/**
	 * Queue the data to be transmitted to a peripheral.
	 *
	 * In controller mode, the first call to a write() methods has to be
	 * preceeded by beginTransmission() and the last call to a write() method
	 * has to be followed by a call to endTransmission().
	 *
	 * In peripheral mode, the beginTransmission() and endTransmission()
	 * methods are not used, and it directly buffers the data to be sent
	 * when requested from the controller.
	 *
	 * @param value A single byte to write.
	 *
	 * @param buffer A byte array to write.
	 * @param size The size of the array.
	 *
	 * @param str A string to write as a byte array.
	 *
	 * @return Number of bytes written.
	 */
	size_t write(uint8_t value);
	size_t write(const uint8_t *buffer, size_t size);
	using Print::write;  // Uses write(const char *str) from the api/Print

	/**
	 * Get the number of bytes available to be read.
	 *
	 * In controller mode this indicates the available data after a
	 * `requestFrom()` call.
	 * In peripheral mode it can be called inside the `onReceive()` callback.
	 *
	 * @return Number of bytes available.
	 */
	int available();

	/**
	 * Get a copy of the next byte from the read (RX) buffer without
	 * removing it.
	 *
	 * @return A copy of the next byte from the RX buffer, or -1 if there
	 *         isn't data available.
	 */
	int peek();

	/**
	 * Get a single byte from the read buffer.
	 *
	 * In controller mode this is data read from the peripheral.
	 * In peripheral mode this is data received from the controller.
	 *
	 * @return Number of bytes available.
	 */
	int read();

	/**
	 * Configure the I2C clock frequency.
	 *
	 * By default set to 100 KHz.
	 *
	 * @param freq Frequency in Hz. Valid values are for all Arduino Cores are:
	 *             standard mode = 100_000
	 *             fast mode     = 400_000
	 *             And other modes are valid for for compatible cores:
	 *             fast mode plus  = 1000_000
	 *             high speed mode = 3400_000
	 */
	void setClock(uint32_t freq);

	/**
	 * In peripheral mode, register a callback to receive data.
	 *
	 * @param handler Callback function with a single int parameter indicating
	 *                the number of bytes read that does not return anything.
	 */
	void onReceive(void (*handler)(int));

	/**
	 * In peripheral mode, register a callback to send data on request by
	 * a controller.
	 *
	 * @param handler Callback function with no arguments and that does not
	 *                return anything.
	 */
	void onRequest(void (*handler)(void));

	/**
	 * Configure the time out.
	 *
	 * By default the time out is not enabled, a call to this method is
	 * needed to enable it.
	 *
	 * @param timeout Time out in microseconds, default value is 100ms.
	 *                Set to zero to disable it.
	 * @param reset_with_timeout Boolean to indicate if the device should
	 *                           reset when the time out expires.
	 */
	void setWireTimeout(uint32_t timeout = 100000, bool reset_on_timeout = false);

	/**
	 * Clear the time out flag.
	 */
	void clearWireTimeoutFlag(void);

	/**
	 * Check if a time out has occurred since the last time the flag was
	 * cleared.
	 *
	 * @return True if a time out occurred.
	 */
	bool getWireTimeoutFlag(void);

private:
	inline bool isInit() {return _i2cN != I2C_UNKNOWN; }
};

class I2CWrapper
{
private:
	uint8_t         _sdaPin;
	uint8_t         _sclPin;
	TwoWire         _realI2C;    // Настоящий I2C
	FakeI2C         _fakeI2C;    // Заглушка
	HardwareI2C*    _i2cPtr;     // Указатель на активный объект
public:
	I2CWrapper(uint8_t sdaPin = PIN_NC, uint8_t sclPin = PIN_NC)
	: _sdaPin(sdaPin),
	  _sclPin(sclPin),
	  _i2cPtr(nullptr)
	{
		if(sdaPin != PIN_NC &&
		   sclPin != PIN_NC)
		{
			// Настоящий I2C
			_realI2C = TwoWire(sdaPin, sclPin);
			_i2cPtr = &_realI2C;
		}
		else
		{
			// Заглушка
			_fakeI2C = FakeI2C();
			_i2cPtr = &_fakeI2C;
		}
	}

	// Доступ к внутреннему объекту через оператор ->
	HardwareI2C* operator->() { return _i2cPtr; }
	HardwareI2C& operator*()  { return *_i2cPtr; }
};

}  // namespace arduino

extern arduino::I2CWrapper I2C1;
extern arduino::HardwareI2C& Wire;
