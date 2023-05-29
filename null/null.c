#include <libwdsp.h>

void wdsp_process(float *in_buffer[BLOCK_SIZE], float *out_buffer[BLOCK_SIZE])
{
	for (int i = 0; i < BLOCK_SIZE; i++)
	{
		float l_samp = in_buffer[0][i];
		float r_samp = in_buffer[1][i];

		out_buffer[0][i] = l_samp;
		out_buffer[1][i] = r_samp;
	}
}

void wdsp_init(void)
{
	// This checks if the bypass setting was set in the config.ini for
	// the board we're currently compiling for.
#if CONFIG_EFFECT_BYPASS == true
	io_digital_out(BYPASS_L, true);
	io_digital_out(BYPASS_R, true);
#endif
}