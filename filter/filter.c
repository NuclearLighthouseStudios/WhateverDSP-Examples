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
	lp_l.pos = 0.0f;
	lp_l.vel = 0.0f;
	lp_r.pos = 0.0f;
	lp_r.vel = 0.0f;

	hp_l.pos = 0.0f;
	hp_l.vel = 0.0f;
	hp_r.pos = 0.0f;
	hp_r.vel = 0.0f;
}

void wdsp_process(float *in_buffer[], float *out_buffer[])
{
	bool clip = false;

	bool bypass = io_digital_in(BUTTON_1);

#if CONFIG_EFFECT_TRUE_BYPASS == true
	io_digital_out(BYPASS_L, !bypass);
	io_digital_out(BYPASS_R, !bypass);
#endif

#if CONFIG_EFFECT_HARDWARE_MUTE == true
	io_digital_out(MUTE, io_digital_in(BUTTON_2));
#endif

	lp_l.cutoff = io_analog_in(POT_1);
	lp_r.cutoff = io_analog_in(POT_1);

	hp_l.cutoff = io_analog_in(POT_2);
	hp_r.cutoff = io_analog_in(POT_2);

	lp_l.resonance = io_analog_in(POT_3) * 0.95f;
	lp_r.resonance = io_analog_in(POT_3) * 0.95f;

	float vol = io_analog_in(POT_4);

	for (int i = 0; i < BLOCK_SIZE; i++)
	{
		float l_samp = in_buffer[0][i];
		float r_samp = in_buffer[1][i];

		l_samp = filter_lp_iir(l_samp, &lp_l);
		r_samp = filter_lp_iir(r_samp, &lp_r);

		l_samp = filter_hp_iir(l_samp, &hp_l);
		r_samp = filter_hp_iir(r_samp, &hp_r);

		if ((fabs(l_samp) > 0.9f) || (fabs(r_samp) > 0.9f))
			clip = true;

	#if CONFIG_EFFECT_TRUE_BYPASS == true
		out_buffer[0][i] = l_samp * vol;
		out_buffer[1][i] = r_samp * vol;
	#else
		out_buffer[0][i] = bypass ? in_buffer[0][i] : l_samp * vol;
		out_buffer[1][i] = bypass ? in_buffer[1][i] : r_samp * vol;
	#endif
	}

	io_digital_out(LED_1, clip);
}
