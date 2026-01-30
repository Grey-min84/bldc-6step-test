/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2025 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32g4xx_hal.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */


/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
/* USER CODE BEGIN ET */

/* USER CODE END ET */

/* Exported constants --------------------------------------------------------*/
/* USER CODE BEGIN EC */
extern TIM_HandleTypeDef htim3;
extern TIM_HandleTypeDef htim6;
extern ADC_HandleTypeDef hadc1;
extern ADC_HandleTypeDef hadc2;

/* USER CODE END EC */

/* Exported macro ------------------------------------------------------------*/
/* USER CODE BEGIN EM */

/* USER CODE END EM */

void HAL_TIM_MspPostInit(TIM_HandleTypeDef *htim);

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);

/* USER CODE BEGIN EFP */

/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
#define TIM1_MAX_DUTY 4249
#define TIM7_PERIOD_CNT 65535
#define B1_Pin GPIO_PIN_13
#define B1_GPIO_Port GPIOC
#define B1_EXTI_IRQn EXTI15_10_IRQn
#define A1C6_CURR_C_Pin GPIO_PIN_0
#define A1C6_CURR_C_GPIO_Port GPIOC
#define A1C7_CURR_B_Pin GPIO_PIN_1
#define A1C7_CURR_B_GPIO_Port GPIOC
#define A1C8_VBUS_Pin GPIO_PIN_2
#define A1C8_VBUS_GPIO_Port GPIOC
#define A1C9_BEMF_A_Pin GPIO_PIN_3
#define A1C9_BEMF_A_GPIO_Port GPIOC
#define A1C1_CURR_A_Pin GPIO_PIN_0
#define A1C1_CURR_A_GPIO_Port GPIOA
#define LPUART1_TX_Pin GPIO_PIN_2
#define LPUART1_TX_GPIO_Port GPIOA
#define LPUART1_RX_Pin GPIO_PIN_3
#define LPUART1_RX_GPIO_Port GPIOA
#define T3C2_POLE_U_LOW_Pin GPIO_PIN_7
#define T3C2_POLE_U_LOW_GPIO_Port GPIOA
#define A2C11_BEMF_C_Pin GPIO_PIN_5
#define A2C11_BEMF_C_GPIO_Port GPIOC
#define T1C2_POLE_V_LOW_Pin GPIO_PIN_0
#define T1C2_POLE_V_LOW_GPIO_Port GPIOB
#define T3C4_POLE_W_LOW_Pin GPIO_PIN_1
#define T3C4_POLE_W_LOW_GPIO_Port GPIOB
#define GPO_EXBOARD_LED_Pin GPIO_PIN_2
#define GPO_EXBOARD_LED_GPIO_Port GPIOB
#define GPE_HALL_3_Pin GPIO_PIN_10
#define GPE_HALL_3_GPIO_Port GPIOB
#define GPE_HALL_3_EXTI_IRQn EXTI15_10_IRQn
#define A1C14_BEMF_B_Pin GPIO_PIN_11
#define A1C14_BEMF_B_GPIO_Port GPIOB
#define GPO_DBG_3_Pin GPIO_PIN_8
#define GPO_DBG_3_GPIO_Port GPIOC
#define T1C1_POLE_U_HIGH_Pin GPIO_PIN_8
#define T1C1_POLE_U_HIGH_GPIO_Port GPIOA
#define T1C2_POLE_V_HIGH_Pin GPIO_PIN_9
#define T1C2_POLE_V_HIGH_GPIO_Port GPIOA
#define T1C3_POLE_W_HIGH_Pin GPIO_PIN_10
#define T1C3_POLE_W_HIGH_GPIO_Port GPIOA
#define T_SWDIO_Pin GPIO_PIN_13
#define T_SWDIO_GPIO_Port GPIOA
#define T_SWCLK_Pin GPIO_PIN_14
#define T_SWCLK_GPIO_Port GPIOA
#define GPE_HALL_1_Pin GPIO_PIN_15
#define GPE_HALL_1_GPIO_Port GPIOA
#define GPE_HALL_1_EXTI_IRQn EXTI15_10_IRQn
#define GPO_DBG_1_Pin GPIO_PIN_10
#define GPO_DBG_1_GPIO_Port GPIOC
#define GPE_HALL_2_Pin GPIO_PIN_3
#define GPE_HALL_2_GPIO_Port GPIOB
#define GPE_HALL_2_EXTI_IRQn EXTI3_IRQn
#define GPO_DBG_2_Pin GPIO_PIN_7
#define GPO_DBG_2_GPIO_Port GPIOB

/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
