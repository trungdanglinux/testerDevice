/*
 * myi2c.c
 *
 *  Created on: Dec 14, 2020
 *      Author: jussi
 */
#include "tim.h"
#include "main.h"
#include "gpio.h"

 unsigned char scoutIDs[10] ="ID:";
    unsigned char scoutRegion[10] ="Conf:";


void I2C_out(unsigned char d) //I2C Output
{
	int n;
	for(n=0;n<8;n++)
	{
		if((d&0x80)==0x80) LL_GPIO_SetOutputPin(SDA_GPIO_Port, SDA_Pin);
		else LL_GPIO_ResetOutputPin(SDA_GPIO_Port, SDA_Pin);
		delay_us(2);
		d=(d<<1);
		LL_GPIO_ResetOutputPin(SCL_GPIO_Port, SCL_Pin);
		delay_us(2);
		LL_GPIO_SetOutputPin(SCL_GPIO_Port, SCL_Pin);
		delay_us(2);
		LL_GPIO_ResetOutputPin(SCL_GPIO_Port, SCL_Pin);
		delay_us(2);
	}
	LL_GPIO_SetOutputPin(SCL_GPIO_Port, SCL_Pin);
	delay_us(2);
	while(IN(GPIOF,15))
	{
		LL_GPIO_ResetOutputPin(SCL_GPIO_Port, SCL_Pin);
		delay_us(2);
		LL_GPIO_SetOutputPin(SCL_GPIO_Port, SCL_Pin);
		delay_us(2);
	}
	LL_GPIO_ResetOutputPin(SCL_GPIO_Port, SCL_Pin);
	delay_us(2);
}

void I2C_Start(void)
{
	LL_GPIO_SetOutputPin(SCL_GPIO_Port, SCL_Pin);
	delay_us(2);
	LL_GPIO_SetOutputPin(SDA_GPIO_Port, SDA_Pin);
	delay_us(2);
	LL_GPIO_ResetOutputPin(SDA_GPIO_Port, SDA_Pin);
	delay_us(2);
	LL_GPIO_ResetOutputPin(SCL_GPIO_Port, SCL_Pin);
	delay_us(2);
}

void I2C_Stop(void)
{
	LL_GPIO_ResetOutputPin(SDA_GPIO_Port, SDA_Pin);
	delay_us(2);
	LL_GPIO_ResetOutputPin(SCL_GPIO_Port, SCL_Pin);
	delay_us(2);
	LL_GPIO_SetOutputPin(SCL_GPIO_Port, SCL_Pin);
	delay_us(2);
	LL_GPIO_SetOutputPin(SDA_GPIO_Port, SDA_Pin);
	delay_us(2);
}

void LCD_write(unsigned char *text)
{
	int n;
	I2C_Start();
	I2C_out(0x7C);	//LCD address
	I2C_out(0x40);	//data stream up next
	delay_us(40);
	for(n=0;n<16;n++)
	{
		I2C_out(text[n]);
		delay_us(40);
	}
	I2C_Stop();
}

void LCD_digit(unsigned char *digit, uint8_t counter){
	I2C_Start();
	I2C_out(0x7C);	//LCD address
		I2C_out(0x40);	//data stream up next
		delay_us(40);
		for(int i=0;i<counter;i++){
			I2C_out(digit[i]);
			delay_us(40);
		}

		I2C_Stop();
}
void LCD_Clear()
{
	I2C_Start();
	I2C_out(0x7C);	//LCD address
	I2C_out(0x00);	//One control byte,
	I2C_out(0x01);	//clear screen
	HAL_Delay(2);
	I2C_Stop();
}

void LCD_CursorOn(uint8_t ON)
{
	I2C_Start();
	I2C_out(0x7C);	//LCD address
	I2C_out(0x00);	//One control byte,
	if(ON) I2C_out(0x0D);	//clear screen
	else I2C_out(0x0C);
	delay_us(40);
	I2C_Stop();
}

void LCD_cursorXY(uint8_t x, uint8_t y)
{
	if((x>39) || (y>1)) return; 	//out of bounds, get bent
	if(y) x+=0x40;
	x+=0x80;
	I2C_Start();
	I2C_out(0x7C);	//LCD address
	I2C_out(0x00);	//One Control byte, RS=0
	I2C_out(x);
	delay_us(40);
	I2C_Stop();
}
void set_structure(){
		LCD_Clear();
		LCD_cursorXY(0,0);
		LCD_digit(scoutIDs,4);
		LCD_cursorXY(7,0);
		LCD_digit(scoutRegion,6);
		LCD_cursorXY(0,1);
		LCD_digit((unsigned char *)"M:",2);

}

void init_LCD()
{
	I2C_Start();
	I2C_out(0x7C);		//LCD address
	I2C_out(0x00);		//Control Byte
	I2C_out(0x38);		//8 bit, 2 lines, instruction table 0
	HAL_Delay(10);
	I2C_out(0x39);		//instruction table 1
	HAL_Delay(10);
	I2C_out(0x14);		//frequency setting
	delay_us(40);
	I2C_out(0x70);		//contrast setting
	delay_us(40);
	I2C_out(0x5E);		//power setting
	delay_us(40);
	I2C_out(0x6D);		//follower control
	delay_us(40);
	I2C_out(0x0D);		//cursor on
	delay_us(40);
	I2C_out(0x06);		//cursor increment, no shift
	HAL_Delay(10);
	I2C_out(0x01);		//clear display
	HAL_Delay(2);
	I2C_Stop();
	HAL_Delay(100);
	unsigned char TEXT[16]="Let's Testing!";
	LCD_write(TEXT);
	HAL_Delay(1000);
    LCD_Clear();
    set_structure();
}
