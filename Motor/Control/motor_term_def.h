#ifndef __MOTOR_TERM_DEF_H__
#define __MOTOR_TERM_DEF_H__

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

#endif