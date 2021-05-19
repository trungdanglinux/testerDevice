/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
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
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "adc.h"
#include "dma.h"
#include "fatfs.h"
#include "i2c.h"
#include "sdmmc.h"
#include "spi.h"
#include "tim.h"
#include "usart.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "lcd.h"
#include "hexes.h"

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */
uint8_t buttons;
uint8_t x,y;
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */
uint8_t GetButtons();
/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
FILINFO fno;
FRESULT fresult;  // result
UINT br, bw;  // File read/write count
/**** capacity related *****/
FATFS *pfs;
DWORD fre_clust;
uint32_t total, free_space;
#define BUFFER_SIZE 256
char buffer[BUFFER_SIZE];  // to store strings..
char digitID[10] = {'0','1','2','3','4','5','6','7','8','9'};
uint8_t check= 0,z=0;
typedef unsigned char * Word;
Word digit1 =(Word) digitID;
Word digit2 =(Word) digitID;
Word digit3=(Word) digitID;
char * Conf ="EU";
char * mode ="Test&Program";

int bufsize (char *buf)
{
	int i=0;
	while (*(buf++) != '\0') i++;
	return i;
}
void clear_buffer (void)
{
	for (int i=0; i<BUFFER_SIZE; i++) buffer[i] = '\0';
}
void Mode(){
	if (x >=2 && y== 1){
		char * modes[3]= {"Test&Program","ChangeConfig","ChangeID"};
		if (z >2) z=0;
		LCD_cursorXY(3,1);
		LCD_digit((Word)modes[z],12);
		mode= modes[z];
		z++;

	}
}
void SwitchRegion(){
	if (x>=12 && x <=16 && y==0){
	   char * region[2]={"US","EU"};
		if(check >1) check=0;
		LCD_cursorXY(12,0);
		LCD_digit((Word)region[check],3);
		Conf = region[check];
		check++;
	}
}

void addValue(){  //Increase each of three digits with the range from 0-9
			  if ((x == 3) && y == 0){ // first digit in the index (3,0) of LCD
				  uint8_t limitPlus = (uint8_t)*digit1 - '0';
				  if (limitPlus < 10){
					  if(limitPlus==9) digit1--;
					  digit1++;
					  LCD_digit(digit1,1);
				  }
				  HAL_Delay(100);
			  }
			  if ( x == 4  && y == 0){//Second digit in the index (4,0) of LCD
							  uint8_t limitPlus = (uint8_t)*digit2 - '0';
							  if (limitPlus < 10){
								  if(limitPlus==9) digit2--;
								  digit2++;
								  LCD_digit(digit2,1);

							  }
							  HAL_Delay(100);
				}
			  if ( x== 5 && y == 0){//Last digit in the index (5,0) of LCD
							  uint8_t limitPlus = (uint8_t)*digit3 - '0';
							  if (limitPlus < 10){
								  if(limitPlus==9) digit3--;
								  digit3++;
								  LCD_digit(digit3,1);

							  }
							  HAL_Delay(100);
			  }
  }
void substractValue(){   //Decrease each of three digits with the range from 0-9
	if (x == 3 && y == 0){
		uint8_t limitSubtract =(uint8_t)*digit1 - '0'; // convert the character to number
		 if (limitSubtract >= 0){
		  if(limitSubtract == 0) digit1++;
		  	  digit1--;
		  	  LCD_digit(digit1,1);
		 }
		  HAL_Delay(100);
	}
	if ( x == 4 && y == 0){
			uint8_t limitSubtract =(uint8_t)*digit2 - '0';
			 if (limitSubtract >= 0){
			  if(limitSubtract == 0) digit2++;
			  	  digit2--;
			  	  LCD_digit(digit2,1);
			 }
			  HAL_Delay(100);
	}
	if (x== 5 && y == 0){
			uint8_t limitSubtract =(uint8_t)*digit3 - '0';
			 if (limitSubtract >= 0){
			  if(limitSubtract == 0) digit3++;
			  	  digit3--;
			  	  LCD_digit(digit3,1);
			 }
			  HAL_Delay(100);
	}
}
//Print ADC signal
void PrintADC(char * type){
	typedef char * words;
	uint16_t raw=0;
	if (type == (words)"VIN" || type ==(words) "VBAT" || type ==(words)"VSOLAR"){
		HAL_ADC_Start(&hadc1);
		HAL_ADC_PollForConversion(&hadc1, HAL_MAX_DELAY);
		raw = HAL_ADC_GetValue(&hadc1);
		if (type == (words)"VBAT"){
			raw= (2*raw);
			if (raw > 3102){ //battery voltage is over 2.5V
					HAL_GPIO_WritePin(GPIOE,EN_VIN_Pin,GPIO_PIN_SET);
					HAL_Delay(1500);
				}
			 if (raw > 5087 && raw < 5336){
					HAL_GPIO_WritePin(GPIOE,EN_VIN_Pin,GPIO_PIN_RESET);
					HAL_Delay(1500);
			}
		}
	}
	else if (type == (words)"A1V8" || type == (words)"AN_3V3"){
		HAL_ADC_Start(&hadc3);
		HAL_ADC_PollForConversion(&hadc3, HAL_MAX_DELAY);
		raw = HAL_ADC_GetValue(&hadc3);
		if (type == (words)"A1V8" && raw > 2358 && raw < 2730){
			HAL_GPIO_WritePin(GPIOE,EN_VSWITCHED_Pin,GPIO_PIN_RESET);
			HAL_Delay(3000);
		}
	}
	myprintf ("%d ",raw);
}
/* Testing power management */
//Testing Voltage Input
void Testing_VIN(){

	HAL_GPIO_WritePin(GPIOE,EN_VIN_Pin,GPIO_PIN_SET);
	HAL_Delay(2000);
	PrintADC("VIN");
	HAL_GPIO_WritePin(GPIOE,EN_VIN_Pin,GPIO_PIN_RESET);
}
//Testing Solar analog
void Testing_VSOLAR(){

	HAL_GPIO_WritePin(GPIOE,EN_SOLAR_Pin,GPIO_PIN_SET);
	HAL_Delay(1500);
	PrintADC("VSOLAR");
	HAL_GPIO_WritePin(GPIOE,EN_SOLAR_Pin,GPIO_PIN_RESET);
}
//Testing Battery
void Testing_VBAT(){
	HAL_GPIO_WritePin(GPIOE,EN_VIN_Pin,GPIO_PIN_SET);
	HAL_Delay(1500);
	HAL_GPIO_WritePin(GPIOE,EN_VBAT_Pin,GPIO_PIN_SET);
	PrintADC("VBAT");
}
//Testing Switch
void Testing_Switch(){
	HAL_GPIO_WritePin(GPIOE,EN_VSWITCHED_Pin,GPIO_PIN_SET);
	HAL_Delay(3000);
	PrintADC("A1V8");
}
void Testing_3V3(){
	PrintADC("AN_3V3");
}
/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_DMA_Init();
  MX_SDMMC1_SD_Init();
  MX_FATFS_Init();
  MX_TIM1_Init();
  MX_USART3_UART_Init();
  MX_SPI5_Init();
  MX_ADC1_Init();
  MX_SPI1_Init();
  MX_I2C1_Init();
  MX_ADC3_Init();
  /* USER CODE BEGIN 2 */
  HAL_TIM_Base_Start(&htim1);
  LL_GPIO_ResetOutputPin(LCD_RST_GPIO_Port, LCD_RST_Pin);
  HAL_Delay(10);
  LL_GPIO_SetOutputPin(LCD_RST_GPIO_Port, LCD_RST_Pin);
  HAL_Delay(100);
  init_LCD();
  myprintf ("\r\n\r\nzzzzzzzzzzzzzzzzzzzzzzzzzzzz\r\nBST200 Tester\r\n");



#ifdef SD_SELFTEST
  LCD_Clear();
  fresult = f_mount(&SDFatFS, "/", 1);
  sprintf(buffer, "%d ",fresult);
  LCD_write(buffer);
  	if (fresult != FR_OK)
  	{
  		sprintf(buffer, "MountError:%d",fresult);

  		myprintf ("ERROR!!! in mounting SD CARD...(%d)\r\n",fresult);
  	}
  	else
  	{
  		sprintf(buffer, "Mount OK");
  		LCD_write(buffer);
  		myprintf("SD CARD mounted successfully...\r\n");
  	}


  	/*************** Card capacity details ********************/

  	/* Check free space */
  	f_getfree("", &fre_clust, &pfs);

  	total = (uint32_t)((pfs->n_fatent - 2) * pfs->csize * 0.5);
  	sprintf (buffer, "Size:%lu\r\n",total);
  	myprintf(buffer);

  	LCD_cursorXY(0,1);
  	LCD_write(buffer);

	clear_buffer();
  	free_space = (uint32_t)(fre_clust * pfs->csize * 0.5);
  	sprintf (buffer, "SD CARD Free Space: \t%lu\r\n",free_space);
  	myprintf(buffer);
  	clear_buffer();


  	/************* The following operation is using PUTS and GETS *********************/

  	/* Open file to write/ create a file if it doesn't exist */
      fresult = f_open(&SDFile, "file1.txt", FA_OPEN_ALWAYS | FA_READ | FA_WRITE);
      LCD_cursorXY(2,0);
      sprintf(buffer, "%d ",fresult);
      LCD_write(buffer);
  	/* Writing text */
  	f_puts("This data is from the FILE1.txt. And it was written using ...f_puts... ", &SDFile);

  	/* Close file */
  	fresult = f_close(&SDFile);
    LCD_cursorXY(4,0);
    sprintf(buffer, "%d  ",fresult);
    LCD_write(buffer);

  	if (fresult == FR_OK)myprintf ("File1.txt created and the data is written \r\n");

  	/* Open file to read */
  	fresult = f_open(&SDFile, "file1.txt", FA_READ);
    LCD_cursorXY(6,0);
    sprintf(buffer, "%d ",fresult);
    LCD_write(buffer);

  	/* Read string from the file */
  	f_gets(buffer, f_size(&SDFile), &SDFile);

  	myprintf("File1.txt is opened and it contains the data as shown below\r\n");
  	myprintf(buffer);
  	myprintf("\n\n");

  	/* Close file */
  	fresult=f_close(&SDFile);
    LCD_cursorXY(8,0);
    sprintf(buffer, "%d  ",fresult);
    LCD_write(buffer);

  	clear_buffer();



  	/**************** The following operation is using f_write and f_read **************************/

  	/* Create second file with read write access and open it */
  	fresult = f_open(&SDFile, "file2.txt", FA_CREATE_ALWAYS | FA_WRITE);

  	/* Writing text */
  	strcpy (buffer, "This is File2.txt, written using ...f_write... and it says Hello from Controllerstech\r\n");

  	fresult = f_write(&SDFile, buffer, bufsize(buffer), &bw);

  	myprintf ("File2.txt created and data is written\r\n");

  	/* Close file */
  	f_close(&SDFile);



  	// clearing buffer to show that result obtained is from the file
  	clear_buffer();

  	/* Open second file to read */
  	fresult = f_open(&SDFile, "file2.txt", FA_READ);
  	if (fresult == FR_OK)myprintf ("file2.txt is open and the data is shown below\r\n");

  	/* Read data from the file
  	 * Please see the function details for the arguments */
  	f_read (&SDFile, buffer, f_size(&SDFile), &br);
  	myprintf(buffer);
  	myprintf("\r\n");

  	/* Close file */
  	f_close(&SDFile);

  	clear_buffer();

  	/*********************UPDATING an existing file ***************************/

  	/* Open the file with write access */
  	fresult = f_open(&SDFile, "file2.txt", FA_OPEN_EXISTING | FA_READ | FA_WRITE);

  	/* Move to offset to the end of the file */
  	fresult = f_lseek(&SDFile, f_size(&SDFile));

  	if (fresult == FR_OK)myprintf ("About to update the file2.txt\r\n");

  	/* write the string to the file */
  	fresult = f_puts("This is updated data and it should be in the end", &SDFile);

  	f_close (&SDFile);

  	clear_buffer();

  	/* Open to read the file */
  	fresult = f_open (&SDFile, "file2.txt", FA_READ);

  	/* Read string from the file */
  	fresult = f_read (&SDFile, buffer, f_size(&SDFile), &br);
  	if (fresult == FR_OK)myprintf ("Below is the data from updated file2.txt\r\n");
  	myprintf(buffer);
  	myprintf("\r\n");

  	/* Close file */
  	f_close(&SDFile);

  	clear_buffer();

  	/*************************REMOVING FILES FROM THE DIRECTORY ****************************/

 //Let's not delete these in a hurry, to check on PC whether they existed at all
  	/* 	fresult = f_unlink("/file1.txt");
  	if (fresult == FR_OK) myprintf("file1.txt removed successfully...\r\n");

  	fresult = f_unlink("/file2.txt");
  	if (fresult == FR_OK) myprintf("file2.txt removed successfully...\r\n");*/


/////////////////////////////////////////////////////////////////////////////////////////////////
  	/////////////  Reading in a HEX file   ////////////////////////////////////////////

  	fresult = f_open (&SDFile, "BST200.hex", FA_READ);
  	if(!fresult)
  	{
  		myprintf("Opened BST200.hex OK\r\n");
  		f_gets(buffer, 100, &SDFile);
  		if(!fresult)
  		{
  			myprintf("Read first line (length %d characters):\r\n",strlen(buffer));
  			myprintf("%s\r",buffer);
  			f_gets(buffer, 100, &SDFile);
  			myprintf("Read second line (length %d characters):\r\n",strlen(buffer));
  			myprintf("%s\r",buffer);
  			f_gets(buffer, 100, &SDFile);
  			myprintf("Read third line (length %d characters):\r\n",strlen(buffer));
  			myprintf("%s\r",buffer);
  		}
  		fresult=f_close(&SDFile);
  	}





  	/* Unmount SDCARD */
  	fresult = f_mount(NULL, "/", 1);
  	if (fresult == FR_OK) myprintf ("SD CARD UNMOUNTED successfully...\r\n");
    LCD_cursorXY(10,0);
    sprintf(buffer, "%d",fresult);
    LCD_write(buffer);

//    LL_GPIO_ResetOutputPin(EN_SD_GPIO_Port, EN_SD_Pin);
//    HAL_Delay(1);
#endif
  // if(!ReadFile() ) myprintf("ReadFile complete\r\n");

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */

  while (1)
  {
	  buttons=GetButtons();
	  switch(buttons)
	  {
	  	    case 1:		//Minus
	  	    	substractValue();
	  	    	Mode();
	  	    	//LL_GPIO_SetOutputPin(GREEN_GPIO_Port, GREEN_Pin);
	  	    	break;
	  		case 2:		//Up
	  			y=0;
	  			break;
	  		case 4:		//Down
	  			y=1;
	  			break;
	  		case 8:		//Left
	  			if(x) x--;
	  			else x=0;
	  			break;
	  		case 16:	//Right
	  			if(x<14) x++;
	  			else x=15;
	  			break;
	  		case 32:	//Plus
	  			addValue();
	  			SwitchRegion();
	  			LL_GPIO_ResetOutputPin(GREEN_GPIO_Port, GREEN_Pin);
	  			break;
	  		case 64:	//Test
	  			break;
	  		default:
	  			break;
	  }

	  LCD_cursorXY(x,y);
	  HAL_Delay(100);


	  char totalDigits[3]={digit1[0],digit2[0],digit3[0]};

	  if (buttons == 64){

		 totalDigits[0]=digit1[6];
		 totalDigits[1]=digit2[6];
		 totalDigits[2]=digit3[6];

		 LCD_cursorXY(3,0);
		 LCD_digit((Word)totalDigits,3);
		 LCD_cursorXY(12,0);
		 LCD_digit((Word)Conf,2);
		 LCD_cursorXY(3,1);
		 LCD_write((Word)mode);
		 x=3,y=0;
		 HAL_Delay(400);
	  }
	  Testing_3V3();
	 // Testing_VIN();
	 // Testing_VSOLAR();
	  // Testing_VBAT();
	 // Testing_Switch();
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
  }
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  LL_FLASH_SetLatency(LL_FLASH_LATENCY_1);
  while(LL_FLASH_GetLatency()!= LL_FLASH_LATENCY_1)
  {
  }
  LL_PWR_SetRegulVoltageScaling(LL_PWR_REGU_VOLTAGE_SCALE3);
  LL_PWR_EnableOverDriveMode();
  LL_RCC_HSI_SetCalibTrimming(16);
  LL_RCC_HSI_Enable();

   /* Wait till HSI is ready */
  while(LL_RCC_HSI_IsReady() != 1)
  {

  }
  LL_RCC_PLL_ConfigDomain_SYS(LL_RCC_PLLSOURCE_HSI, LL_RCC_PLLM_DIV_16, 192, LL_RCC_PLLP_DIV_4);
  LL_RCC_PLL_ConfigDomain_48M(LL_RCC_PLLSOURCE_HSI, LL_RCC_PLLM_DIV_16, 192, LL_RCC_PLLQ_DIV_4);
  LL_RCC_PLL_Enable();

   /* Wait till PLL is ready */
  while(LL_RCC_PLL_IsReady() != 1)
  {

  }
  LL_RCC_SetAHBPrescaler(LL_RCC_SYSCLK_DIV_1);
  LL_RCC_SetAPB1Prescaler(LL_RCC_APB1_DIV_1);
  LL_RCC_SetAPB2Prescaler(LL_RCC_APB2_DIV_1);
  LL_RCC_SetSysClkSource(LL_RCC_SYS_CLKSOURCE_PLL);

   /* Wait till System clock is ready */
  while(LL_RCC_GetSysClkSource() != LL_RCC_SYS_CLKSOURCE_STATUS_PLL)
  {

  }
  LL_SetSystemCoreClock(48000000);

   /* Update the time base */
  if (HAL_InitTick (TICK_INT_PRIORITY) != HAL_OK)
  {
    Error_Handler();
  }
  LL_RCC_SetCK48MClockSource(LL_RCC_CK48M_CLKSOURCE_PLL);
  LL_RCC_SetSDMMCClockSource(LL_RCC_SDMMC1_CLKSOURCE_PLL48CLK);
  LL_RCC_SetUSARTClockSource(LL_RCC_USART3_CLKSOURCE_PCLK1);
  LL_RCC_SetI2CClockSource(LL_RCC_I2C1_CLKSOURCE_PCLK1);
}

/* USER CODE BEGIN 4 */


uint8_t GetButtons()
{
	uint16_t value=LL_GPIO_ReadInputPort(GPIOG);	//G2...G8 are buttons Up-Down-Left-Right-Plus-Minus-Test
	value=~value;									//invert so that button pressed = 1
	value=(((value>>2)&0x60) | ((value>>1)&0x1F));	//drop unnecessary bits and rearrange to a row 8,7,5..1
	return (uint8_t) value;							//only return 8 bits, that's enough
}
/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
