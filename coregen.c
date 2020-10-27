#include "coregen.h"
#include "tinydsp.h"


int allocate_wt(coregen_cfg_t *cfg, int wavetable_size) {
	cfg->window_size = wavetable_size;
	//CG_DEBUG("Read size is: %d\r\n", cfg->window_size);
	if (wavetable_size == 0) {
		return CG_WTLENGTH_ERROR;
	}
	cfg->wt_f_data = malloc(sizeof(*cfg->wt_f_data) * wavetable_size);
	if (!cfg->wt_f_data)
		return CG_MEMORY_ERROR;
	memset(cfg->wt_f_data, 0, cfg->window_size * sizeof *cfg->wt_f_data);
	cfg->wt_t_data = malloc(sizeof(*cfg->wt_t_data) * wavetable_size);
	if (!cfg->wt_t_data) {
		free(cfg->wt_f_data);
		return CG_MEMORY_ERROR;
	}
	memset(cfg->wt_t_data, 0, cfg->window_size * sizeof *cfg->wt_t_data);
	cfg->wt_f_data_raw = malloc(sizeof(*cfg->wt_f_data_raw) * wavetable_size);
	if (!cfg->wt_f_data_raw) {
		free(cfg->wt_f_data);
		free(cfg->wt_t_data);
		return CG_MEMORY_ERROR;
	}
	memset(cfg->wt_f_data_raw, 0, cfg->window_size * sizeof *cfg->wt_f_data_raw);
	return CG_OK;
}
int morph_wt(coregen_cfg_t *wt_one, coregen_cfg_t *wt_two,
		coregen_cfg_t *wt_result, float morph_fraq, int morph_type) {
	wt_result->fs = wt_one->fs;
	wt_result->window_size = wt_one->window_size;
	wt_result->base_frequency = wt_one->base_frequency;
	wt_result->target_frequency = wt_one->target_frequency;
	wt_result->phase = 0.0f;

	if (morph_type == MORPH_SUM) {
		for (int i = 0; i < wt_one->window_size; i++) {
			wt_result->wt_f_data_raw[i] = morph_fraq
					* (wt_one->wt_f_data_raw[i])
					+ (1.0f - morph_fraq) * (wt_two->wt_f_data_raw[i]);
		}
	}
	return CG_OK;
}

int load_wt_from_file(coregen_cfg_t *cfg, const char *filename) {
	int wavetable_size = 0;
	int index = 0;
	int result = CG_OK;
	FILE *file = fopen(filename, "r");
	if (!file)
		return CG_NO_FILE;

	fscanf(file, "%d\n", &wavetable_size);
	cfg->window_size = wavetable_size;
	//CG_DEBUG("Read size is: %d\r\n", cfg->window_size);
	if (wavetable_size == 0) {
		return CG_WTLENGTH_ERROR;
	}
	cfg->wt_f_data = malloc(sizeof(*cfg->wt_f_data) * wavetable_size);
	if (!cfg->wt_f_data)
		return CG_MEMORY_ERROR;
	cfg->wt_t_data = malloc(sizeof(*cfg->wt_t_data) * wavetable_size);
	if (!cfg->wt_t_data) {
		free(cfg->wt_f_data);
		return CG_MEMORY_ERROR;
	}
	cfg->wt_f_data_raw = malloc(sizeof(*cfg->wt_f_data_raw) * wavetable_size);
	if (!cfg->wt_f_data_raw) {
		free(cfg->wt_f_data);
		free(cfg->wt_t_data);
		return CG_MEMORY_ERROR;
	}
	while (fscanf(file, "%f *\n", &cfg->wt_t_data[index]) != EOF) {
		//CG_DEBUG("%d: %f\n", index, cfg->wt_f_data_raw[index]);
		//CG_DEBUG("%d: %f\n", index, cfg->wt_f_data_raw[index]);
		index++;
		if (index > cfg->window_size) {
			result = CG_WTLENGTH_ERROR;
			break;
		}
	}
	fclose(file);
	// Convert time-domain data to frequency domain
	tdsp_rfft_cfg config;
	tdsp_result res;
	res = tdsp_rfft_init(&config, cfg->window_size, false);
	if (res != TDSP_OK)
		return CG_IFFT_FAIL;
	res = tdsp_rfft_perform(&config, cfg->wt_t_data, cfg->wt_f_data_raw);
	tdsp_rfft_cleanup(&config);
	if (res != TDSP_OK)
		return CG_IFFT_FAIL;

	memset(cfg->wt_t_data, 0, cfg->window_size * sizeof *cfg->wt_t_data);
	return result;
}

int destroy_wt(coregen_cfg_t *cfg)
{
	if (cfg->wt_f_data)
		free(cfg->wt_f_data);
	if (cfg->wt_f_data_raw)
			free(cfg->wt_f_data);
	if (cfg->wt_t_data)
		free(cfg->wt_t_data);
	return CG_OK;
}

int generage_wt(coregen_cfg_t *cfg) {
	tdsp_rfft_cfg config;
	tdsp_result res;
	res = tdsp_rfft_init(&config, cfg->window_size, true);
	if (res != TDSP_OK)
		return CG_IFFT_FAIL;
	res = tdsp_rfft_perform(&config, cfg->wt_f_data, cfg->wt_t_data);
	tdsp_rfft_cleanup(&config);
	if (res != TDSP_OK)
		return CG_IFFT_FAIL;
	return CG_OK;
}

int regenerate_wavetable(coregen_cfg_t *cfg, int cutoff_harmonic) {
	tdsp_rfft_cfg config;
	tdsp_result res;
	res = tdsp_rfft_init(&config, cfg->window_size, true);
	if (res != TDSP_OK)
		return CG_IFFT_FAIL;

	// Copy and limit harmonics
	memset(cfg->wt_f_data, 0, cfg->window_size * sizeof *cfg->wt_f_data);
	//memcpy(cfg->wt_f_data, cfg->wt_f_data_raw, (cfg->window_size - cutoff_harmonic) * sizeof *cfg->wt_f_data);
	memcpy(cfg->wt_f_data, cfg->wt_f_data_raw, cutoff_harmonic * sizeof *cfg->wt_f_data);

	res = tdsp_rfft_perform(&config, cfg->wt_f_data, cfg->wt_t_data);
	tdsp_rfft_cleanup(&config);
	if (res != TDSP_OK)
		return CG_IFFT_FAIL;
	return CG_OK;
}

/* Cut all harminics with frequency higher then limiting frequency
 * Notice, that limit is related to the base sample base frequency, if you sweep
 * sample faster, all frequencies are multiplied to sweep speed factor!
 */
int limit_harmonics(coregen_cfg_t *cfg, float limiting_freq) {
	int limiting_index;
	/* Recalculate limiting freq to new target frequency */
	limiting_freq /= cfg->target_frequency / cfg->base_frequency;
	limiting_index = (2 * limiting_freq / cfg->base_frequency) - 3;
	/* limiting freq should be recalculated like this:
	 * limiting_freq /= cfg->target_frequency / cfg->base_frequency;
	 * after substitution to this:
	 * limiting_index = (2 * limiting_freq / cfg->base_frequency) - 3;
	 * we've got such formula of limiting index, we should zero all values
	 * with index higher then this
	 */
	limiting_index = (cfg->fs / (cfg->target_frequency)) + 3;
	//printf("Harmonics limited to: %f\r\n", limiting_freq);
	//printf("Target freq is: %f\r\n", cfg->target_frequency);
	//CG_DEBUG("Actual limiting freq: %f\r\n",
	//		limiting_freq / (cfg->target_frequency / cfg->base_frequency));

	//printf("Limiting index is: %d\r\n", limiting_index);
	return limiting_index;
}

int set_target_freq(coregen_cfg_t *cfg, float target_frequency) {

	/* Frequency of higher harmonics should be not higher then fs/2 */
	cfg->target_frequency = target_frequency;
	//int cutoff_harmonic = limit_harmonics(cfg, cfg->fs / 2);
	int cutoff_harmonic = limit_harmonics(cfg, cfg->fs / 2);
	regenerate_wavetable(cfg, cutoff_harmonic);

	return CG_OK;
}

float get_next_sample(coregen_cfg_t *cfg)
{
	float phase_change;
	int m, n; /* wavetable search indexes */
	float slope;
	float interpolated_value;
	phase_change = cfg->target_frequency / cfg->base_frequency;
	/* Interpolate wavetable */
	m = cfg->phase;
	n = cfg->phase + 1;
	slope = (cfg->wt_t_data[n] - cfg->wt_t_data[m]) / (n - m);
	interpolated_value = cfg->wt_t_data[m] + slope * (cfg->phase - m);
	CG_DEBUG("Phase is: %f, nearest points are: %d, %d\r\n", cfg->phase, m, n);
	CG_DEBUG("Corresponding values in table are: %f, %f\r\n", cfg->wt_t_data[m], cfg->wt_t_data[n]);
	CG_DEBUG("Slope is: %f\r\n", slope);
	CG_DEBUG("Interpolated value: %f\r\n", interpolated_value);
	/* update phase change */

	cfg->phase += phase_change;
	if (cfg->phase > cfg->window_size) {
		cfg->phase -= cfg->window_size;
	}
	return interpolated_value;
}

void display_wt_timedomain(coregen_cfg_t *cfg)
{
	printf("\r\n");
	for (int i = 0; i < cfg->window_size; i++) {
		printf("%.9g,\r\n", cfg->wt_t_data[i]);
	}
}

void display_wt_freqdomain(coregen_cfg_t *cfg)
{
	float frequency = 0;
	for (int i = 0; i < cfg->window_size; i++) {
		CG_DEBUG("Index: %d\r\n", i);
		if ((i + 1) % 2) {
			CG_DEBUG("%f Hz\t:%f\r\n", frequency, cfg->wt_f_data_raw[i]);
			//CG_DEBUG("%f Hz\t:%f\r\n", frequency, cfg->wt_f_data_raw[i]);
			frequency += cfg->base_frequency;
		} else {
			CG_DEBUG("\t\t %f\r\n", cfg->wt_f_data_raw[i]);
		}
	}
}

void coregen_init(coregen_cfg_t *cfg, int fs, int ws)
{
	cfg->fs = fs;
	cfg->window_size = ws;
	cfg->base_frequency = 1.0 * fs / ws;
	cfg->phase = 0.0f;
	//CG_DEBUG("CG INIT: base freq is:%f\r\n", cfg->base_frequency);
}
