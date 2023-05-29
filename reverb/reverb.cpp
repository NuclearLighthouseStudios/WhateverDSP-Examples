#include <libwdsp.h>

#include "revmodel.hpp"

revmodel reverb;

bool active = false;

float silence[BLOCK_SIZE] = { 0 };

// This is a bit of a hack to get at the system ticks variable from the library.
// We should probably come up with a better way to do that.
volatile extern unsigned long int sys_ticks;

void wdsp_process(float *in_buffer[BLOCK_SIZE], float *out_buffer[BLOCK_SIZE])
{
	if (active)
	{
		reverb.processreplace(in_buffer[0], in_buffer[1], out_buffer[0], out_buffer[1], BLOCK_SIZE, 1);
	}
	else
	{
		for (int i = 0; i < BLOCK_SIZE; i++)
		{
			out_buffer[0][i] = in_buffer[0][i];
			out_buffer[1][i] = in_buffer[1][i];
		}

		reverb.processmix(silence, silence, out_buffer[0], out_buffer[1], BLOCK_SIZE, 1);
	}
}

void wdsp_idle(void)
{
	static unsigned long int button_start;
	static bool button_state = false;
	static bool just_activated;

	reverb.setwet(io_analog_in(POT_1));
	reverb.setroomsize(io_analog_in(POT_2));
	reverb.setmode(io_digital_in(BUTTON_1));

	reverb.update();

	if (io_digital_in(BUTTON_1) != button_state)
	{
		button_state = io_digital_in(BUTTON_1);

		if (button_state == true)
		{
			button_start = sys_ticks;
			if (!active)
			{
				active = true;
				just_activated = true;
			}
		}
		else
		{
			if (just_activated)
				just_activated = false;
			else
				if (active && (sys_ticks - button_start < 500))
					active = false;
		}
	}

	io_digital_out(LED_1, active);
}

void wdsp_init(void)
{
	reverb.setdry(1.0f);

#ifdef fxdsp
	io_digital_out(BYPASS_L, true);
	io_digital_out(BYPASS_R, true);
#endif
}