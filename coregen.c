#include "coregen.h"
#include "tinydsp.h"

void generage_wt(coregen_cfg_t *cfg, float input_freqs[], float result[]) {
	tdsp_rfft_cfg config;
	//limit_harmonics(wavetable, 200.0f);
	tdsp_rfft_init(&config, cfg->window_size, true);
	tdsp_rfft_perform(&config, input_freqs, result);
	tdsp_rfft_cleanup(&config);
}

/* Cut all harminics with frequency higher then limiting frequency */
void limit_harmonics(coregen_cfg_t *cfg, float input_wavetable[], float limiting_freq) {
	int limiting_index;
	limiting_index = (2 * limiting_freq / cfg->base_frequency) - 3;
	for (;
			limiting_index < cfg->window_size;
			limiting_index++) {
		input_wavetable[limiting_index] = 0;
	}
}

float get_next_sample(coregen_cfg_t *cfg, float input_wavetable[], float target_frequency)
{
	float phase_change;
	int m, n; /* wavetable search indexes */
	float slope;
	float interpolated_value;
	phase_change = target_frequency / cfg->base_frequency;
	/* Interpolate wavetable */
	m = cfg->phase;
	n = cfg->phase + 1;
	slope = (input_wavetable[n] - input_wavetable[m]) / (n - m);
	interpolated_value = input_wavetable[m] + slope * (cfg->phase - m);
#ifdef DEBUG
	printf("Phase is: %f, nearest points are: %d, %d\r\n", cfg->phase, m, n);
	printf("Corresponding values in table are: %f, %f\r\n", input_wavetable[m], input_wavetable[n]);
	printf("Slope is: %f\r\n", slope);
	printf("Interpolated value: %f\r\n", interpolated_value);
#endif
	/* update phase change */
	cfg->phase += phase_change;
	if (cfg->phase > cfg->window_size)
		cfg->phase -= cfg->window_size;
	return interpolated_value;
}

void display_wt_timedomain(coregen_cfg_t *cfg, float input_wavetable[])
{
	printf("\r\n");
	for (int i = 0; i < cfg->window_size; i++) {
		printf("%f,\r\n", input_wavetable[i]);
	}
}

void display_wt_freqdomain(coregen_cfg_t *cfg, float input_wavetable[])
{
	float frequency = 0;
	for (int i = 0; i < cfg->window_size; i++) {
		if ((i + 1) % 2) {
			printf("%f Hz\t:%f\r\n", frequency, input_wavetable[i]);
			frequency += cfg->base_frequency;
		} else {
			printf("\t\t %f\r\n", input_wavetable[i]);
		}
	}
}

void coregen_init(coregen_cfg_t *cfg, int fs, int ws)
{
	cfg->fs = fs;
	cfg->window_size = ws;
	cfg->base_frequency = fs / ws;
}

float get_scanning_factor(coregen_cfg_t *cfg, float target_freq)
{
	return target_freq / cfg->base_frequency;
}
