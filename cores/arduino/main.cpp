#include <Arduino.h>

#ifdef __cplusplus
/* include outside of extern C block, this is basically a C++ library */
extern "C" {
#endif /* __cplusplus */

#include "milandr/systick.h"

#ifdef __cplusplus
}
#endif /* __cplusplus */

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
	systick_config();
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
