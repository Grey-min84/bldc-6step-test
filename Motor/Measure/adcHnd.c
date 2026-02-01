#include "adcHnd.h"
#include "measSupport.h"

float g_fCurrOffset[eADC_CH_MAX];
float g_fCurrMeas[3];


uint16_t g_usThrottleAdc = 0;

uint32_t adc_multimode_buffer[ADC_BUFFER_LENGTH*ADC_SAMPLE_PER_CH];
uint16_t g_adc_buffer_ch1[ADC_BUFFER_LENGTH*ADC_SAMPLE_PER_CH];
uint16_t g_adc_buffer_ch2[ADC_BUFFER_LENGTH*ADC_SAMPLE_PER_CH];


void Init_6step_adcSampling(_6StepCtlCtx_t* ctx){
	HAL_ADCEx_MultiModeStart_DMA(&hadc1, adc_multimode_buffer, ADC_BUFFER_LENGTH*ADC_SAMPLE_PER_CH);

}





void AdcOffsetMeas(){

    HAL_Delay(100);

    for(int idx=0; idx<200; idx++) {

	for(int jdx = 0; jdx < ADC_BUFFER_LENGTH; jdx++) {

		u32 avgVal = 0;

        avgVal = Calculate_AverageU32_lower(adc_multimode_buffer, jdx, ADC_SAMPLE_PER_CH,  ADC_BUFFER_LENGTH);
        g_adc_buffer_ch1[jdx] = (uint16_t)(avgVal & 0xFFFF);        // ADC1 채널

        avgVal = Calculate_AverageU32_upper(adc_multimode_buffer, jdx, ADC_SAMPLE_PER_CH,  ADC_BUFFER_LENGTH);
        g_adc_buffer_ch2[jdx] = (uint16_t)(avgVal & 0xFFFF);        // ADC2 채널

    }

		HAL_Delay(1);
	}
}


void AdcMeas(){

    for(int jdx = 0; jdx < ADC_BUFFER_LENGTH*ADC_SAMPLE_PER_CH; jdx++) {

        g_adc_buffer_ch1[jdx] = (uint16_t)(adc_multimode_buffer[jdx] & 0xFFFF);        // ADC1 채널
        g_adc_buffer_ch2[jdx] = (uint16_t)((adc_multimode_buffer[jdx] >> 16) & 0xFFFF);        // ADC2 채널
	

    }


}