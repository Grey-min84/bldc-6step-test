#include "spd_ctrl.h"
#include "six_step.h"

#define DEF_RPM_CTL_KP			(0.01f)
#define DEF_RPM_CTL_KI			(0.0002f)
#define DEF_RPM_CTL_KD			(0.0f)
#define DEF_RPM_CTL_KA			(1.0f)
#define DEF_RPM_CTL_MAX_ERR		(100)
#define DEF_RPM_CTL_MAX_OUTPUT	(2599)
#define DEF_RPM_CTL_MIN_OUTPUT	(0)
#define DEF_RPM_CTL_SLEW_RATE	(5)

void SpeedControl_Init(MotorRpmCtrl_t* pxSpdCtrl, u32 uiMaxPeriodCnt){

	u32 maxOutput = uiMaxPeriodCnt * 75 / 100;	// 75% of max period count

    InitPIDCtrlVarFloat(&pxSpdCtrl->m_xPid);
	SetPIDGainFloat(&pxSpdCtrl->m_xPid, DEF_RPM_CTL_KP, DEF_RPM_CTL_KI, DEF_RPM_CTL_KD, DEF_RPM_CTL_KA);
	SetPIDConfigVarFloat(&pxSpdCtrl->m_xPid, DEF_RPM_CTL_MAX_ERR, maxOutput, DEF_RPM_CTL_MIN_OUTPUT);
	pxSpdCtrl->m_xPid.m_fSlewRate = DEF_RPM_CTL_SLEW_RATE;

    pxSpdCtrl->m_ucIgnitePwr = 400;
	pxSpdCtrl->ucMinPwr = 200;
}


void SpeedControl_loop(void* args){

	int iPidOut = 0;
	float fPid_ErrInput = 0.0f;

    _6StepCtlCtx_t* px6stepCtx = (_6StepCtlCtx_t*)args;
    MotorRpmCtrl_t* pxSpdCtrl = px6stepCtx->pxSpdCtl;


	switch(pxSpdCtrl->m_ucCtlState){

		case eSPD_CTL_STATE_DISABLED:
		case eSPD_CTL_STATE_IDLE:
			pxSpdCtrl->m_ucIgnited = 0;
			pxSpdCtrl->m_ucDirChngFlag = 0;
			pxSpdCtrl->m_ucDirChngDelayTm = 0;
			break;


		case eSPD_CTL_STATE_IGNITING:
			px6stepCtx->iSetDuty 	= pxSpdCtrl->m_ucIgnitePwr;
			px6stepCtx->ucDir 		= pxSpdCtrl->m_ucDir;

			px6stepCtx->fpCommTb_unipolar(px6stepCtx->pxDrvUnipolar, px6stepCtx->ucCurrHallSts,  px6stepCtx->iSetDuty, px6stepCtx->ucDir );

			if(pxSpdCtrl->m_ucRpmMeasCnt > HALL_DT_BUF*4){
                    
				pxSpdCtrl->m_ucRpmMeasCnt = HALL_DT_BUF*4;
				pxSpdCtrl->m_xPid.m_ErrorSum = 0.0f;
				pxSpdCtrl->m_ucCtlState = eSPD_CTL_STATE_RUNNING;
			}

			pxSpdCtrl->m_ucDir_pre = pxSpdCtrl->m_ucDir;
			break;

		case eSPD_CTL_STATE_RUNNING:
			// 정상 구동 상태
			fPid_ErrInput = (float)(pxSpdCtrl->m_iTargtRpm - pxSpdCtrl->m_iCurrRpm);

			iPidOut = (int)CalcPidFloat_Incremental(&pxSpdCtrl->m_xPid, fPid_ErrInput);

			if(iPidOut < pxSpdCtrl->ucMinPwr){
				iPidOut = pxSpdCtrl->ucMinPwr;
			}
			
            px6stepCtx->iSetDuty = (u32)iPidOut;

			if(pxSpdCtrl->m_ucDir != pxSpdCtrl->m_ucDir_pre){

				pxSpdCtrl->m_ucCtlState = eSPD_CTL_STATE_DIR_CHANGING;
				pxSpdCtrl->m_ucDirChngDelayTm = 0;
			}

			pxSpdCtrl->m_ucDir_pre = pxSpdCtrl->m_ucDir;
			break;

		case eSPD_CTL_STATE_DIR_CHANGING:
			pxSpdCtrl->m_ucDirChngDelayTm++;

			if(pxSpdCtrl->m_ucDirChngDelayTm >= 10){
				pxSpdCtrl->m_ucDirChngDelayTm = 0;

				pxSpdCtrl->m_ucDirChngFlag = 1;
				pxSpdCtrl->m_xPid.m_ErrorSum = 0.0f;
				pxSpdCtrl->m_ucCtlState = eSPD_CTL_STATE_IGNITING;
			}
			else {
				px6stepCtx->fpCommTb_unipolar(px6stepCtx->pxDrvUnipolar, 0,  px6stepCtx->iSetDuty, px6stepCtx->ucDir );
				return;
			}
			break;

		default:
			break;
	}




}
