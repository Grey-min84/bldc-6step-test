#include "measSpd.h"
#include "hall.h"

float g_fRpm_filt;
float g_fRpm_obs;
//extern MotorRpmCtrl_t g_xBrkMotorRpmCtl;


float rpm_est = 0.0f;





////////
const float alpha = 0.15f;
uint32_t dt_local = 0;
float rpm_raw = 0.0f;


uint32_t no_hall_cnt = 0;


//static uint32_t last_tick_snapshot;

uint32_t time_since_hall;
uint32_t avg_dt_us = 0;
float fRpmValid = 0;


speed_state_t speed_state = SPEED_TIMEOUT;




void SpdCalcTimer(){

    uint32_t now = HAL_GetTick();

    uint32_t time_since_hall = now - last_hall_tick;

    dt_local = hall_dt_us;   // atomic read (32bit OK)

    if (dt_local > 0)
        avg_dt_us = MovAvg_HallSensor_dt_us(dt_local);

#if 0
    if(g_xBrkMotorRpmCtl.m_ucDirChngFlag != 0){

        g_xBrkMotorRpmCtl.m_ucDirChngFlag = 0;
        g_xBrkMotorRpmCtl.m_ucRpmMeasCnt = 0;
        MovAvg_HallSensor_Init();
        speed_state = SPEED_TIMEOUT;
        rpm_est = 0.0f;
    }
#endif

    switch (speed_state) {
        case SPEED_VALID:
            if (time_since_hall > HALL_TIMEOUT_FACTOR)
                speed_state = SPEED_TIMEOUT;

            if (dt_local > 0 ) {

            #if 0
                g_xBrkMotorRpmCtl.m_ucRpmMeasCnt++;

                if(g_xBrkMotorRpmCtl.m_ucRpmMeasCnt > HALL_DT_BUF*4){
                    
                    g_xBrkMotorRpmCtl.m_ucRpmMeasCnt = HALL_DT_BUF*4;
                    g_xBrkMotorRpmCtl.m_ucIgnited = 1;
                }
            #endif

                if(avg_dt_us != 0)
                    rpm_raw =  CalcHallSensor_RawRPM(avg_dt_us);
                else 
                    rpm_raw = 0;

                g_fRpm_filt = g_fRpm_filt + alpha * (rpm_raw - g_fRpm_filt);
            }
            else {
                //g_xBrkMotorRpmCtl.m_ucRpmMeasCnt = 0;
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
        

#if 0
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
#endif
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


