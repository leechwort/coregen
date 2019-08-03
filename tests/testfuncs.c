#include "testfuncs.h"
#define TEST_DEBUG
#define WINDOW_SIZE 1024

extern float wavetable[WINDOW_SIZE];
float output_buffer[WINDOW_SIZE];

void test_setup(void) {
	/* Nothing */
}

void test_teardown(void) {
	/* Nothing */
}

void limit_harmonics(float input_wavetable[], float cutoff_frequency) {
	const int wavetable_length = 1024;
	const int samplerate = 44100;
	int limiting_index = 10;
	for (limiting_index = 20;
			limiting_index < 1024;
			limiting_index++) {
		input_wavetable[limiting_index] = 0;
	}
}

MU_TEST(sine_wave_fftr_check) {
  tdsp_rfft_cfg config;
  limit_harmonics(wavetable, 200.0f);
  tdsp_rfft_init(&config, WINDOW_SIZE, true);
  tdsp_rfft_perform(&config, wavetable, output_buffer);
  tdsp_rfft_cleanup(&config);
#ifdef TEST_DEBUG
  printf("\r\n");
	for (int i = 0; i < WINDOW_SIZE; i++) {
		printf("%f,\r\n", output_buffer[i]);
		//printf("%f\r\n", wavetable[i]);

	}
#endif
}
/*
MU_TEST(sine_wave_ifftr_check) {
  tdsp_rfft_cfg config;
  tdsp_rfft_init(&config, WINDOW_SIZE, true);
  tdsp_rfft_perform(&config, ifft_input_buffer, output_buffer);
  tdsp_rfft_cleanup(&config);
#ifdef TEST_DEBUG
  printf("\r\n");
  for (int i = 0; i < WINDOW_SIZE; i++) {
    printf("%f:\t%f\r\n", output_buffer[i], ifft_correct_result[i]);
  }
#endif
  for (int i = 0; i < WINDOW_SIZE; i++) {
    mu_assert_double_eq(output_buffer[i], ifft_correct_result[i]);
  }
}
*/
MU_TEST_SUITE(test_suite) {
	MU_SUITE_CONFIGURE(&test_setup, &test_teardown);
	MU_RUN_TEST(sine_wave_fftr_check);
	//MU_RUN_TEST(sine_wave_ifftr_check);
}

void run_tests(void)
{
	MU_RUN_SUITE(test_suite);
	MU_REPORT();
}
