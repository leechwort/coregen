#include "coregen.h"
#include <stdio.h>
#include <stdint.h>
#include <math.h>

#define Fs 44100

#define WT_SIZE 1024
//extern float wavetable_freqs[WT_SIZE];
float wavetable_amps[WT_SIZE];
void generate_audio(coregen_cfg_t *cfg)
{
	// Create audio buffer
	int16_t buf[Fs] = { 0 }; // buffer
	int n;                   // buffer index

	// Generate 1 second of audio data
	for (n = 0; n < Fs; ++n)
		buf[n] = 16383.0 * get_next_sample(cfg);

	// Pipe the audio data to ffmpeg, which writes it to a wav file
	FILE *pipeout;
	pipeout = popen("ffmpeg -y -f s16le -ar 44100 -ac 1 -i - beep.wav", "w");
	fwrite(buf, 2, Fs, pipeout);
	pclose(pipeout);
}
int main() {
	int result;
	coregen_cfg_t config;
	coregen_init(&config, Fs, WT_SIZE);
	result = load_wt_from_file(&config,
			"/home/artsin/Dev/coregen/waves/sawtooth_512.cgw");
	CG_DEBUG("LOAD RESULT: %d\r\n", result);
	set_target_freq(&config, 2000.0f);
	//display_wt_timedomain(&config);
	//display_wt_freqdomain(&config);

	/* Example generation */
	/*for (int i = 0; i < 10; i++) {
		printf("-------------------\r\n");
		printf("Sample: %d\r\n", i);
		get_next_sample(&config);
	}*/
	generate_audio(&config);
	return 0;
}
