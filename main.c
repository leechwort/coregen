#include "coregen.h"
#include <stdio.h>
#include <stdint.h>
#include <math.h>

#define N 44100



#define WINDOW_SIZE 1024
extern float wavetable_freqs[WINDOW_SIZE];
float wavetable_amps[WINDOW_SIZE];

int main()
{
	float target_freq = 440.0f;
	float limiting_freq = 800.0f;
	coregen_cfg_t config;
	coregen_init(&config, N, WINDOW_SIZE);
	//limit_harmonics(&config, wavetable_freqs, 800.0f / config.base_frequency);
	generage_wt(&config, wavetable_freqs, wavetable_amps);
	//display_wt_timedomain(&config, wavetable_amps);
	display_wt_freqdomain(&config, wavetable_freqs);
	/*for (int i = 0; i < 10; i++)
	{
		printf("-------------------\r\n");
		printf("Sample: %d\r\n", i);
		get_next_sample(&config, wavetable_amps, 440.0f);
	}*/

    // Create audio buffer
    int16_t buf[N] = {0}; // buffer
    int n;                // buffer index
    double Fs = 44100.0;  // sampling frequency

    // Generate 1 second of audio data - it's just a 1 kHz sine wave
    for (n=0 ; n<N ; ++n) buf[n] = 16383.0 * get_next_sample(&config, wavetable_amps, target_freq);

    // Pipe the audio data to ffmpeg, which writes it to a wav file
    FILE *pipeout;
    pipeout = popen("ffmpeg -y -f s16le -ar 44100 -ac 1 -i - beep.wav", "w");
    fwrite(buf, 2, N, pipeout);
    pclose(pipeout);

	return 0;
}
