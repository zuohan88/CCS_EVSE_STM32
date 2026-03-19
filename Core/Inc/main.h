/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2024 STMicroelectronics.
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
#include "app.h"
/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
/* USER CODE BEGIN ET */

/* USER CODE END ET */

/* Exported constants --------------------------------------------------------*/
/* USER CODE BEGIN EC */

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
#define CP_OFF_Pin GPIO_PIN_13
#define CP_OFF_GPIO_Port GPIOC
#define RELAY_POS_Pin GPIO_PIN_14
#define RELAY_POS_GPIO_Port GPIOC
#define RELAY_NEG_Pin GPIO_PIN_15
#define RELAY_NEG_GPIO_Port GPIOC
#define ADC1_IN6__Pin GPIO_PIN_0
#define ADC1_IN6__GPIO_Port GPIOC
#define ADC1_IN6_C1_Pin GPIO_PIN_1
#define ADC1_IN6_C1_GPIO_Port GPIOC
#define ADC1_IN8__Pin GPIO_PIN_2
#define ADC1_IN8__GPIO_Port GPIOC
#define ADC1_IN8_C3_Pin GPIO_PIN_3
#define ADC1_IN8_C3_GPIO_Port GPIOC
#define ADC1_IN1__Pin GPIO_PIN_0
#define ADC1_IN1__GPIO_Port GPIOA
#define ADC1_IN1_A1_Pin GPIO_PIN_1
#define ADC1_IN1_A1_GPIO_Port GPIOA
#define ADC1_IN3__Pin GPIO_PIN_2
#define ADC1_IN3__GPIO_Port GPIOA
#define ADC1_IN3_A3_Pin GPIO_PIN_3
#define ADC1_IN3_A3_GPIO_Port GPIOA
#define PILOT_ADC_Pin GPIO_PIN_4
#define PILOT_ADC_GPIO_Port GPIOA
#define QCA_INT_Pin GPIO_PIN_4
#define QCA_INT_GPIO_Port GPIOC
#define QCA_INT_EXTI_IRQn EXTI4_IRQn
#define QCA_RST_Pin GPIO_PIN_5
#define QCA_RST_GPIO_Port GPIOC
#define QCA_CS_Pin GPIO_PIN_0
#define QCA_CS_GPIO_Port GPIOB
#define LED_1_Pin GPIO_PIN_1
#define LED_1_GPIO_Port GPIOB
#define LED_2_Pin GPIO_PIN_2
#define LED_2_GPIO_Port GPIOB
#define INPUT_4_Pin GPIO_PIN_15
#define INPUT_4_GPIO_Port GPIOB
#define INPUT_3_Pin GPIO_PIN_6
#define INPUT_3_GPIO_Port GPIOC
#define INPUT_2_Pin GPIO_PIN_7
#define INPUT_2_GPIO_Port GPIOC
#define INPUT_1_Pin GPIO_PIN_8
#define INPUT_1_GPIO_Port GPIOC
#define RELAY_4_Pin GPIO_PIN_9
#define RELAY_4_GPIO_Port GPIOC
#define RELAY_3_Pin GPIO_PIN_8
#define RELAY_3_GPIO_Port GPIOA
#define RELAY_2_Pin GPIO_PIN_9
#define RELAY_2_GPIO_Port GPIOA
#define RELAY_1_Pin GPIO_PIN_10
#define RELAY_1_GPIO_Port GPIOA
#define T_SWDIO_Pin GPIO_PIN_13
#define T_SWDIO_GPIO_Port GPIOA
#define T_SWCLK_Pin GPIO_PIN_14
#define T_SWCLK_GPIO_Port GPIOA
#define RS485_RX_Pin GPIO_PIN_10
#define RS485_RX_GPIO_Port GPIOC
#define RS485_RXC11_Pin GPIO_PIN_11
#define RS485_RXC11_GPIO_Port GPIOC
#define PILOT_PWM_Pin GPIO_PIN_9
#define PILOT_PWM_GPIO_Port GPIOB

/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
