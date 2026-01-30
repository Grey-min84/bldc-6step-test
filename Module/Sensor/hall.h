#ifndef __SENS_HALL_H__
#define __SENS_HALL_H__
#include "IF_Hal.h"

#define BCM_MOTOR_POLE_PAIRS    (7)
#define SPD_TASK_INTEVAL_MS    (1)
#define HALL_TIMEOUT_FACTOR   (25)
#define HALL_DT_BUF (16)

typedef struct {
	volatile uint8_t hall_state_prev;
	volatile uint32_t last_hall_tick;
	volatile uint32_t hall_dt_us;
	volatile uint32_t last_cnt;
}HallPeriodHnd_t;

// extern volatile uint32_t last_hall_tick;
// extern volatile uint32_t hall_dt_us;




void MeasHallPeriod(HallPeriodHnd_t* pxHallPeriod, uint8_t u, uint8_t v, uint8_t w);
float CalcHallSensor_RawRPM(uint32_t _dt_us);

uint32_t MovAvg_HallSensor_dt_us(uint32_t new_dt);
void MovAvg_HallSensor_Init();
#endif