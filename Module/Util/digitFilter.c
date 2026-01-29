#include "digitFilter.h"
#include <string.h>


void BubbleSort(int32_t *l_iDataArray, uint16_t l_iCnt)
{
	int i, j;
	int l_fW;

	//bubble sort r
	for (i = (l_iCnt - 1); i > 0; i--)
	{
		for (j = 0; j < i; j++)
		{
			if (l_iDataArray[j] > l_iDataArray[j + 1]) // wrong order? - swap them
			{
				l_fW = l_iDataArray[j];
				l_iDataArray[j] = l_iDataArray[j + 1];
				l_iDataArray[j + 1] = l_fW;
			}
		}
	}
}

void InitAverageFilter(PAVG_FILTER_VAR l_pstVar, int32_t l_iCoeff)
{
	l_pstVar->m_Coeff = l_iCoeff;
	l_pstVar->m_PreValue = 0;
}

int32_t AverageFilter(PAVG_FILTER_VAR l_pstVar, int32_t l_iNewValue)
{
	int32_t l_iRet = 0;
	
	l_iRet = (l_pstVar->m_Coeff*l_iNewValue + (AVG_FILTER_PRECISION-l_pstVar->m_Coeff)*l_pstVar->m_PreValue)/AVG_FILTER_PRECISION ;
	l_pstVar->m_PreValue = l_iRet;

	return l_iRet;
}

void InitHighPassFilter(HighPassFilter_t *l_pstVar, float l_fCutoffFreq, float l_fSampleTime, int l_iPrecision)
{
	float l_fTau = 0.0f;
	float l_fAlpha = 0.0f;

	l_fTau = 1/(2*PI*l_fCutoffFreq);
	l_fAlpha = l_fTau/(l_fTau+l_fSampleTime);

	l_pstVar->m_iAlpha = (int32_t)(l_fAlpha*l_iPrecision);
	l_pstVar->m_iPrecision = l_iPrecision;
	l_pstVar->m_iPreHpfValue = 0;
	l_pstVar->m_iPreInputValue = 0;
}

int32_t HighPassFilter(HighPassFilter_t *l_pstVar, int32_t l_iNewValue)
{
	int32_t l_iRes = 0;

	l_iRes = (l_pstVar->m_iAlpha*(l_pstVar->m_iPreHpfValue + l_iNewValue - l_pstVar->m_iPreInputValue))/l_pstVar->m_iPrecision;

	l_pstVar->m_iPreHpfValue = l_iRes;
	l_pstVar->m_iPreInputValue = l_iNewValue;

	return l_iRes;
}

void InitMedianFilterVar(MedianFilter_t *l_pstVar, uint16_t l_usCount)
{
	if(l_usCount >= MAX_MEDIAN_FILTER_COUNT)
	{
		l_usCount = MAX_MEDIAN_FILTER_COUNT;
	}

	l_pstVar->m_BuffCount = l_usCount;
	l_pstVar->m_BuffIndex = 0;

	memset(l_pstVar->m_Buff, 0, sizeof(uint32_t) * MAX_MEDIAN_FILTER_COUNT);
	memset(l_pstVar->m_BuffCopy, 0, sizeof(uint32_t) * MAX_MEDIAN_FILTER_COUNT);
}

int32_t MedianFilter(MedianFilter_t *l_pstVar, int32_t l_iNewValue)
{
	int32_t l_iRet = 0;
	uint16_t l_usCenterIndex;

	l_usCenterIndex = l_pstVar->m_BuffCount/2;

	//if(l_pstVar->m_Buff != 0 )
	//{
	l_pstVar->m_Buff[ l_pstVar->m_BuffIndex ] = l_iNewValue;
	memcpy((void*)l_pstVar->m_BuffCopy, (void*)l_pstVar->m_Buff, l_pstVar->m_BuffCount*sizeof(int32_t));
	BubbleSort(l_pstVar->m_BuffCopy, l_pstVar->m_BuffCount );
	l_iRet = l_pstVar->m_BuffCopy[l_usCenterIndex];
	l_pstVar->m_BuffIndex++;
	if( l_pstVar->m_BuffIndex >= l_pstVar->m_BuffCount )
		l_pstVar->m_BuffIndex = 0;
	//}

	return l_iRet;
}

