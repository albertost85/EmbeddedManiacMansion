/*
 * delay.h
 *
 *  Created on: Feb 5, 2012
 *      Author: Marcel
 *
 *  Modified on: Feb 16, 2014
 *
 *      Remember to modify the SysTick_Handler in stm32f4xx.h, writing
 *      inside Delay_TimingDecrement(); and include at top this header.
 *
 */

#ifndef DELAY_H_
#define DELAY_H_

#ifdef __cplusplus
 extern "C" {
#endif

#include "stm32f4xx.h"

void Delay_Init(void);
void Delay_TimingDecrement(void);
void Delay_ms(uint32_t nms);
void Delay_us(uint32_t nus);

#ifdef __cplusplus
}
#endif

#endif /* DELAY_H_ */
