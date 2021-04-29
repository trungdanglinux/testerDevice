/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2020 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under BSD 3-Clause license,
  * the "License"; You may not use this file except in compliance with the
  * License. You may obtain a copy of the License at:
  *                        opensource.org/licenses/BSD-3-Clause
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
#include "stm32f7xx_hal.h"
#include "stm32f7xx_ll_rcc.h"
#include "stm32f7xx_ll_bus.h"
#include "stm32f7xx_ll_system.h"
#include "stm32f7xx_ll_exti.h"
#include "stm32f7xx_ll_cortex.h"
#include "stm32f7xx_ll_utils.h"
#include "stm32f7xx_ll_pwr.h"
#include "stm32f7xx_ll_dma.h"
#include "stm32f7xx_hal.h"
#include "stm32f7xx_ll_gpio.h"

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
#define SCL_Pin GPIO_PIN_14
#define SCL_GPIO_Port GPIOF
#define SDA_Pin GPIO_PIN_15
#define SDA_GPIO_Port GPIOF
#define MINUS_Pin GPIO_PIN_1
#define MINUS_GPIO_Port GPIOG
#define LCD_RST_Pin GPIO_PIN_13
#define LCD_RST_GPIO_Port GPIOE
#define UP_Pin GPIO_PIN_2
#define UP_GPIO_Port GPIOG
#define DOWN_Pin GPIO_PIN_3
#define DOWN_GPIO_Port GPIOG
#define LEFT_Pin GPIO_PIN_4
#define LEFT_GPIO_Port GPIOG
#define RIGHT_Pin GPIO_PIN_5
#define RIGHT_GPIO_Port GPIOG
#define PLUS_Pin GPIO_PIN_7
#define PLUS_GPIO_Port GPIOG
#define TEST_Pin GPIO_PIN_8
#define TEST_GPIO_Port GPIOG
#define RED_Pin GPIO_PIN_9
#define RED_GPIO_Port GPIOG
#define GREEN_Pin GPIO_PIN_10
#define GREEN_GPIO_Port GPIOG
/* USER CODE BEGIN Private defines */
//#define SD_SELFTEST
/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/