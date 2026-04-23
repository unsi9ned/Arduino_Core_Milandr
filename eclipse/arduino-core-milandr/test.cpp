#include <Arduino.h>

#define LED_BUILTIN   D15
#define BUTTON        D14

void setup(void)
{
	pinMode(LED_BUILTIN, OUTPUT);
	pinMode(BUTTON, INPUT);
}

void loop(void)
{
#if 1
	digitalWrite(LED_BUILTIN, HIGH); // turn the LED on (HIGH is the voltage level)
	delay(1000);                       // wait for a second
	digitalWrite(LED_BUILTIN, LOW); // turn the LED off by making the voltage LOW
	delay(1000);                       // wait for a second
#else
	digitalWrite(LED_BUILTIN, digitalRead(BUTTON) == LOW ? HIGH : LOW);
#endif
}
