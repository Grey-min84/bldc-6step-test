#ifndef __MOTOR_TERM_DEF_H__
#define __MOTOR_TERM_DEF_H__
#include "IF_Hal.h"
enum SIX_STEP_STS{
	_6STEP_HiZ = 0,
	_6STEP_PWM_IN,
	_6STEP_LOWSIDE_ON,
};


enum BLDC_POLE{
	POLE_U = 1,
	POLE_V= 2,
	POLE_W= 3,
};

typedef struct DrvPwm_Unipolar_tag{
	IPwm_t* pxPwmU_highSide;
	IGpio_t* pxPinU_lowSide;

	IPwm_t* pxPwmV_highSide;
	IGpio_t* pxPinV_lowSide;

	IPwm_t* pxPwmW_highSide;
	IGpio_t* pxPinW_lowSide;
} DrvPwm_Unipolar_t;




typedef struct DrvPwm_Bipolar_tag{
	IPwm_t* pxPwmU_highSide;
    IPwm_t* pxPwmU_lowSide;

    IPwm_t* pxPwmV_highSide;
    IPwm_t* pxPwmV_lowSide;

    IPwm_t* pxPwmW_highSide;
    IPwm_t* pxPwmW_lowSide;
} DrvPwm_Bipolar_t;

#endif