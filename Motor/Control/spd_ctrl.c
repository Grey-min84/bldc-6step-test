#include "spd_ctrl.h"
#include "six_step.h"

#define DEF_RPM_CTL_KP			(0.01f)
#define DEF_RPM_CTL_KI			(0.0005f)
#define DEF_RPM_CTL_KD			(0.0f)
#define DEF_RPM_CTL_KA			(1.0f)
#define DEF_RPM_CTL_MAX_ERR		(100)
#define DEF_RPM_CTL_MAX_OUTPUT	(2599)
#define DEF_RPM_CTL_MIN_OUTPUT	(0)
#define DEF_RPM_CTL_SLEW_RATE	(5)

void SpeedControl_Init(MotorRpmCtrl_t* pxSpdCtrl){

    InitPIDCtrlVarFloat(&pxSpdCtrl->m_xPid);
	SetPIDGainFloat(&pxSpdCtrl->m_xPid, DEF_RPM_CTL_KP, DEF_RPM_CTL_KI, DEF_RPM_CTL_KD, DEF_RPM_CTL_KA);
	SetPIDConfigVarFloat(&pxSpdCtrl->m_xPid, DEF_RPM_CTL_MAX_ERR, DEF_RPM_CTL_MAX_OUTPUT, DEF_RPM_CTL_MIN_OUTPUT);
	pxSpdCtrl->m_xPid.m_fSlewRate = DEF_RPM_CTL_SLEW_RATE;

    pxSpdCtrl->m_ucIgnitePwr = 400;
}


void SpeedControl_loop(void* args){

	float fPidOut = 0.0f;
	float fPid_ErrInput = 0.0f;

    _6StepCtlCtx_t* px6stepCtx = (_6StepCtlCtx_t*)args;


    MotorRpmCtrl_t* pxSpdCtrl = px6stepCtx->pxSpdCtl;


	if(pxSpdCtrl->m_ucDir != pxSpdCtrl->m_ucDir_pre){
		pxSpdCtrl->m_ucIgnited = 0;
		pxSpdCtrl->m_ucDirChngFlag = 1;
	}

	

	if(pxSpdCtrl->m_ucOutputOn != 0){

		if(pxSpdCtrl->m_ucIgnited == 1){

			fPid_ErrInput = (float)(pxSpdCtrl->m_iTargtRpm - pxSpdCtrl->m_iCurrRpm);

			fPidOut = CalcPidFloat_Incremental(&pxSpdCtrl->m_xPid, fPid_ErrInput);

			//RunDcMotor(g_pstBrkDcMotor, pxSpdCtrl->m_ucDir, (uint16_t)fPidOut);
            px6stepCtx->iSetDuty = (u32)fPidOut;
		}
		else {

			// 모터 구동 후 초기에 RPM 측정이 안정화 될때까지 PWM 제어 수행
            px6stepCtx->iSetDuty = pxSpdCtrl->m_ucIgnitePwr;
		}

		pxSpdCtrl->m_ucDir_pre = pxSpdCtrl->m_ucDir;
		
	}
	else {
		px6stepCtx->iSetDuty = 0;
	}


}
