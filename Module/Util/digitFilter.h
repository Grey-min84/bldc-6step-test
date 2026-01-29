#ifndef __DIGIT_FILTER_H__
#define __DIGIT_FILTER_H__
#include "typeSimple.h"


#define AVG_FILTER_PRECISION 1000 
#define MAX_MEDIAN_FILTER_COUNT 7
#define PI 3.141592f

/* Average Filter Variables */
typedef struct _AVERAGE_FILTER_VAR_
{
	int32_t m_Coeff;
	int32_t m_PreValue;
}AVG_FILTER_VAR, *PAVG_FILTER_VAR;

/* Median Filter Variables */
typedef struct _MEDIAN_FILTER_VAR_
{
	int32_t m_BuffIndex;
	uint16_t m_BuffCount;
	int32_t m_Buff[MAX_MEDIAN_FILTER_COUNT];
	int32_t m_BuffCopy[MAX_MEDIAN_FILTER_COUNT];
}MedianFilter_t;

/* High Pass Filter Variables */
typedef struct _HIGH_PASS_FILTER_
{
	int32_t m_iAlpha;
	int32_t m_iPreInputValue;
	int32_t m_iPreHpfValue;
	int32_t m_iPrecision;
}HighPassFilter_t;

void InitAverageFilter(PAVG_FILTER_VAR l_pstVar, int32_t l_iCoeff);
int32_t AverageFilter(PAVG_FILTER_VAR l_pstVar, int32_t l_NewValue);

void InitMedianFilterVar(MedianFilter_t *l_pstVar, uint16_t l_usCount);
int32_t MedianFilter(MedianFilter_t *l_pstVar, int32_t l_NewValue);

void BubbleSort(int32_t *l_iDataArray, uint16_t l_iCnt);

void InitHighPassFilter(HighPassFilter_t *l_pstVar, float l_fCutoffFreq, float l_fSampleTime, int l_iPrecision);
int32_t HighPassFilter(HighPassFilter_t *l_pstVar, int32_t l_iNewValue);


#endif