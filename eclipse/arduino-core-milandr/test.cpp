#include <Arduino.h>

typedef struct
{
	void (*setup)();
	void (*loop)();
}
tTest;

typedef enum
{
	TEST_BLINK,
	HELLOWORLD,
	BUTTON,
	UART_ECHO,
	SPI_EEPROM
}
tTestId;

extern void example_blink_init();
extern void example_blink_process();
extern void example_helloworld_init();
extern void example_helloworld_process();
extern void example_button_init();
extern void example_button_process();
extern void example_uartecho_init();
extern void example_uartecho_process();
extern void example_spi_eeprom_init();
extern void example_spi_eeprom_process();


tTest testVariant[] =
{
	[TEST_BLINK] = {.setup = example_blink_init, .loop = example_blink_process},
	[HELLOWORLD] = {.setup = example_helloworld_init, .loop = example_helloworld_process},
	[BUTTON] = {.setup = example_button_init, .loop = example_button_process},
	[UART_ECHO] = {.setup = example_uartecho_init, .loop = example_uartecho_process},
	[SPI_EEPROM] = {.setup = example_spi_eeprom_init, .loop = example_spi_eeprom_process},
};

static tTestId currentTest = SPI_EEPROM;

//------------------------------------------------------------------------------
// Инициализация
//------------------------------------------------------------------------------
void setup(void)
{
	testVariant[currentTest].setup();
}

void loop(void)
{
	testVariant[currentTest].loop();
}

