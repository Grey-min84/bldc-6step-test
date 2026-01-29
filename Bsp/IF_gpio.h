#ifndef __IF_GPIO_H__
#define __IF_GPIO_H__
#include "typeSimple.h"
#include "IF_timer.h"
#include "portStm32_Gpio.h"

#define MONO_PIN_HIGH       1
#define MONO_PIN_LOW        0

#define GPIO_PIN_IN 	0
#define GPIO_PIN_OUT 	1
#define GPIO_PIN_EDGE   2

#define GPI_FILTER_ON 		1
#define GPI_FILTER_OFF  	0


typedef void (*EdgeCallback)(void*);

typedef struct _GPIO_T_{
    //struct _GPIO_T_ *m_pstNext;

	Gpio_HwWrapper* pxGpioPin;
	//u8 ucFilter;
	u8 ucValue;
    //u8 ucUseFilter;
	u16 usId;
    //u8 ucMode;

    EdgeCallback fpEdgeCb;
    void* vxCbArgs;
}IGpio_t;


// typedef struct _GPIO_DRIVER_HEADER_
// {
// 	u16 usTotalNrOfGpio;
// 	IGpio_t* pxListHead;
// }GpioPinList_Header_t;




typedef struct {
	IGpio_t* pxU;
	IGpio_t* pxV;
	IGpio_t* pxW;
}HallSensePin_t;


void InitGpioList(TimerContainer_t* timSrc) ;
void GpioPin_Def(u16 _usId, IGpio_t* pxPinNode, Gpio_HwWrapper* _pxPin);




uint8_t ReadGpio(IGpio_t *pxGpioNode);
void WriteGpio(IGpio_t *pxGpioNode, u8 l_ucPinState);
void ToggleGpio(IGpio_t *pxGpioNode);


void OnGpio_EdgeIsr_Callback(u16 usPin);
void GpioIsr_RegisterCallback(IGpio_t* pxPinNode, EdgeCallback fpCb, void* _args);


//IGpio_t *CheckDuplicate_N_makeList(IGpio_t* pxPin);
//void DigitalFilterCallback(void *l_pParam);

#endif