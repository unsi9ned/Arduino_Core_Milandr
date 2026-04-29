#include <Arduino.h>

void example_blink_init()
{
	pinMode(LED_BUILTIN, OUTPUT);
}

void example_blink_process()
{
	digitalWrite(LED_BUILTIN, HIGH);
	delay(1000);
	digitalWrite(LED_BUILTIN, LOW);
	delay(1000);
}
