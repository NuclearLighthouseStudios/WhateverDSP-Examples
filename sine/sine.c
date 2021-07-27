#include <stdbool.h>
#include <stdlib.h>
#include <math.h>

#include <libwdsp.h>

float sample_rate;
float phase;

void wdsp_init(unsigned long int _sample_rate)
{
	sample_rate = _sample_rate;
}

void wdsp_process(float in_buffer[][2], float out_buffer[][2], unsigned long int nBlocks)
{
	for (int i = 0; i < nBlocks; i++)
	{
		float sample = sinf(phase);

		phase += (440.0f * 2.0f * M_PI) / sample_rate;
		if (phase > 2.0f * M_PI)
			phase -= 2.0f * M_PI;

		out_buffer[i][0] = sample;
		out_buffer[i][1] = sample;
	}
}
