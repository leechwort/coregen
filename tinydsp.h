/*
 *  Copyright (c) 2019, Artem Synytsyn <a.synytsyn@gmail.com>
 *  All rights reserved.
 *
 *  SPDX-License-Identifier: BSD-3-Clause
 *  See COPYING file for more information.
 */

#ifndef TINYDSP_H
#define TINYDSP_H

#include <stdbool.h>

/* Choose FFT engine USE_KISSFFT - generic USE_ARMDSP - ARM-specific */
#define USE_KISSFFT
//#define USE_ARMDSP

#if defined(USE_KISSFFT)
#include "kiss_fft.h"
#include "kiss_fftr.h"
#elif defined(USE_ARMDSP)
#include "arm_math.h"
#include "arm_const_structs.h"
#endif

typedef struct {

#if defined(USE_KISSFFT)
	kiss_fftr_cfg cfg;
#elif defined(USE_ARMDSP)
	arm_rfft_fast_instance_f32 S;
	arm_status status;
#endif
	bool is_ifft;
	int fftsize;
	float *input_data;
	float *output_data;
} tdsp_rfft_cfg;

typedef enum
{
    TDSP_OK,
    TDSP_FAIL
} tdsp_result;

/**
* @brief  Initialization function for the floating-point real FFT.
* @param[in,out] *cfg           points to an tdsp_rfft_cfg structure.
* @param[in]     fftLen         length of the Real Sequence.
* @param[in]     is_ifft        RFFT if flag is false, RIFFT if flag is true
* @return        The function returns TDSP_OK if initialization is successful or TDSP_FAIL
*                if <code>fftsize</code> is not a supported value.
*
* \par Description:
* \par
* The parameter <code>fftLen</code> Specifies length of RFFT/CIFFT process.
* Supported FFT Lengths are 32, 64, 128, 256, 512, 1024, 2048, 4096.
* \par
* This Function also initializes Twiddle factor table pointer and Bit reversal table pointer.
*/
tdsp_result tdsp_rfft_init(tdsp_rfft_cfg *cfg, unsigned int fftsize, bool is_ifft);
/**
* @brief Processing function for the floating-point real FFT.
* @param[in]  *cfg            points to an tdsp_rfft_cfg structure.
* @param[in]  *input_data     points to the input buffer.
* @param[in]  *output_data    points to the output buffer.
* @return     The function returns TDSP_OK if FFT procedure was ok
*/
tdsp_result tdsp_rfft_perform(tdsp_rfft_cfg *cfg, float *input_data, float *output_data);
/**
* @brief Cleanup function for the floating-point real FFT.
* @param[in]  *cfg            points to an tdsp_rfft_cfg structure.
* @return     The function returns TDSP_OK if FFT procedure was ok
*/
tdsp_result tdsp_rfft_cleanup(tdsp_rfft_cfg *cfg);

#endif /* TINYDSP_H */
