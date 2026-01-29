#include "portStm32_Adc.h"


void portStm32_InitAdcDrive(HwAdcWrapper_t* pxHwWrapper, ADC_HandleTypeDef* _pxHwAdc){

    pxHwWrapper->pxHwAdc = _pxHwAdc;

}


void portStm32_StartAdc_dma(HwAdcWrapper_t* pxHwWrapper, u16* pusBuf){

    u16 usChCnt = 0;

    usChCnt = portStm32_GetAdc_ChannelCnt(pxHwWrapper);

    HAL_ADC_Start_DMA(pxHwWrapper->pxHwAdc, (uint32_t *)pusBuf, usChCnt);

}



u16 portStm32_GetAdc_ChannelCnt(HwAdcWrapper_t* pxHwWrapper){

    u16 rtn = 0;

    rtn = pxHwWrapper->pxHwAdc->Init.NbrOfConversion;

    return rtn;
}