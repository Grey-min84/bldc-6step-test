#ifndef __MEAS_SUPPORT_H__
#define __MEAS_SUPPORT_H__

#include <stdint.h>

void Extract_Channel_Data(uint16_t* adcBuf, uint16_t *output, uint8_t channel, uint16_t usSamplesPerCh,  uint16_t usNumChannels);
float Calculate_Average_float(uint16_t* adcBuf, uint8_t channel, uint16_t usSamplesPerCh,  uint16_t usNumChannels);
uint32_t Calculate_Average_u32(uint16_t* adcBuf, uint8_t channel, uint16_t usSamplesPerCh,  uint16_t usNumChannels);

uint32_t Calculate_AverageU32_lower(uint32_t* adcBuf, uint8_t channel, uint16_t usSamplesPerCh,  uint16_t usNumChannels);
uint32_t Calculate_AverageU32_upper(uint32_t* adcBuf, uint8_t channel, uint16_t usSamplesPerCh,  uint16_t usNumChannels);
#endif