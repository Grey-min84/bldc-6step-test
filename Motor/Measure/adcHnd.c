#include "adcHnd.h"
#include "measSupport.h"
#include "digitFilter.h"

float g_fCurrOffset[eADC_CH_MAX];
float g_fCurrMeas[3];


uint16_t g_usThrottleAdc = 0;

uint32_t adc_multimode_buffer[ADC_BUFFER_LENGTH*ADC_SAMPLE_PER_CH];
uint16_t g_adc_buffer_ch1[ADC_BUFFER_LENGTH*ADC_SAMPLE_PER_CH];
uint16_t g_adc_buffer_ch2[ADC_BUFFER_LENGTH*ADC_SAMPLE_PER_CH];

static AVG_FILTER_VAR g_xAdcAvgFilter[eADC_IDX_MAX];
static MedianFilter_t g_xAdcMedianFilter[eADC_IDX_MAX];

static int32_t g_iAdcFilteredVal[eADC_IDX_MAX];



static u16 DismissMinorBits(u16 rawVal, u8 bitsToDismiss){

    return (rawVal >> bitsToDismiss) << bitsToDismiss;
}



void Init_6step_adcSampling(_6StepCtlCtx_t* ctx){
	HAL_ADCEx_MultiModeStart_DMA(&hadc1, adc_multimode_buffer, ADC_BUFFER_LENGTH*ADC_SAMPLE_PER_CH);


    for(int idx = 0; idx < eADC_IDX_MAX; idx++){

        InitAverageFilter(&g_xAdcAvgFilter[idx], 200);
        InitMedianFilterVar(&g_xAdcMedianFilter[idx], 7);
    }
}





void AdcOffsetMeas(){

    HAL_Delay(100);

    for(int idx=0; idx<200; idx++) {

	for(int jdx = 0; jdx < ADC_BUFFER_LENGTH; jdx++) {

		u32 avgVal = 0;

        avgVal = Calculate_AverageU32_lower(adc_multimode_buffer, jdx, ADC_SAMPLE_PER_CH,  ADC_BUFFER_LENGTH);
        g_adc_buffer_ch1[jdx] = DismissMinorBits((uint16_t)(avgVal & 0xFFFF), 2);        // ADC1 채널

        avgVal = Calculate_AverageU32_upper(adc_multimode_buffer, jdx, ADC_SAMPLE_PER_CH,  ADC_BUFFER_LENGTH);
        g_adc_buffer_ch2[jdx] = DismissMinorBits((uint16_t)(avgVal & 0xFFFF), 2);        // ADC2 채널
    }

		HAL_Delay(1);
	}
}


void AdcMeas(){

    for(int jdx = 0; jdx < ADC_BUFFER_LENGTH*ADC_SAMPLE_PER_CH; jdx++) {

        g_adc_buffer_ch1[jdx] = (uint16_t)(adc_multimode_buffer[jdx] & 0xFFFF);        // ADC1 채널
        g_adc_buffer_ch2[jdx] = (uint16_t)((adc_multimode_buffer[jdx] >> 16) & 0xFFFF);        // ADC2 채널
	

    }

    for(int idx = 0; idx < eADC_IDX_MAX; idx++){

        int32_t rawAdcVal = 0;
        int32_t filteredAdcVal = 0;

       rawAdcVal = GetRawAdcValue((Adc_Idx_e)idx);

        filteredAdcVal = AverageFilter(&g_xAdcAvgFilter[idx], rawAdcVal);
        g_iAdcFilteredVal[idx] = MedianFilter(&g_xAdcMedianFilter[idx], filteredAdcVal);

    }

}

int32_t GetFilteredAdcValue(Adc_Idx_e eIdx){

    int32_t adcVal = 0;

    adcVal = g_iAdcFilteredVal[eIdx];

   return adcVal;
}


int32_t GetRawAdcValue(Adc_Idx_e eIdx){

    int32_t adcVal = 0;

    ADC_CH1_IDX_e eCh1Idx;
    ADC_CH2_IDX_e eCh2Idx;

   switch(eIdx){
        case eADC_IDX_CURR_A:
            eCh1Idx = eADC_CH_CURR_A;
            adcVal = g_adc_buffer_ch1[eCh1Idx];
            break;
        case eADC_IDX_CURR_B:
            eCh1Idx = eADC_CH_CURR_B;
            adcVal = g_adc_buffer_ch1[eCh1Idx];
            break;
        case eADC_IDX_CURR_C:
            eCh1Idx = eADC_CH_CURR_C;
            adcVal = g_adc_buffer_ch1[eCh1Idx];
            break;
        case eADC_IDX_THROTTLE:
            eCh2Idx = eADC_CH_THROTTLE;
            adcVal = g_adc_buffer_ch2[eCh2Idx];
            break;
        case eADC_IDX_BEMF_A:
            eCh1Idx = eADC_CH_BEMF_A;
            adcVal = g_adc_buffer_ch1[eCh1Idx];
            break;
        case eADC_IDX_BEMF_B:
            eCh1Idx = eADC_CH_BEMF_B;
            adcVal = g_adc_buffer_ch1[eCh1Idx];
            break;
        case eADC_IDX_BEMF_C:
            eCh2Idx = eADC_CH_BEMF_C;
            adcVal = g_adc_buffer_ch2[eCh2Idx];
            break;
        case eADC_IDX_TEMP:
            eCh1Idx = eADC_CH_TEMP;
            adcVal = g_adc_buffer_ch1[eCh1Idx];
            break;
        default:
            adcVal = 0;
            break;
   }

   return adcVal;
}
