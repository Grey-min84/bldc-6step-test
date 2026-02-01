#ifndef ADC_HND_H_
#define ADC_HND_H_
#include "IF_Hal.h"
#include "six_step.h"

enum eADC_CH_IDX{
	eADC_CH_CURR_A = 0,
	eADC_CH_CURR_B,
	eADC_CH_CURR_C,
	eADC_CH_VBUS,
	eADC_CH_BEMF_A,
	eADC_CH_BEMF_B,
	eADC_CH_BEMF_C,
	eADC_CH_MAX
};


#define ADC_SAMPLE_PER_CH   (4)
#define ADC_BUFFER_LENGTH    6



void AdcMeas();

void Init_6step_adcSampling(_6StepCtlCtx_t* ctx);
#endif