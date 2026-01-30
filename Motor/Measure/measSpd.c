#include <math.h>

#include "measSpd.h"
#include "hall.h"
#include "six_step.h"



const float alpha = 0.15f;

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

                if(pxSpdCtl->m_ucRpmMeasCnt > HALL_DT_BUF*4){
                    
                    pxSpdCtl->m_ucRpmMeasCnt = HALL_DT_BUF*4;
                    pxSpdCtl->m_ucIgnited = 1;
                }


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
            }
            break;
    }

    if (dt_local > 0){

        pxHallSpdMeas->g_fRpm_obs = speed_observer_step(pxHallSpdMeas);  
    }
        

#if 1
    if(pxSpdCtl->m_ucIgnited == 1){
        float diff = fabs(pxSpdCtl->m_iCurrRpm - pxHallSpdMeas->g_fRpm_obs);

        float chkValidRate = diff / pxSpdCtl->m_iCurrRpm;

        if(chkValidRate < 0.5f){
            pxSpdCtl->m_iCurrRpm = pxHallSpdMeas->g_fRpm_obs;
        }
    }
    else {
        pxSpdCtl->m_iCurrRpm = pxHallSpdMeas->g_fRpm_obs;
    }
    

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


