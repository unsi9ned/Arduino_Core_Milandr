#include "milandr_hal.h"
#include "variant.h"

//------------------------------------------------------------------------------
// Полная таблица пинов с их функциональным назначением
//------------------------------------------------------------------------------
extern const tMilandrPin pinTable[][PIN_MUX_LINES_NUM][1];

//------------------------------------------------------------------------------
// Таблица пинов UART по умолчанию
//------------------------------------------------------------------------------
#define SSP_MOSI_LINE     (SSP_TXD_LINE - SSP_TXD_LINE)
#define SSP_MISO_LINE     (SSP_RXD_LINE - SSP_TXD_LINE)
#define SSP_CLK_LINE      (SSP_CLK_LINE - SSP_TXD_LINE)
#define SSP_CS_LINE       (SSP_FSS_LINE - SSP_TXD_LINE)
#define SSP_GET_LINE(n)   ((n < SSP_TXD_LINE) ? PERIPH_UNKNOWN_LINE : (n - SSP_TXD_LINE))
#define SSP_LINE_CNT      (SSP_FSS_LINE - SSP_TXD_LINE + 1)

#ifndef PIN_SPI1_SS
#define PIN_SPI1_SS       0xFF
#endif

#ifndef PIN_SPI1_MOSI
#define PIN_SPI1_MOSI     0xFF
#endif

#ifndef PIN_SPI1_MISO
#define PIN_SPI1_MISO     0xFF
#endif

#ifndef PIN_SPI1_SCK
#define PIN_SPI1_SCK      0xFF
#endif

#ifndef PIN_SPI2_SS
#define PIN_SPI2_SS       0xFF
#endif

#ifndef PIN_SPI2_MOSI
#define PIN_SPI2_MOSI     0xFF
#endif

#ifndef PIN_SPI2_MISO
#define PIN_SPI2_MISO     0xFF
#endif

#ifndef PIN_SPI2_SCK
#define PIN_SPI2_SCK      0xFF
#endif

static const uint8_t spiDefaultPins[SSP_COUNT][SSP_LINE_CNT] =
{
	[SSP_1] =
	{
		[SSP_MOSI_LINE] = PIN_SPI1_MOSI,
		[SSP_MISO_LINE] = PIN_SPI1_MISO,
		[SSP_CLK_LINE] = PIN_SPI1_SCK,
		[SSP_CS_LINE] = PIN_SPI1_SS,
	},
	[SSP_2] =
	{
		[SSP_MOSI_LINE] = PIN_SPI2_MOSI,
		[SSP_MISO_LINE] = PIN_SPI2_MISO,
		[SSP_CLK_LINE] = PIN_SPI2_SCK,
		[SSP_CS_LINE] = PIN_SPI2_SS,
	}
};

//------------------------------------------------------------------------------
// Возвращает номера пинов в соответствии с variant платы
//------------------------------------------------------------------------------
uint8_t milandr_spi_pin(tSspVariant spi, tPeriphLineVariant line)
{
	uint8_t index = SSP_GET_LINE(line);

	if(spi >= SSP_COUNT || index >= SSP_LINE_CNT)
		return 0xFF;
	else
		return spiDefaultPins[spi][index];
}
