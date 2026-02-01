#include "six_step.h"
#include "main.h"
#include "L6398.h"
#include "boardNuclG431.h"
#include "measSupport.h"
#include "measCurr.h"
#include "tiny_printf.h"
#include "adcHnd.h"
#include "testing.h"

/* ********************************
debug pin added 3ch
PB7
PC8
PC10
******************************** */




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
	ctx->ucThrottleSts = eTHROTTLE_DISABLE;

	ctx->uiMaxPeriodCnt = Pwm1_getPeriod(ctx->pxDrvUnipolar->pxPwmU_highSide);

	g_xTmHallChecker.args = (void*)ctx;
	g_xTmHallChecker.fpTmTask = TmCheckHallState;
	g_xTmHallChecker.uiPeriod = 1000;
	g_xTmHallChecker.ucTimerStatus = HARD_TIMER_STARTED;

	RegisterTimer(&g_xTmContainerMain, &g_xTmHallChecker);

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
	ctx->ucIsThrottleOn = 0;

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



void SixStep_Main(_6StepCtlCtx_t* px6Step, uint8_t ucStopToken){

	ThrottleControl_Test(px6Step);

	DataLoggingManage(&g_xTickCount, px6Step, ucStopToken);
}





static void TmCountingHelper(void* args){
	CountingTick_t* pxTick = (CountingTick_t*)args;

	pxTick->uiLog++;
	pxTick->uiAlwaysLog++;
	pxTick->uiKeeepAlive++;
}
