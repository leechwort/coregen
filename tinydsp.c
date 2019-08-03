/*
 *  Copyright (c) 2019, Artem Synytsyn <a.synytsyn@gmail.com>
 *  All rights reserved.
 *
 *  SPDX-License-Identifier: BSD-3-Clause
 *  See COPYING file for more information.
 */
#include "tinydsp.h"
#include <stdio.h>
#include <math.h>
#include <string.h>
#include <complex.h>

tdsp_result tdsp_rfft_init(tdsp_rfft_cfg *cfg, unsigned int fftsize, bool is_ifft)
{
  cfg->is_ifft = is_ifft;
  cfg->fftsize = fftsize;
#if defined(USE_KISSFFT)
	cfg->cfg = kiss_fftr_alloc(fftsize, is_ifft, NULL, NULL);
#elif defined(USE_ARMDSP)
	cfg->status = arm_rfft_fast_init_f32(&cfg->S, fftsize);
	if ( cfg->status != ARM_MATH_SUCCESS)
	  {
	    return TDSP_FAIL;
	  }
#endif
	return TDSP_OK;
}

tdsp_result tdsp_rfft_perform(tdsp_rfft_cfg *cfg, float *input_data, float *output_data)
{
#if defined(USE_KISSFFT)
  if (cfg->is_ifft) {
    kiss_fftri(cfg->cfg, (const kiss_fft_scalar* )&input_data[0], (kiss_fft_cpx *)&output_data[0]);
    /* Output data should be normalized to the window size, as is done in ARM version */
    for (int i = 0; i < cfg->fftsize; i++)
      output_data[i]  /= cfg->fftsize;
  } else {
      kiss_fftr(cfg->cfg, (const kiss_fft_scalar* )&input_data[0], (kiss_fft_cpx *)&output_data[0]);
  }
#elif defined(USE_ARMDSP)
	arm_rfft_fast_f32(&cfg->S, input_data, output_data, cfg->is_ifft);
#endif
	return TDSP_OK;
}

tdsp_result tdsp_rfft_cleanup(tdsp_rfft_cfg *cfg)
{
#if defined(USE_KISSFFT)
	free(cfg->cfg);
#endif
	return TDSP_OK;
}
