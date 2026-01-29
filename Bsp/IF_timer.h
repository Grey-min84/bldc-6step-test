#ifndef __IF_TIMER_H__
#define __IF_TIMER_H__
#include "portStm32_Timer.h"

#define HARD_TIMER_STOPPED 0
#define HARD_TIMER_STARTED 1
#define MAX_TIMER_TASK      (16)

typedef void (*tmCallback)(void*);




typedef struct TimerCounting_tag{
	Tm_HwWrapper* vxHwTimer;

}TimerCounter_t;


typedef struct TimerTask_tag{

	void* args;
	tmCallback fpTmTask;

	u8 usId;
	u8 ucTimerStatus;
	u32 uiCount;
	u32 uiPeriod;

}TimerTask_t;


typedef struct TimerContainer_tag{
    Tm_HwWrapper* vxHwTimer;
    TimerTask_t *apxTasks[MAX_TIMER_TASK];
    u8 ucRegisterCount;
}TimerContainer_t;







u8 InitTimer(TimerContainer_t* pxTmContainer, Tm_HwWrapper* vxHwTimer);
TimerTask_t CreateTimerTask(tmCallback vxCb, void* args, u32 period, u8 StartCondition);
u8 RegisterTimer(TimerContainer_t* pxTmContainer, TimerTask_t* pxTimer);
void TimerContainerCtl(TimerContainer_t* pxTmContainer, u8 OnOff);
void HWTimerCallback(TimerContainer_t* genericTimer);


void InitCountingTimer(TimerCounter_t* pxTimer);
u32 GetTimerCount(TimerCounter_t* pxTimer);
void ResetTimerCount(TimerCounter_t* pxTimer);


#endif