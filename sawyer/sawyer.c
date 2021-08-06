#include <stdbool.h>
#include <stdlib.h>
#include <math.h>

#include <libwdsp.h>

#include "filters.h"

#define NUMVOICES 20
#define NUMOSCS 4

struct osc
{
	float phase;
	float freq;
	float vol;
	float output;
};

struct voice
{
	struct osc oscs[NUMOSCS];
	int note;
	float freq;
	float output[2];
	float env;
	struct filter lpg[2];
	bool active;
	bool sustained;
};

static struct voice voices[NUMVOICES];
static int currvoice = 0;

static const unsigned long int sample_rate = SAMPLE_RATE;
static const float sample_rate_inverse = 1.0f / (float)SAMPLE_RATE;

static bool sustain = false;

struct delay_line
{
	float *buffer;
	float damp;
	float output;
	int length;
	int pos;
};

#define NUMVERBS 4
static struct delay_line reverb[NUMVERBS];
static float reverb_times[NUMVERBS] = { 0.11187f, 0.0932f, 0.17123f, 0.2343f };

static struct filter reverb_lp;
static struct filter reverb_hp;

/* greets to wrl */
static inline float shape_tanh(const float x)
{
	/* greets to aleksey vaneev */
	const float ax = fabsf(x);
	const float x2 = x * x;

	return (x * (2.45550750702956f + 2.45550750702956f * ax + (0.893229853513558f + 0.821226666969744f * ax) * x2) /
		(2.44506634652299f + (2.44506634652299f + x2) * fabsf(x + 0.814642734961073f * x * ax)));
}

static inline float midi_to_freq(float note)
{
	return 440.0f * powf(2.0f, (note - 69.0f) / 12.0f);
}

static inline void osc_init(struct osc *osc, float freq, float vol)
{
	osc->freq = freq;
	osc->vol = vol;
	osc->output = 0;
	osc->phase = (float)rand() / (float)RAND_MAX;
}

static inline void osc_process(struct osc *osc)
{
	osc->phase += osc->freq * sample_rate_inverse;

	if (osc->phase >= 1.0f)
		osc->phase -= 1.0f;

	osc->output = (osc->phase - 0.5f) * osc->vol;
}

static inline void voice_init(struct voice *voice, int note, bool active)
{
	voice->note = note;
	voice->freq = midi_to_freq(note);

	for (int osc = 0; osc < NUMOSCS; osc++)
	{
		float detune = ((float)rand() / (float)RAND_MAX - 0.5f) * 0.25f;
		float freq = midi_to_freq(note + detune - (12 * (osc % 2)));
		osc_init(&(voice->oscs[osc]), freq, 1.0f);
	}

	voice->env = 0;

	for (int flt = 0; flt < 2; flt++)
		filter_init(&(voice->lpg[flt]), voice->env, ((float)rand() / (float)RAND_MAX) * 0.35f + 0.55f);

	voice->output[0] = 0;
	voice->output[1] = 0;

	voice->active = active;
}

static inline void voice_process(struct voice *voice)
{
	voice->output[0] = 0.0f;
	voice->output[1] = 0.0f;

	if ((!voice->active) && (voice->env < 0.01f))
		return;

	for (int osc = 0; osc < NUMOSCS; osc++)
	{
		osc_process(&(voice->oscs[osc]));
		voice->output[osc % 2] += voice->oscs[osc].output;
	}

	if (voice->active)
		voice->env += (1.0f - voice->env) * 0.25f * sample_rate_inverse;
	else
		voice->env -= voice->env * 0.1f * sample_rate_inverse;

	// if (voice->active)
	// 	voice->env = 1;
	// else
	// 	voice->env = 0;

	voice->lpg[0].cutoff = voice->env * 0.4f;
	voice->lpg[1].cutoff = voice->env * 0.4f;

	voice->output[0] = filter_lp_iir(voice->output[0], &(voice->lpg[0])) * voice->env;
	voice->output[1] = filter_lp_iir(voice->output[1], &(voice->lpg[1])) * voice->env;
}

static inline void delay_line_init(struct delay_line *delay_line, float length, float damp)
{
	delay_line->length = length * sample_rate;
	delay_line->buffer = malloc(sizeof(float) * delay_line->length);

	for (int i = 0; i < delay_line->length; i++)
		delay_line->buffer[i] = 0.0f;

	delay_line->pos = 0;
	delay_line->damp = damp;
	delay_line->output = 0;
}

static inline void delay_line_process(struct delay_line *delay_line, float input)
{
	delay_line->output = delay_line->buffer[delay_line->pos] + input;

	delay_line->buffer[delay_line->pos] = delay_line->buffer[delay_line->pos] * delay_line->damp + input;

	delay_line->pos++;
	delay_line->pos %= delay_line->length;
}

void wdsp_init(void)
{
	for (int voice = 0; voice < NUMVOICES; voice++)
	{
		voice_init(&voices[voice], 64, false);
	}

	for (int i = 0; i < NUMVERBS; i++)
	{
		delay_line_init(&(reverb[i]), reverb_times[i], 0.05f);
	}

	filter_init(&reverb_lp, 0.25f, 0.0f);
	filter_init(&reverb_hp, 0.001f, 0.0f);
}

void wdsp_process(float *in_buffer[BLOCK_SIZE], float *out_buffer[BLOCK_SIZE])
{
	float volume = io_analog_in(POT_4) * 0.5f;

	for (int i = 0; i < BLOCK_SIZE; i++)
	{
		// float l_sample = in_buffer[0][i];
		// float r_sample = in_buffer[1][i];

		float sample[2] = { 0.0f };

		for (int voice = 0; voice < NUMVOICES; voice++)
		{
			voice_process(&voices[voice]);
			sample[0] += voices[voice].output[0] * 0.1f;
			sample[1] += voices[voice].output[1] * 0.1f;
		}

		sample[0] = shape_tanh(sample[0]);
		sample[1] = shape_tanh(sample[1]);

		float rev_samp = 0.0f;

		for (int i = 0; i < NUMVERBS; i++)
		{
			rev_samp += reverb[i].output * (1.0f / (float)NUMVERBS);
		}

		rev_samp = filter_lp_iir(rev_samp, &reverb_lp);
		rev_samp = filter_hp_iir(rev_samp, &reverb_hp);

		sample[0] += rev_samp * 0.5f;
		sample[1] += rev_samp * 0.5f;

		for (int i = 0; i < NUMVERBS; i++)
		{
			delay_line_process(&(reverb[i]), sample[i % 2]);
		}

		out_buffer[0][i] = (sample[0] + (reverb[0].output + reverb[1].output)) * volume;
		out_buffer[1][i] = (sample[1] + (reverb[2].output + reverb[3].output)) * volume;
	}
}

void wdsp_idle(void)
{
	midi_message *message = midi_get_message();

	if (message != NULL)
	{
		if (message->command == NOTE_ON)
		{
			int note = message->data.note.note;

			int active_count = 0;
			while ((voices[currvoice].active) && (active_count < NUMVOICES))
			{
				currvoice++;
				active_count++;
				currvoice %= NUMVOICES;
			}

			voice_init(&(voices[currvoice]), note, true);

			currvoice++;
			currvoice %= NUMVOICES;
		}
		else if (message->command == NOTE_OFF)
		{
			int note = message->data.note.note;

			for (int i = 0; i < NUMVOICES; i++)
			{
				if (voices[i].note == note)
				{
					if (sustain)
						voices[i].sustained = true;
					else
						voices[i].active = false;
				}
			}
		}
		else if (message->command == CONTROL_CHANGE)
		{
			if (message->data.control.param == 64)
			{
				sustain = message->data.control.value > 10;

				if (!sustain)
				{
					for (int i = 0; i < NUMVOICES; i++)
					{
						if (voices[i].sustained)
						{
							voices[i].active = false;
							voices[i].sustained = false;
						}
					}
				}
			}
		}
	}
}