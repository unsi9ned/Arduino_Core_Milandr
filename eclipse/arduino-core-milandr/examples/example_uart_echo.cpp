#include <Arduino.h>

void example_uartecho_init()
{
	Serial.begin(115200UL);
	Serial.setTimeout(100);
}

void example_uartecho_process()
{
	while (Serial.available())
	{
		int c = Serial.read();
		Serial.write(c);
	}
}

