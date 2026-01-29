#ifndef __SENS_HALL_H__
#define __SENS_HALL_H__
#include "IF_Hal.h"

#define BCM_MOTOR_POLE_PAIRS    (7)
#define SPD_TASK_INTEVAL_MS    (1)
#define HALL_TIMEOUT_FACTOR   (25)
#define HALL_DT_BUF (16)


typedef struct BldcCommTb_tag{
	uint8_t Hall;
	int32_t Rotator_angle;
	uint8_t U_sts;
	uint8_t V_sts;
	uint8_t W_sts;
}BldcCommTb_t;

typedef struct BldcHallTb_tage{
	uint8_t _U;
	uint8_t _V;
	uint8_t _W;
    // uint8_t ucHallCombi;

    // IGpio_t* pxU;
	// IGpio_t* pxV;
	// IGpio_t* pxW;

}BldcHallTb_t;

extern volatile uint32_t last_hall_tick;
extern volatile uint32_t hall_dt_us;


void MeasHallPeriod(uint8_t u, uint8_t v, uint8_t w);
float CalcHallSensor_RawRPM(uint32_t _dt_us);
#endif