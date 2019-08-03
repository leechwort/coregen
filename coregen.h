#ifndef __COREGEN_H
#define __COREGEN_H

typedef struct {
	int fs;
	int window_size;
	float base_frequency;
	float phase; /* from 0.0f to window size */
} coregen_cfg_t;
//#define DEBUG
void coregen_init(coregen_cfg_t *cfg, int fs, int ws);
void generage_wt(coregen_cfg_t *cfg, float input_freqs[], float result[]);
void limit_harmonics(coregen_cfg_t *cfg, float input_wavetable[], float limiting_freq);
float get_scanning_factor(coregen_cfg_t *cfg, float target_freq);
float get_next_sample(coregen_cfg_t *cfg, float input_wavetable[], float target_frequency);
/* Misc functions */
void display_wt_timedomain(coregen_cfg_t *cfg, float input_wavetable[]);
void display_wt_freqdomain(coregen_cfg_t *cfg, float input_wavetable[]);
#endif /* __COREGEN_H */
