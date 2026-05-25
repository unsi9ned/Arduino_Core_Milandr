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

// Генератор на основе 32-битного LFSR (Linear Feedback Shift Register)
// Полином: x^32 + x^22 + x^2 + x + 1
static uint32_t _randomState = 1;

/**
 * Generate a pseudo random number from zero to (max - 1).
 *
 * @param max Upper bound for the generated number, exclusive of this number.
 *
 * @return A random integer between 0 and (max - 1).
 */
long random(long max)
{
	if(max <= 0)
	{
		return 0;
	}

	// Один шаг LFSR
	uint32_t lsb = _randomState & 1u;
	_randomState >>= 1;

	if(lsb)
	{
		_randomState ^= 0xB4BCD35C;  // Полином для LFSR
	}

	return (long) (_randomState % (uint32_t) max);
}

/**
 * Generate a pseudo random number from min to (max - 1).
 *
 * @param min Lower bound for the generated number, inclusive of this number.
 * @param max Upper bound for the generated number, exclusive of this number.
 *
 * @return A random integer between 0 and (max - 1).
 */
long random(long min, long max)
{
	if(min >= max)
	{
		return min;
	}
	return random(max - min) + min;
}

/**
 * Seeds the pseudo random number generator with a specific value.
 *
 * This function should be called before random(), otherwise the sequence of
 * values returned by random() will always be the same.
 *
 * Calling this function with a seed of 0 has no effect.
 */
void randomSeed(unsigned long seed)
{
	if(seed)
	{
		_randomState = (uint32_t) seed;
	}
	// Если seed == 0 — ничего не делаем, сохраняем текущее состояние
}
