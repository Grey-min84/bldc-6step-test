#ifndef __BLDC_SIX_STEP_CTL_H__
#define __BLDC_SIX_STEP_CTL_H__

#include <string.h>

#include "IF_Hal.h"
#include "motor_term_def.h"
#include "cli.h"
#include "measSpd.h"
#include "spd_ctrl.h"

typedef enum eADC_IDX{
    eADC_IDX_CURR_A = 0,
    eADC_IDX_CURR_B,
    eADC_IDX_CURR_C,
    eADC_IDX_THROTTLE,
    eADC_IDX_BEMF_A,
    eADC_IDX_BEMF_B,
    eADC_IDX_BEMF_C,
    eADC_IDX_TEMP,
    eADC_IDX_RESV,
    eADC_IDX_MAX,
}Adc_Idx_e;

enum e6STEP_CTL{
	e6STEP_CTL_UNIPOLAR = 0,
	e6STEP_CTL_BIPOLAR,
};

enum eCTL_MODE{
	eCTL_MODE_OPENLOOP = 0,
	eCTL_MODE_DUTY,
	eCTL_MODE_SPEED,
	eCTL_MODE_TORQUE,
};



enum cTHROTTLE_STS{
	eTHROTTLE_DISABLE = 0,
	eTHROTTLE_OFF ,
	eTHROTTLE_WAIT,
	eTHROTTLE_RDY,
	eTHROTTLE_ON,
	eTHROTTLE_UNDER_THR,
};





enum SIX_STEP_POS_IDX{
	eSECTION_EXCEP1 = 0,
	eSECTION_1 = 1,
	eSECTION_2 = 2,
	eSECTION_3 = 3,
	eSECTION_4 = 4,
	eSECTION_5 = 5,
	eSECTION_6 = 6,
	eSECTION_EXCEP2 = 7,
	eSECTION_MAX = 8
};

typedef void (*ApplyUniPolarCb)(DrvPwm_Unipolar_t* pvDriver, uint8_t state, u32 pwmVal, u8 dir);
typedef void (*ApplyBiPolarCb)(DrvPwm_Bipolar_t* pvDriver, uint8_t state, u32 pwmVal, u8 dir);

typedef struct _6StepCtlCtx_tag{
	ApplyBiPolarCb fpCommTb_bipolar;
	DrvPwm_Bipolar_t* pxDrvBipolar;

	ApplyUniPolarCb fpCommTb_unipolar;
	DrvPwm_Unipolar_t* pxDrvUnipolar;

	u8 ucCtlMode;
	u8 ucIsThrottleOn;
	u8 ucThrottleSts;
	
	u32 iSetDuty;
	u8 ucDir;
	u8 ucBrkOn;
	u8 ucCurrHallSts;

	u32 uiMaxPeriodCnt;

	u8 ucIsIgnited;

	IGpio_t xGpe_HallU ;
	IGpio_t xGpe_HallV ;
	IGpio_t xGpe_HallW ;

	HallSpdMeas_t xHallSpdMeas;

	HallPeriodHnd_t xHallPeriodCalc;

	MotorRpmCtrl_t* pxSpdCtl;

}_6StepCtlCtx_t;


void SixStep_Main(_6StepCtlCtx_t* px6Step, CountingTick_t* pxTick, uint8_t ucStopToken);


void OnEdge_commutation(void* args);




void Init_6Step_Unipolar(_6StepCtlCtx_t* ctx, DrvPwm_Unipolar_t* pvDriver);
void Init_6step_adcSampling(_6StepCtlCtx_t* ctx);
void Init_6step_speedCtrl(_6StepCtlCtx_t* ctx);




//void HallEdgeDetected(void* args);
uint8_t Check_Valid_HallCode(uint8_t state);
void TmCheckHallState(void* ctx);

void CliControl(cli_args_t *args, void* param);


void DataLoggingManage(CountingTick_t* pxTick, _6StepCtlCtx_t* px6Step, uint8_t ucStopToken);



#endif
