/*
 * random.h
 *
 *  Created on: Feb 21, 2017
 *      Author: UdL
 *
 *
 *      WARNING: REMEMBER TO ACTIVATE THE PLL48MHZ CLOCK
 */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef RANDOM_H_
#define RANDOM_H_


#ifdef __cplusplus
 extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx.h"
#include "arm_math.h"

 // Timeout for new data waiting
 #define RANDOM_TIMEOUT	10000

/* Functions ------------------------------------------------------------------*/
// Random Number Generator Initialization
void Random_Init(void);

// Random get number
uint8_t Random_GetNum(uint32_t* number);

#ifdef __cplusplus
}
#endif

#endif /* RANDOM_H_ */
