#include "portStm32_Timer.h"
#include "IF_timer.h"
#include "IF_pwm.h"

extern TimerContainer_t* g_pxTmContainerMain ;
extern IPwm_t* g_apxHwCbContainer[5];
extern uint8_t g_ucHwCbCount;

extern void Pwm1_HwPeriodElapsedCallback(IPwm_t* pxHw);


void StartHWTimer(Tm_HwWrapper* pxHwTimer){

    TIM_HandleTypeDef* pxSTTimer = pxHwTimer->pxTimer;

    HAL_TIM_Base_Start_IT(pxSTTimer);
}


void StopHWTimer(Tm_HwWrapper* pxHwTimer){

    TIM_HandleTypeDef* pxSTTimer = pxHwTimer->pxTimer;

    HAL_TIM_Base_Stop_IT(pxSTTimer);
}




void portSTM32_InitCountingTimer(Tm_HwWrapper* pxTimer){

    TIM_HandleTypeDef* pxSTTimer = pxTimer->pxTimer;

}


u32 portSTM32_GetTimerCount(Tm_HwWrapper* pxTimer){

    TIM_HandleTypeDef* pxSTTimer = pxTimer->pxTimer;

    return __HAL_TIM_GET_COUNTER(pxSTTimer);
}


void portSTM32_ResetTimerCount(Tm_HwWrapper* pxTimer){

    TIM_HandleTypeDef* pxSTTimer = pxTimer->pxTimer;
    __HAL_TIM_SET_COUNTER(pxSTTimer, 0);
}









 //Stm32 Timer Period Elapsed Callback
 void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
 {
   /* USER CODE BEGIN Callback 0 */

   /* USER CODE END Callback 0 */

   /* USER CODE BEGIN Callback 1 */
   if( htim->Instance == TIM6 ) {

     HWTimerCallback(g_pxTmContainerMain);
   }
   else if(htim->Instance == TIM3) {
     //Other Timer Callback if needed
     for(uint8_t i=0; i<g_ucHwCbCount; i++){

            if(g_apxHwCbContainer[i]->pxPwmSrc->pxPwmHw == htim){
                Pwm1_HwPeriodElapsedCallback(g_apxHwCbContainer[i]);
            }
        }
   }
   /* USER CODE END Callback 1 */
 }
