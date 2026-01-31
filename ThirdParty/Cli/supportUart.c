/*
 * uart.c
 *
 *  Created on: 2020. 12. 8.
 *      Author: baram
 */


#include "supportUart.h"
//#include "cdc.h"
#include "qbuffer.h"


#define _USE_HW_UART



#define _USE_UART1
#define _USE_UART2


static uint8_t is_open[UART_MAX_CH];

#ifdef _USE_UART2
static qbuffer_t qbuffer[UART_MAX_CH];
static uint8_t rx_buf[256];


//UART_HandleTypeDef huart1;
//DMA_HandleTypeDef  hdma_usart1_rx;
#endif


UART_HandleTypeDef* g_UartHwArr[2];



uint8_t uartInit(void)
{
  for (int i=0; i<UART_MAX_CH; i++)
  {
    is_open[i] = 0;
  }


  return 1;
}

uint8_t uartOpen(uint8_t ch, UART_HandleTypeDef* uartHw)
{
  uint8_t ret = 0;


  switch(ch)
  {
    case _DEF_UART1:
      is_open[ch] = 1;
      ret = 1;

      if(HAL_UART_Receive_DMA(uartHw, (uint8_t *)&rx_buf[0], 256) != HAL_OK)
      {
        ret = 0;
        return ret;
      }

      qbufferCreate(&qbuffer[ch], &rx_buf[0], 256);

      g_UartHwArr[ch] = uartHw;

      qbuffer[ch].in  = qbuffer[ch].len - uartHw->hdmarx->Instance->CNDTR;
      qbuffer[ch].out = qbuffer[ch].in;
      break;

    case _DEF_UART2:
      g_UartHwArr[ch] = uartHw;
      // huart1.Instance         = USART1;
      // huart1.Init.BaudRate    = baud;
      // huart1.Init.WordLength  = UART_WORDLENGTH_8B;
      // huart1.Init.StopBits    = UART_STOPBITS_1;
      // huart1.Init.Parity      = UART_PARITY_NONE;
      // huart1.Init.Mode        = UART_MODE_TX_RX;
      // huart1.Init.HwFlowCtl   = UART_HWCONTROL_NONE;
      // huart1.Init.OverSampling= UART_OVERSAMPLING_16;

      // HAL_UART_DeInit(&huart1);

      // qbufferCreate(&qbuffer[ch], &rx_buf[0], 256);

      
      ret = 1;
      is_open[ch] = 1;

      if(HAL_UART_Receive_DMA(uartHw, (uint8_t *)&rx_buf[0], 256) != HAL_OK)
      {
        ret = 0;
      }

      g_UartHwArr[ch] = uartHw;

      qbuffer[ch].in  = qbuffer[ch].len - uartHw->hdmarx->Instance->CNDTR;
      qbuffer[ch].out = qbuffer[ch].in;
      

      break;
  }

  return ret;
}

uint32_t uartAvailable(uint8_t ch)
{
  uint32_t ret = 0;

  UART_HandleTypeDef* uartHw = g_UartHwArr[ch];

  switch(ch)
  {
    case _DEF_UART1:
      qbuffer[ch].in = (qbuffer[ch].len - uartHw->hdmarx->Instance->CNDTR);
      ret = qbufferAvailable(&qbuffer[ch]);
      //ret = cdcAvailable();
      break;

    case _DEF_UART2:

      qbuffer[ch].in = (qbuffer[ch].len - uartHw->hdmarx->Instance->CNDTR);
      ret = qbufferAvailable(&qbuffer[ch]);

      break;
  }

  return ret;
}

uint8_t uartRead(uint8_t ch)
{
  uint8_t ret = 0;

  switch(ch)
  {
    case _DEF_UART1:
      //ret = cdcRead();
      qbufferRead(&qbuffer[ch], &ret, 1);
      break;

    case _DEF_UART2:
      #ifdef _USE_UART2
      qbufferRead(&qbuffer[ch], &ret, 1);
      #endif
      break;
  }

  return ret;
}

uint32_t uartWrite(uint8_t ch, uint8_t *p_data, uint32_t length)
{
  uint32_t ret = 0;
  HAL_StatusTypeDef status;

  UART_HandleTypeDef* uartHw = g_UartHwArr[ch];

  switch(ch)
  {
    case _DEF_UART1:
      //ret = cdcWrite(p_data, length);
      status = HAL_UART_Transmit(uartHw, p_data, length, 100);
      if (status == HAL_OK)
      {
        ret = length;
      }
      break;

    case _DEF_UART2:

      status = HAL_UART_Transmit(uartHw, p_data, length, 100);
      if (status == HAL_OK)
      {
        ret = length;
      }

      break;
  }

  return ret;
}

uint32_t uartPrintf(uint8_t ch, char *fmt, ...)
{
  char buf[256];
  va_list args;
  int len;
  uint32_t ret;

  va_start(args, fmt);
  len = vsnprintf(buf, 256, fmt, args);

  ret = uartWrite(ch, (uint8_t *)buf, len);

  va_end(args);


  return ret;
}

uint32_t uartGetBaud(uint8_t ch)
{
  uint32_t ret = 0;
  UART_HandleTypeDef* uartHw = g_UartHwArr[ch];

  switch(ch)
  {
    case _DEF_UART1:
      //ret = cdcGetBaud();
      break;

    case _DEF_UART2:
      #ifdef _USE_UART2
      ret = uartHw->Init.BaudRate;
      #endif
      break;
  }

  return ret;
}



void HAL_UART_ErrorCallback(UART_HandleTypeDef *huart)
{
  if (huart->Instance == USART1)
  {
  }
}

//void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
//{
//#if 0
//  if (huart->Instance == USART1)
//  {
//    qbufferWrite(&qbuffer[_DEF_UART2], &rx_data[_DEF_UART2], 1);
//
//    HAL_UART_Receive_IT(&huart1, (uint8_t *)&rx_data[_DEF_UART2], 1);
//  }
//#endif
//}




