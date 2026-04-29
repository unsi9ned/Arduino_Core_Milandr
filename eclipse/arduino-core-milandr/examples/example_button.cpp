#include <Arduino.h>

void example_button_init()
{
	pinMode(USER_BUTTON, INPUT);
	pinMode(LED_BUILTIN, OUTPUT);

	Serial.begin(115200UL);
	Serial.setTimeout(100);
}

void example_button_process()
{
	PinStatus status = digitalRead(USER_BUTTON);
	static PinStatus oldStatus = LOW;

	if(oldStatus != status)
	{
		if(status == LOW)
		{
			digitalWrite(LED_BUILTIN, HIGH);
			Serial.print("Button is ON\n");
		}
		else
		{
			digitalWrite(LED_BUILTIN, LOW);
			Serial.print("Button is OFF\n");
		}
	}

	oldStatus = status;
}

