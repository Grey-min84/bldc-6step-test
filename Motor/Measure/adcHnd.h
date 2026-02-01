#ifndef ADC_HND_H_
#define ADC_HND_H_
#include "IF_Hal.h"
#include "six_step.h"








typedef enum eADC_CH1_IDX{
	eADC_CH_CURR_A = 0,
	eADC_CH_CURR_B,
	eADC_CH_CURR_C,
	eADC_CH_BEMF_A,
	eADC_CH_BEMF_B,
	eADC_CH_TEMP,
	eADC_CH_MAX
}ADC_CH1_IDX_e;


typedef enum eADC_CH2_IDX{
	eADC_CH_BEMF_C = 0,
	eADC_CH_RESV,
	eADC_CH_THROTTLE,
}ADC_CH2_IDX_e;

#define ADC_SAMPLE_PER_CH   (4)
#define ADC_BUFFER_LENGTH    6


int32_t GetFilteredAdcValue(Adc_Idx_e eIdx);
void AdcMeas();
int32_t GetRawAdcValue(Adc_Idx_e eIdx);
void Init_6step_adcSampling(_6StepCtlCtx_t* ctx);
#endif