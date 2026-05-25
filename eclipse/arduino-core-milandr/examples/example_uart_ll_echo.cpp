#include <Arduino.h>
#include "milandr/milandr_hal.h"

void example_uart_ll_echo_init()
{
	Serial.begin(115200UL);
}

void example_uart_ll_echo_process()
{
	if(milandr_uart_available(UART_1))
	{
		int c = milandr_uart_read(UART_1);
		milandr_uart_write(UART_1, c);
	}
}

