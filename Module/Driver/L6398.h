#ifndef __MOTOR_DRIVER_L6398_H__
#define __MOTOR_DRIVER_L6398_H__
#include "motor_term_def.h"
#include "IF_Hal.h"

typedef struct L6398_Unipolar_tag{
	IPwm_t* pxPwmU_highSide;
	IGpio_t* pxPinU_lowSide;

	IPwm_t* pxPwmV_highSide;
	IGpio_t* pxPinV_lowSide;

	IPwm_t* pxPwmW_highSide;
	IGpio_t* pxPinW_lowSide;
} L6398_Unipolar_t;




typedef struct L6398_Bipolar_tag{
	IPwm_t* pxPwmU_highSide;
    IPwm_t* pxPwmU_lowSide;

    IPwm_t* pxPwmV_highSide;
    IPwm_t* pxPwmV_lowSide;

    IPwm_t* pxPwmW_highSide;
    IPwm_t* pxPwmW_lowSide;
} L6398_Bipolar_t;



void InitL6398_Unipolar(L6398_Unipolar_t* pxDrive, fpPeriodCb fpCb, void* _args);

void Apply_L6398_CommutationUnipolar(void* args, uint8_t state, float pwmVal);

void DrvL6398_6Step_UniPolar_GateCtl(L6398_Unipolar_t* pxDrv, u8 phase, u8 ctl, float duty);
void DrvL6398_6Step_BiPolar_GateCtl(L6398_Bipolar_t* pxDrv, u8 phase, u8 ctl, float duty);


#endif