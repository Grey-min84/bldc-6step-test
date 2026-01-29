/*
 * uart.h
 *
 *  Created on: 2020. 12. 8.
 *      Author: baram
 */

#ifndef SRC_COMMON_HW_INCLUDE_UART_H_
#define SRC_COMMON_HW_INCLUDE_UART_H_


//#include "hw_def.h"

#include "main.h"

#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>
//#ifdef _USE_HW_UART

#define HW_UART_MAX_CH      2
#define UART_MAX_CH         HW_UART_MAX_CH


uint8_t     uartInit(void);
uint8_t     uartOpen(uint8_t ch, UART_HandleTypeDef* uartHw);
uint32_t uartAvailable(uint8_t ch);
uint8_t  uartRead(uint8_t ch);
uint32_t uartWrite(uint8_t ch, uint8_t *p_data, uint32_t length);
uint32_t uartPrintf(uint8_t ch, char *fmt, ...);
uint32_t uartGetBaud(uint8_t ch);

//#endif
#ifdef __cplusplus
}
#endif
#endif /* SRC_COMMON_HW_INCLUDE_UART_H_ */
