#ifndef __SPD_CTRL_H__
#define __SPD_CTRL_H__
#include "measSpd.h"
#include  "pid_ctrl.h"


typedef struct {
	PIDCtrlFloat_t m_xPid;
	uint8_t m_ucOutputOn;
	uint8_t m_ucDir;
	uint8_t m_ucDir_pre;
	uint8_t m_ucDirChngFlag;
	uint16_t m_ucRpmMeasCnt;
	int32_t m_iCurrRpm;
	int32_t m_iTargtRpm;
	uint8_t m_ucIgnited;
	uint16_t m_ucIgniteTm;
	uint16_t m_ucIgnitePwr;
}MotorRpmCtrl_t;

void SpeedControl_loop(void* px6stepCtx);
void SpeedControl_Init(MotorRpmCtrl_t* pxSpdCtrl);
#endif
