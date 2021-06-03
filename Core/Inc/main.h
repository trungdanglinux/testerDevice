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
#define GREEN_Pin GPIO_PIN_0
#define GREEN_GPIO_Port GPIOC
#define MINUS_Pin GPIO_PIN_3
#define MINUS_GPIO_Port GPIOC
#define RED_Pin GPIO_PIN_3
#define RED_GPIO_Port GPIOA
#define AN_24V_Pin GPIO_PIN_4
#define AN_24V_GPIO_Port GPIOA
#define AN_VBAT_Pin GPIO_PIN_5
#define AN_VBAT_GPIO_Port GPIOA
#define AN_1V8_Pin GPIO_PIN_6
#define AN_1V8_GPIO_Port GPIOA
#define EN_VSWITCHED_Pin GPIO_PIN_12
#define EN_VSWITCHED_GPIO_Port GPIOF
#define SCL_Pin GPIO_PIN_14
#define SCL_GPIO_Port GPIOF
#define SDA_Pin GPIO_PIN_15
#define SDA_GPIO_Port GPIOF
#define CS_DUT_Pin GPIO_PIN_1
#define CS_DUT_GPIO_Port GPIOG
#define LCD_RST_Pin GPIO_PIN_13
#define LCD_RST_GPIO_Port GPIOE
#define PULL_WP_Pin GPIO_PIN_12
#define PULL_WP_GPIO_Port GPIOB
#define PULL_RST_Pin GPIO_PIN_13
#define PULL_RST_GPIO_Port GPIOB
#define EN_BOOTS_Pin GPIO_PIN_15
#define EN_BOOTS_GPIO_Port GPIOB
#define EN_3V3_Pin GPIO_PIN_15
#define EN_3V3_GPIO_Port GPIOD
#define UP_Pin GPIO_PIN_2
#define UP_GPIO_Port GPIOG
#define DOWN_Pin GPIO_PIN_3
#define DOWN_GPIO_Port GPIOG
#define TX_Pin GPIO_PIN_6
#define TX_GPIO_Port GPIOC
#define RX_Pin GPIO_PIN_7
#define RX_GPIO_Port GPIOC
#define CONNECT_Pin GPIO_PIN_15
#define CONNECT_GPIO_Port GPIOA
#define TEST_Pin GPIO_PIN_4
#define TEST_GPIO_Port GPIOD
#define PLUS_Pin GPIO_PIN_5
#define PLUS_GPIO_Port GPIOD
#define RIGHT_Pin GPIO_PIN_6
#define RIGHT_GPIO_Port GPIOD
#define LEFT_Pin GPIO_PIN_7
#define LEFT_GPIO_Port GPIOD
#define EN_VIN_Pin GPIO_PIN_3
#define EN_VIN_GPIO_Port GPIOB
#define EN_VBAT_Pin GPIO_PIN_4
#define EN_VBAT_GPIO_Port GPIOB
#define EN_VSOLAR_Pin GPIO_PIN_5
#define EN_VSOLAR_GPIO_Port GPIOB
#define SCL_DUT_Pin GPIO_PIN_8
#define SCL_DUT_GPIO_Port GPIOB
#define SDA_DUT_Pin GPIO_PIN_9
#define SDA_DUT_GPIO_Port GPIOB
/* USER CODE BEGIN Private defines */
//#define SD_SELFTEST
#define MINUS (!LL_GPIO_IsInputPinSet(MINUS_GPIO_Port,MINUS_Pin))
#define PLUS (!LL_GPIO_IsInputPinSet(PLUS_GPIO_Port,PLUS_Pin))
#define RIGHT (!LL_GPIO_IsInputPinSet(RIGHT_GPIO_Port,RIGHT_Pin))
#define LEFT (!LL_GPIO_IsInputPinSet(LEFT_GPIO_Port,LEFT_Pin))
#define UP (!LL_GPIO_IsInputPinSet(UP_GPIO_Port,UP_Pin))
#define DOWN (!LL_GPIO_IsInputPinSet(DOWN_GPIO_Port,DOWN_Pin))
#define TEST (!LL_GPIO_IsInputPinSet(TEST_GPIO_Port,TEST_Pin))
/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
