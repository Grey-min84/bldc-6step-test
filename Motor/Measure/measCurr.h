#ifndef __MEAS_CURRENT_H__
#define __MEAS_CURRENT_H__

#include "measSupport.h"
#include "IF_Hal.h"

enum eADC_CH_IDX{
	eADC_CH_CURR_A = 0,
	eADC_CH_CURR_B,
	eADC_CH_CURR_C,
	eADC_CH_VBUS,
	eADC_CH_BEMF_A,
	eADC_CH_BEMF_B,
	eADC_CH_BEMF_C,
	eADC_CH_MAX
};


#define ADC_SAMPLE_PER_CH   (4)
#define ADC_BUFFER_LENGTH    6


#endif