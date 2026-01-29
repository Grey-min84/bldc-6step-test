#include "hall.h"

volatile uint8_t hall_state;
volatile uint8_t hall_state_prev;
volatile uint32_t last_hall_tick = 0;
volatile uint32_t hall_counter = 0;
volatile uint32_t hall_dt_us;
volatile uint32_t last_cnt;

void MeasHallPeriod(uint8_t u, uint8_t v, uint8_t w){

    hall_state = (u << 2) | (v << 1) | w;

    hall_counter++;

    if(hall_state_valid(hall_state) == 0){
        // invalid state, ignore
        return;
    }

    if (hall_state != hall_state_prev)
    {
        uint32_t now = TIM7->CNT;
        if (now >= last_cnt)
            hall_dt_us = now - last_cnt;
        else
            hall_dt_us = (65536 - last_cnt) + now;

        last_cnt = now;
        hall_state_prev = hall_state;
    }

     last_hall_tick = HAL_GetTick();
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





