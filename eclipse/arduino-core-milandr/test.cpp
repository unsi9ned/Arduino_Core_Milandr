#include <Arduino.h>

void setup(void)
{
	pinMode(LED_BUILTIN, OUTPUT);
	pinMode(USER_BUTTON, INPUT);
}

void loop(void)
{
#if 1
	digitalWrite(LED_BUILTIN, HIGH); // turn the LED on (HIGH is the voltage level)
	delay(500);                       // wait for a second
	digitalWrite(LED_BUILTIN, LOW); // turn the LED off by making the voltage LOW
	delay(500);                       // wait for a second
#else
	digitalWrite(LED_BUILTIN, digitalRead(USER_BUTTON) == LOW ? HIGH : LOW);
#endif
}
