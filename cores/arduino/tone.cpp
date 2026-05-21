#include "api/Common.h"
#include "milandr/milandr_hal.h"

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
	milandr_tone_generate(_pin, (uint16_t)frequency);
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
	milandr_tone_deinit(_pin);
}
