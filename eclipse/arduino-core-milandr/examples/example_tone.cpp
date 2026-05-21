#include <Arduino.h>

void example_tone_init()
{
	tone(TONE0, 1000);
	analogWrite(PF6, 200);
	tone(TONE2, 5000);
	analogWrite(PF6, 127);
	tone(TONE2, 1333);
	tone(TONE0, 2000);
	analogWrite(PF6, 64);
	pinMode(PF6, INPUT);
	tone(TONE0, 1666);
	pinMode(TONE0, INPUT);
	pinMode(TONE2, INPUT);
	tone(TONE2, 5000);
	noTone(TONE2);
}

void example_tone_process()
{

}
