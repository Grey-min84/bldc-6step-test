#ifndef __MOTOR_DRIVER_L6398_H__
#define __MOTOR_DRIVER_L6398_H__
#include "motor_term_def.h"
#include "IF_Hal.h"





void InitL6398_Unipolar(DrvPwm_Unipolar_t* pxDrive, fpPeriodCb fpCb, void* _args);

void Apply_L6398_CommutationUnipolar(DrvPwm_Unipolar_t* args, uint8_t state, float pwmVal);

void DrvL6398_6Step_UniPolar_GateCtl(DrvPwm_Unipolar_t* pxDrv, u8 phase, u8 ctl, float duty);
void DrvL6398_6Step_BiPolar_GateCtl(DrvPwm_Bipolar_t* pxDrv, u8 phase, u8 ctl, float duty);


#endif