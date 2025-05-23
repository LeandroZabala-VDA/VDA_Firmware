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
#include "stm32f3xx_hal.h"
#include "filters.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
/* USER CODE BEGIN ET */
extern FiltroMediaMovil filtro_potentiometer;
extern FiltroMediaMovil filtro_out_PID;
/* USER CODE END ET */

/* Exported constants --------------------------------------------------------*/
/* USER CODE BEGIN EC */

/* USER CODE END EC */

/* Exported macro ------------------------------------------------------------*/
/* USER CODE BEGIN EM */

/* USER CODE END EM */

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);

/* USER CODE BEGIN EFP */

/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
#define SW1_Pin GPIO_PIN_13
#define SW1_GPIO_Port GPIOC
#define SW2_Pin GPIO_PIN_14
#define SW2_GPIO_Port GPIOC
#define SW3_Pin GPIO_PIN_15
#define SW3_GPIO_Port GPIOC
#define ACQ_LED_Pin GPIO_PIN_6
#define ACQ_LED_GPIO_Port GPIOB
#define POWER_LED_Pin GPIO_PIN_7
#define POWER_LED_GPIO_Port GPIOB
#define FF_D_Pin GPIO_PIN_8
#define FF_D_GPIO_Port GPIOB
#define FF_CLK_Pin GPIO_PIN_9
#define FF_CLK_GPIO_Port GPIOB

/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
