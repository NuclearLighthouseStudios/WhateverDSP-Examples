#include <stdbool.h>
#include <stdlib.h>
#include <math.h>

#include <libwdsp.h>

#include "dsp.h"

struct filter lp_l;
struct filter lp_r;

struct filter hp_l;
struct filter hp_r;

void wdsp_init(void)
{
	lp_l.pos = 0;
	lp_l.vel = 0;
	lp_r.pos = 0;
	lp_r.vel = 0;

	hp_l.pos = 0;
	hp_l.vel = 0;
	hp_r.pos = 0;
	hp_r.vel = 0;
}

void wdsp_process(float in_buffer[NUM_STREAMS][BLOCK_SIZE], float out_buffer[NUM_STREAMS][BLOCK_SIZE])
{
	bool clip = false;

	bool bypass = io_digital_in(BUTTON_1);
	io_digital_out(MUTE, io_digital_in(BUTTON_2));

	lp_l.cutoff = io_analog_in(POT_1);
	lp_r.cutoff = io_analog_in(POT_1);

	hp_l.cutoff = io_analog_in(POT_2);
	hp_r.cutoff = io_analog_in(POT_2);

	lp_l.resonance = io_analog_in(POT_3)*0.95;
	lp_r.resonance = io_analog_in(POT_3)*0.95;

	float vol = io_analog_in(POT_4);

	for (int i = 0; i < BLOCK_SIZE; i++)
	{
		float l_samp = in_buffer[0][i];
		float r_samp = in_buffer[1][i];

		l_samp = filter_lp_iir(l_samp, &lp_l);
		r_samp = filter_lp_iir(r_samp, &lp_r);

		l_samp = filter_hp_iir(l_samp, &hp_l);
		r_samp = filter_hp_iir(r_samp, &hp_r);

		if ((fabs(l_samp) > 0.9) || (fabs(r_samp) > 0.9))
			clip = true;

		out_buffer[0][i] = bypass ? in_buffer[i][0] : l_samp * vol;
		out_buffer[1][i] = bypass ? in_buffer[i][1] : r_samp * vol;
	}

	io_digital_out(LED_1, clip);
}
