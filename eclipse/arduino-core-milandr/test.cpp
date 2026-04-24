#include <Arduino.h>

void setup(void)
{
	pinMode(LED_BUILTIN, OUTPUT);
	pinMode(USER_BUTTON, INPUT);

	Serial.begin(115200ul);
}

void loop(void)
{
#if 0
	digitalWrite(LED_BUILTIN, HIGH); // turn the LED on (HIGH is the voltage level)
	delay(500);                       // wait for a second
	digitalWrite(LED_BUILTIN, LOW); // turn the LED off by making the voltage LOW
	delay(500);                       // wait for a second
#else
	PinStatus status = digitalRead(USER_BUTTON);

	if(status == LOW)
	{
		digitalWrite(LED_BUILTIN, HIGH);
		Serial.print("Button is ON");
	}
	else
	{
		digitalWrite(LED_BUILTIN, LOW);
		Serial.print("Button is OFF");
	}
#endif
}
