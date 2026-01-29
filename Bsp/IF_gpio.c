#include "IF_gpio.h"

static IGpio_t* axEdgeIsrPins[10] = {NULL,};
static uint8_t ucEdgeIsrPinCnt = 0;


//void GpioPin_Def(u16 _usId, u8 _ucMode, u8 _ucUseFilter, IGpio_t* pxPinNode, Gpio_HwWrapper* _pxPin)
void GpioPin_Def(u16 _usId, IGpio_t* pxPinNode, Gpio_HwWrapper* _pxPin){

    Gpio_HwWrapper* pxPin = _pxPin;


    pxPinNode->pxGpioPin = (void*)pxPin;
    pxPinNode->usId = _usId;
    //pxPinNode->ucMode = _ucMode;
    //pxPinNode->ucUseFilter = _ucUseFilter;
    

    pxPinNode->ucValue = 0;
    //pxPinNode->ucFilter = 0;
    //pxPinNode->m_pstNext = NULL;
}


void GpioIsr_RegisterCallback(IGpio_t* pxPinNode, EdgeCallback fpCb, void* _args){

    pxPinNode->fpEdgeCb = fpCb;
    pxPinNode->vxCbArgs = _args;

    axEdgeIsrPins[ucEdgeIsrPinCnt] = pxPinNode;
    ucEdgeIsrPinCnt++;
}


#if 0
GpioPinList_Header_t g_stGpioDriverHeader;


void InitGpioList(TimerContainer_t* timSrc) {

	g_stGpioDriverHeader.pxListHead = NULL;
    g_stGpioDriverHeader.usTotalNrOfGpio = 0;

    // static TimerTask_t xTmTask_digitFilt;

	// xTmTask_digitFilt = CreateTimerTask(DigitalFilterCallback, (void*)0, 1, HARD_TIMER_STARTED);
    // RegisterTimer(timSrc, &xTmTask_digitFilt);

}


IGpio_t *CheckDuplicate_N_makeList(IGpio_t* pxNode)
{
	IGpio_t *l_pstCurNode = NULL;

    if(g_stGpioDriverHeader.pxListHead == NULL){

        g_stGpioDriverHeader.pxListHead = pxNode;
        g_stGpioDriverHeader.usTotalNrOfGpio++;
        return pxNode;
    }

	l_pstCurNode = g_stGpioDriverHeader.pxListHead;

	if(l_pstCurNode->usId == pxNode->usId){
		return NULL;
	}


	while( l_pstCurNode->m_pstNext != NULL ){

		l_pstCurNode = l_pstCurNode->m_pstNext;

		if(l_pstCurNode->usId == pxNode->usId){
			return NULL;
		}

	}

    l_pstCurNode->m_pstNext = pxNode;
    g_stGpioDriverHeader.usTotalNrOfGpio++;

	return pxNode;
}



void DigitalFilterCallback(void *l_pParam)
{
	int i;
	uint8_t l_ucDiTemp;
	IGpio_t *l_pstCurNode;


	l_pstCurNode = g_stGpioDriverHeader.pxListHead;

	for( i= 0; i<g_stGpioDriverHeader.usTotalNrOfGpio ; i++ ){

		if( l_pstCurNode->ucMode == GPIO_PIN_IN ) {

			l_ucDiTemp = ReadGpio(l_pstCurNode);

			if( l_pstCurNode->ucUseFilter ==  GPI_FILTER_ON) {

				l_pstCurNode->ucFilter <<= 1;
                l_pstCurNode->ucFilter= l_pstCurNode->ucFilter | l_ucDiTemp;

                if(l_pstCurNode->ucFilter == 0x00 ) {

                    l_pstCurNode->ucValue = 0;
                }
                else if(l_pstCurNode->ucFilter == 0xFF) {

                    l_pstCurNode->ucValue = 1;
                }
			}
			else {

				l_pstCurNode->ucValue = l_ucDiTemp;
			}
		}

		l_pstCurNode = l_pstCurNode->m_pstNext;
	}
}
#endif


uint8_t ReadGpio(IGpio_t *pxGpioNode)
{
	return portHw_readPin(pxGpioNode->pxGpioPin);
}

void WriteGpio(IGpio_t *pxGpioNode, u8 l_ucPinState)
{
	pxGpioNode->ucValue = l_ucPinState;

    portHw_writePin(pxGpioNode->pxGpioPin, pxGpioNode->ucValue);
}

void ToggleGpio(IGpio_t *pxGpioNode)
{
    portHw_togglePin(pxGpioNode->pxGpioPin);

}


void OnGpio_EdgeIsr_Callback(u16 _usPin){



    //axEdgeIsrPins
    for(uint8_t idx=0; axEdgeIsrPins[idx] != NULL; ++idx){
        Gpio_HwWrapper* pxPin = axEdgeIsrPins[idx]->pxGpioPin;

        if(axEdgeIsrPins[idx]->fpEdgeCb != NULL && pxPin->usPin == _usPin){
            axEdgeIsrPins[idx]->fpEdgeCb(axEdgeIsrPins[idx]->vxCbArgs);
        }
    }
    


}
