#include "coregen.h"
#include <stdio.h>
#include <stdint.h>
#include <math.h>

#define Fs 44100

#define WT_SIZE 2048


void generate_audio(coregen_cfg_t *cfg, float sample_length)
{
	// Create audio buffer
	int16_t buf[Fs] = { 0 };// buffer
	int n;			// buffer index

	set_target_freq(cfg, 50);
	// Generate 1 second of audio data
	for (n = 0; n < Fs * sample_length; ++n)
		buf[n] = 16383.0 * get_next_sample(cfg);

	// Pipe the audio data to ffmpeg, which writes it to a wav file
	FILE *pipeout;
	pipeout = popen("ffmpeg -y -f s16le -ar 44100 -ac 1 -i - beep_2048.wav", "w");
	fwrite(buf, 2, Fs, pipeout);
	pclose(pipeout);
}

void generate_audio_swipe(coregen_cfg_t *cfg, float sample_length, float freq_start, float freq_stop)
{
	int n;			// buffer index
	int16_t buf[Fs * 20] = { 0 };
	// Generate 1 second of audio data
	float freq_step = (freq_stop - freq_start) / (Fs * sample_length);
	for (n = 0; n < Fs * sample_length; ++n) {
		buf[n] = 16383.0 * get_next_sample(cfg);
		set_target_freq(cfg, freq_start + freq_step * n);
	}
	// Pipe the audio data to ffmpeg, which writes it to a wav file
	FILE *pipeout;
	pipeout = popen("ffmpeg -y -f s16le -ar 44100 -ac 1 -i - beep_2048.wav", "w");
	fwrite(buf, 2, Fs * sample_length, pipeout);
	pclose(pipeout);
}
int main() {
	int result;
	coregen_cfg_t wt_one, wt_two, wt_mix;
	coregen_init(&wt_one, Fs, WT_SIZE);
	coregen_init(&wt_two, Fs, WT_SIZE);
	coregen_init(&wt_mix, Fs, WT_SIZE);
	load_wt_from_file(&wt_one,
			"/home/artsin/Dev/coregen/waves/exp_sin_2048.cgw");

	load_wt_from_file(&wt_two,
			"/home/artsin/Dev/coregen/waves/sawtooth_2048.cgw");
	allocate_wt(&wt_mix, WT_SIZE);

	morph_wt(&wt_one, &wt_two, &wt_mix, 0.95, MORPH_SUM);
	//CG_DEBUG("LOAD RESULT: %d\r\n", result);
	//regenerate_wavetable(&config, 0);
	//set_target_freq(&config, 1500.0f);

	//generate_audio_swipe(&config, 3, 50, 2000);
	//display_wt_freqdomain(&wt_mix);
	generate_audio_swipe(&wt_mix, 5, 20, 10000);
	//generate_audio(&wt_mix, 1);
	return 0;
}
