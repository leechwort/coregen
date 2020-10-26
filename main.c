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
	coregen_cfg_t config;
	coregen_init(&config, Fs, WT_SIZE);
	result = load_wt_from_file(&config,
			"/home/artsin/Dev/coregen/waves/sawtooth_2048.cgw");

	//CG_DEBUG("LOAD RESULT: %d\r\n", result);
	regenerate_wavetable(&config, 0);

	set_target_freq(&config, 1500.0f);
	//CG_DEBUG("Base frequency: %f\r\n", config.base_frequency);
	//CG_DEBUG("Target frequency: %f\r\n", config.target_frequency);

	/* Example generation */
	//for (int i = 0; i < 10; i++) {
	//	printf("-------------------\r\n");
	//	printf("Sample: %d\r\n", i);
	//	get_next_sample(&config);
	//}

	//generate_audio(&config, 1);
	//generate_audio_swipe(&config, 3, 50, 2000);
	generate_audio_swipe(&config, 20, 50, 15000);
	return 0;
}
