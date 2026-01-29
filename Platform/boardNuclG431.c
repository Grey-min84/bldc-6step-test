#include "boardNuclG431.h"

//#include "boardBCM.h"

extern TIM_HandleTypeDef htim1;
extern TIM_HandleTypeDef htim6;
extern TIM_HandleTypeDef htim3;

extern ADC_HandleTypeDef hadc1;

MedianFilter_t g_xMedFilter[ADC_MAX_CHANNEL];
AVG_FILTER_VAR g_xAvgFilter[ADC_MAX_CHANNEL];

u16 g_ausAdcRawData[ADC_MAX_CHANNEL];
u16 g_ausAdcFilterData[ADC_MAX_CHANNEL];



static Pwm1Ch_HwWrapper g_xPwmHw_6stepUnipolar_U = {&htim3, TIM_CHANNEL_2, 4249};
static Gpio_HwWrapper g_xPin_6stepUnipolar_U = {GPIOA, GPIO_PIN_8};

static Pwm1Ch_HwWrapper g_xPwmHw_6stepUnipolar_V = {&htim3, TIM_CHANNEL_3, 4249};
static Gpio_HwWrapper g_xPin_6stepUnipolar_V = {GPIOA, GPIO_PIN_9};

static Pwm1Ch_HwWrapper g_xPwmHw_6stepUnipolar_W = {&htim3, TIM_CHANNEL_4, 4249};
static Gpio_HwWrapper g_xPin_6stepUnipolar_W = {GPIOA, GPIO_PIN_10};


static Gpio_HwWrapper portHw_HallU = {GPE_HALL_1_GPIO_Port, GPE_HALL_1_Pin };
static Gpio_HwWrapper portHw_HallV = {GPE_HALL_2_GPIO_Port, GPE_HALL_2_Pin };
static Gpio_HwWrapper portHw_HallW = {GPE_HALL_3_GPIO_Port, GPE_HALL_3_Pin };



void PlatformConfig_BaseTimer(TimerContainer_t* pxTmContainer, TimerCounter_t* pxTmCounter){

  static Tm_HwWrapper basicTimer;


  basicTimer.pxTimer = &htim6;

  InitTimer(pxTmContainer, &basicTimer);

  pxTmCounter->vxHwTimer = &basicTimer;
}


void PlatformCOnfig_PwmPeriodCallback(TimerContainer_t* pxTmContainerPwm){

  static Tm_HwWrapper basicTimer;

  basicTimer.pxTimer = &htim6;

  InitTimer(pxTmContainerPwm, &basicTimer);

}


void PlatformConfig_6stepUniPolar(IPwm_t* pxPhaseUHigh, IPwm_t* pxPhaseVHigh, IPwm_t* pxPhaseWHigh,
  IGpio_t* pxPhaseULow, IGpio_t* pxPhaseVLow, IGpio_t* pxPhaseWLow
){


  GpioPin_Def(1, pxPhaseULow, &g_xPin_6stepUnipolar_U);
  GpioPin_Def(1, pxPhaseVLow, &g_xPin_6stepUnipolar_V);
  GpioPin_Def(1, pxPhaseWLow, &g_xPin_6stepUnipolar_W);

  Pwm1_InitHw(pxPhaseUHigh, &g_xPwmHw_6stepUnipolar_U, 1);
  Pwm1_InitHw(pxPhaseVHigh, &g_xPwmHw_6stepUnipolar_V, 2);
  Pwm1_InitHw(pxPhaseWHigh, &g_xPwmHw_6stepUnipolar_W, 3);
}




void PlatformConfig_HallSens_ISR(IGpio_t* pxHallU, IGpio_t* pxHallV, IGpio_t* pxHallW, EdgeCallback fpCb, void* _args){

  GpioPin_Def(4, pxHallU, &portHw_HallU);
  GpioPin_Def(5, pxHallV, &portHw_HallV);
  GpioPin_Def(6, pxHallW, &portHw_HallW);


  //HallEdgeDetected
  GpioIsr_RegisterCallback(pxHallU, fpCb, _args);
  GpioIsr_RegisterCallback(pxHallV, fpCb, _args);
  GpioIsr_RegisterCallback(pxHallW, fpCb, _args);



}


void PlatformConfig_Analog(AdcModule_t* pxMod, TimerContainer_t* tmContainer){

  // static HwAdcWrapper_t xAdcWrapper;

  // xAdcWrapper.pxHwAdc = &hadc1;

  // InitAdcDrive(pxMod, &xAdcWrapper, g_ausAdcRawData);

  // InitAdcDataFilter(pxMod, g_ausAdcFilterData, g_xMedFilter, g_xAvgFilter);

  // StartStopAdcDataFilter(pxMod, tmContainer, 1);

  //StartStopAdcWithDMA(pxMod, 1);

}
