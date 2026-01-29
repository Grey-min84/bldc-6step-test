#ifndef __IF_PWM_H__
#define __IF_PWM_H__
#include "portStm32_Pwm.h"

#define PWM_POLE_U_POS           (1)
#define PWM_POLE_U_NEG           (2)

#define PWM_POLE_V_POS           (3)
#define PWM_POLE_V_NEG           (4)

#define PWM_POLE_W_POS           (5)
#define PWM_POLE_W_NEG           (6)


typedef void (*fpPeriodCb)(void*);

enum eBLDC_PWM_CH{
    ePWM_POLE_U_POS = 0,
    ePWM_POLE_U_NEG,

    ePWM_POLE_V_POS,
    ePWM_POLE_V_NEG,

    ePWM_POLE_W_POS,
    ePWM_POLE_W_NEG,

    eBLDC_PWM_MAX
};




typedef struct PWM_Complementary_tag{
    u8 ucChP;
    u8 ucChN;
}PWM_Complementary_t;




typedef struct IPwm_tag{
    Pwm1Ch_HwWrapper* pxPwmSrc;
    u8 ucChId;

    fpPeriodCb fpPeriodDoneCb;
    void* args;
}IPwm_t;










void Pwm1_StartStop(IPwm_t* pxPwm, u8 OnOff);
void Pwm1_InitHw(IPwm_t* pxPwm, Pwm1Ch_HwWrapper* pxHw, u8 ucBldcCh);
void Pwm1_Generate(IPwm_t* pxCtx, float fDuty);
void Pwm1_AddCallbackPeriodDone(IPwm_t* pxPwm, fpPeriodCb fpCb, void* _args);

void Pwm2_StartStop(IPwm_t* pxPwm, u8 OnOff);
void Pwm2_InitHw(IPwm_t* pxPwm, Pwm1Ch_HwWrapper* pxHw, u8 ucBldcCh);
void Pwm2_Generate_Compl(IPwm_t* pxCtx, float fDuty);
void Pwm2_Generate_Pos(IPwm_t* pxCtx, float fDuty);
void Pwm2_Generate_Neg(IPwm_t* pxCtx, float fDuty);
void Pwm2_SetLevel_Pos(IPwm_t* pxCtx, u8 lev);
void Pwm2_SetLevel_Neg(IPwm_t* pxCtx, u8 lev);


#endif