#include "L6398.h"
#include "boardNuclG431.h"

IPwm_t g_xPwm_phaseU_highside;
IPwm_t g_xPwm_phaseV_highside;
IPwm_t g_xPwm_phaseW_highside;

IGpio_t g_xGpo_phaseU_lowside;
IGpio_t g_xGpo_phaseV_lowside;
IGpio_t g_xGpo_phaseW_lowside;

static inline uint8_t Hall_MapPrevOfForward(uint8_t hall);

void InitL6398_Unipolar(DrvPwm_Unipolar_t* pxDrive, fpPeriodCb fpCb, void* _args){

    PlatformConfig_6stepUniPolar(&g_xPwm_phaseU_highside, &g_xPwm_phaseV_highside, &g_xPwm_phaseW_highside,
		&g_xGpo_phaseU_lowside, &g_xGpo_phaseV_lowside, &g_xGpo_phaseW_lowside);



	pxDrive->pxPwmU_highSide = &g_xPwm_phaseU_highside;
	pxDrive->pxPinU_lowSide = &g_xGpo_phaseU_lowside;

	pxDrive->pxPwmV_highSide = &g_xPwm_phaseV_highside;
	pxDrive->pxPinV_lowSide = &g_xGpo_phaseV_lowside;

	pxDrive->pxPwmW_highSide = &g_xPwm_phaseW_highside;
	pxDrive->pxPinW_lowSide = &g_xGpo_phaseW_lowside;

    Pwm1_AddCallbackPeriodDone(pxDrive->pxPwmU_highSide, fpCb, _args);
    // Pwm1_AddCallbackPeriodDone(pxDrive->pxPwmV_highSide, fpCb, _args);
    // Pwm1_AddCallbackPeriodDone(pxDrive->pxPwmW_highSide, fpCb, _args);

// 	g_xDriverUniPolar.pxPwmU_highSide = &g_xPwm_phaseU_highside;
// 	g_xDriverUniPolar.pxPinU_lowSide = &g_xGpo_phaseU_lowside;

// 	g_xDriverUniPolar.pxPwmV_highSide = &g_xPwm_phaseV_highside;
// 	g_xDriverUniPolar.pxPinV_lowSide = &g_xGpo_phaseV_lowside;

// 	g_xDriverUniPolar.pxPwmW_highSide = &g_xPwm_phaseW_highside;
// 	g_xDriverUniPolar.pxPinW_lowSide = &g_xGpo_phaseW_lowside;
}




void Apply_L6398_CommutationUnipolar(DrvPwm_Unipolar_t* pvDriver, uint8_t state, u32 pwmVal, u8 dir) {

    u8 hallState = state;

    if (dir != 0) {

        // 핵심: REV일 때는 "정방향 이전 홀"로 치환해서
        // 기존 정방향 스위치를 그대로 태운다.
        hallState = Hall_MapPrevOfForward(state);
    }
    else {
        hallState = state;
    }

	switch (hallState)
	{
		case 4:  // Hall: 001 -> B-PWM, C-Low
			//DrvL6398_6Step_UniPolar_GateCtl(DrvPwm_Unipolar_t* pxDrv, u8 phase, u8 ctl, float duty)
			DrvL6398_6Step_UniPolar_GateCtl(pvDriver, POLE_U, _6STEP_PWM_IN, pwmVal);
			DrvL6398_6Step_UniPolar_GateCtl(pvDriver, POLE_V, _6STEP_HiZ, 0);
			DrvL6398_6Step_UniPolar_GateCtl(pvDriver, POLE_W, _6STEP_LOWSIDE_ON, 0);
			break;

		case 2:  // Hall: 010 -> A-PWM, C-Low  -----
            DrvL6398_6Step_UniPolar_GateCtl(pvDriver, POLE_U, _6STEP_LOWSIDE_ON, 0);  // A: Low
			DrvL6398_6Step_UniPolar_GateCtl(pvDriver, POLE_V, _6STEP_PWM_IN, pwmVal); // B: PWM
			DrvL6398_6Step_UniPolar_GateCtl(pvDriver, POLE_W, _6STEP_HiZ, 0);  		// C: Off
			break;

		case 6:  // Hall: 011 -> A-PWM, B-Low
			DrvL6398_6Step_UniPolar_GateCtl(pvDriver, POLE_U, _6STEP_HiZ, 0);  		// A: Off
			DrvL6398_6Step_UniPolar_GateCtl(pvDriver, POLE_V, _6STEP_PWM_IN, pwmVal); // B: PWM
			DrvL6398_6Step_UniPolar_GateCtl(pvDriver, POLE_W, _6STEP_LOWSIDE_ON, 0);  // C: Low
			break;

		case 1:  // Hall: 100 -> C-PWM, A-Low
			DrvL6398_6Step_UniPolar_GateCtl(pvDriver, POLE_U, _6STEP_HiZ, 0);  		// A: Off
			DrvL6398_6Step_UniPolar_GateCtl(pvDriver, POLE_V, _6STEP_LOWSIDE_ON, 0);  // B: Low
			DrvL6398_6Step_UniPolar_GateCtl(pvDriver, POLE_W, _6STEP_PWM_IN, pwmVal); // C: PWM
			break;

		case 5:  // Hall: 101 -> C-PWM, B-Low
			DrvL6398_6Step_UniPolar_GateCtl(pvDriver, POLE_U, _6STEP_PWM_IN, pwmVal); // A: PWM
			DrvL6398_6Step_UniPolar_GateCtl(pvDriver, POLE_V, _6STEP_LOWSIDE_ON, 0);  // B: Low
			DrvL6398_6Step_UniPolar_GateCtl(pvDriver, POLE_W, _6STEP_HiZ, 0);  		// C: Off
			break;

		case 3:  // Hall: 110 -> B-PWM, A-Low
			DrvL6398_6Step_UniPolar_GateCtl(pvDriver, POLE_U, _6STEP_LOWSIDE_ON, 0);  // A: Low
			DrvL6398_6Step_UniPolar_GateCtl(pvDriver, POLE_V, _6STEP_HiZ, 0);  		// B: Off
			DrvL6398_6Step_UniPolar_GateCtl(pvDriver, POLE_W, _6STEP_PWM_IN, pwmVal); // C: PWM
			break;

        case 7:  // align
			DrvL6398_6Step_UniPolar_GateCtl(pvDriver,POLE_U, _6STEP_PWM_IN, pwmVal);  
			DrvL6398_6Step_UniPolar_GateCtl(pvDriver,POLE_V, _6STEP_LOWSIDE_ON, 0);  	
			DrvL6398_6Step_UniPolar_GateCtl(pvDriver,POLE_W, _6STEP_LOWSIDE_ON, 0);  	
			break;

		default:  // Invalid states (0, 7)
			DrvL6398_6Step_UniPolar_GateCtl(pvDriver, POLE_U, _6STEP_HiZ, 0);  // A: Off
			DrvL6398_6Step_UniPolar_GateCtl(pvDriver, POLE_V, _6STEP_HiZ, 0);  // B: Off
			DrvL6398_6Step_UniPolar_GateCtl(pvDriver, POLE_W, _6STEP_HiZ, 0);  // C: Off
			break;
	}
}




static inline uint8_t Hall_MapPrevOfForward(uint8_t hall)
{
    switch (hall)
    {
        case 1: return 6;
        case 3: return 4;
        case 2: return 5;
        case 6: return 1;
        case 4: return 3;
        case 5: return 2;
        default: return hall; // 0,7 등은 그대로
    }
}


void DrvL6398_6Step_UniPolar_GateCtl(DrvPwm_Unipolar_t* pxDrv, u8 phase, u8 ctl, uint32_t duty){


    switch (phase)
    {
        case POLE_U:  // Phase A
            switch(ctl){
                case _6STEP_HiZ:
                    Pwm1_generate(pxDrv->pxPwmU_highSide, 0);
                    WriteGpio(pxDrv->pxPinU_lowSide, MONO_PIN_HIGH);
                    break;

                case _6STEP_PWM_IN:
                    Pwm1_generate(pxDrv->pxPwmU_highSide, duty);
                    WriteGpio(pxDrv->pxPinU_lowSide, MONO_PIN_HIGH);
                    break;

                case _6STEP_LOWSIDE_ON:
                    Pwm1_generate(pxDrv->pxPwmU_highSide, 0);
                    WriteGpio(pxDrv->pxPinU_lowSide, MONO_PIN_LOW);
                    break;
            }
            break;

        case POLE_V:  // Phase B
            switch(ctl){
                case _6STEP_HiZ:
                    Pwm1_generate(pxDrv->pxPwmV_highSide, 0);
                    WriteGpio(pxDrv->pxPinV_lowSide, MONO_PIN_HIGH);
                    break;

                case _6STEP_PWM_IN:
                    Pwm1_generate(pxDrv->pxPwmV_highSide, duty);
                    WriteGpio(pxDrv->pxPinV_lowSide, MONO_PIN_HIGH);
                    break;

                case _6STEP_LOWSIDE_ON:
                    Pwm1_generate(pxDrv->pxPwmV_highSide, 0);
                    WriteGpio(pxDrv->pxPinV_lowSide, MONO_PIN_LOW);
                    break;
            }
            break;

        case POLE_W:  // Phase C
            switch(ctl){
                case _6STEP_HiZ:
                    Pwm1_generate(pxDrv->pxPwmW_highSide, 0);
                    WriteGpio(pxDrv->pxPinW_lowSide, MONO_PIN_HIGH);
                    break;

                case _6STEP_PWM_IN:
                    Pwm1_generate(pxDrv->pxPwmW_highSide, duty);
                    WriteGpio(pxDrv->pxPinW_lowSide, MONO_PIN_HIGH);
                    break;

                case _6STEP_LOWSIDE_ON:
                    Pwm1_generate(pxDrv->pxPwmW_highSide, 0);
                    WriteGpio(pxDrv->pxPinW_lowSide, MONO_PIN_LOW);
                    break;
            }
            break;
    }



    

}

