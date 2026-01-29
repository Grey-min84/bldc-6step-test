#ifndef __PORT_STM32_TIMER_H__
#define __PORT_STM32_TIMER_H__

#include "typeSimple.h"
#include "stm32g4xx_hal.h"




typedef struct {
    TIM_HandleTypeDef* pxTimer;
}Tm_HwWrapper;



void StopHWTimer(Tm_HwWrapper* pxHwTimer);
void StartHWTimer(Tm_HwWrapper* pxHwTimer);
//void OnTimerPeriodExpired(Tm_HwWrapper* pxHwTimer);


void portSTM32_InitCountingTimer(Tm_HwWrapper* pxTimer);
u32 portSTM32_GetTimerCount(Tm_HwWrapper* pxTimer);
void portSTM32_ResetTimerCount(Tm_HwWrapper* pxTimer);


#endif
