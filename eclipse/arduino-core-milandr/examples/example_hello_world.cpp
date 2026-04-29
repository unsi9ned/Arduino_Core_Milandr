#include <Arduino.h>

void example_helloworld_init()
{
	Serial.begin(115200UL);
	Serial.setTimeout(100);
}

void example_helloworld_process()
{
	Serial.print("Hello world\n");
	delay(1000);
}

