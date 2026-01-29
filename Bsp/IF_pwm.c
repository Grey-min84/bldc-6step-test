#include "IF_pwm.h"


IPwm_t* g_apxHwCbContainer[5];
uint8_t g_ucHwCbCount = 0;


void Pwm1_StartStop(IPwm_t* pxPwm, u8 OnOff){

    portSTM32_PWM_StartStop(pxPwm->pxPwmSrc, OnOff);
}


void Pwm1_InitHw(IPwm_t* pxPwm, Pwm1Ch_HwWrapper* pxHw, u8 ucChId){

    pxPwm->pxPwmSrc = pxHw;
    pxPwm->ucChId = ucChId;
}


void Pwm1_AddCallbackPeriodDone(IPwm_t* pxPwm, fpPeriodCb fpCb, void* _args){

    pxPwm->fpPeriodDoneCb = fpCb;
    pxPwm->args = _args;

    g_apxHwCbContainer[g_ucHwCbCount++] = pxPwm;
}   


void Pwm1_Generate(IPwm_t* pxPwm, float fDuty){

    Pwm1Ch_HwWrapper* pxHw = pxPwm->pxPwmSrc;
    float inDuty = (float)((float)pxHw->uiMaxDuty * (fDuty / 100.0f));

    portSTM32_PWM_Generate(pxPwm->pxPwmSrc, (u32)inDuty);
}




void Pwm1_HwPeriodElapsedCallback(IPwm_t* pxHw){

    //Find IPwm_t from pxHw
    //Call the callback function
    if(pxHw->fpPeriodDoneCb != NULL){
        pxHw->fpPeriodDoneCb(pxHw->args);
    }
}





void Pwm2_StartStop(IPwm_t* pxPwm, u8 OnOff){


}

void Pwm2_InitHw(IPwm_t* pxPwm, Pwm1Ch_HwWrapper* pxHw, u8 ucBldcCh){


}

void Pwm2_Generate_Compl(IPwm_t* pxCtx, float fDuty){


}

void Pwm2_Generate_Pos(IPwm_t* pxCtx, float fDuty){


}

void Pwm2_Generate_Neg(IPwm_t* pxCtx, float fDuty){


}

void Pwm2_SetLevel_Pos(IPwm_t* pxCtx, u8 lev){


}

void Pwm2_SetLevel_Neg(IPwm_t* pxCtx, u8 lev){


}
