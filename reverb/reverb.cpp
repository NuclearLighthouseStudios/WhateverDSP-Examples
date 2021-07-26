#include <libwdsp.h>

#include "revmodel.hpp"

revmodel reverb;

void wdsp_process(float in_buffer[][2], float out_buffer[][2], unsigned long int nBlocks)
{
	reverb.processreplace((float *)in_buffer, (float *)in_buffer + 1, (float *)out_buffer, (float *)out_buffer + 1, nBlocks, 2);
}

void wdsp_idle(void)
{
	float wet_dry = io_analog_in(POT_1);
	reverb.setdry(1.0f - wet_dry);
	reverb.setwet(wet_dry);

	reverb.setroomsize(io_analog_in(POT_2));
	reverb.setwidth(io_analog_in(POT_3));
	reverb.setdamp(1.0f - io_analog_in(POT_4));

	reverb.setmode(io_digital_in(BUTTON_1));

	reverb.update();
}