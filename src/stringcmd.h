/*
 * stringcmd.h
 *
 *  Created on: Mar 29, 2012
 *      Author: Marcel
 */

#ifndef STRINGCMD_H_
#define STRINGCMD_H_

#include "stm32f4xx.h"

#define CMD_OF   500

// Macro de valor absolut
#ifndef ABS
#define ABS(X)  ((X) > 0 ? (X) : -(X))
#endif

// Functions
void getWord(char dest[], uint8_t pos, char source[]);
void strCat(char d[], char s[]);
uint8_t isStrEqual(char d[], char s[]);
uint8_t isSubStrEqual(char d[], char s[], uint16_t start, uint16_t end);
void strCopy(char d[], char s[]);
uint16_t strLen(char s[]);
uint8_t AreNumbers(char s[], uint16_t len);
uint8_t isNumber(char d[]);
int32_t str2num(char s[]);
void num2str(int32_t num, char* s);
uint32_t num2binary(uint8_t dada);
uint8_t bit_test(uint16_t dada, uint8_t pos);
void float2str(float num, uint8_t precision, char* s);

#endif /* STRINGCMD_H_ */
