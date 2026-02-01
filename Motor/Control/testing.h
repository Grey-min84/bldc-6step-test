#ifndef __TESTING_H__
#define __TESTING_H__
#include "six_step.h"


void ThrottleControl_Test(_6StepCtlCtx_t* px6Step);


void CliControl(cli_args_t *args, void* param);
void DataLoggingManage(CountingTick_t* pxTick, _6StepCtlCtx_t* px6Step, uint8_t ucStopToken);
#endif