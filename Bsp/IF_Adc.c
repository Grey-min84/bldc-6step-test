#include "IF_Adc.h"

#include <string.h>

//static u16 g_usAdcRawBuf[32];


void InitAdcDrive(AdcModule_t* pxMod, HwAdcWrapper_t* pxHw, u16* pusBuf){

    pxMod->pxHw = pxHw;
    pxMod->pusRawAdcData = pusBuf;
    pxMod->usNbrOfChannel =  portStm32_GetAdc_ChannelCnt(pxHw);
}



void StartStopAdcWithDMA(AdcModule_t* pxMod, u8 onOff){

    
    portStm32_StartAdc_dma(pxMod->pxHw, pxMod->pusRawAdcData);
}






void InitAdcDataFilter(AdcModule_t* pxMod, u16* pusFilteredData, MedianFilter_t* pxMedFilter, AVG_FILTER_VAR* pxAvgFilter){

    u16 chCount = portStm32_GetAdc_ChannelCnt(pxMod->pxHw);

    memset(pusFilteredData, 0, sizeof(u16)*chCount);

    for(u16 idx=0; idx<chCount; ++idx){
        pxMedFilter[idx].m_BuffCount = 0;
        pxMedFilter[idx].m_BuffIndex = 0;
        memset(pxMedFilter[idx].m_Buff, 0, sizeof(s32)*MAX_MEDIAN_FILTER_COUNT);
        memset(pxMedFilter[idx].m_BuffCopy, 0, sizeof(s32)*MAX_MEDIAN_FILTER_COUNT);

        InitMedianFilterVar( &pxMedFilter[idx], MAX_MEDIAN_FILTER_COUNT );
    }


    for(u16 idx=0; idx<chCount; ++idx){
        pxAvgFilter[idx].m_Coeff = 0;
        pxAvgFilter[idx].m_PreValue = 0;
        InitAverageFilter(&pxAvgFilter[idx], 500 );
    }

    pxMod->pusFilterData = pusFilteredData;
    pxMod->pxAdcAvgFilVar = pxAvgFilter;
    pxMod->pxAdcMedFilVar = pxMedFilter;
}


void StartStopAdcDataFilter(AdcModule_t* pxMod, TimerContainer_t* timSrc, u8 onOff){

    static TimerTask_t xTmTask_digitFilt;

    if(onOff != 0)
	    xTmTask_digitFilt = CreateTimerTask(SignalFilterTimerCallback, (void*)pxMod, 1, HARD_TIMER_STARTED);
    else
        xTmTask_digitFilt = CreateTimerTask(SignalFilterTimerCallback, (void*)pxMod, 1, HARD_TIMER_STOPPED);
    
        
    RegisterTimer(timSrc, &xTmTask_digitFilt);
}



void SignalFilterTimerCallback(void *l_pParam)
{
    AdcModule_t* pxAdcMode = (AdcModule_t*)l_pParam;

	int32_t l_aiTempAdcFilterData[ADC_MAX_CHANNEL] = {0,};



	//ADC Value Filtering
	for(u8 idx=0 ; idx<pxAdcMode->usNbrOfChannel ; idx++ )
	{
		// Median Filter
		l_aiTempAdcFilterData[idx] = MedianFilter( &pxAdcMode->pxAdcMedFilVar[idx], pxAdcMode->pusRawAdcData[idx] );
		// Average Filter
		pxAdcMode->pusFilterData[idx] = AverageFilter(&pxAdcMode->pxAdcAvgFilVar[idx], l_aiTempAdcFilterData[idx] );
	}

}

