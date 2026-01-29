#ifndef __MEAS_SPEED_H__
#define __MEAS_SPEED_H__

#include <stdint.h>
// #ifndef __MEAS_HALL_RPM_H__
// #define __MEAS_HALL_RPM_H__
// #include "misc.h"

// #define BCM_MOTOR_POLE_PAIRS    (7)

typedef enum {
    SPEED_VALID,
    SPEED_TIMEOUT
} speed_state_t;




//float CalcHallSensor_RawRPM(uint32_t _dt_us);
float speed_observer_step(speed_state_t state, uint32_t hall_dt_avg);


#if 0
// c file
#include "MeasHallRPM.h"
#include "TaskBrakeModule.h"
#include <math.h>
#include <string.h>

#define SPD_TASK_INTEVAL_MS    (1)
#define HALL_TIMEOUT_FACTOR   (25)
#define HALL_DT_BUF (16)

volatile uint32_t hall_dt_us;
volatile uint32_t last_cnt;

volatile uint8_t hall_state;
volatile uint8_t hall_state_prev;
volatile uint32_t last_hall_tick = 0;
volatile uint32_t hall_counter = 0;

extern float g_fRpm_filt;
extern float g_fRpm_obs;
extern MotorRpmCtrl_t g_xBrkMotorRpmCtl;


float rpm_est = 0.0f;



static inline uint8_t hall_state_valid(uint8_t s);


void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin){

    uint8_t u = HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_3);
    uint8_t v = HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_4);
    uint8_t w = HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_5);


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







void SpeedTask(void *arg)
{
    const float alpha = 0.15f;
    uint32_t dt_local = 0;
    float rpm_raw = 0.0f;
    

    uint32_t no_hall_cnt = 0;


    //static uint32_t last_tick_snapshot;
    uint32_t now;
    uint32_t time_since_hall;
    uint32_t avg_dt_us = 0;
    float fRpmValid = 0;


    speed_state_t speed_state = SPEED_TIMEOUT;
    MovAvg_HallSensor_Init();


    for (;;) {

        now = HAL_GetTick();

        time_since_hall = now - last_hall_tick;

        dt_local = hall_dt_us;   // atomic read (32bit OK)

        if (dt_local > 0)
            avg_dt_us = MovAvg_HallSensor_dt_us(dt_local);

        if(g_xBrkMotorRpmCtl.m_ucDirChngFlag != 0){

            g_xBrkMotorRpmCtl.m_ucDirChngFlag = 0;
            g_xBrkMotorRpmCtl.m_ucRpmMeasCnt = 0;
            MovAvg_HallSensor_Init();
            speed_state = SPEED_TIMEOUT;
            rpm_est = 0.0f;
        }


        switch (speed_state) {
            case SPEED_VALID:
                if (time_since_hall > HALL_TIMEOUT_FACTOR)
                    speed_state = SPEED_TIMEOUT;

                if (dt_local > 0 ) {

                    g_xBrkMotorRpmCtl.m_ucRpmMeasCnt++;

                    if(g_xBrkMotorRpmCtl.m_ucRpmMeasCnt > HALL_DT_BUF*4){
                        
                        g_xBrkMotorRpmCtl.m_ucRpmMeasCnt = HALL_DT_BUF*4;
                        g_xBrkMotorRpmCtl.m_ucIgnited = 1;
                    }

                    if(avg_dt_us != 0)
                        rpm_raw =  CalcHallSensor_RawRPM(avg_dt_us);
                    else 
                        rpm_raw = 0;

                    g_fRpm_filt = g_fRpm_filt + alpha * (rpm_raw - g_fRpm_filt);
                }
                else {
                    g_xBrkMotorRpmCtl.m_ucRpmMeasCnt = 0;
                }
                break;

            case SPEED_TIMEOUT:
                if (time_since_hall < HALL_TIMEOUT_FACTOR){

                    no_hall_cnt = 0;
                    speed_state = SPEED_VALID;
                }
                    
                no_hall_cnt += 1;

                if (no_hall_cnt > 100) {
                    g_fRpm_filt *= 0.9f; // 서서히 0으로
                }

                if(g_fRpm_filt < 0.5f){
                    g_fRpm_filt = 0.0f;
                }
                break;
        }

        if (dt_local > 0){

            g_fRpm_obs = speed_observer_step(speed_state, avg_dt_us);  
        }
        	


        if(g_xBrkMotorRpmCtl.m_ucIgnited == 1){
            float diff = fabs(fRpmValid - g_fRpm_obs);

            float chkValidRate = diff / fRpmValid;

            if(chkValidRate < 0.5f){
                fRpmValid = g_fRpm_obs;
            }
        }
        else {
            fRpmValid = g_fRpm_obs;
        }
        

        g_xBrkMotorRpmCtl.m_fCurrRpm = g_fRpm_obs;

        vTaskDelay(pdMS_TO_TICKS(1)); // 2ms
    }
}







// 간단한 1차 관측기
float speed_observer_step(speed_state_t state, uint32_t hall_dt_avg)
{
    
    static float K_obs = 0.1f;

    if (state == SPEED_VALID)
    {
        float rpm_raw = 0.0f;

        if(hall_dt_avg != 0)
            rpm_raw =  CalcHallSensor_RawRPM(hall_dt_avg);        
        else 
            rpm_raw =  0;  
        

        rpm_est = (K_obs * (rpm_raw - rpm_est)) + rpm_est;
    }
    else
    {
        // Hall 미발생 시 모델 유지
        rpm_est *= 0.98f;   // 매우 약한 감쇠
        if (rpm_est < 0.5f)
            rpm_est = 0.0f;
    }


    // 기준 RPM은 어떻게 구해야 하는가....
    if (rpm_est < 200)
        K_obs = 0.1f;
    else if (rpm_est < 800)
        K_obs = 0.25f;
    else
        K_obs = 0.5f;

    return rpm_est;
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






#endif

#endif