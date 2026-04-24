/*
  Copyright (c) 2011 Arduino.  All right reserved.

  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.

  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
  See the GNU Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public
  License along with this library; if not, write to the Free Software
  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
*/

#include "api/Common.h"
#include "milandr/milandr_hal.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Get the number of milliseconds since the programme started running.
 *
 * The time at which any counters overflow should be documented here, together
 * with the resolution (minimum time step).
 *
 * @return Time elapsed since startup, in milliseconds.
 */
uint32_t millis(void)
{
	// ToDo: ensure no interrupts
	return milandr_current_millis();
}

/**
 * Get the number of microseconds since the programme started running.
 *
 * The time at which any counters overflow should be documented here, together
 * with the resolution (minimum time step).
 *
 * @return Time elapsed since startup, in microseconds.
 */
uint32_t micros(void)
{
	return milandr_current_micros();
}

/**
 * Wait for a given amount of time.
 *
 * This function generally busy waits, but it must not disable interrupts.
 *
 * @param ms Time to wait in milliseconds.
 */
void delay(uint32_t ms)
{
	milandr_delay(ms);
}

/**
 * Like delay(), with with microseconds instead of milliseconds.
 *
 * @param us Time to wait in microseconds.
 */
void delayMicroseconds(unsigned int us)
{
	milandr_delay_microseconds(us);
}

#ifdef __cplusplus
}
#endif
