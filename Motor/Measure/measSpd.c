#include <math.h>

#include "measSpd.h"
#include "six_step.h"

#define PERIOD_CNT (TIM7_PERIOD_CNT+1)
#define COUNTER_SRC  (TIM7->CNT)

const float alpha = 0.15f;

static inline uint8_t hall_state_valid(uint8_t s);

HallSpdMeas_t g_xHallSpdMeas = {
    .no_hall_cnt = 0,
    .speed_state = SPEED_TIMEOUT,
    .avg_dt_us = 0,
    .rpm_est = 0.0f,
    .rpm_raw = 0.0f,
    .g_fRpm_filt = 0.0f,
    .g_fRpm_obs = 0.0f
};



void SpdCalc_init(HallSpdMeas_t* pxSpdMeas){

    memset(pxSpdMeas, 0, sizeof(HallSpdMeas_t));
    pxSpdMeas->speed_state = SPEED_TIMEOUT;

    MovAvg_HallSensor_Init();
}







void MeasHallPeriod(HallPeriodHnd_t* pxHallPeriod, uint8_t u, uint8_t v, uint8_t w){

    volatile uint8_t hall_state = (u << 2) | (v << 1) | w;


    if(hall_state_valid(hall_state) == 0){
        // invalid state, ignore
        return;
    }

    if (hall_state != pxHallPeriod->hall_state_prev)
    {

        // counting 타이머의 클럭은 1MHz이므로 카운트 하나는 1us
        uint32_t now = COUNTER_SRC;

        if (now >= pxHallPeriod->last_cnt){
            
            pxHallPeriod->hall_dt_us = now - pxHallPeriod->last_cnt;
        }
        else {
            // overflow 처리
            // 카운팅 타이머의 주기는 PERIOD_CNT(=65536)
            pxHallPeriod->hall_dt_us = (PERIOD_CNT - pxHallPeriod->last_cnt) + now;
        }
            

        pxHallPeriod->last_cnt = now;
        pxHallPeriod->hall_state_prev = hall_state;
    }

     pxHallPeriod->last_hall_tick = HAL_GetTick();
}






float CalcHallSensor_RawRPM(uint32_t _dt_us){

    //1e6f = 1 * 10^6   -> 1 Mhz Timer frequency

    return 60.0f * 1e6f / (float)((float)_dt_us * 6.0f * (float)BCM_MOTOR_POLE_PAIRS);

}






void SpeedCalculation(void* args){

    _6StepCtlCtx_t* px6stpCtx = (_6StepCtlCtx_t*)args;

    HallSpdMeas_t* pxHallSpdMeas        = &px6stpCtx->xHallSpdMeas;
    HallPeriodHnd_t* pxHallPeriodHnd    = &px6stpCtx->xHallPeriodCalc;
    MotorRpmCtrl_t* pxSpdCtl            = px6stpCtx->pxSpdCtl;

    uint32_t now = HAL_GetTick();
    uint32_t dt_local = 0;

    uint32_t time_since_hall = now - pxHallPeriodHnd->last_hall_tick;

    dt_local = pxHallPeriodHnd->hall_dt_us;   // atomic read (32bit OK)

    if (dt_local > 0)
        pxHallSpdMeas->avg_dt_us = MovAvg_HallSensor_dt_us(dt_local);


    if(pxSpdCtl->m_ucDirChngFlag != 0){

        pxSpdCtl->m_ucDirChngFlag = 0;
        pxSpdCtl->m_ucRpmMeasCnt = 0;
        MovAvg_HallSensor_Init();
        pxHallSpdMeas->speed_state = SPEED_TIMEOUT;
        pxHallSpdMeas->rpm_est = 0.0f;
    }


    switch (pxHallSpdMeas->speed_state) {

        case SPEED_VALID:
            if (time_since_hall > HALL_TIMEOUT_FACTOR)
                pxHallSpdMeas->speed_state = SPEED_TIMEOUT;

            if (dt_local > 0 ) {

                pxSpdCtl->m_ucRpmMeasCnt++;

                // if(pxSpdCtl->m_ucRpmMeasCnt > HALL_DT_BUF*4){
                    
                //     pxSpdCtl->m_ucRpmMeasCnt = HALL_DT_BUF*4;
                //     pxSpdCtl->m_ucIgnited = 1;
                // }


                if(pxHallSpdMeas->avg_dt_us != 0)
                    pxHallSpdMeas->rpm_raw =  CalcHallSensor_RawRPM(pxHallSpdMeas->avg_dt_us);
                else 
                    pxHallSpdMeas->rpm_raw = 0;

                pxHallSpdMeas->g_fRpm_filt = pxHallSpdMeas->g_fRpm_filt + alpha * (pxHallSpdMeas->rpm_raw - pxHallSpdMeas->g_fRpm_filt);
            }
            else {

                pxSpdCtl->m_ucRpmMeasCnt = 0;
            }
            break;

        case SPEED_TIMEOUT:
            if (time_since_hall < HALL_TIMEOUT_FACTOR){

                pxHallSpdMeas->no_hall_cnt = 0;
                pxHallSpdMeas->speed_state = SPEED_VALID;
            }
                
            pxHallSpdMeas->no_hall_cnt += 1;

            if (pxHallSpdMeas->no_hall_cnt > 100) {
                pxHallSpdMeas->g_fRpm_filt *= 0.9f; // 서서히 0으로
            }

            if(pxHallSpdMeas->g_fRpm_filt < 0.5f){
                pxHallSpdMeas->g_fRpm_filt = 0.0f;
                pxSpdCtl->m_iCurrRpm = 0;
            }
            break;
    }

    if (dt_local > 0 && pxHallSpdMeas->g_fRpm_filt > 0.5f){

        pxHallSpdMeas->g_fRpm_obs = speed_observer_step(pxHallSpdMeas);  
    }
        

#if 1
    // if(pxSpdCtl->m_ucIgnited == 1){
    //     float diff = fabs(pxSpdCtl->m_iCurrRpm - pxHallSpdMeas->g_fRpm_obs);

    //     float chkValidRate = diff / pxSpdCtl->m_iCurrRpm;

    //     if(chkValidRate < 0.5f){
    //         pxSpdCtl->m_iCurrRpm = pxHallSpdMeas->g_fRpm_obs;
    //     }
    // }
    // else {
    //     pxSpdCtl->m_iCurrRpm = pxHallSpdMeas->g_fRpm_obs;
    // }
    

    pxSpdCtl->m_iCurrRpm = pxHallSpdMeas->g_fRpm_obs;
#endif
}



// 간단한 1차 관측기
float speed_observer_step(HallSpdMeas_t* pxSpdMeas)
{
    
    static float K_obs = 0.1f;

    if (pxSpdMeas->speed_state == SPEED_VALID)
    {
        float rpm_raw = 0.0f;

        if(pxSpdMeas->avg_dt_us != 0)
            rpm_raw =  CalcHallSensor_RawRPM(pxSpdMeas->avg_dt_us);        
        else 
            rpm_raw =  0;  
        

        pxSpdMeas->rpm_est = (K_obs * (rpm_raw - pxSpdMeas->rpm_est)) + pxSpdMeas->rpm_est;
    }
    else
    {
        // Hall 미발생 시 모델 유지
        pxSpdMeas->rpm_est *= 0.98f;   // 매우 약한 감쇠
        if (pxSpdMeas->rpm_est < 0.5f)
            pxSpdMeas->rpm_est = 0.0f;
    }


    // 기준 RPM은 어떻게 구해야 하는가....
    if (pxSpdMeas->rpm_est < 200)
        K_obs = 0.1f;
    else if (pxSpdMeas->rpm_est < 800)
        K_obs = 0.25f;
    else
        K_obs = 0.5f;

    return pxSpdMeas->rpm_est;
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

