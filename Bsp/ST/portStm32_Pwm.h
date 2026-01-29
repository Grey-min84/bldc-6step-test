#ifndef __PORT_STM32_PWM_H__
#define __PORT_STM32_PWM_H__

#include "typeSimple.h"
#include "stm32g4xx_hal.h"


typedef struct{
    TIM_HandleTypeDef* pxPwmHw;
    uint32_t uiChannel;
    //uint32_t uiBldcPole;
    uint32_t uiMaxDuty;
}Pwm1Ch_HwWrapper;

typedef struct {
    TIM_HandleTypeDef* pxPwmHw;
    uint32_t uiChannel;
    //uint32_t uiBldcPole;
    uint32_t uiMaxDuty;
}PWM2Ch_HwWrapper;


void portSTM32_PwmChannelInit(Pwm1Ch_HwWrapper* pxPwm, TIM_HandleTypeDef* pxHwTimer, u32 uiHwCh, u32 uiMaxDuty);
void portSTM32_PWM_StartStop(Pwm1Ch_HwWrapper* pxPwm,  u8 ucOnOff);
void portSTM32_PWM_Generate(Pwm1Ch_HwWrapper* pxPwm, u32 uiDuty);


void portSTM32_PWMDual_Init(PWM2Ch_HwWrapper* pxPwm, TIM_HandleTypeDef* pxHwTimer, u32 uiHwCh, u32 uiMaxDuty);

void portSTM32_PWMDual_StartStop(PWM2Ch_HwWrapper* pxPwm,  u8 ucOnOff);

void portSTM32_PWMDual_Generate(PWM2Ch_HwWrapper* pxDualPwm, u32 uiDuty);

void portSTM32_PWMDual_NChannelHigh(PWM2Ch_HwWrapper* pxDualPwm);
void portSTM32_PWMDual_NChannelLow(PWM2Ch_HwWrapper* pxDualPwm);
void portSTM32_PWMDual_Complementary(PWM2Ch_HwWrapper* pxDualPwm, u32 uiChP, u32 uiChN, u8 ucMode);

#endif
