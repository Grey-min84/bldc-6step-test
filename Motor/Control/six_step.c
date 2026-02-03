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




uint32_t g_auiHallExceptSts[7] = {0,};


void OnEdge_commutation(void* args)
{
    uint8_t state = 0;
    uint8_t IsHallStsValid = 0;
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



	IsHallStsValid = Check_Valid_HallCode(state, px6Step->ucDir);

	if(IsHallStsValid != 0){
		g_auiHallExceptSts[IsHallStsValid]++;
	}


	px6Step->ucCurrHallSts = state;
	px6Step->ucIsIgnited = 1;

	px6Step->fpCommTb_unipolar(px6Step->pxDrvUnipolar, state,  px6Step->iSetDuty, px6Step->ucDir );

	MeasHallPeriod(&px6Step->xHallPeriodCalc,  read_u, read_v, read_w);
}



void SixStep_Main(_6StepCtlCtx_t* px6Step, CountingTick_t* pxTick, uint8_t ucStopToken){

	ThrottleControl_Test(px6Step);

	DataLoggingManage(pxTick, px6Step, ucStopToken);

	if(pxTick->uiAdcFilter >= 1){
		pxTick->uiAdcFilter = 0;
	 	AdcMeas();
	}
}





// dir = 0
static const uint8_t next_cw[8] = {
    0, //000 invalid
    3, //001 ->011
    6, //010 ->110
    2, //011 ->010
    5, //100 ->101
    1, //101 ->001? (예시는 상황에 맞게 수정)
    4, //110 ->100
    0  //111 invalid
};

// dir = 1
static const uint8_t next_ccw[8] = {
    0, //000 invalid
    5, //001 ->101
    3, //010 ->011
    1, //011 ->001
    6, //100 ->110
    4, //101 ->100? (예시는 상황에 맞게 수정)
    2, //110 ->010
    0  //111 invalid
};



uint8_t Check_Valid_HallCode(uint8_t state, uint8_t dir){

	static volatile uint8_t g_hall_prev = 0;
	static volatile uint32_t g_last_tick = 0;


	uint32_t t = TIM7->CNT;
    uint32_t dt = t - g_last_tick;

    if (dt < 5) { // 10us 이내 재발생은 노이즈로 간주 (초기값 예시)
        return 1;
    }

    g_last_tick = t;


	if(state == 0 || state == 7){
		return 5; // 000 또는 111은 불가능한 홀 상태
	}
	/* ****************************************************************
	1-bit만 바뀌었는지(그레이 코드) 체크: prev XOR now의 비트 수가 1인지
	정상적인 6스텝 BLDC 홀 전이에서는 “한 번에 오직 1비트만 바뀐다”
	따라서 이전 상태와 현재 상태의 XOR 결과는 반드시 다음과 같다.
	001 (1비트 변화)
	010 (1비트 변화)
	100 (1비트 변화)
	**************************************************************** */
	uint8_t diff = (g_hall_prev ^ state);
    if (!(diff == 0x01 || diff == 0x02 || diff == 0x04)) {
		
		g_hall_prev = state;
        return 2; // 두 비트 이상 변하면 노이즈 가능성 큼
    }


	if(dir == 0){
		// 방향별 허용 전이 체크(여기서는 CCW 예시)
		if (next_cw[g_hall_prev] != state) {
			
			g_hall_prev = state;
			return 3; // 허용 전이 아니면 무시
		}
	}
	else {
		 // 방향별 허용 전이 체크(여기서는 CW 예시)
		if (next_ccw[g_hall_prev] != state) {
			
			g_hall_prev = state;
			return 4; // 허용 전이 아니면 무시
		}
	}

	



	g_hall_prev = state;

	return 0;
}



static void TmCountingHelper(void* args){
	CountingTick_t* pxTick = (CountingTick_t*)args;

	pxTick->uiLog++;
	pxTick->uiAlwaysLog++;
	pxTick->uiKeeepAlive++;
	pxTick->uiAdcFilter++;
}
