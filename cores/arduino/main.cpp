#include <Arduino.h>

EXTERN_C_BEGIN

#include "milandr/milandr_hal.h"

EXTERN_C_END

// This function can be overwritten by the variant-specific code
void initVariant() __attribute__((weak));

/**
 * Code to initialise the board goes here.
 */
EXTERN_C_BEGIN

void init(void)
{
	milandr_hal_init();
	milandr_systick_config();
}

EXTERN_C_END

int main(void)
{
	init();
	initVariant();

	setup();

	for (;;)
	{
		loop();

		// User can defined this function in a sketch to run after every loop iteration
		if(arduino::serialEventRun)
			arduino::serialEventRun();
	}

	return 0;
}
