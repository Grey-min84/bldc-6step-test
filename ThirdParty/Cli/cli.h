/*
 * cli.h
 *
 *  Created on: 2020. 12. 19.
 *      Author: baram
 */

#ifndef SRC_COMMON_HW_INCLUDE_CLI_H_
#define SRC_COMMON_HW_INCLUDE_CLI_H_

//#include "hw_def.h"
#include "supportDef.h"
#include "main.h"

//#ifdef _USE_HW_CLI
#ifdef __cplusplus
extern "C" {
#endif


#define      HW_CLI_CMD_LIST_MAX    16
#define      HW_CLI_CMD_NAME_MAX    16
#define      HW_CLI_LINE_HIS_MAX    4
#define      HW_CLI_LINE_BUF_MAX    48

#define CLI_CMD_LIST_MAX      HW_CLI_CMD_LIST_MAX
#define CLI_CMD_NAME_MAX      HW_CLI_CMD_NAME_MAX

#define CLI_LINE_HIS_MAX      HW_CLI_LINE_HIS_MAX
#define CLI_LINE_BUF_MAX      HW_CLI_LINE_BUF_MAX


#define MAX_UNIT_TEST_ITEM    (6)

typedef struct CliStructure{
    const char ucList[16];
    void* param;
}CliStructure_t;

typedef struct
{
  uint16_t   argc;
  char     **argv;

  int32_t  (*getData)(uint8_t index);
  float    (*getFloat)(uint8_t index);
  char    *(*getStr)(uint8_t index);
  uint8_t     (*isStr)(uint8_t index, const char *p_str);
} cli_args_t;



// typedef struct {
//     //uint8_t bIsTestOn;
//     uint8_t bIsFactoryTest;
//     uint8_t bIsAllLog;
//     uint8_t testCmd;
// }UnitTestMain;



uint8_t cliInit(void* pvParam);
uint8_t cliOpen(uint8_t ch, UART_HandleTypeDef* uartHw);
uint8_t cliOpenLog(uint8_t ch, UART_HandleTypeDef* uartHw);
uint8_t cliMain();
void cliPrintf(const char *fmt, ...);
uint8_t cliAdd(const char *cmd_str, void (*p_func)(cli_args_t *, void*), void* pParam, uint8_t isTestOn);
uint8_t cliKeepLoop(void);
void cliTestEnable( uint8_t ucEnable);
void cmdInputEchoCtl(uint8_t onOff);
//#endif
#ifdef __cplusplus
}
#endif

#endif /* SRC_COMMON_HW_INCLUDE_CLI_H_ */
