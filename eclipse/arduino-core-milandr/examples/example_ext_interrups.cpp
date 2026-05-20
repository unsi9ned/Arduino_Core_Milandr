#include <Arduino.h>
#include "MDR32FxQI_config.h"

EXTERN_C_BEGIN

void EXT_INT1_IRQHandler(void)
{
	NVIC_DisableIRQ(EXT_INT1_IRQn);
	NVIC_ClearPendingIRQ(EXT_INT1_IRQn);
	return;
}

EXTERN_C_END

void example_exti_init()
{
	pinMode(USER_BUTTON, INPUT);
	pinMode(LED_BUILTIN, OUTPUT);
	pinMode(PA0, INPUT);
	milandr_gpio_sel_alter_func(PA0);
	/* Interrupts Enable */
	NVIC_EnableIRQ(EXT_INT1_IRQn);

	Serial.begin(115200UL);
	Serial.setTimeout(100);
}

void example_exti_process()
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

