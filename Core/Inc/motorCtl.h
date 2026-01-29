#ifndef __MOTORCTL_H__
#define __MOTORCTL_H__
#include "main.h"


uint8_t ReadHallSensors(void);
void SetPhase(uint8_t phase, uint8_t state, uint16_t);
void ApplyCommutation(uint8_t state, uint16_t);
void ApplyCommutation2(uint8_t state, uint16_t);
void ApplyCommutation_DRV832x(uint8_t state, uint16_t pwmVal);
#endif