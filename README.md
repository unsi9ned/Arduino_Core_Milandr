# Arduino Core для микроконтроллеров Milandr (MDR32FxFI)

[![License: LGPL v2.1](https://img.shields.io/badge/License-LGPL%20v2.1-blue.svg)](https://www.gnu.org/licenses/lgpl-2.1)
[![GitHub release](https://img.shields.io/github/v/release/unsi9ned/Arduino_Core_Milandr)](https://github.com/unsi9ned/Arduino_Core_Milandr/releases)

## Введение
Этот репозиторий добавляет поддержку микроконтроллеров **MDR32FxFI** в Arduino IDE 1.8.x.

Данное ядро разработано на базе:
- [Standard Peripherals Library (SPL)](https://support.milandr.ru/products/mikrokontrollery_i_protsessory/32_razryadnye_mikrokontrollery/k1986ve92fi/) - официальный пакет драйверов стандартной периферии от компании Миландр версии v1.3.2
- [CMSIS](https://www.keil.arm.com/cmsis): Cortex Microcontroller Software Interface Standard (CMSIS) - это независимый от производителя уровень аппаратной абстракции для серии процессоров Cortex®-M, который определяет общие интерфейсы инструментов
- [GNU Arm Embedded Toolchain](https://developer.arm.com/open-source/gnu-toolchain/gnu-rm): компилятор Arm Embedded GCC, библиотеки и другие инструменты, необходимые для разработки ПО для устройств на ядре Cortex®-M. Пакеты предоставляются благодаря [The xPack 3rd Party Development Tools](https://github.com/xpack-dev-tools/):
	- [https://github.com/xpack-dev-tools/arm-none-eabi-gcc-xpack](https://github.com/xpack-dev-tools/arm-none-eabi-gcc-xpack)
	- [https://github.com/xpack-dev-tools/openocd-xpack](https://github.com/xpack-dev-tools/openocd-xpack)

## Установка
Данный репозиторий доступен в виде пакета, который можно установить с помощью [Менеджера плат Arduino](https://docs.arduino.cc/learn/starting-guide/cores/)

Для установки пакета необходимо выполнить следующие действия:
1. Откройте **Файл → Настройки** в Arduino IDE.
2. В поле **Дополнительные ссылки для Менеджера плат** вставьте:
   `https://raw.githubusercontent.com/unsi9ned/Arduino_Core_Milandr/refs/heads/master/package_milandr_index.json`
3. Перейдите в **Инструменты → Плата → Менеджер плат**.
4. Найдите **Milandr MDR32FxFI** и нажмите **Установить**.

## Особенности
- Поддержка цифровых и аналоговых пинов (`pinMode`, `digitalRead`, `digitalWrite`, `analogRead`, `analogWrite`)
- Аппаратная поддержка интерфейсов: UART (`Serial`), SPI, I2C (`Wire`)
- Поддержка ШИМ (`PWM`) и ЦАП (`DAC`)
- Внешние прерывания (`attachInterrupt`, `detachInterrupt`)
- Функции генерации звука (`tone`/`noTone`)
- Функции управления временем и задержками (`delay`, `delayMicroseconds`, `millis`, `micros`)
- Функции генерации случайных чисел (`random`)

## Поддерживаемые платы
- [Milandr K1986BE92FI-Mini](https://github.com/unsi9ned/Arduino_Core_Milandr/tree/master/variants/MDR32FxQI/MDR1211FI) (128KB Flash, 32KB RAM, Cortex-M3 @ 80MHz)

