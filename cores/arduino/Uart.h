#pragma once

#include "api/HardwareSerial.h"
#include "milandr/milandr_hal.h"

namespace arduino {

class Uart;

class FakeUart : public HardwareSerial
{
public:
	FakeUart() = default;
	virtual ~FakeUart() = default;
	void begin(const unsigned long baudrate) override {}
	void begin(const unsigned long baudrate, const uint16_t config) override {}
	void end() override {};
	int available() override {return 0;}
	int peek() override {return 0;}
	int read() override {return 0;}
	void flush() override {}
	size_t write(const uint8_t c) override {return 0;}
	size_t write(const uint8_t* buf, const size_t size) override  {return 0;}
	operator bool() override  {return false;}
};

class Uart : public HardwareSerial {

private:
	uint8_t      _rxPin;
	uint8_t      _txPin;
	tUartVariant _uartN;
public:
	Uart(uint8_t rxPin = 0xFF, uint8_t txPin = 0xFF);
	virtual ~Uart() = default;

	/**
	 * Configure the UART to start operating with a given baud rate.
	 *
	 * @param baudrate UART baud rate in bytes per second.
	 * @param config Configure the UART settings with #define with this format:
	 *               SERIAL_<word_size><parity><stop_bits>, where:
	 *                 - word_size = bits per word, from `5` to `8`
	 *                 - parity = parity bit configuration for error detection,
	 *                            values can be `N`, `0`, or `1`
	 *                 - stop_bits = Number of stop bits, from `1` to `2`
	 *               For example, the default value is configured with
	 *               SERIAL_8N1 = 8 bit words, no parity, and 1 stop bit.
	 */
	void begin(const unsigned long baudrate);
	void begin(const unsigned long baudrate, const uint16_t config);

	/**
	 * Disable the UART and free up the pins.
	 */
	void end();

	/**
	 * Get the number of bytes available to read from the RX buffer.
	 *
	 * @return Number of bytes available to read.
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
	 * Get a single byte from the read (RX) buffer.
	 *
	 * @code
	 * if (Serial.available() > 0) {
	 *     int single_byte = Serial.read();
	 * }
	 * @endcode
	 *
	 * @return The next byte from the RX buffer, or -1 if there isn't any.
	 */
	int read();

	/**
	 * Blocks until all the serial data out (TX) is sent.
	 */
	void flush();

	/**
	 * Sends data out (TX).
	 *
	 * @param c A single character to send out.
	 *
	 * @param buf A byte array to send out.
	 * @param size The size of the buf data array.
	 *
	 * @param str A string to send out
	 */
	size_t write(const uint8_t c);
	size_t write(const uint8_t* buf, const size_t size);
	using Print::write;  // Uses write(const char *str) from the api/Print

	/**
	 * Indicates if it Serial is ready.
	 *
	 * Can be used to wait until ready:
	 * @code
	 * Serial.begin(115200);
	 * while (!Serial);
	 * @endcode
	 */
	operator bool();
};

class UartWrapper
{
private:
	uint8_t         _rxPin;
	uint8_t         _txPin;
	Uart            _realUart;   // Настоящий UART
	FakeUart        _fakeUart;   // Заглушка
	HardwareSerial* _uartPtr;    // Указатель на активный объект
public:
	UartWrapper(uint8_t rxPin = 0xFF, uint8_t txPin = 0xFF) :
		_rxPin(rxPin),
		_txPin(txPin),
		_uartPtr(nullptr)
	{
		if(rxPin != 0xFF && txPin != 0xFF)
		{
			// Настоящий UART
			_realUart = Uart(rxPin, txPin);
			_uartPtr = &_realUart;
		}
		else
		{
			// Заглушка
			_fakeUart = FakeUart();
			_uartPtr = &_fakeUart;
		}
	}

	// Доступ к внутреннему объекту через оператор ->
	HardwareSerial* operator->() { return _uartPtr; }
	HardwareSerial& operator*()  { return *_uartPtr; }
};

}  // namespace arduino

// Declare user accessible Serial instance
extern arduino::UartWrapper Uart1;
extern arduino::UartWrapper Uart2;
extern arduino::UartWrapper Uart3;
extern arduino::HardwareSerial& Serial;
