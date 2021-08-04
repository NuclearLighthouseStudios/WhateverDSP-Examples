#include <stdbool.h>
#include <stdlib.h>
#include <math.h>

#include <libwdsp.h>

#include "airhorn.h"

unsigned int sample_pos = 0;
bool playing = false;
bool trigger = false;

void wdsp_process(float in_buffer[NUM_STREAMS][BLOCK_SIZE], float out_buffer[NUM_STREAMS][BLOCK_SIZE])
{
	if (io_digital_in(BUTTON_1) && !trigger)
	{
		sample_pos = 0;
		playing = true;
	}

	trigger = io_digital_in(BUTTON_1);

	for (int i = 0; i < BLOCK_SIZE; i++)
	{
		float sample = 0;
		if (playing)
		{
			sample = (((short int *)airhorn_raw)[sample_pos++]) / (float)0xffff;
			if (sample_pos >= airhorn_raw_len/2)
			{
				playing = false;
				sample_pos = 0;
			}
		}

		out_buffer[0][i] = sample;
		out_buffer[1][i] = sample;
	}

	io_digital_out(LED_1, playing);
}
