#ifndef __IF_ADC_H__
#define __IF_ADC_H__
#include "IF_timer.h"
#include "portStm32_Adc.h"
#include "digitFilter.h"

#define ADC_MAX_CHANNEL     (16)

typedef void (*SigFiltCb)(void*);

typedef struct {
    HwAdcWrapper_t* pxHw;
    u16 usNbrOfChannel;

    u16* pusRawAdcData;
    u16* pusFilterData;
    AVG_FILTER_VAR* pxAdcAvgFilVar;
    MedianFilter_t* pxAdcMedFilVar;

}AdcModule_t;


void InitAdcDrive(AdcModule_t* pxMod, HwAdcWrapper_t* pxHw, u16* pusBuf);
void StartStopAdcWithDMA(AdcModule_t* pxMod, u8 onOff);


void InitAdcDataFilter(AdcModule_t* pxMod, u16* pusFilteredData, MedianFilter_t* pxMedFilter, AVG_FILTER_VAR* pxAvgFilter);
void StartStopAdcDataFilter(AdcModule_t* pxMod, TimerContainer_t* timSrc, u8 onOff);
void SignalFilterTimerCallback(void *l_pParam);
#endif