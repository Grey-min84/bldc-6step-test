#ifndef __BLDC_SIX_STEP_CTL_H__
#define __BLDC_SIX_STEP_CTL_H__


#include "IF_Hal.h"
#include "motor_term_def.h"
#include "hall.h"
#include "cli.h"

enum eADC_CH_IDX{
	eADC_CH_CURR_A = 0,
	eADC_CH_CURR_B,
	eADC_CH_CURR_C,
	eADC_CH_VBUS,
	eADC_CH_BEMF_A,
	eADC_CH_BEMF_B,
	eADC_CH_BEMF_C,
	eADC_CH_MAX
};



enum e6STEP_CTL{
	e6STEP_CTL_UNIPOLAR = 0,
	e6STEP_CTL_BIPOLAR,
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

typedef void (*ApplyUniPolarCb)(void* pvDriver, uint8_t state, float pwmVal);
typedef void (*ApplyBiPolarCb)(void* pvDriver, uint8_t state, float pwmVal);

typedef struct _6StepCtlCtx_tag{
	ApplyBiPolarCb fpCommTb_bipolar;
	ApplyUniPolarCb fpCommTb_unipolar;
	void* pvDriver;
	
	float fSetDuty;

	uint32_t uiSetRpm;
	uint8_t ucCurrSts;
	uint8_t ucIsIgnited;

	IGpio_t xGpe_HallU ;
	IGpio_t xGpe_HallV ;
	IGpio_t xGpe_HallW ;

}_6StepCtlCtx_t;


//void BldcPwrOut_t PhaseFind(uint8_t step);



void OnEdge_Commutation_withHallSens(void* args);




void Init_6Step_Unipolar(_6StepCtlCtx_t* ctx, void* pvDriver);
void Apply_L6398_CommutationUnipolar(void* pxDriver, uint8_t state, float pwmVal);

//void HallEdgeDetected(void* args);

void TmCheckHallState(void* ctx);

void CliControl(cli_args_t *args, void* param);






#endif
