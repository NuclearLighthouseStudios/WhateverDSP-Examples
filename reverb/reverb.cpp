#include <libwdsp.h>

#include "revmodel.hpp"

revmodel reverb;

void wdsp_process(float in_buffer[NUM_STREAMS][BLOCK_SIZE], float out_buffer[NUM_STREAMS][BLOCK_SIZE])
{
	reverb.processreplace(in_buffer[0], in_buffer[1], out_buffer[0], out_buffer[1], BLOCK_SIZE, 1);
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