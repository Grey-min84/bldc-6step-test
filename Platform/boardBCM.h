#ifndef __ST_BOARD_BCM_H__
#define __ST_BOARD_BCM_H__

#if 0
#include "main.h"

#include "portStm32_Pwm.h"
#include "portStm32_Timer.h"
#include "portStm32_Gpio.h"
#include "portStm32_Adc.h"

#include "IF_pwm.h"
#include "IF_timer.h"
#include "IF_uart.h"
#include "IF_gpio.h"
#include "IF_Adc.h"

enum eHALL_ID{
	eHALL_U = 0,
	eHALL_V,
	eHALL_W,
};

void BSPConfig_TimPwm(BldcPWM_Ctx_t* pxPwmCtx, TimerContainer_t* pxTmContainer,TimerCounter_t* pxTmCounter);
void BSPConfig_HallSens(HallSensePin_t* _pxHallPin, IGpio_t* pxHallU, IGpio_t* pxHallV, IGpio_t* pxHallW, EdgeCallback cb);
void BSPConfig_Analog(AdcModule_t* pxMod, TimerContainer_t* tmContainer);

#endif

#endif
