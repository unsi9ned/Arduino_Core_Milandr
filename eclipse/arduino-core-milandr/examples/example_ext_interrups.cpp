#include <Arduino.h>

#define CALLBACK_WITH_PARAM        1

#if CALLBACK_WITH_PARAM

void led_toggle(void * pin)
{
	uint8_t in = (uint32_t)pin % DMAX;

	if(digitalRead(in))
		digitalWrite(LED_BUILTIN, LOW);
	else
		digitalWrite(LED_BUILTIN, HIGH);

	if(digitalRead(USER_BUTTON))
		Serial.print("Button is OFF\n");
	else
		Serial.print("Button is ON\n");
}

#else

void led_toggle2()
{
	if(digitalRead(USER_BUTTON))
		digitalWrite(LED_BUILTIN, LOW);
	else
		digitalWrite(LED_BUILTIN, HIGH);
}

#endif

void example_exti_init()
{
	pinMode(USER_BUTTON, INPUT);
	pinMode(LED_BUILTIN, OUTPUT);

#if CALLBACK_WITH_PARAM
	attachInterruptParam(USER_BUTTON, led_toggle, CHANGE, (void*)USER_BUTTON);
#else
	attachInterrupt(USER_BUTTON, led_toggle2, CHANGE);
#endif

	Serial.begin(115200UL);
	Serial.setTimeout(100);
}

void example_exti_process()
{

}

