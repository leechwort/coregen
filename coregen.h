#ifndef __COREGEN_H
#define __COREGEN_H

//#define DEBUG

#ifdef DEBUG
#define CG_DEBUG(fmt, args...)    printf(fmt, ## args)
#else
#define CG_DEBUG(fmt, args...)
#endif

typedef struct {
	int fs;
	int window_size;
	float base_frequency;
	float target_frequency;
	float phase; /* from 0.0f to window size */
	float *wt_f_data; /* wavetable amplitudes in frequency domain */
	float *wt_t_data; /* wavetable amplitudes in time domain */
	float *wt_f_data_raw; /* same as wt_f_data, but without harmonics limit */
} coregen_cfg_t;

enum morph_method {
	MORPH_SUM,
	MORPH_SUB,
	MORPH_DIV,
	MORPH_PROD
};

enum coregen_error {
	CG_OK,
	CG_MEMORY_ERROR,
	CG_NO_FILE,
	CG_WTLENGTH_ERROR,
	CG_IFFT_FAIL
};
/* We need to recalculate limit of harmonics when frequency changes
 * more then this value
 */

void coregen_init(coregen_cfg_t *cfg, int fs, int ws);
int load_wt_from_file(coregen_cfg_t *cfg, const char *filename);
int allocate_wt(coregen_cfg_t *cfg, int wavetable_size);
int morph_wt(coregen_cfg_t *wt_one, coregen_cfg_t *wt_two,
		coregen_cfg_t *wt_result, float morph_fraq, int morph_type);
int set_target_freq(coregen_cfg_t *cfg, float target_frequency);
int regenerate_wavetable(coregen_cfg_t *cfg, int cutoff_harmonic);
float get_next_sample(coregen_cfg_t *cfg);
/* Misc functions */
void display_wt_timedomain(coregen_cfg_t *cfg);
void display_wt_freqdomain(coregen_cfg_t *cfg);
#endif /* __COREGEN_H */
