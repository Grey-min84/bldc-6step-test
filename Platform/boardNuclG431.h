#ifndef __ST_NUCLEO_G431_H__
#define __ST_NUCLEO_G431_H__

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



void InverterConfig_6Step_Unipolar();



void PlatformConfig_BaseTimer(TimerContainer_t* pxTmContainer,TimerCounter_t* pxTmCounter);

void PlatformConfig_6stepUniPolar(IPwm_t* pxPhaseUHigh, IPwm_t* pxPhaseVHigh, IPwm_t* pxPhaseWHigh,
  IGpio_t* pxPhaseULow, IGpio_t* pxPhaseVLow, IGpio_t* pxPhaseWLow);

void PlatformConfig_HallSens_ISR(IGpio_t* pxHallU, IGpio_t* pxHallV, IGpio_t* pxHallW, EdgeCallback fpCb, void* _args);
void PlatformConfig_Analog(AdcModule_t* pxMod, TimerContainer_t* tmContainer);
#endif
