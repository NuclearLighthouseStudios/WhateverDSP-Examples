#include <stdbool.h>
#include <stdlib.h>
#include <math.h>

#include <libwdsp.h>

#include "airhorn.h"

unsigned int sample_pos = 0;
bool playing = false;
bool trigger = false;

void wdsp_process(float **in_buffer, float **out_buffer)
{
	if (io_digital_in(BUTTON_1) && !trigger)
	{
		sample_pos = 0;
		playing = true;
	}

	trigger = io_digital_in(BUTTON_1);
	float volume = io_analog_in(POT_1);

	for (int i = 0; i < BLOCK_SIZE; i++)
	{
		float sample = 0;
		if (playing)
		{
			// The samples are stored as 16 bit values so we have to scale
			// them to the required -1 to 1 range.
			sample = (((short int *)airhorn_raw)[sample_pos++]) / (float)0xffff;
			if (sample_pos >= airhorn_raw_len / 2)
			{
				playing = false;
				sample_pos = 0;
			}
		}

		out_buffer[0][i] = in_buffer[0][i] + sample * volume;
		out_buffer[1][i] = in_buffer[1][i] + sample * volume;
	}

	io_digital_out(LED_1, playing);
}

void wdsp_init(void)
{
	// This checks if the bypass variable was set in the config.ini for
	// the board we're currently compiling for.
#if CONFIG_EFFECT_BYPASS == true
	io_digital_out(BYPASS_L, true);
	io_digital_out(BYPASS_R, true);
#endif
}