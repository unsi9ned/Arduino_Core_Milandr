/*
 * Arduino Core for Milandr MCUs
 * Copyright (c) 2026 Andrey Osipov
 *
 * This file is part of Arduino_Core_Milandr.
 * Project home: https://github.com/unsi9ned/Arduino_Core_Milandr
 * Author's website: https://hamlab.net
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 */

#include "api/Common.h"
#include "milandr/milandr_hal.h"

static void abortTone(void * pin)
{
	uint8_t arduinoPin = (uint32_t)pin % milandr_gpio_count();
	noTone(arduinoPin);
}

/**
 * Generate a square wave on the specified pin & frequency at a 50% duty cycle.
 *
 * In the official API, only a single tone can be played at a time.
 * When this functions is called when a tone is already playing on a different
 * pin, then this function will have no effect. If the same pin is used, the
 * tone will be updated to the new frequency.
 *
 * To create multiple tones on different pins, the user must call noTone()
 * before calling tone() on a different pin.
 *
 * This function is non-blocking, so when called with a duration value, the
 * function will return immediately while the tone plays in the background.
 *
 * @param _pin The pin to output the tone.
 * @param frequency The frequency of the tone in Hz.
 * @param duration The duration of the tone in milliseconds.
 *                 If set to 0, the tone will play indefinitely or until
 *                 noTone() is called.
 *                 Optional argument with a default value of zero.
 */
void tone(uint8_t _pin, unsigned int frequency, unsigned long duration)
{
	if(milandr_tone_generate(_pin, (uint16_t)frequency) && duration)
	{
		milandr_set_delayed_task(abortTone, (void*)(uint32_t)_pin, duration);
	}
}

/**
 * Stop the generation of a tone on a pin.
 *
 * If the given pin is not playing a tone, or no tone is being played,
 * this function will have no effect.
 *
 * @param _pin The pin to stop playing the tone.
 */
void noTone(uint8_t _pin)
{
	milandr_cancel_delayed_task();
	milandr_tone_deinit(_pin);
}
