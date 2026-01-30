#include <string.h>

#include "hall.h"


// volatile uint8_t hall_state_prev;
// volatile uint32_t last_hall_tick = 0;
// volatile uint32_t hall_dt_us;
// volatile uint32_t last_cnt;


// HallPeriodHnd_t g_xHallPeriodHnd = {
//     .hall_state_prev = 0,
//     .last_hall_tick = 0,
//     .hall_dt_us = 0,
//     .last_cnt = 0
// };
static inline uint8_t hall_state_valid(uint8_t s);


void MeasHallPeriod(HallPeriodHnd_t* pxHallPeriod, uint8_t u, uint8_t v, uint8_t w){

    volatile uint8_t hall_state = (u << 2) | (v << 1) | w;


    if(hall_state_valid(hall_state) == 0){
        // invalid state, ignore
        return;
    }

    if (hall_state != pxHallPeriod->hall_state_prev)
    {
        uint32_t now = TIM7->CNT;

        if (now >= pxHallPeriod->last_cnt)
            pxHallPeriod->hall_dt_us = now - pxHallPeriod->last_cnt;
        else
            pxHallPeriod->hall_dt_us = (65536 - pxHallPeriod->last_cnt) + now;

        pxHallPeriod->last_cnt = now;
        pxHallPeriod->hall_state_prev = hall_state;
    }

     pxHallPeriod->last_hall_tick = HAL_GetTick();
}






float CalcHallSensor_RawRPM(uint32_t _dt_us){

    //1e6f = 1 * 10^6   -> 1 Mhz Timer frequency

    return 60.0f * 1e6f / (float)((float)_dt_us * 6.0f * (float)BCM_MOTOR_POLE_PAIRS);

}




uint32_t dt_buf[HALL_DT_BUF];
uint8_t dt_idx;
uint64_t dt_sum;

uint32_t MovAvg_HallSensor_dt_us(uint32_t dt){
    dt_sum -= dt_buf[dt_idx];
    dt_buf[dt_idx] = dt;
    dt_sum += dt;

    dt_idx = (dt_idx + 1) % HALL_DT_BUF;

    return (uint32_t)(dt_sum / HALL_DT_BUF);
}


void MovAvg_HallSensor_Init(){
    memset(dt_buf, 0, HALL_DT_BUF*sizeof(uint32_t));
    dt_idx = 0;
    dt_sum = 0;
}






static inline uint8_t hall_state_valid(uint8_t s)
{
    switch (s)
    {
        case 0b001:
        case 0b011:
        case 0b010:
        case 0b110:
        case 0b100:
        case 0b101:
            return 1;
        default:
            return 0;
    }
}
