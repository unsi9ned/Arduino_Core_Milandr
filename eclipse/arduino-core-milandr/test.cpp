#include <Arduino.h>
#include <SPI.h>

using namespace arduino;
#define UART_N UART_1
extern int milandr_uart_available(tUartVariant uartN);
extern int milandr_uart_read(tUartVariant uartN);
extern size_t milandr_uart_write(tUartVariant uartN, const uint8_t c);
extern size_t milandr_uart_send(tUartVariant n, const uint8_t* buf, const size_t size);

void setup(void)
{
	pinMode(LED_BUILTIN, OUTPUT);
	pinMode(USER_BUTTON, INPUT);

	Serial.begin(115200ul);
	Serial.setTimeout(100);

	SPI.begin();
}

void loop(void)
{
#if 0
	digitalWrite(LED_BUILTIN, HIGH); // turn the LED on (HIGH is the voltage level)
	Serial.print("LED is ON\n");
	delay(1000);                       // wait for a second
	digitalWrite(LED_BUILTIN, LOW); // turn the LED off by making the voltage LOW
	Serial.print("LED is OFF\n");
	delay(1000);                       // wait for a second
#elif 0
	while (Serial.available())
	{
		char c = Serial.read();
		Serial.print(c);
	}
#elif 1
	if (Serial.available())
	{
		String str = Serial.readString();
		Serial.println(str);
	}
#elif 1
	if(milandr_uart_available(UART_N))
	{
		int c = milandr_uart_read(UART_N);
		milandr_uart_write(UART_N, c);
	}
#elif 1
	char * str = "abcdefghijklmnopqrstuvwxyz\n";
	milandr_uart_send(UART_N, (const uint8_t*)str, 27);
	delay(1000);
#else
	PinStatus status = digitalRead(USER_BUTTON);

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
#endif
}
