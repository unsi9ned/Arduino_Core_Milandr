#include <Arduino.h>
#include <SPI.h>

using namespace arduino;

#define CS_PIN      PIN_SPI1_SS

void example_spi_eeprom_init()
{
	Serial.begin(115200UL);
	SPI.begin();

	// Тест 1: Чтение JEDEC ID (если подключена флеш-память)
	// Для Winbond 25Q32 ожидаем: 0xEF4016
	pinMode(CS_PIN, OUTPUT);
	digitalWrite(CS_PIN, HIGH);

	Serial.print("Test - JEDEC ID: ");

	SPI.beginTransaction(SPISettings(1000000, MSBFIRST, SPI_MODE0));
	digitalWrite(CS_PIN, LOW);
	SPI.transfer(0x9F);                    // Команда Read JEDEC ID
	uint8_t manID = SPI.transfer(0xFF);    // Manufacturer ID
	uint8_t memType = SPI.transfer(0xFF);  // Memory Type
	uint8_t cap = SPI.transfer(0xFF);      // Capacity
	digitalWrite(CS_PIN, HIGH);
	SPI.endTransaction();

	Serial.print("0x");
	Serial.print(manID, HEX);
	Serial.print(memType, HEX);
	Serial.println(cap, HEX);
}

void example_spi_eeprom_process()
{

}
