#include "portStm32_Pwm.h"


static void TIM_ComplementaryChCtl(TIM_TypeDef *TIMx, uint32_t Channel, uint32_t ChannelNState);




void portSTM32_PwmChannelInit(Pwm1Ch_HwWrapper* pxPwm, TIM_HandleTypeDef* pxHwTimer, u32 uiHwCh, u32 uiMaxDuty){

    pxPwm->uiChannel = uiHwCh;
    pxPwm->pxPwmHw = pxHwTimer;
    //pxCtx->uiBldcPole = uiBldcPole;
    pxPwm->uiMaxDuty = uiMaxDuty;
}





void portSTM32_PWM_StartStop(Pwm1Ch_HwWrapper* pxPwm, u8 ucOnOff){

    TIM_HandleTypeDef* pxHwTimer = pxPwm->pxPwmHw;
    //u32 uiTmChannel = pxCtx->uiLookupChTb[ucBldcCh];

    if(ucOnOff != 0){
        HAL_TIM_PWM_Start(pxHwTimer, pxPwm->uiChannel);
    }
    else {
        HAL_TIM_PWM_Stop(pxHwTimer, pxPwm->uiChannel);
    }
}




void portSTM32_PWM_Generate(Pwm1Ch_HwWrapper* pxPwm, u32 uiDuty){
    
    TIM_HandleTypeDef* pxHwTimer = pxPwm->pxPwmHw;
    //u32 uiTmChannel = pxCtx->uiLookupChTb[ucBldcCh];
    
    __HAL_TIM_SET_COMPARE(pxHwTimer, pxPwm->uiChannel, uiDuty) ;
}







void portSTM32_PWMDual_Init(PWM2Ch_HwWrapper* pxPwm, TIM_HandleTypeDef* pxHwTimer, u32 uiHwCh, u32 uiMaxDuty){

    pxPwm->uiChannel = uiHwCh;
    pxPwm->pxPwmHw = pxHwTimer;
    //pxCtx->uiBldcPole = uiBldcPole;
    pxPwm->uiMaxDuty = uiMaxDuty;
}



void portSTM32_PWMDual_StartStop(PWM2Ch_HwWrapper* pxPwm,  u8 ucOnOff){

    if(ucOnOff != 0){
        HAL_TIM_PWM_Start(pxPwm->pxPwmHw, pxPwm->uiChannel);
        HAL_TIMEx_PWMN_Start(pxPwm->pxPwmHw, pxPwm->uiChannel);
    }
    else {
        HAL_TIM_PWM_Stop(pxPwm->pxPwmHw, pxPwm->uiChannel);
        HAL_TIMEx_PWMN_Stop(pxPwm->pxPwmHw, pxPwm->uiChannel);
    }
}


void portSTM32_PWMDual_Generate(PWM2Ch_HwWrapper* pxDualPwm, u32 uiDuty){

    __HAL_TIM_SET_COMPARE(pxDualPwm->pxPwmHw, pxDualPwm->uiChannel, uiDuty) ;
}


void portSTM32_PWMDual_NChannelHigh(PWM2Ch_HwWrapper* pxDualPwm){

    TIM_ComplementaryChCtl(pxDualPwm->pxPwmHw->Instance, pxDualPwm->uiChannel, TIM_CCxN_ENABLE);
    __HAL_TIM_SET_COMPARE(pxDualPwm->pxPwmHw, pxDualPwm->uiChannel, 0) ;
}


void portSTM32_PWMDual_NChannelLow(PWM2Ch_HwWrapper* pxDualPwm){

    TIM_ComplementaryChCtl(pxDualPwm->pxPwmHw->Instance, pxDualPwm->uiChannel, TIM_CCxN_DISABLE);
    //__HAL_TIM_SET_COMPARE(pxDualPwm->pxPwmHw, pxDualPwm->uiChannel, 0) ;
}


void portSTM32_PWMDual_Complementary(PWM2Ch_HwWrapper* pxDualPwm, u32 uiChP, u32 uiChN, u8 ucMode){


}




static void TIM_ComplementaryChCtl(TIM_TypeDef *TIMx, uint32_t Channel, uint32_t ChannelNState)
{
  uint32_t tmp;

  tmp = TIM_CCER_CC1NE << (Channel & 0xFU); /* 0xFU = 15 bits max shift */

  /* Reset the CCxNE Bit */
  TIMx->CCER &=  ~tmp;

  /* Set or reset the CCxNE Bit */
  TIMx->CCER |= (uint32_t)(ChannelNState << (Channel & 0xFU)); /* 0xFU = 15 bits max shift */
}

