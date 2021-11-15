/*
 * joystick.h
 *
 *  Created on: Oct 25, 2016
 *      Author: UdL
 */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef JOYSTICK_H_
#define JOYSTICK_H_


#ifdef __cplusplus
 extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx.h"
#include "arm_math.h"

#define JOY_X_PIN			GPIO_Pin_0
#define JOY_X_GPIO_PORT		GPIOB
#define JOY_X_GPIO_CLK		RCC_AHB1Periph_GPIOB
#define JOY_X_ADC_CH		ADC_Channel_8

#define JOY_Y_PIN			GPIO_Pin_1
#define JOY_Y_GPIO_PORT		GPIOB
#define JOY_Y_GPIO_CLK		RCC_AHB1Periph_GPIOB
#define JOY_Y_ADC_CH		ADC_Channel_9

#define JOY_ADC				ADC2
#define JOY_ADC_CLK			RCC_APB2Periph_ADC2


/* Functions ------------------------------------------------------------------*/
// Joystick Initialization
void Joystick_Init(void);

// Reads ADC conversion
void Joystick_ReadXY(uint16_t* xdata, uint16_t* ydata);

// Reads Pushbutton status
uint8_t Joystick_ReadPushButton(void);

// Reads all joystick data with XY axes in millimeters (from -10 to 10mm)
void Joystick_Read(float32_t* xdata, float32_t* ydata);



#ifdef __cplusplus
}
#endif

#endif /* JOYSTICK_H_ */
