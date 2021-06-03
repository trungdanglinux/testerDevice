/**
  ******************************************************************************
  * @file    i2c.c
  * @brief   This file provides code for the configuration
  *          of the I2C instances.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2021 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under Ultimate Liberty license
  * SLA0044, the "License"; You may not use this file except in compliance with
  * the License. You may obtain a copy of the License at:
  *                             www.st.com/SLA0044
  *
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "i2c.h"

/* USER CODE BEGIN 0 */
#include "usart.h"


/* USER CODE END 0 */

I2C_HandleTypeDef hi2c1;

/* I2C1 init function */
void MX_I2C1_Init(void)
{

  hi2c1.Instance = I2C1;
  hi2c1.Init.Timing = 0x20303E5D;
  hi2c1.Init.OwnAddress1 = 0;
  hi2c1.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
  hi2c1.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
  hi2c1.Init.OwnAddress2 = 0;
  hi2c1.Init.OwnAddress2Masks = I2C_OA2_NOMASK;
  hi2c1.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
  hi2c1.Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;
  if (HAL_I2C_Init(&hi2c1) != HAL_OK)
  {
    Error_Handler();
  }
  /** Configure Analogue filter
  */
  if (HAL_I2CEx_ConfigAnalogFilter(&hi2c1, I2C_ANALOGFILTER_ENABLE) != HAL_OK)
  {
    Error_Handler();
  }
  /** Configure Digital filter
  */
  if (HAL_I2CEx_ConfigDigitalFilter(&hi2c1, 0) != HAL_OK)
  {
    Error_Handler();
  }

}

void HAL_I2C_MspInit(I2C_HandleTypeDef* i2cHandle)
{

  GPIO_InitTypeDef GPIO_InitStruct = {0};
  if(i2cHandle->Instance==I2C1)
  {
  /* USER CODE BEGIN I2C1_MspInit 0 */

  /* USER CODE END I2C1_MspInit 0 */

    __HAL_RCC_GPIOB_CLK_ENABLE();
    /**I2C1 GPIO Configuration
    PB8     ------> I2C1_SCL
    PB9     ------> I2C1_SDA
    */
    GPIO_InitStruct.Pin = SCL_DUT_Pin|SDA_DUT_Pin;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_OD;
    GPIO_InitStruct.Pull = GPIO_PULLUP;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF4_I2C1;
    HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

    /* I2C1 clock enable */
    __HAL_RCC_I2C1_CLK_ENABLE();
  /* USER CODE BEGIN I2C1_MspInit 1 */

  /* USER CODE END I2C1_MspInit 1 */
  }
}

void HAL_I2C_MspDeInit(I2C_HandleTypeDef* i2cHandle)
{

  if(i2cHandle->Instance==I2C1)
  {
  /* USER CODE BEGIN I2C1_MspDeInit 0 */

  /* USER CODE END I2C1_MspDeInit 0 */
    /* Peripheral clock disable */
    __HAL_RCC_I2C1_CLK_DISABLE();

    /**I2C1 GPIO Configuration
    PB8     ------> I2C1_SCL
    PB9     ------> I2C1_SDA
    */
    HAL_GPIO_DeInit(SCL_DUT_GPIO_Port, SCL_DUT_Pin);

    HAL_GPIO_DeInit(SDA_DUT_GPIO_Port, SDA_DUT_Pin);

  /* USER CODE BEGIN I2C1_MspDeInit 1 */

  /* USER CODE END I2C1_MspDeInit 1 */
  }
}

/* USER CODE BEGIN 1 */
void WriteEEprom(uint16_t address,uint8_t *data,uint8_t DataNumber){
	 myprintf("Writing in EEprom...");
	 LL_GPIO_SetOutputPin(EN_3V3_GPIO_Port,EN_3V3_Pin);
	 LL_GPIO_SetOutputPin(CONNECT_GPIO_Port,CONNECT_Pin);
	 LL_GPIO_SetOutputPin(PULL_RST_GPIO_Port,PULL_RST_Pin);
	 LL_GPIO_SetOutputPin(PULL_WP_GPIO_Port,PULL_WP_Pin);
	 HAL_Delay(20); // Essential to delay for a few milliseconds r/w data
	 if (HAL_I2C_Mem_Write(&hi2c1,AA08address,address,1,data,DataNumber,50) == HAL_OK) myprintf(" Writing from EEprom successfully \r\n") ;
	 else myprintf("Writing Error from EEprom \r\n");
	 HAL_Delay(50);
}
uint8_t ReadEEprom(uint16_t address){
	  myprintf("Reading in EEprom...");
	  uint8_t ReadData[1];
	  LL_GPIO_SetOutputPin(EN_3V3_GPIO_Port,EN_3V3_Pin);
	  LL_GPIO_SetOutputPin(CONNECT_GPIO_Port,CONNECT_Pin);
	  LL_GPIO_SetOutputPin(PULL_RST_GPIO_Port,PULL_RST_Pin);
	  LL_GPIO_SetOutputPin(PULL_WP_GPIO_Port,PULL_WP_Pin);
	  HAL_Delay(20);   // Essential to delay for a few milliseconds r/w data
	  if(HAL_I2C_Mem_Read(&hi2c1,AA08address,address,1,&ReadData[0],1,50) == HAL_OK) myprintf(" Reading from EEprom successfully \r\n");
	  else myprintf("Reading Error from EEprom \r\n");
	  return ReadData[0];
}
/* USER CODE END 1 */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
