#include <stdio.h>

#include <libwdsp.h>

void wdsp_process(float in_buffer[NUM_STREAMS][BLOCK_SIZE], float out_buffer[NUM_STREAMS][BLOCK_SIZE])
{
	for (int i = 0; i < BLOCK_SIZE; i++)
	{
		float l_samp = in_buffer[0][i];
		float r_samp = in_buffer[1][i];

		out_buffer[0][i] = l_samp;
		out_buffer[1][i] = r_samp;
	}
}
