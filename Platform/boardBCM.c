#include "boardBCM.h"

#if 0
extern TIM_HandleTypeDef htim1;
extern TIM_HandleTypeDef htim6;
extern TIM_HandleTypeDef htim15;

extern ADC_HandleTypeDef hadc1;

MedianFilter_t g_xMedFilter[ADC_MAX_CHANNEL];
AVG_FILTER_VAR g_xAvgFilter[ADC_MAX_CHANNEL];

u16 g_ausAdcRawData[ADC_MAX_CHANNEL];
u16 g_ausAdcFilterData[ADC_MAX_CHANNEL];

void BSPConfig_TimPwm(BldcPWM_Ctx_t* pxPwmCtx,  TimerContainer_t* pxTmContainer, TimerCounter_t* pxTmCounter){

  static Pwm1Ch_HwWrapper xPwmHw_poleU_pos;
  static Pwm1Ch_HwWrapper xPwmHw_poleU_neg;

  static Pwm1Ch_HwWrapper xPwmHw_polev_pos;
  static Pwm1Ch_HwWrapper xPwmHw_poleV_neg;

  static Pwm1Ch_HwWrapper xPwmHw_poleW_pos;
  static Pwm1Ch_HwWrapper xPwmHw_poleW_neg;

  static Tm_HwWrapper basicTimer;

  portSTM32_PwmChannelInit(&xPwmHw_poleU_pos, &htim1, TIM_CHANNEL_1, 3199);
  portSTM32_PwmChannelInit(&xPwmHw_poleU_neg, &htim1, TIM_CHANNEL_2, 3199);

  portSTM32_PwmChannelInit(&xPwmHw_polev_pos, &htim1, TIM_CHANNEL_3, 3199);
  portSTM32_PwmChannelInit(&xPwmHw_poleV_neg, &htim1, TIM_CHANNEL_4, 3199);

  portSTM32_PwmChannelInit(&xPwmHw_poleW_pos, &htim15, TIM_CHANNEL_1, 3199);
  portSTM32_PwmChannelInit(&xPwmHw_poleW_neg, &htim15, TIM_CHANNEL_2, 3199);


  PWM_ChannelMatching(pxPwmCtx, &xPwmHw_poleU_pos, ePWM_POLE_U_POS);
  PWM_ChannelMatching(pxPwmCtx, &xPwmHw_poleU_neg, ePWM_POLE_U_NEG);

  PWM_ChannelMatching(pxPwmCtx, &xPwmHw_polev_pos, ePWM_POLE_V_POS);
  PWM_ChannelMatching(pxPwmCtx, &xPwmHw_poleV_neg, ePWM_POLE_V_NEG);

  PWM_ChannelMatching(pxPwmCtx, &xPwmHw_poleW_pos, ePWM_POLE_W_POS);
  PWM_ChannelMatching(pxPwmCtx, &xPwmHw_poleW_neg, ePWM_POLE_W_NEG);



  basicTimer.pxTimer = &htim6;

  InitTimer(pxTmContainer, &basicTimer);

  pxTmCounter->vxHwTimer = &basicTimer;
}


void BSPConfig_HallSens(HallSensePin_t* _pxHallPin, IGpio_t* pxHallU, IGpio_t* pxHallV, IGpio_t* pxHallW, EdgeCallback cb){

  static Gpio_HwWrapper portHw_HallU = {GPIOA, GPE3_BLDC_HU_Pin };
  static Gpio_HwWrapper portHw_HallV = {GPIOA, GPE4_BLDC_HV_Pin };
  static Gpio_HwWrapper portHw_HallW = {GPIOA, GPE5_BLDC_HW_Pin };


  GpioPin_Def(eHALL_U, GPIO_PIN_EDGE, GPI_FILTER_OFF, pxHallU, &portHw_HallU);
  GpioPin_Def(eHALL_V, GPIO_PIN_EDGE, GPI_FILTER_OFF, pxHallV, &portHw_HallV);
  GpioPin_Def(eHALL_W, GPIO_PIN_EDGE, GPI_FILTER_OFF, pxHallW, &portHw_HallW);

  //HallEdgeDetected
  GpioIsr_RegisterCallback(pxHallU, cb, NULL);
  GpioIsr_RegisterCallback(pxHallV, cb, NULL);
  GpioIsr_RegisterCallback(pxHallW, cb, NULL);


  CheckDuplicate_N_makeList(pxHallU);
  CheckDuplicate_N_makeList(pxHallV);
  CheckDuplicate_N_makeList(pxHallW);

  _pxHallPin->pxU = pxHallU;
  _pxHallPin->pxV = pxHallV;
  _pxHallPin->pxW = pxHallW;


}


void BSPConfig_Analog(AdcModule_t* pxMod, TimerContainer_t* tmContainer){

  static HwAdcWrapper_t xAdcWrapper;

  xAdcWrapper.pxHwAdc = &hadc1;

  InitAdcDrive(pxMod, &xAdcWrapper, g_ausAdcRawData);

  InitAdcDataFilter(pxMod, g_ausAdcFilterData, g_xMedFilter, g_xAvgFilter);

  StartStopAdcDataFilter(pxMod, tmContainer, 1);

  StartStopAdcWithDMA(pxMod, 1);

}

#endif
