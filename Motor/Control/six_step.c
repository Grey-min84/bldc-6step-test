#include "six_step.h"
#include "main.h"
#include "L6398.h"
#include "boardNuclG431.h"
#include "measSupport.h"
#include "measCurr.h"

// IGpio_t g_xGpe_HallU ;
// IGpio_t g_xGpe_HallV ;
// IGpio_t g_xGpe_HallW ;


/* ********************************
debug pin added 3ch
PB7
PC8
PC10
******************************** */
extern uint32_t adc_multimode_buffer[ADC_BUFFER_LENGTH*ADC_SAMPLE_PER_CH];
extern uint16_t g_adc_buffer_ch1[ADC_BUFFER_LENGTH*ADC_SAMPLE_PER_CH];
extern uint16_t g_adc_buffer_ch2[ADC_BUFFER_LENGTH*ADC_SAMPLE_PER_CH];



TimerContainer_t g_xTmContainerMain;
TimerCounter_t g_xTmCounterMain;


static TimerTask_t g_xTmSpdMeasure;
static TimerTask_t g_xTmSpdControl;
static TimerTask_t g_xTmHallChecker;



void Init_6Step_Unipolar(_6StepCtlCtx_t* ctx, DrvPwm_Unipolar_t* pvDriver){


	PlatformConfig_BaseTimer(&g_xTmContainerMain, &g_xTmCounterMain);
	

	ctx->fpCommTb_unipolar = Apply_L6398_CommutationUnipolar;
	
	ctx->pxDrvUnipolar = pvDriver;

	PlatformConfig_HallSens_ISR(&ctx->xGpe_HallU, &ctx->xGpe_HallV, &ctx->xGpe_HallW, 
		OnEdge_commutation, (void*)ctx);


	ctx->ucIsIgnited = 0;
	ctx->iSetDuty = 0;
	ctx->ucCtlMode = eCTL_MODE_DUTY;



	g_xTmHallChecker.args = (void*)ctx;
	g_xTmHallChecker.fpTmTask = TmCheckHallState;
	g_xTmHallChecker.uiPeriod = 1000;
	g_xTmHallChecker.ucTimerStatus = HARD_TIMER_STARTED;

	RegisterTimer(&g_xTmContainerMain, &g_xTmHallChecker);

}




void Init_6step_adcSampling(_6StepCtlCtx_t* ctx){
	HAL_ADCEx_MultiModeStart_DMA(&hadc1, adc_multimode_buffer, ADC_BUFFER_LENGTH*ADC_SAMPLE_PER_CH);

}


void Init_6step_speedCtrl(_6StepCtlCtx_t* ctx){

	SpeedControl_Init(ctx->pxSpdCtl);
	SpdCalc_init(&ctx->xHallSpdMeas);

	// Init speed measurement
	g_xTmSpdMeasure.args = (void*)ctx;
	g_xTmSpdMeasure.fpTmTask = SpeedCalculation;
	g_xTmSpdMeasure.uiPeriod = 1;
	g_xTmSpdMeasure.ucTimerStatus = HARD_TIMER_STARTED;

	RegisterTimer(&g_xTmContainerMain, &g_xTmSpdMeasure);


	// Init speed control
	g_xTmSpdControl.args = (void*)ctx;
	g_xTmSpdControl.fpTmTask = SpeedControl_loop;
	g_xTmSpdControl.uiPeriod = 1;
	g_xTmSpdControl.ucTimerStatus = HARD_TIMER_STARTED;

	RegisterTimer(&g_xTmContainerMain, &g_xTmSpdControl);
}







void TmCheckHallState(void* args){


	uint8_t state = 0;
	uint8_t read = 0;

	_6StepCtlCtx_t* ctx = (_6StepCtlCtx_t*)args;

	if(ctx->iSetDuty < 1.0f){
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

	ctx->ucCurrHallSts = state;
	ctx->fpCommTb_unipolar(ctx->pxDrvUnipolar, state,  ctx->iSetDuty );
}



void OnEdge_commutation(void* args)
{
    uint8_t state = 0;
	uint8_t read_u, read_v, read_w = 0;
	_6StepCtlCtx_t* px6Step = (_6StepCtlCtx_t*)args;

//	static u32 uiTimePre = 0;
//	u32 uiTimeCurr = 0;

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

	px6Step->ucCurrHallSts = state;
	px6Step->ucIsIgnited = 1;

	px6Step->fpCommTb_unipolar(px6Step->pxDrvUnipolar, state,  px6Step->iSetDuty );

	MeasHallPeriod(&px6Step->xHallPeriodCalc,  read_u, read_v, read_w);
}











void CliControl(cli_args_t *args, void* param){

	_6StepCtlCtx_t* px6Step = (_6StepCtlCtx_t*)param;
	MotorRpmCtrl_t* pxSpdCtrl = px6Step->pxSpdCtl;

	if(args->argc >= 1 ){

		if(args->isStr(0, "rpm") == 1){
			int rpm;
			rpm = args->getData(1);

			if(0 < rpm && rpm < 9000){
				pxSpdCtrl->m_iTargtRpm = rpm;
				px6Step->ucCtlMode = eCTL_MODE_SPEED;
			}

		}
		else if(args->isStr(0, "duty") == 1){
			int duty;

			duty = args->getData(1);

			if(0 <= duty && duty < 4000){
				px6Step->iSetDuty = duty;
				px6Step->ucCtlMode = eCTL_MODE_DUTY;
			}
		}
	}

}





