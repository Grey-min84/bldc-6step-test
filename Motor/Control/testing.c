#include "testing.h"
#include "adcHnd.h"

#define THROTTLE_THRESHOLD	200

void ThrottleControl_Test(_6StepCtlCtx_t* px6Step){

    MotorRpmCtrl_t* pxSpdCtrl = px6Step->pxSpdCtl;
	static u8 ucPrevThrottleOn = 0;

	u16 throttleVal = (u16)GetFilteredAdcValue(eADC_IDX_THROTTLE);

	if(px6Step->ucIsThrottleOn != ucPrevThrottleOn){

		if(px6Step->ucIsThrottleOn != 0){
			px6Step->ucThrottleSts = eTHROTTLE_RDY;
		}
		else {
			px6Step->ucThrottleSts = eTHROTTLE_OFF;
		}
	}


	switch(px6Step->ucThrottleSts){

		case eTHROTTLE_RDY:
			if(throttleVal > THROTTLE_THRESHOLD){
				px6Step->ucCtlMode = eCTL_MODE_SPEED;
				px6Step->ucThrottleSts = eTHROTTLE_ON;

                pxSpdCtrl->m_ucIgnitePwr = THROTTLE_THRESHOLD;

				pxSpdCtrl->m_ucCtlState = eSPD_CTL_STATE_IGNITING;
			}
			break;

		case eTHROTTLE_ON:
			{
				throttleVal   = (throttleVal >> 2) << 2;


				s32 rpm = (throttleVal - THROTTLE_THRESHOLD) * (2000) / (4090 - THROTTLE_THRESHOLD);
				pxSpdCtrl->m_iTargtRpm = rpm;
			}
			break;

		case eTHROTTLE_UNDER_THR:
			if(throttleVal < THROTTLE_THRESHOLD){
				px6Step->ucThrottleSts = eTHROTTLE_RDY;
				pxSpdCtrl->m_iTargtRpm = 0;
			}
			break;

		case eTHROTTLE_OFF:
			pxSpdCtrl->m_ucCtlState = eSPD_CTL_STATE_IDLE;
			pxSpdCtrl->m_iTargtRpm = 0;
			px6Step->iSetDuty = 0;

			px6Step->ucThrottleSts = eTHROTTLE_DISABLE;
			break;

		case eTHROTTLE_DISABLE:
			// do nothing
			break;

		default:
			break;
	}



	ucPrevThrottleOn = px6Step->ucIsThrottleOn;


}








static uint8_t g_ucIsLogOn = 0;




void CliControl(cli_args_t *args, void* param){

	_6StepCtlCtx_t* px6Step = (_6StepCtlCtx_t*)param;
	MotorRpmCtrl_t* pxSpdCtrl = px6Step->pxSpdCtl;

	if(args->argc >= 1 ){

		if(args->isStr(0, "throttle") == 1){
			int throttleOn = 0;

			throttleOn = args->getData(1);

			if(throttleOn == 0){
				px6Step->ucIsThrottleOn = 0;
			}
			else {
                if(GetRawAdcValue(eADC_IDX_THROTTLE) > THROTTLE_THRESHOLD){
                    printf("Throttle Start value must be under 200  (val:%d)\r\n", GetRawAdcValue(eADC_IDX_THROTTLE));
                    return;
                }

				px6Step->ucIsThrottleOn = 1;

				px6Step->ucThrottleSts = eTHROTTLE_RDY;
				px6Step->ucCtlMode = eCTL_MODE_SPEED;
			}

			printf("Throttle control:%s\r\n", (px6Step->ucIsThrottleOn != 0) ? "on" : "off");
		}
		else

		if(args->isStr(0, "rpm") == 1){
			int rpm;
			u8 dir = 0;

			dir = args->getData(1);
			rpm = args->getData(2);

			px6Step->ucIsThrottleOn = 0;

			if(0 < rpm && rpm < 9000){

				pxSpdCtrl->m_iTargtRpm = rpm;
				pxSpdCtrl->m_ucCtlState = eSPD_CTL_STATE_IGNITING;
				pxSpdCtrl->m_ucDir = dir;	
				px6Step->ucCtlMode = eCTL_MODE_SPEED;
			}
			else if(rpm == 0){
				pxSpdCtrl->m_ucCtlState = eSPD_CTL_STATE_IDLE;
				pxSpdCtrl->m_iTargtRpm = 0;
				px6Step->iSetDuty = 0;
			}

			printf("Target RPM:%d\r\n", pxSpdCtrl->m_iTargtRpm);

		}
		else if(args->isStr(0, "ignite_pwr") == 1){
			int ignitePwr = 0;
			ignitePwr = args->getData(1);

			if(0 < ignitePwr && ignitePwr < px6Step->uiMaxPeriodCnt * 25 / 100){
				pxSpdCtrl->m_ucIgnitePwr = ignitePwr;
			}

			printf("ignite power:%d\r\n", pxSpdCtrl->m_ucIgnitePwr);
		}
		else if(args->isStr(0, "duty") == 1){
			int duty;

			duty = args->getData(1);

			pxSpdCtrl->m_ucCtlState = eSPD_CTL_STATE_IDLE;

			if(0 <= duty && duty < px6Step->uiMaxPeriodCnt * 75 / 100){
				px6Step->iSetDuty = duty;
				px6Step->ucCtlMode = eCTL_MODE_DUTY;
			}

			printf("Set Duty:%d\r\n", px6Step->iSetDuty);
		}
		else if(args->isStr(0, "log") == 1){

			if(args->isStr(1, "on") == 1){
				g_ucIsLogOn = 1;
			}
			else if(args->isStr(1, "off") == 1){
				g_ucIsLogOn = 0;
			}

			printf("Data log:%s\r\n", (g_ucIsLogOn != 0) ? "on" : "off");
		}
		else {
			printf("\r\n");
			printf("rpm <dir> <rpm>         : set target rpm\r\n");
			printf("duty <duty>             : set duty (0~max 75%%)\r\n");
			printf("ignite_pwr <power>      : set ignite power (0~max25%%)\r\n");	
			printf("log <on/off>            : data log on/off\r\n");
			printf("throttle <0/1>         : throttle control off/on\r\n");
		}
	}

}







void DataLoggingManage(CountingTick_t* pxTick, _6StepCtlCtx_t* px6Step, uint8_t ucStopToken){
	
	uint8_t strBuf[64] = {0,};

	MotorRpmCtrl_t* pxSpdCtrl = px6Step->pxSpdCtl;
	HallSpdMeas_t* pxHallSpdMeas = &px6Step->xHallSpdMeas;

	static u32 plot_time = 0;
	
	if(g_ucIsLogOn != 0){

		if(ucStopToken == 'z'){
			g_ucIsLogOn = 0;
			return;
		}

		if(pxTick->uiLog >= 50){
			pxTick->uiLog = 0;

			printf("pidOut:%.1f, pidErr:%.1f, rpm_filt:%.1f, rpm_obs:%.1f, dt:%d\r\n",
				pxSpdCtrl->fPidOut,
				pxSpdCtrl->fPid_ErrInput,
				pxHallSpdMeas->g_fRpm_filt,
				pxHallSpdMeas->g_fRpm_obs,
				pxHallSpdMeas->avg_dt_us);

		}
		
	}

	if(pxTick->uiAlwaysLog >= 20){
		pxTick->uiAlwaysLog = 0;

		plot_time = plot_time + 20;

		// always log
		sprintf((char*)strBuf, "%d\t%d\t%d\t%.1f\t%d\n", 
					plot_time,
					pxSpdCtrl->m_iTargtRpm,
					pxSpdCtrl->m_iCurrRpm,
					pxHallSpdMeas->g_fRpm_filt,
					pxHallSpdMeas->avg_dt_us
		);

		HAL_UART_Transmit(&huart3, strBuf, strlen((char*)strBuf), HAL_MAX_DELAY);
	}
}
