#ifndef __MEAS_SPEED_H__
#define __MEAS_SPEED_H__

#include <stdint.h>
#include <string.h>
#include "IF_Hal.h"

#define BCM_MOTOR_POLE_PAIRS    (7)
#define SPD_TASK_INTEVAL_MS    (1)
#define HALL_TIMEOUT_FACTOR   (25)
#define HALL_DT_BUF (16)

typedef enum {
    SPEED_VALID,
    SPEED_TIMEOUT
} speed_state_t;



typedef struct {
	volatile uint8_t hall_state_prev;
	volatile uint32_t last_hall_tick;
	volatile uint32_t hall_dt_us;
	volatile uint32_t last_cnt;
}HallPeriodHnd_t;

typedef struct {
    u32 no_hall_cnt;
    speed_state_t speed_state;
    u32 avg_dt_us;

    float rpm_est;
    float rpm_raw;
    float g_fRpm_filt;
    float g_fRpm_obs;
}HallSpdMeas_t;



void MeasHallPeriod(HallPeriodHnd_t* pxHallPeriod, uint8_t u, uint8_t v, uint8_t w);
float CalcHallSensor_RawRPM(uint32_t _dt_us);

uint32_t MovAvg_HallSensor_dt_us(uint32_t new_dt);
void MovAvg_HallSensor_Init();


void SpeedCalculation(void* args);
void SpdCalc_init(HallSpdMeas_t* pxSpdMeas);


//float CalcHallSensor_RawRPM(uint32_t _dt_us);
float speed_observer_step(HallSpdMeas_t* pxSpdMeas);


#endif