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
#include "stm32f1xx_hal.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

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

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);

/* USER CODE BEGIN EFP */

/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
#define A0_carrier1_Pin GPIO_PIN_10
#define A0_carrier1_GPIO_Port GPIOE
#define A1_carrier1_Pin GPIO_PIN_11
#define A1_carrier1_GPIO_Port GPIOE
#define A2_carrier1_Pin GPIO_PIN_12
#define A2_carrier1_GPIO_Port GPIOE
#define UD_carrier1_Pin GPIO_PIN_13
#define UD_carrier1_GPIO_Port GPIOE
#define WR_carrier1_Pin GPIO_PIN_14
#define WR_carrier1_GPIO_Port GPIOE
#define RD_carrier1_Pin GPIO_PIN_15
#define RD_carrier1_GPIO_Port GPIOE
#define RT_carrier1_Pin GPIO_PIN_10
#define RT_carrier1_GPIO_Port GPIOB
#define SP_carrier1_Pin GPIO_PIN_11
#define SP_carrier1_GPIO_Port GPIOB

/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
