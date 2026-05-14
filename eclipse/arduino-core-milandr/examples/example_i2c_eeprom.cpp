#include <Arduino.h>
#include <Wire.h>

#define EEPROM_ADDR     0x50

void example_i2c_eeprom_init()
{
	Serial.begin(115200);
	while (!Serial);

	Wire.begin();
	Serial.println("=== I2C EEPROM 24C256 Test ===\n");

	// 1. Сканируем устройство на шине
	Serial.print("Scanning I2C bus... ");
	Wire.beginTransmission(EEPROM_ADDR);
	uint8_t error = Wire.endTransmission();

	if(error == 0)
	{
		Serial.print("Found at 0x");
		Serial.println(EEPROM_ADDR, HEX);
	}
	else
	{
		Serial.print("Error ");
		Serial.print(error);
		Serial.println(" - Check wiring/pull-ups!");
		while (1);
	}

	// 2. Записываем тестовый байт
	uint16_t testAddr = 0;
	uint8_t testData[] = {'D', 'E', 'A', 'D', 'B', 'E', 'A', 'F'};
	uint8_t readBack[sizeof(testData)];

	Wire.beginTransmission(EEPROM_ADDR);
	Wire.write((uint8_t) (testAddr >> 8));   // Старший байт адреса
	Wire.write((uint8_t) (testAddr & 0xFF)); // Младший байт адреса
	Wire.write(testData, sizeof(testData));                    // Данные
	uint8_t writeResult = Wire.endTransmission();

	if(writeResult == 0)
	{
		Serial.print("Write 0xDEADBEAF");
		Serial.println(" to address 0x0000 - OK");
	}
	else
	{
		Serial.print("Write failed! Error: ");
		Serial.println(writeResult);
		while (1);
	}

	// 3. Ждем завершения внутренней записи
	delay(10);

	// 4. Читаем обратно
	Wire.beginTransmission(EEPROM_ADDR);
	Wire.write((uint8_t) (testAddr >> 8));
	Wire.write((uint8_t) (testAddr & 0xFF));
	Wire.endTransmission();

	Wire.requestFrom((uint8_t) EEPROM_ADDR, (uint8_t) sizeof(testData));

	int i = 0;

	if(!Wire.available())
	{
		Serial.println("Read failed - no data available!");
	}
	else
	{
		Serial.print("Read from address 0x0000: ");

		while(Wire.available() && i < sizeof(readBack))
		{
			uint8_t readData = Wire.read();
			readBack[i++] = readData;
		}

		// 5. Проверяем совпадение
		for(int i = 0; i < sizeof(testData); i++)
		{
			if(readBack[i] != testData[i])
			{
				Serial.print("\n*** MISMATCH! Expected 0x");
				Serial.print(testData[i], HEX);
				Serial.print(", got 0x");
				Serial.print(readBack[i], HEX);
				Serial.println(" ***");
				while(1);
			}
			else
			{
//				Serial.write('\'');
//				Serial.write(readBack[i]);
//				Serial.write('\'');
//				Serial.write(',');
//				Serial.write(' ');

				Serial.print('\'');
				Serial.print((char)readBack[i]);
				Serial.print('\'');
				Serial.print(',');
				Serial.print(' ');
			}
		}

		Serial.println("\n*** I2C EEPROM Test PASSED! ***");
	}
}

void example_i2c_eeprom_process()
{
}
