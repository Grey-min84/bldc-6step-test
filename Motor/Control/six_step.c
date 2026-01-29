#include "six_step.h"
#include "main.h"
#include "L6398.h"
#include "boardNuclG431.h"
#include "measSupport.h"

// IGpio_t g_xGpe_HallU ;
// IGpio_t g_xGpe_HallV ;
// IGpio_t g_xGpe_HallW ;


/* ********************************
debug pin added 3ch
PB7
PC8
PC10
******************************** */


#define ADC_SAMPLE_PER_CH   (4)
#define ADC_BUFFER_LENGTH    6

TimerContainer_t g_xTmContainerMain;
TimerContainer_t g_xTmContainerPwm;
TimerCounter_t g_xTmCounterMain;


float g_fBemfVolt[3];
float g_fBemfVolt_filtered[3];
float g_fAdcVolt[eADC_CH_MAX];
float g_fCurrOffset[eADC_CH_MAX];
float g_fCurrMeas[3];


uint32_t g_uiOneCycleTime = 0;
uint32_t g_uiOneCycleTimeFilter = 0;
uint32_t g_uiOneCycleOverFlowCnt = 0;
uint32_t g_uiZeroDelOverflowCnt = 0;

uint32_t adc_multimode_buffer[ADC_BUFFER_LENGTH*ADC_SAMPLE_PER_CH];
uint16_t g_adc_buffer_ch1[ADC_BUFFER_LENGTH*ADC_SAMPLE_PER_CH];
uint16_t g_adc_buffer_ch2[ADC_BUFFER_LENGTH*ADC_SAMPLE_PER_CH];

static float LowPassFilter(float fInput, float fPrevOutput, float fAlpha);

void TmCheckCounter(void* args);


void AdcSampling(void* args);

void Init_6Step_Unipolar(_6StepCtlCtx_t* ctx, void* pvDriver){

	static TimerTask_t xTmTask1;
	static TimerTask_t xTmTask2;


	ctx->fpCommTb_unipolar = Apply_L6398_CommutationUnipolar;
	ctx->fSetDuty = 0;
	ctx->pvDriver = pvDriver;

	PlatformConfig_HallSens_ISR(&ctx->xGpe_HallU, &ctx->xGpe_HallV, &ctx->xGpe_HallW, 
		OnEdge_Commutation_withHallSens, (void*)ctx);

	//Pwm1_AddCallbackPeriodDone(ctx->, fpPeriodCb fpCb, void* _args);

	ctx->ucIsIgnited = 0;



	HAL_ADCEx_MultiModeStart_DMA(&hadc1, adc_multimode_buffer, ADC_BUFFER_LENGTH*ADC_SAMPLE_PER_CH);

	HAL_Delay(100);
	printf("Start measure current offset~\r\n");
	// Start Measure Current Offset
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
#if 0
		for(int i = 0; i < ADC_BUFFER_LENGTH; i++) {

			g_adc_buffer_ch1[i] = (uint16_t)(adc_multimode_buffer[i] & 0xFFFF);        // ADC1 ?��?��?��
			g_adc_buffer_ch2[i] = (uint16_t)((adc_multimode_buffer[i] >> 16) & 0xFFFF); // ADC2 ?��?��?�� (마�?막만 ?��?��)
		}

		g_fCurrOffset[eADC_CH_CURR_A] = ((float)g_adc_buffer_ch1[0]) * (3.3f / 4095.0f);
		g_fCurrOffset[eADC_CH_CURR_B] = ((float)g_adc_buffer_ch1[2]) * (3.3f / 4095.0f);
		g_fCurrOffset[eADC_CH_CURR_C] = ((float)g_adc_buffer_ch1[1]) * (3.3f / 4095.0f);
		g_fCurrOffset[eADC_CH_BEMF_A] = ((float)g_adc_buffer_ch1[3]) * (3.3f / 4095.0f);
		g_fCurrOffset[eADC_CH_VBUS]   = ((float)g_adc_buffer_ch1[5]) * (3.3f / 4095.0f);
		g_fCurrOffset[eADC_CH_BEMF_B] = ((float)g_adc_buffer_ch1[4]) * (3.3f / 4095.0f);
		g_fCurrOffset[eADC_CH_BEMF_C] = ((float)g_adc_buffer_ch2[0]) * (3.3f / 4095.0f);
#endif
		HAL_Delay(1);
	}

	printf("Setting Done~\r\n");






	PlatformConfig_BaseTimer(&g_xTmContainerMain, &g_xTmCounterMain);

	xTmTask1.args = (void*)ctx;
	xTmTask1.fpTmTask = TmCheckHallState;
	xTmTask1.uiPeriod = 1000;
	xTmTask1.ucTimerStatus = HARD_TIMER_STARTED;

	RegisterTimer(&g_xTmContainerMain, &xTmTask1);


	xTmTask2.args = (void*)ctx;
	xTmTask2.fpTmTask = TmCheckCounter;
	xTmTask2.uiPeriod = 1;
	xTmTask2.ucTimerStatus = HARD_TIMER_STARTED;

	RegisterTimer(&g_xTmContainerMain, &xTmTask2);
}


void TmCheckCounter(void* args){
	g_uiOneCycleOverFlowCnt++;
	g_uiZeroDelOverflowCnt++;
}



void TmCheckHallState(void* args){

	
	//g_uiOneCycleTime

	uint8_t state = 0;
	uint8_t read = 0;

	_6StepCtlCtx_t* ctx = (_6StepCtlCtx_t*)args;

	if(ctx->fSetDuty < 1.0f){
		ctx->ucIsIgnited = 0;
	}


	if(ctx->ucIsIgnited != 0){
		return;
	}



	read = ReadGpio(&ctx->xGpe_HallU);
	
    if (read != 0){
    	state |= 0x01;
    }

	read = ReadGpio(&ctx->xGpe_HallV);

	if (read != 0){
		state |= 0x02;
	}


	read = ReadGpio(&ctx->xGpe_HallW);

	if (read != 0){
		state |= 0x04;
	}

	ctx->ucCurrSts = state;
	ctx->fpCommTb_unipolar(ctx->pvDriver, state,  ctx->fSetDuty );

	
	
}



void OnEdge_Commutation_withHallSens(void* args)
{
    uint8_t state = 0;
	uint8_t read_u, read_v, read_w = 0;
	_6StepCtlCtx_t* px6Step = (_6StepCtlCtx_t*)args;

	static u32 uiTimePre = 0;
	u32 uiTimeCurr = 0;

	read_u = ReadGpio(&px6Step->xGpe_HallU);
	
    if (read_u != 0){
    	state |= 0x01;
    }

	read_v = ReadGpio(&px6Step->xGpe_HallV);

	if (read_v != 0){
		state |= 0x02;
	}


	read_w = ReadGpio(&px6Step->xGpe_HallW);

	if (read_w != 0){
		state |= 0x04;
	}

	px6Step->ucCurrSts = state;
	px6Step->ucIsIgnited = 1;

	px6Step->fpCommTb_unipolar(px6Step->pvDriver, state,  px6Step->fSetDuty );

	MeasHallPeriod(read_u, read_v, read_w);
		

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

	 

}


#define ZERO_CROSS_NOT_DETECTED (0)
#define ZERO_CROSS_RISING_EDGE   (1)	
#define ZERO_CROSS_FALLING_EDGE  (2)

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











void CliControl(cli_args_t *args, void* param){

	_6StepCtlCtx_t* px6Step = (_6StepCtlCtx_t*)param;


	if(args->argc >= 1 ){

		if(args->isStr(0, "rpm") == 1){
			int rpm;
			rpm = args->getData(1);

			if(0 < rpm && rpm < 9000){
				px6Step->uiSetRpm = rpm;
			}

		}
		else if(args->isStr(0, "duty") == 1){
			float duty;

			duty = args->getFloat(1);

			if(-0.01 < duty && duty < 90){
				px6Step->fSetDuty = duty;
			}
		}
	}

}





#if 0
uint16_t g_usMaxPowerTm = 0;
uint32_t g_uiOverFlowCnt = 0;
uint32_t g_uiElectricPeriod = 0;
float g_fElectricRPM = 0.0f;
uint8_t g_ucMeasRpmHall = 5;
void CalcPeriod_OverflowCnt(void* args);


void InitBldcMeasRPM(TimerContainer_t* pxTmContainer){
	static TimerTask_t xTmTask1;

	xTmTask1 = CreateTimerTask(CalcPeriod_OverflowCnt, (void*)0, 1, HARD_TIMER_STARTED);
	RegisterTimer(pxTmContainer, &xTmTask1);
}

void CalcPeriod_OverflowCnt(void* args){
	g_uiOverFlowCnt++;

	if(g_usMaxPowerTm != 0){
		g_usMaxPowerTm--;
	}
}

// Unit 1usec
uint32_t GetRotatePerPeriod(TimerCounter_t* pxCntTimer){

  uint32_t l_uiCurTimeCnt;
  uint32_t uiPeriod = 0;


//   l_uiCurTimeCnt = __HAL_TIM_GET_COUNTER(&htim6);
//   __HAL_TIM_SET_COUNTER(&htim6, 0);

  l_uiCurTimeCnt = GetTimerCount(pxCntTimer);
  ResetTimerCount(pxCntTimer);

  uiPeriod = l_uiCurTimeCnt + (g_uiOverFlowCnt * 1000);


  g_uiOverFlowCnt = 0;


  return uiPeriod;
}


float GetPRM_fromPeriod(float uiPeriod){

  float fRotPerSec = 0.0f;
  float fRPM = 0.0f;

  if(uiPeriod > 0 ){

      fRotPerSec = 1000.0f / (0.001f * uiPeriod);

      fRPM = fRotPerSec * 60.0f;
  }
  else {
      fRPM = 0.0f;
  }


  return fRPM;
}





void HallEdgeDetected(void* args){
	uint8_t ucHall_u = 0, ucHall_v = 0, ucHall_w = 0;
	uint8_t ucHallCombi = 0;

	// ucHall_u = ReadGpio(g_xBldcCtlCtx.xHallPin.pxU);
	// ucHall_v = ReadGpio(g_xBldcCtlCtx.xHallPin.pxV);
	// ucHall_w = ReadGpio(g_xBldcCtlCtx.xHallPin.pxW);

	ucHallCombi = (ucHall_u) + (ucHall_v << 1) + (ucHall_w << 2);

	if(ucHallCombi == 5){
		// g_uiElectricPeriod = GetRotatePerPeriod(g_xBldcCtlCtx.pxTmCounter);
		// g_fElectricRPM = GetPRM_fromPeriod(g_uiElectricPeriod);
	}
}
#endif


static float LowPassFilter(float fInput, float fPrevOutput, float fAlpha){
	return (fAlpha * fInput) + ((1.0f - fAlpha) * fPrevOutput);
}

