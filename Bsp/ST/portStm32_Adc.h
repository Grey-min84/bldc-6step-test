#ifndef __PORT_STM32_ADC_H__
#define __PORT_STM32_ADC_H__

#include "typeSimple.h"
#include "stm32g4xx_hal.h"

typedef struct {

    ADC_HandleTypeDef* pxHwAdc;

}HwAdcWrapper_t;


void portStm32_InitAdcDrive(HwAdcWrapper_t* pxHwWrapper, ADC_HandleTypeDef* _pxHwAdc);
void portStm32_StartAdc_dma(HwAdcWrapper_t* pxHwWrapper, u16* pusBuf);
u16 portStm32_GetAdc_ChannelCnt(HwAdcWrapper_t* pxHwWrapper);


#endif
