#include <Arduino.h>

void stop_tone()
{
	noTone(TONE0);
}

void example_tone_init()
{
	tone(TONE0, 1333, 10000);
	attachInterrupt(USER_BUTTON, stop_tone, FALLING);
}

void example_tone_process()
{

}
