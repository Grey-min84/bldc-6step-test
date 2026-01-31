#include "six_step.h"
#include "main.h"
#include "L6398.h"
#include "boardNuclG431.h"
#include "measSupport.h"
#include "measCurr.h"
#include "tiny_printf.h"



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
static TimerTask_t g_xTmCounting;

static MotorRpmCtrl_t g_xMotorRpmCtrl;


static void TmCountingHelper(void* args);



void Init_6Step_Unipolar(_6StepCtlCtx_t* ctx, DrvPwm_Unipolar_t* pvDriver){


	PlatformConfig_BaseTimer(&g_xTmContainerMain, &g_xTmCounterMain);
	

	ctx->fpCommTb_unipolar = Apply_L6398_CommutationUnipolar;
	ctx->pxDrvUnipolar = pvDriver;

	PlatformConfig_HallSens_ISR(&ctx->xGpe_HallU, &ctx->xGpe_HallV, &ctx->xGpe_HallW, 
		OnEdge_commutation, (void*)ctx);


	ctx->ucIsIgnited = 0;
	ctx->iSetDuty = 0;
	ctx->ucDir = 0;
	ctx->ucCtlMode = eCTL_MODE_DUTY;

	ctx->uiMaxPeriodCnt = Pwm1_getPeriod(ctx->pxDrvUnipolar->pxPwmU_highSide);

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


	ctx->pxSpdCtl = &g_xMotorRpmCtrl;

	SpeedControl_Init(ctx->pxSpdCtl, ctx->uiMaxPeriodCnt);
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

	// 초기에는 비활성화
	ctx->pxSpdCtl->m_ucCtlState = eSPD_CTL_STATE_IDLE;

	RegisterTimer(&g_xTmContainerMain, &g_xTmSpdControl);



	// Main loop 안에서 타이밍에 맞게 실행해야 되는 기능들 tick 관리
	g_xTmCounting.args = (void*)&g_xTickCount;
	g_xTmCounting.fpTmTask = TmCountingHelper;
	g_xTmCounting.uiPeriod = 1;
	g_xTmCounting.ucTimerStatus = HARD_TIMER_STARTED;

	RegisterTimer(&g_xTmContainerMain, &g_xTmCounting);
}







void TmCheckHallState(void* args){


	uint8_t state = 0;
	uint8_t read = 0;

	_6StepCtlCtx_t* ctx = (_6StepCtlCtx_t*)args;


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



	if(ctx->ucCtlMode == eCTL_MODE_SPEED){
		return;
	}


	if(ctx->iSetDuty < 100){
		ctx->ucIsIgnited = 0;
	}


	if(ctx->ucIsIgnited != 0){
		return;
	}
	
	ctx->fpCommTb_unipolar(ctx->pxDrvUnipolar, state,  ctx->iSetDuty, ctx->ucDir );
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

	px6Step->fpCommTb_unipolar(px6Step->pxDrvUnipolar, state,  px6Step->iSetDuty, px6Step->ucDir );

	MeasHallPeriod(&px6Step->xHallPeriodCalc,  read_u, read_v, read_w);
}






uint8_t g_ucIsLogOn = 0;




void CliControl(cli_args_t *args, void* param){

	_6StepCtlCtx_t* px6Step = (_6StepCtlCtx_t*)param;
	MotorRpmCtrl_t* pxSpdCtrl = px6Step->pxSpdCtl;

	if(args->argc >= 1 ){

		if(args->isStr(0, "rpm") == 1){
			int rpm;
			u8 dir = 0;

			dir = args->getData(1);
			rpm = args->getData(2);

			if(0 < rpm && rpm < 9000){

				pxSpdCtrl->m_iTargtRpm = rpm;
				pxSpdCtrl->m_ucCtlState = eSPD_CTL_STATE_IGNITING;
				pxSpdCtrl->m_ucDir = dir;	
				px6Step->ucCtlMode = eCTL_MODE_SPEED;
			}
			else if(rpm == 0){
				pxSpdCtrl->m_ucCtlState = eSPD_CTL_STATE_IDLE;
				pxSpdCtrl->m_iTargtRpm = 0;
				px6Step->iSetDuty = 0;
				
			}

			printf("Target RPM:%d\r\n", pxSpdCtrl->m_iTargtRpm);

		}
		else if(args->isStr(0, "ignite_pwr") == 1){
			int ignitePwr = 0;
			ignitePwr = args->getData(1);

			if(0 < ignitePwr && ignitePwr < px6Step->uiMaxPeriodCnt * 25 / 100){
				pxSpdCtrl->m_ucIgnitePwr = ignitePwr;
			}

			printf("ignite power:%d\r\n", pxSpdCtrl->m_ucIgnitePwr);
		}
		else if(args->isStr(0, "duty") == 1){
			int duty;

			duty = args->getData(1);

			pxSpdCtrl->m_ucCtlState = eSPD_CTL_STATE_IDLE;

			if(0 <= duty && duty < px6Step->uiMaxPeriodCnt * 75 / 100){
				px6Step->iSetDuty = duty;
				px6Step->ucCtlMode = eCTL_MODE_DUTY;
			}

			printf("Set Duty:%d\r\n", px6Step->iSetDuty);
		}
		else if(args->isStr(0, "log") == 1){

			if(args->isStr(1, "on") == 1){
				g_ucIsLogOn = 1;
			}
			else if(args->isStr(1, "off") == 1){
				g_ucIsLogOn = 0;
			}

			printf("Data log:%s\r\n", (g_ucIsLogOn != 0) ? "on" : "off");
		}
		else {
			printf("\r\n");
			printf("rpm <dir> <rpm>         : set target rpm\r\n");
			printf("duty <duty>             : set duty (0~max 75%%)\r\n");
			printf("ignite_pwr <power>      : set ignite power (0~max25%%)\r\n");	
			printf("log <on/off>            : data log on/off\r\n");
		}
	}

}








void DataLoggingManage(CountingTick_t* pxTick, _6StepCtlCtx_t* px6Step, uint8_t ucStopToken){
	
	uint8_t strBuf[64] = {0,};

	MotorRpmCtrl_t* pxSpdCtrl = px6Step->pxSpdCtl;
	HallSpdMeas_t* pxHallSpdMeas = &px6Step->xHallSpdMeas;

	static u32 plot_time = 0;
	
	if(g_ucIsLogOn != 0){

		if(ucStopToken == 'z'){
			g_ucIsLogOn = 0;
			return;
		}

		if(pxTick->uiLog >= 50){
			pxTick->uiLog = 0;

			printf("pidOut:%.1f, pidErr:%.1f, rpm_filt:%.1f, rpm_obs:%.1f, dt:%d\r\n",
				pxSpdCtrl->fPidOut,
				pxSpdCtrl->fPid_ErrInput,
				pxHallSpdMeas->g_fRpm_filt,
				pxHallSpdMeas->g_fRpm_obs,
				pxHallSpdMeas->avg_dt_us);

		}
		
	}

	if(pxTick->uiAlwaysLog >= 20){
		pxTick->uiAlwaysLog = 0;

		plot_time = plot_time + 20;

		// always log
		sprintf((char*)strBuf, "%d\t%d\t%d\t%.1f\t%d\n", 
					plot_time,
					pxSpdCtrl->m_iTargtRpm,
					pxSpdCtrl->m_iCurrRpm,
					pxHallSpdMeas->g_fRpm_filt,
					pxHallSpdMeas->avg_dt_us
		);

		HAL_UART_Transmit(&huart3, strBuf, strlen((char*)strBuf), HAL_MAX_DELAY);
	}
}




static void TmCountingHelper(void* args){
	CountingTick_t* pxTick = (CountingTick_t*)args;

	pxTick->uiLog++;
	pxTick->uiAlwaysLog++;
	pxTick->uiKeeepAlive++;
}
