#include "measCurr.h"

float g_fCurrOffset[eADC_CH_MAX];
float g_fCurrMeas[3];

uint32_t adc_multimode_buffer[ADC_BUFFER_LENGTH*ADC_SAMPLE_PER_CH];
uint16_t g_adc_buffer_ch1[ADC_BUFFER_LENGTH*ADC_SAMPLE_PER_CH];
uint16_t g_adc_buffer_ch2[ADC_BUFFER_LENGTH*ADC_SAMPLE_PER_CH];






void OffsetCurrentMeas(){

    HAL_Delay(100);

    for(int idx=0; idx<2000; idx++) {

	for(int i = 0; i < ADC_BUFFER_LENGTH; i++) {

		u32 avgVal = 0;

		switch(i){
			case 0:
				avgVal = Calculate_AverageU32_lower(adc_multimode_buffer, 0, ADC_SAMPLE_PER_CH,  ADC_BUFFER_LENGTH);
				g_fCurrOffset[eADC_CH_CURR_A] = ((float)avgVal) * (3.3f / 4095.0f);
				break;
			case 2:
				avgVal = Calculate_AverageU32_lower(adc_multimode_buffer, 2, ADC_SAMPLE_PER_CH,  ADC_BUFFER_LENGTH);
				g_fCurrOffset[eADC_CH_CURR_B] = ((float)avgVal) * (3.3f / 4095.0f);
				break;
			case 1:
				avgVal = Calculate_AverageU32_lower(adc_multimode_buffer, 1, ADC_SAMPLE_PER_CH,  ADC_BUFFER_LENGTH);
				g_fCurrOffset[eADC_CH_CURR_C] = ((float)avgVal) * (3.3f / 4095.0f);
				break;
			case 3:
				avgVal = Calculate_AverageU32_lower(adc_multimode_buffer, 3, ADC_SAMPLE_PER_CH,  ADC_BUFFER_LENGTH);
				g_fCurrOffset[eADC_CH_BEMF_A] = ((float)avgVal) * (3.3f / 4095.0f);
				break;

			case 5:
				avgVal = Calculate_AverageU32_lower(adc_multimode_buffer, 5, ADC_SAMPLE_PER_CH,  ADC_BUFFER_LENGTH);
				g_fCurrOffset[eADC_CH_VBUS] = ((float)avgVal) * (3.3f / 4095.0f);
				break;

			case 4:
				avgVal = Calculate_AverageU32_lower(adc_multimode_buffer, 4, ADC_SAMPLE_PER_CH,  ADC_BUFFER_LENGTH);
				g_fCurrOffset[eADC_CH_BEMF_B] = ((float)avgVal) * (3.3f / 4095.0f);
				break;
		}

        avgVal = Calculate_AverageU32_upper(adc_multimode_buffer, 0, ADC_SAMPLE_PER_CH,  ADC_BUFFER_LENGTH);
		g_fCurrOffset[eADC_CH_BEMF_C] = ((float)avgVal) * (3.3f / 4095.0f);
    }

		HAL_Delay(1);
	}
}


#if 0



#define ZERO_CROSS_NOT_DETECTED (0)
#define ZERO_CROSS_RISING_EDGE   (1)	
#define ZERO_CROSS_FALLING_EDGE  (2)

float g_fBemfVolt[3];
float g_fBemfVolt_filtered[3];
float g_fAdcVolt[eADC_CH_MAX];

void AdcSampling(void* args){

	static u8 ucZeroCrossA = 0;
	static float fBemfA_prev = 0.0f;
	static u32 uiZcRiseTime = 0;
	static u32 uiZcFallTime = 0;

	u32 uiCurrTime = 0;

	uint32_t avgVal = 0;

	//HAL_GPIO_WritePin(GPO_DBG_3_GPIO_Port, GPO_DBG_3_Pin, GPIO_PIN_SET);

	avgVal = Calculate_AverageU32_lower(adc_multimode_buffer, 3, ADC_SAMPLE_PER_CH,  ADC_BUFFER_LENGTH);
	g_fAdcVolt[eADC_CH_BEMF_A] = ((float)avgVal) * (3.3f / 4095.0f);
	//g_TestSmple[eADC_CH_BEMF_A] = avgVal;

	avgVal = Calculate_AverageU32_lower(adc_multimode_buffer, 4, ADC_SAMPLE_PER_CH,  ADC_BUFFER_LENGTH);
	g_fAdcVolt[eADC_CH_BEMF_B] = ((float)avgVal) * (3.3f / 4095.0f);
	//g_TestSmple[eADC_CH_BEMF_B] = avgVal;

	avgVal = Calculate_AverageU32_upper(adc_multimode_buffer, 4, ADC_SAMPLE_PER_CH,  ADC_BUFFER_LENGTH);
	g_fAdcVolt[eADC_CH_BEMF_C] = ((float)avgVal) * (3.3f / 4095.0f);
	//g_TestSmple[eADC_CH_BEMF_C] = avgVal;

	g_fBemfVolt[0] = g_fAdcVolt[eADC_CH_BEMF_A] - g_fCurrOffset[eADC_CH_BEMF_A];
	g_fBemfVolt[1] = g_fAdcVolt[eADC_CH_BEMF_B] - g_fCurrOffset[eADC_CH_BEMF_B];
	g_fBemfVolt[2] = g_fAdcVolt[eADC_CH_BEMF_C] - g_fCurrOffset[eADC_CH_BEMF_C];
 
	g_fBemfVolt_filtered[0] = LowPassFilter(g_fBemfVolt[0], g_fBemfVolt_filtered[0], 0.2f);
	g_fBemfVolt_filtered[1] = LowPassFilter(g_fBemfVolt[1], g_fBemfVolt_filtered[1], 0.2f);
	g_fBemfVolt_filtered[2] = LowPassFilter(g_fBemfVolt[2], g_fBemfVolt_filtered[2], 0.2f);

	if(g_fBemfVolt_filtered[0] > 0.1f){
		HAL_GPIO_WritePin(GPO_DBG_2_GPIO_Port, GPO_DBG_2_Pin, GPIO_PIN_RESET);
	}
	else {
		HAL_GPIO_WritePin(GPO_DBG_2_GPIO_Port, GPO_DBG_2_Pin, GPIO_PIN_SET);
	}

	if( (fBemfA_prev <= 1.0f) && (g_fBemfVolt_filtered[0] > 1.0f) ){
		// Rising edge detected
		ucZeroCrossA = ZERO_CROSS_RISING_EDGE;
		uiZcRiseTime = __HAL_TIM_GET_COUNTER(&htim6);
		g_uiZeroDelOverflowCnt = 0;
		//HAL_GPIO_WritePin(GPO_DBG_2_GPIO_Port, GPO_DBG_2_Pin, GPIO_PIN_RESET);
		
	}
	else if( (fBemfA_prev >= 1.0f) && (g_fBemfVolt_filtered[0] < 1.0f) ){
		ucZeroCrossA = ZERO_CROSS_FALLING_EDGE;
		uiZcRiseTime = __HAL_TIM_GET_COUNTER(&htim6);
		g_uiZeroDelOverflowCnt = 0;
		//HAL_GPIO_WritePin(GPO_DBG_2_GPIO_Port, GPO_DBG_2_Pin, GPIO_PIN_RESET);
	}

	if(ucZeroCrossA == ZERO_CROSS_RISING_EDGE || ucZeroCrossA == ZERO_CROSS_FALLING_EDGE){
		
		uiCurrTime = __HAL_TIM_GET_COUNTER(&htim6);

		//if(uiCurrTime - uiZcRiseTime > g_uiOneCycleTime / 2){
		if((1000 * g_uiZeroDelOverflowCnt) + uiCurrTime - uiZcRiseTime > g_uiOneCycleTime / 2){
			if(ucZeroCrossA == ZERO_CROSS_RISING_EDGE){
				HAL_GPIO_WritePin(GPO_DBG_1_GPIO_Port, GPO_DBG_1_Pin, GPIO_PIN_RESET);
			}
			else if(ucZeroCrossA == ZERO_CROSS_FALLING_EDGE){
				HAL_GPIO_WritePin(GPO_DBG_1_GPIO_Port, GPO_DBG_1_Pin, GPIO_PIN_SET);
			}
			
			ucZeroCrossA = ZERO_CROSS_NOT_DETECTED;
			
		}
	}


	fBemfA_prev = g_fBemfVolt_filtered[0];


}




uint32_t avgVal = 0;

avgVal = Calculate_AverageU32_lower(adc_multimode_buffer, 0, ADC_SAMPLE_PER_CH,  ADC_BUFFER_LENGTH);
g_fAdcVolt[eADC_CH_CURR_A] = ((float)avgVal) * (3.3f / 4095.0f);

avgVal = Calculate_AverageU32_lower(adc_multimode_buffer, 2, ADC_SAMPLE_PER_CH,  ADC_BUFFER_LENGTH);
g_fAdcVolt[eADC_CH_CURR_B] = ((float)avgVal) * (3.3f / 4095.0f);

avgVal = Calculate_AverageU32_lower(adc_multimode_buffer, 1, ADC_SAMPLE_PER_CH,  ADC_BUFFER_LENGTH);
g_fAdcVolt[eADC_CH_CURR_C] = ((float)avgVal) * (3.3f / 4095.0f);

avgVal = Calculate_AverageU32_lower(adc_multimode_buffer, 3, ADC_SAMPLE_PER_CH,  ADC_BUFFER_LENGTH);
g_fAdcVolt[eADC_CH_VBUS] = ((float)avgVal) * (3.3f / 4095.0f);

g_fCurrMeas[0] = g_fAdcVolt[eADC_CH_CURR_A] - g_fCurrOffset[eADC_CH_CURR_A];
g_fCurrMeas[1] = g_fAdcVolt[eADC_CH_CURR_B] - g_fCurrOffset[eADC_CH_CURR_B];
g_fCurrMeas[2] = g_fAdcVolt[eADC_CH_CURR_C] - g_fCurrOffset[eADC_CH_CURR_C];


#endif