#include <Arduino.h>

void example_brightness_init()
{
	pinMode(LED_BUILTIN, OUTPUT);
}

void example_brightness_process()
{
	for(uint16_t i = 0; i < 256; i += 32)
	{
		analogWrite(LED_BUILTIN, i);
		delay(1000);
	}
}
