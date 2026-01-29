#include "L6398.h"
#include "boardNuclG431.h"

IPwm_t g_xPwm_phaseU_highside;
IPwm_t g_xPwm_phaseV_highside;
IPwm_t g_xPwm_phaseW_highside;

IGpio_t g_xGpo_phaseU_lowside;
IGpio_t g_xGpo_phaseV_lowside;
IGpio_t g_xGpo_phaseW_lowside;

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




void Apply_L6398_CommutationUnipolar(DrvPwm_Unipolar_t* pvDriver, uint8_t state, float pwmVal)
{

	DrvPwm_Unipolar_t* pxDriver = pvDriver;

	switch (state)
	{
		case 4:  // Hall: 001 -> B-PWM, C-Low
			//DrvL6398_6Step_UniPolar_GateCtl(DrvPwm_Unipolar_t* pxDrv, u8 phase, u8 ctl, float duty)
			DrvL6398_6Step_UniPolar_GateCtl(pxDriver, POLE_U, _6STEP_PWM_IN, (float)pwmVal);
			DrvL6398_6Step_UniPolar_GateCtl(pxDriver, POLE_V, _6STEP_HiZ, (float)0);
			DrvL6398_6Step_UniPolar_GateCtl(pxDriver, POLE_W, _6STEP_LOWSIDE_ON, (float)0);
			break;

		case 2:  // Hall: 010 -> A-PWM, C-Low  -----
            DrvL6398_6Step_UniPolar_GateCtl(pxDriver, POLE_U, _6STEP_LOWSIDE_ON, 0);  // A: Low
			DrvL6398_6Step_UniPolar_GateCtl(pxDriver, POLE_V, _6STEP_PWM_IN, pwmVal); // B: PWM
			DrvL6398_6Step_UniPolar_GateCtl(pxDriver, POLE_W, _6STEP_HiZ, 0);  		// C: Off
			break;

		case 6:  // Hall: 011 -> A-PWM, B-Low
			DrvL6398_6Step_UniPolar_GateCtl(pxDriver, POLE_U, _6STEP_HiZ, 0);  		// A: Off
			DrvL6398_6Step_UniPolar_GateCtl(pxDriver, POLE_V, _6STEP_PWM_IN, pwmVal); // B: PWM
			DrvL6398_6Step_UniPolar_GateCtl(pxDriver, POLE_W, _6STEP_LOWSIDE_ON, 0);  // C: Low
			break;

		case 1:  // Hall: 100 -> C-PWM, A-Low
			DrvL6398_6Step_UniPolar_GateCtl(pxDriver, POLE_U, _6STEP_HiZ, 0);  		// A: Off
			DrvL6398_6Step_UniPolar_GateCtl(pxDriver, POLE_V, _6STEP_LOWSIDE_ON, 0);  // B: Low
			DrvL6398_6Step_UniPolar_GateCtl(pxDriver, POLE_W, _6STEP_PWM_IN, pwmVal); // C: PWM
			break;

		case 5:  // Hall: 101 -> C-PWM, B-Low
			DrvL6398_6Step_UniPolar_GateCtl(pxDriver, POLE_U, _6STEP_PWM_IN, pwmVal); // A: PWM
			DrvL6398_6Step_UniPolar_GateCtl(pxDriver, POLE_V, _6STEP_LOWSIDE_ON, 0);  // B: Low
			DrvL6398_6Step_UniPolar_GateCtl(pxDriver, POLE_W, _6STEP_HiZ, 0);  		// C: Off
			break;

		case 3:  // Hall: 110 -> B-PWM, A-Low
			DrvL6398_6Step_UniPolar_GateCtl(pxDriver, POLE_U, _6STEP_LOWSIDE_ON, 0);  // A: Low
			DrvL6398_6Step_UniPolar_GateCtl(pxDriver, POLE_V, _6STEP_HiZ, 0);  		// B: Off
			DrvL6398_6Step_UniPolar_GateCtl(pxDriver, POLE_W, _6STEP_PWM_IN, pwmVal); // C: PWM
			break;

        case 7:  // align
			DrvL6398_6Step_UniPolar_GateCtl(pxDriver,POLE_U, _6STEP_PWM_IN, pwmVal);  
			DrvL6398_6Step_UniPolar_GateCtl(pxDriver,POLE_V, _6STEP_LOWSIDE_ON, 0);  	
			DrvL6398_6Step_UniPolar_GateCtl(pxDriver,POLE_W, _6STEP_LOWSIDE_ON, 0);  	
			break;

		default:  // Invalid states (0, 7)
			DrvL6398_6Step_UniPolar_GateCtl(pxDriver, POLE_U, _6STEP_HiZ, 0);  // A: Off
			DrvL6398_6Step_UniPolar_GateCtl(pxDriver, POLE_V, _6STEP_HiZ, 0);  // B: Off
			DrvL6398_6Step_UniPolar_GateCtl(pxDriver, POLE_W, _6STEP_HiZ, 0);  // C: Off
			break;
	}
}





void DrvL6398_6Step_UniPolar_GateCtl(DrvPwm_Unipolar_t* pxDrv, u8 phase, u8 ctl, float duty){


    switch (phase)
    {
        case POLE_U:  // Phase A
            switch(ctl){
                case _6STEP_HiZ:
                    Pwm1_Generate(pxDrv->pxPwmU_highSide, 0.0f);
                    WriteGpio(pxDrv->pxPinU_lowSide, MONO_PIN_HIGH);
                    break;

                case _6STEP_PWM_IN:
                    Pwm1_Generate(pxDrv->pxPwmU_highSide, duty);
                    WriteGpio(pxDrv->pxPinU_lowSide, MONO_PIN_HIGH);
                    break;

                case _6STEP_LOWSIDE_ON:
                    Pwm1_Generate(pxDrv->pxPwmU_highSide, 0.0f);
                    WriteGpio(pxDrv->pxPinU_lowSide, MONO_PIN_LOW);
                    break;
            }
            break;

        case POLE_V:  // Phase B
            switch(ctl){
                case _6STEP_HiZ:
                    Pwm1_Generate(pxDrv->pxPwmV_highSide, 0.0f);
                    WriteGpio(pxDrv->pxPinV_lowSide, MONO_PIN_HIGH);
                    break;

                case _6STEP_PWM_IN:
                    Pwm1_Generate(pxDrv->pxPwmV_highSide, duty);
                    WriteGpio(pxDrv->pxPinV_lowSide, MONO_PIN_HIGH);
                    break;

                case _6STEP_LOWSIDE_ON:
                    Pwm1_Generate(pxDrv->pxPwmV_highSide, 0.0f);
                    WriteGpio(pxDrv->pxPinV_lowSide, MONO_PIN_LOW);
                    break;
            }
            break;

        case POLE_W:  // Phase C
            switch(ctl){
                case _6STEP_HiZ:
                    Pwm1_Generate(pxDrv->pxPwmW_highSide, 0.0f);
                    WriteGpio(pxDrv->pxPinW_lowSide, MONO_PIN_HIGH);
                    break;

                case _6STEP_PWM_IN:
                    Pwm1_Generate(pxDrv->pxPwmW_highSide, duty);
                    WriteGpio(pxDrv->pxPinW_lowSide, MONO_PIN_HIGH);
                    break;

                case _6STEP_LOWSIDE_ON:
                    Pwm1_Generate(pxDrv->pxPwmW_highSide, 0.0f);
                    WriteGpio(pxDrv->pxPinW_lowSide, MONO_PIN_LOW);
                    break;
            }
            break;
    }



    

}


