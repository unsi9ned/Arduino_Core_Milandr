#include <Arduino.h>

extern void analogWriteResolution(int resolution);

void example_brightness_init()
{
	analogWriteResolution(12);
}

void example_brightness_process()
{
	for(uint16_t i = 0; i < 4096; i += 32)
	{
		analogWrite(LED_BUILTIN, i);
		delay(10);
	}
}
