/*
 * Hexes.c
 *
 *  Created on: Dec 16, 2020
 *      Author: jussi
 */
#include <stdio.h>
#include <string.h>
#include "fatfs.h"
#include "usart.h"
#include "Hexes.h"
#include "spi.h"
#include "tim.h"

uint16_t ExtAdd=0;

#define HI(x) (uint8_t)(x >> 8)
#define LO(x) (uint8_t)(x & 0xFF)


uint8_t OpenFile()
{
	FRESULT fresult;
	uint8_t hexreturn;


	fresult = f_mount(&SDFatFS, "/", 1);
  	if (fresult != FR_OK)
  	{
  		myprintf ("Can't mount SD CARD (%d)\r\n",fresult);
  		return 1;
  	}
  	else
  	{
  		myprintf("SD CARD mounted\r\n");
  	}

	fresult = f_open (&SDFile, hexfile, FA_READ);
	if(fresult)
	{
	  	myprintf ("Can't open file (%d)\r\n",fresult);
	  	return 1;
	}
	else myprintf("Opened %s OK\r\n",hexfile);
	return 0;

}



uint8_t CloseFile()
{
	FRESULT fresult;

	fresult=f_close(&SDFile);

  	fresult = f_mount(NULL, "/", 1);
  	if (fresult == FR_OK)
  	{
  		myprintf ("SD CARD Unmounted\r\n");
  		return 0;
  	}
  	return 1;
}

uint8_t OpenSTM()
{
	uint8_t response=0;
	uint16_t ID=0;

	SPI_Transfer(0x5A);	//Start
	SPI_Transfer(0x00); //Get ACK
	response=SPI_Transfer(0xFF);
	if(response==0x79);	//ACK
	else return 1;
	SPI_Transfer(0x79); //ACK

	SPI_Transfer(0x5A);	//Start
	SPI_Transfer(0x02); //Get ID
	SPI_Transfer(0xFD); //Complement of above
	response=SPI_Transfer(0xFF);
	if(response==0x79);	//ACK
	else return 2;
	response=SPI_Transfer(0xFF);
	ID=SPI_Transfer(0xFF)<<8;
	ID+=SPI_Transfer(0xFF);
	response=SPI_Transfer(0xFF);
	if(response==0x79);	//ACK
	else return 3;
	if(ID!=0x458) return 4;		//STM32F410 ID

	SPI_Transfer(0x5A);	//Start
	SPI_Transfer(0x92); //Read Unprotect
	SPI_Transfer(0x6D); //Complement of above
	response=SPI_Transfer(0xFF);
	if(response==0x79);	//ACK
	else return 5;
	myprintf("Memory mass erase\r\n");
	HAL_Delay(4200);	//wait for mass erase	///////////////////////////HOW LOW CAN YOU GO?!?!?
	response=SPI_Transfer(0xFF);
	if(response==0x79);	//ACK
	else return 6;

	SPI_Transfer(0x5A);	//Start
	SPI_Transfer(0x73); //Write Unprotect
	SPI_Transfer(0x8C); //Complement of above
	response=SPI_Transfer(0xFF);
	if(response==0x79);	//ACK
	else return 7;
	response=SPI_Transfer(0xFF);
	if(response==0x79);	//ACK
	else return 8;
}

uint8_t CloseSTM()
{

}



uint8_t ProcessLines()
{
	uint16_t LineNr=0;
	char InputBuffer[(MAXLINE>>2)+5];

	for(;;)			//endless loop until end of file
	{
		LineNr++;
		myprintf("%d\r",LineNr);
		uint8_t res=ReadLine(LineNr,InputBuffer);
		if(res==0);			//line read ok
		else if(res==200)	//EOF detected
		{
			myprintf("\r\nEOF\r\n");
			break;
		}
		else
		{
			myprintf("Read error on line #%d",LineNr);
			return 1;		//error
		}

		if(InputBuffer[3]==0x00)
		{
			res=WriteLine(InputBuffer);
			if(res)
			{
				myprintf("Write error on line #%d",LineNr);
				return 2;
			}
			res=VerifyLine(InputBuffer);
			if(res)
			{
				myprintf("Verify error on line #%d",LineNr);
				return 3;
			}
		}
	}
	return 0;
}




uint8_t ReadLine(uint16_t line, char *hexes)		//line is only passed to keep track of debug output
{
	char buffer[MAXLINE];
	char asciipair[3];
	uint8_t bytecount;
	uint16_t address;
	uint8_t type;
	uint8_t CRCsum=0;

	f_gets(buffer, MAXLINE, &SDFile);

  	if(buffer[0]!=':')
  	{
  		myprintf ("Wrong start of line\r\n");
  		return 1;
  	}

  	char *point=buffer+1;					//first checking how many bytes on this row
  	strncpy(asciipair,point,2);
  	asciipair[2]='\0';
  	hexes[0]=htoi(asciipair);
  	bytecount=hexes[0];

#ifdef HEXDEBUG myprintf("Read values");
#endif
  	for(uint8_t i=0;i<bytecount+4;i++)		//read all in
  	{
  			point+=2;
  		  	strncpy(asciipair,point,2);
  		  	asciipair[2]='\0';
  		  	hexes[i+1]=htoi(asciipair);
#ifdef HEXDEBUG myprintf(" %02x,",hexes[i]);
#endif
  		  	CRCsum+=hexes[i];
  	}
#ifdef HEXDEBUG myprintf(" %02x.\r\n",hexes[bytecount+4]);
#endif

  	address=hexes[1]<<16+hexes[2];
  	type=hexes[3];
  	CRCsum=~CRCsum+1;
  	if(CRCsum!=hexes[bytecount+4])
  	{
  		myprintf("Checksum Error\r\n");
  		return 1;
  	}

#ifdef HEXDEBUG
  	myprintf("Read line #%i (%d characters) %s\r",line,strlen(buffer),buffer);
  	myprintf("Bytecount: %d, Address: %d, Record type: %d, CRC: %02x\r\n",bytecount, address, type,CRCsum);
#endif

  	if(type==1) return 200;									//end of file
  	else if(type==4) ExtAdd=hexes[4]<<8+hexes[5];			//Set extended address
  	else if(type==0) ;										//Actual data
  	return 0;											    //others types are cast into oblivion (Ext Segment Address, Start Seg Add, Start Linear Add)
}

uint8_t WriteLine(char *hexes)
{
	uint8_t response=0;
	uint8_t checksum=0;

	SPI_Transfer(0x5A);	//Start
	SPI_Transfer(0x31); //Write Memory
	SPI_Transfer(0xCE); //
	response=SPI_Transfer(0xFF);
	if(response==0x79);	//ACK
	else return 1;				//couldn't start command

	SPI_Transfer(HI(ExtAdd)); //MSB
	SPI_Transfer(LO(ExtAdd)); //
	SPI_Transfer(hexes[1]);   //
	SPI_Transfer(hexes[2]);   //LSB
	checksum=HI(ExtAdd)^LO(ExtAdd)^hexes[1]^hexes[2];
	SPI_Transfer(checksum);

	response=SPI_Transfer(0xFF);
	if(response==0x79);	//ACK
	else return 2;				//address error

	HAL_Delay(1);
	checksum=0;
	if(!hexes[0]) return 3;		//error, no data even though data frame!
	SPI_Transfer(hexes[0]-1);	//number of bytes-1
	for(uint8_t i=0;i<hexes[0];i++)
	{
		SPI_Transfer(hexes[4+i]);	//actual data
		checksum^=hexes[4+i];		//rolling XOR checksum calculation
	}
	delay_us(100);					//write delay

	response=SPI_Transfer(0xFF);
	if(response==0x79);	//ACK
	else return 4;				//data error

	return 0;
}

uint8_t VerifyLine(char *hexes)
{
	uint8_t response=0;
	uint8_t checksum=0;
	uint8_t readBack[MAXLINE>>2)+5]

	SPI_Transfer(0x5A);	//Start
	SPI_Transfer(0x11); //Write Memory
	SPI_Transfer(0xEE); //
	response=SPI_Transfer(0xFF);
	if(response==0x79);	//ACK
	else return 1;				//couldn't start command

	SPI_Transfer(HI(ExtAdd)); //MSB
	SPI_Transfer(LO(ExtAdd)); //
	SPI_Transfer(hexes[1]);   //
	SPI_Transfer(hexes[2]);   //LSB
	checksum=HI(ExtAdd)^LO(ExtAdd)^hexes[1]^hexes[2];
	SPI_Transfer(checksum);

	response=SPI_Transfer(0xFF);
	if(response==0x79);	//ACK
	else return 2;				//Address error

	SPI_Transfer(hexes[0]-1);	//number of bytes-1
	SPI_Transfer(~(hexes[0]-1));

	for(uint8_t i=0;i<hexes[0];i++)
	{
		readBack[i]=SPI_Transfer(0xFF);	//getting data
		if(readBack[i]!=hexes[4+i] return 3;		//verify error
	}

	return 0;
}



unsigned int htoi (const char *ptr)
{
unsigned int value = 0;
char ch = *ptr;

/*--------------------------------------------------------------------------*/

    while (ch == ' ' || ch == '\t')
        ch = *(++ptr);

    for (;;) {

        if (ch >= '0' && ch <= '9')
            value = (value << 4) + (ch - '0');
        else if (ch >= 'A' && ch <= 'F')
            value = (value << 4) + (ch - 'A' + 10);
        else if (ch >= 'a' && ch <= 'f')
            value = (value << 4) + (ch - 'a' + 10);
        else
            return value;
        ch = *(++ptr);
    }
}
