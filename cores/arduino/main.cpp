#include <Arduino.h>

EXTERN_C_BEGIN

#include "milandr/milandr_hal.h"

EXTERN_C_END

// This function can be overwritten by the variant-specific code
void initVariant() __attribute__((weak));
void initVariant() {}

/**
 * Code to initialise the board goes here.
 */
#ifdef __cplusplus
extern "C" {
#endif

void init(void)
{
	milandr_systick_config();
}

#ifdef __cplusplus
}
#endif

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
