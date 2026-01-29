#ifndef __PORT_STM32_GPIO_H__
#define __PORT_STM32_GPIO_H__

#include "typeSimple.h"
#include "stm32g4xx_hal.h"


typedef struct {
	GPIO_TypeDef* uiPort;
	u16 usPin;
}Gpio_HwWrapper;


void portHw_GpioDef( GPIO_TypeDef* uiPort, u16 usPin, u8 ucMode, u8 ucUseFilter);
u8 portHw_readPin(Gpio_HwWrapper* pxPin);
void portHw_writePin(Gpio_HwWrapper* pxPin, u8 l_ucPinState);
void portHw_togglePin(Gpio_HwWrapper* pxPin);
void portHw_OnGpio_EdgeIsr_Callback(u16 usPin);
#endif
