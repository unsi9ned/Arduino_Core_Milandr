#include "milandr_hal.h"
#include "variant.h"
#include "MDR32FxQI_config.h"
#include "MDR32FxQI_rst_clk.h"
#include "MDR32FxQI_port.h"
#include "MDR32FxQI_ssp.h"

//------------------------------------------------------------------------------
// Полная таблица пинов с их функциональным назначением
//------------------------------------------------------------------------------
extern const tMilandrPin pinTable[][PIN_MUX_LINES_NUM][1];

//------------------------------------------------------------------------------
// Таблица пинов UART по умолчанию
//------------------------------------------------------------------------------
#define SPI_MOSI_LINE     (SSP_TXD_LINE - SSP_TXD_LINE)
#define SPI_MISO_LINE     (SSP_RXD_LINE - SSP_TXD_LINE)
#define SPI_CLK_LINE      (SSP_CLK_LINE - SSP_TXD_LINE)
#define SPI_CS_LINE       (SSP_FSS_LINE - SSP_TXD_LINE)
#define SPI_GET_LINE(n)   ((n < SSP_TXD_LINE) ? PERIPH_UNKNOWN_LINE : (n - SSP_TXD_LINE))
#define SPI_LINE_CNT      (SSP_FSS_LINE - SSP_TXD_LINE + 1)

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

static const uint8_t spiDefaultPins[SSP_COUNT][SPI_LINE_CNT] =
{
	[SSP_1] =
	{
		[SPI_MOSI_LINE] = PIN_SPI1_MOSI,
		[SPI_MISO_LINE] = PIN_SPI1_MISO,
		[SPI_CLK_LINE] = PIN_SPI1_SCK,
		[SPI_CS_LINE] = PIN_SPI1_SS,
	},
	[SSP_2] =
	{
		[SPI_MOSI_LINE] = PIN_SPI2_MOSI,
		[SPI_MISO_LINE] = PIN_SPI2_MISO,
		[SPI_CLK_LINE] = PIN_SPI2_SCK,
		[SPI_CS_LINE] = PIN_SPI2_SS,
	}
};

static volatile MDR_SSP_TypeDef * spiModules[SSP_COUNT] =
{
	[SSP_1] = MDR_SSP1,
	[SSP_2] = MDR_SSP2,
};

//------------------------------------------------------------------------------
// Таблица масок регистра PER_CLOCK
//------------------------------------------------------------------------------
static const uint32_t perClockTable[SSP_COUNT] =
{
	[SSP_1] = RST_CLK_PCLK_SSP1,
	[SSP_2] = RST_CLK_PCLK_SSP2,
};

//------------------------------------------------------------------------------
// Статические переменные
//------------------------------------------------------------------------------
SSP_InitTypeDef sSSP;

//------------------------------------------------------------------------------
// Возвращает номера пинов в соответствии с variant платы
//------------------------------------------------------------------------------
uint8_t milandr_spi_pin(tSspVariant spi, tPeriphLineVariant line)
{
	uint8_t index = SPI_GET_LINE(line);

	if(spi >= SSP_COUNT || index >= SPI_LINE_CNT)
		return 0xFF;
	else
		return spiDefaultPins[spi][index];
}

//------------------------------------------------------------------------------
// Настройка скорости обмена данными
//------------------------------------------------------------------------------
static void spi_set_speed(MDR_SSP_TypeDef * SPIx, uint32_t speedMax)
{
	RST_CLK_FreqTypeDef RST_CLK_Clocks;
	RST_CLK_GetClocksFreq(&RST_CLK_Clocks);

	uint32_t cpsr = SPIx->CPSR;
	uint32_t cr0 = SPIx->CR0;

	/* Установка максимальной частоты (40 МГЦ)
	 *
	 *
	 * 27.7.2.5 CPSDVSR - Коэффициент  деления  тактовой  частоты.  Записываемое
	 * значение должно быть целым числом в диапазоне от 2 до 254.
	 * Младший  значащий  разряд  регистра  принудительно устанавливается в ноль
	 *
	 * 27.7.2.1 SCR - Скорость последовательного обмена. Значение  поля  SCR
	 * используется  при  формировании  тактового сигнала  обмена  данными.
	 * Информационная  скорость удовлетворяет соотношению:
	 * F_SSPCLK / (CPSDVR • (1 + SCR)),
	 * где  CPSDVR  –  четное  число  в  диапазоне  от  2  до  254
	 * (см. регистр SSPCPSR), а SCR – число от 0 до 255
	 *
	 * 27.6.9 Выбор  частоты  тактового  сигнала  SSPCLK  должен  обеспечивать
	 * поддержку требуемого  диапазона  скоростей  обмена  данными.  Отношение
	 * минимальной  частоты сигнала  SSPCLK  к  максимальной  частоте  сигнала
	 * SSP_CLK  в  режиме  ведомого устройства равно 12, в режиме ведущего – двум.
	 *
	 * FSSPCLKOUT = SSPCLK / (CPSDVSR * (1 + SCR))
	*/
	cpsr = 2;
	cr0 &= ~SSP_CR0_SCR_Msk;

	for(int32_t CPSDVR = 254; CPSDVR > 0; CPSDVR -= 2)
	{
		for(int32_t SCR = 255; SCR >= 0; SCR--)
		{
			uint32_t clkout = RST_CLK_Clocks.CPU_CLK_Frequency / (CPSDVR * (1 + SCR));

			if(clkout >= speedMax)
			{
				cpsr = CPSDVR & 0xFF;
				cr0 &= ~SSP_CR0_SCR_Msk;
				cr0 |= ((uint32_t)(SCR & 0xFF) << SSP_CR0_SCR_Pos);
			}
		}
	}

	SPIx->CPSR = cpsr;
	SPIx->CR0 = cr0;
}

//------------------------------------------------------------------------------
// Инициализация модуля SPI
//
// wordLen = 0..16, обычно 8 или 16
//------------------------------------------------------------------------------
tSspVariant milandr_spi_master_init(uint8_t  mosiPin,
                                    uint8_t  misoPin,
                                    uint8_t  clkPin,
                                    uint8_t  csPin,
                                    uint32_t speedMax,
                                    tMilandrBitOrder bitOrder,
                                    tMilandrSspMode mode)
{
	if(mosiPin > milandr_gpio_count() ||
	   misoPin > milandr_gpio_count() ||
	   clkPin > milandr_gpio_count())
	{
		return UART_UNKNOWN;
	}

	// Пины не могу повторяться
	uint8_t arduinoPins[4] = {mosiPin, misoPin, clkPin, csPin};

	for(int i = 0; i < 3; i++)
	{
		for(int j = i + 1; j < 4; j++)
		{
			if(arduinoPins[i] == arduinoPins[j]) return SSP_UNKNOWN;
		}
	}

	//
	// Сначала выполняем поиск модуля SSPn, соответствующего пинам
	// и описание пинов в таблице pinTable. Пином CS будем управлять вручную,
	// поэтому он не задействован в поиске
	//
	tSspVariant sspN = SSP_UNKNOWN;
	const tMilandrPin * lines[SPI_LINE_CNT - 1] = {NULL, NULL, NULL};
	const tMilandrPin * pins[(SPI_LINE_CNT - 1) * 2] =
	{
		&pinTable[mosiPin][PIN_MUX_ALTER][0],
		&pinTable[mosiPin][PIN_MUX_OVERRID][0],
		&pinTable[misoPin][PIN_MUX_ALTER][0],
		&pinTable[misoPin][PIN_MUX_OVERRID][0],
		&pinTable[clkPin][PIN_MUX_ALTER][0],
		&pinTable[clkPin][PIN_MUX_OVERRID][0],
	};

	for(tSspVariant sspIdx = SSP_1; sspIdx < SSP_COUNT; sspIdx++)
	{
		for(tPeriphLineVariant ln = SSP_TXD_LINE; ln <= SSP_CLK_LINE; ln++)
		{
			const tMilandrPin * temp[(SPI_LINE_CNT - 1) * 2];
			uint8_t cnt = milandr_find_pin(PERIPH_SSP, ln, sspIdx, pins, temp, (SPI_LINE_CNT - 1) * 2);

			if(cnt) lines[SPI_GET_LINE(ln)] = temp[0];
			else    lines[SPI_GET_LINE(ln)] = NULL;
		}

		if(lines[SPI_MOSI_LINE] && lines[SPI_MISO_LINE] && lines[SPI_CLK_LINE])
		{
			sspN = sspIdx;
			break;
		}
	}

	//
	// Не удалось определить модуль UART. Неверная комбинация пинов rxPin/txPin
	//
	if(sspN == SSP_UNKNOWN) return sspN;

	//
	// Производим настройку GPIO
	//
	const tMilandrPin * mosi = lines[SPI_MOSI_LINE];
	const tMilandrPin * miso = lines[SPI_MISO_LINE];
	const tMilandrPin * clk = lines[SPI_CLK_LINE];

	milandr_gpio_clock_enable(mosiPin, true);
	milandr_gpio_clock_enable(misoPin, true);
	milandr_gpio_clock_enable(clkPin, true);
	milandr_gpio_clock_enable(csPin, true);

	milandr_gpio_cfg_input(misoPin);
	milandr_gpio_cfg_output_pp(mosiPin);
	milandr_gpio_cfg_output_pp(clkPin);
	milandr_gpio_cfg_output_pp(csPin);

	if(mosi->mode == PORT_FUNC_ALTER)
		milandr_gpio_sel_alter_func(mosiPin);
	else
		milandr_gpio_sel_override_func(mosiPin);

	if(miso->mode == PORT_FUNC_ALTER)
		milandr_gpio_sel_alter_func(misoPin);
	else
		milandr_gpio_sel_override_func(misoPin);

	if(clk->mode == PORT_FUNC_ALTER)
		milandr_gpio_sel_alter_func(clkPin);
	else
		milandr_gpio_sel_override_func(clkPin);

	//
	// Настройка SSP
	//
	volatile MDR_SSP_TypeDef * SPIx = spiModules[sspN];
	MDR_RST_CLK->PER_CLOCK |= perClockTable[sspN];

	SSP_DeInit((MDR_SSP_TypeDef*)SPIx);

	/* Set Max Speed */
	spi_set_speed((MDR_SSP_TypeDef*)SPIx, speedMax);

	/* SSP MASTER configuration */
	sSSP.SSP_SCR        = (SPIx->CR0 & SSP_CR0_SCR_Msk) >> SSP_CR0_SCR_Pos;
	sSSP.SSP_CPSDVSR    = SPIx->CPSR & 0xFF;
	sSSP.SSP_Mode       = SSP_ModeMaster;
	sSSP.SSP_WordLength = SSP_WordLength8b;
	sSSP.SSP_SPH        = (SSP_Clock_Phase_TypeDef)(mode & SSP_SPH_2Edge);
	sSSP.SSP_SPO        = (SSP_Clock_Polarity_TypeDef)(mode & SSP_SPO_High);
	sSSP.SSP_FRF        = SSP_FRF_SPI_Motorola;
	sSSP.SSP_HardwareFlowControl = SSP_HardwareFlowControl_None;
	SSP_Init ((MDR_SSP_TypeDef*)SPIx, &sSSP);

	/* Enable SSP */
	SSP_Cmd((MDR_SSP_TypeDef*)SPIx, ENABLE);

	return sspN;
}

//------------------------------------------------------------------------------
// Деинициализация SPI, перевод пинов в режим Floating Input
//------------------------------------------------------------------------------
void milandr_spi_master_deinit(uint8_t  mosiPin,
                               uint8_t  misoPin,
                               uint8_t  clkPin,
                               uint8_t  csPin,
                               tSspVariant sspN)
{
	if(sspN >= SSP_COUNT) return;

	volatile MDR_SSP_TypeDef * SPIx = spiModules[sspN];

	SSP_Cmd((MDR_SSP_TypeDef*)SPIx, DISABLE);
	SSP_DeInit((MDR_SSP_TypeDef*)SPIx);
	MDR_RST_CLK->PER_CLOCK &= ~perClockTable[sspN];

	milandr_gpio_sel_port_func(mosiPin);
	milandr_gpio_sel_port_func(misoPin);
	milandr_gpio_sel_port_func(clkPin);
	milandr_gpio_sel_port_func(csPin);
	milandr_gpio_cfg_input(mosiPin);
	milandr_gpio_cfg_input(misoPin);
	milandr_gpio_cfg_input(clkPin);
	milandr_gpio_cfg_input(csPin);
}

//------------------------------------------------------------------------------
// Изменить конфигурацию SSP
// Примечание: порядок бит не настраиваемый, всегда MSB
//------------------------------------------------------------------------------
void milandr_spi_master_set_cfg(tSspVariant sspN,
                                uint32_t speedMax,
                                tMilandrBitOrder bitOrder,
                                tMilandrSspMode mode)
{
	if(sspN >= SSP_COUNT) return;

	volatile MDR_SSP_TypeDef * SPIx = spiModules[sspN];

	SSP_Cmd((MDR_SSP_TypeDef*)SPIx, DISABLE);

	/* Set Max Speed */
	spi_set_speed((MDR_SSP_TypeDef*)SPIx, speedMax);

	/* SSPx CR0 Configuration */
	uint32_t tmpreg = SPIx->CR0;
	tmpreg &= ~(SSP_CR0_SPH | SSP_CR0_SPO);
	tmpreg |= mode & (SSP_CR0_SPH | SSP_CR0_SPO);
	SPIx->CR0 = tmpreg;

	SSP_Cmd((MDR_SSP_TypeDef*)SPIx, ENABLE);
}

//------------------------------------------------------------------------------
// Чтение/запиcь одного байта
//------------------------------------------------------------------------------
uint16_t milandr_spi_master_write(tSspVariant sspN,
                                  uint16_t data,
                                  uint8_t wordLen)
{
	if(sspN >= SSP_COUNT) return 0xFF;

	int timeout = 0;
	volatile MDR_SSP_TypeDef * SPIx = spiModules[sspN];

#if 0
	uint32_t cr0 = SPIx->CR0;
	cr0 &= ~SSP_CR0_DSS_Msk;
	cr0 |= ((wordLen & 0xF) - 1) << SSP_CR0_DSS_Pos;
	SPIx->CR0 = cr0;
#else
	if(wordLen == 16)
		SPIx->CR0 |= 0x8ul;
	else
		SPIx->CR0 &= ~0x8ul;
#endif

	/* Wait for SPI1 Tx buffer empty */
	while (!(SPIx->SR & SSP_SR_TFE) && timeout++ < 1000) {}

	if(timeout >= 1000) return 0xFFFF;

	/* Send SPI1 data */
	SPIx->DR = data;
	/* Receive Data */
	return ((uint16_t)(SPIx->DR));
}

//------------------------------------------------------------------------------
// Чтение/запиcь массива данных
//------------------------------------------------------------------------------
void milandr_spi_master_block_write(tSspVariant sspN,
                                    uint8_t * data,
                                    uint32_t dataLen)
{
	if(sspN >= SSP_COUNT || !data || !dataLen) return;

	volatile MDR_SSP_TypeDef * SPIx = spiModules[sspN];

	// Сброс 16-битной передачи (она могла выполняться ранее)
	SPIx->CR0 &= ~0x8ul;

	while(dataLen--)
	{
		int timeout = 0;

		/* Wait for SPI1 Tx buffer empty */
		while (!(SPIx->SR & SSP_SR_TFE) && timeout++ < 1000) {}

		if(timeout >= 1000)
			*data++ = 0xFF;
		else
		{
			/* Send SPI1 data */
			SPIx->DR = *data;
			/* Receive Data */
			*data++ = ((uint8_t)SPIx->DR);
		}
	}
}
