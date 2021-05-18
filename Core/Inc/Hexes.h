/*
 * Hexes.h
 *
 *  Created on: Dec 16, 2020
 *      Author: jussi
 */

#ifndef INC_HEXES_H_
#define INC_HEXES_H_

#define hexfile "BST200.hex"
#define MAXLINE 100


//#define HEXDEBUG

uint8_t ReadHex();
unsigned int htoi (const char *ptr);
uint8_t ReadLine(uint16_t line,char *hexes);
uint8_t VerifyLine(char *hexes);
uint8_t WriteLine(char *hexes);
extern uint16_t ExtAdd;

#endif /* INC_HEXES_H_ */
