#include <Arduino.h>

extern void analogReadResolution(int resolution);

void example_temperature_init()
{
	Serial.begin(115200);
	analogReadResolution(12);
}

void example_temperature_process()
{
	int value = analogRead(AIN_TEMP);
	Serial.print("Temperature = ");
	Serial.println(value);
	delay(1000);
}
