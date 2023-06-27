#include <math.h>

#include <libwdsp.h>

float sample_rate = SAMPLE_RATE;
float phase;

void wdsp_process(float *in_buffer[], float *out_buffer[])
{
	for (int i = 0; i < BLOCK_SIZE; i++)
	{
		float sample = sinf(phase);

		phase += (440.0f * 2.0f * M_PI) / sample_rate;
		if (phase > 2.0f * M_PI)
			phase -= 2.0f * M_PI;

		out_buffer[0][i] = sample;
		out_buffer[1][i] = sample;
	}
}
