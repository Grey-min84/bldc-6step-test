#ifndef __SPD_CTRL_H__
#define __SPD_CTRL_H__
#include "measSpd.h"
#include  "pid_ctrl.h"


enum eSPD_CTL_STATE {
	eSPD_CTL_STATE_DISABLED = 0,
	eSPD_CTL_STATE_IDLE ,
	eSPD_CTL_STATE_IGNITING,
	eSPD_CTL_STATE_RUNNING,
	eSPD_CTL_STATE_DIR_CHANGING,
	eSPD_CTL_STATE_STOPPED
};


typedef struct {
	PIDCtrlFloat_t m_xPid;
	uint8_t m_ucCtlState;
	uint8_t m_ucDir;
	uint8_t m_ucDir_pre;
	uint8_t m_ucDirChngFlag;
	uint8_t m_ucDirChngDelayTm;
	uint16_t m_ucRpmMeasCnt;
	int32_t m_iCurrRpm;
	int32_t m_iTargtRpm;
	uint8_t m_ucIgnited;
	uint16_t m_ucIgniteTm;
	uint16_t m_ucIgnitePwr;

	float fPidOut;
	float fPid_ErrInput;
	uint16_t ucMinPwr;
}MotorRpmCtrl_t;

void SpeedControl_loop(void* px6stepCtx);
void SpeedControl_Init(MotorRpmCtrl_t* pxSpdCtrl, u32 uiMaxPeriodCnt);
#endif
