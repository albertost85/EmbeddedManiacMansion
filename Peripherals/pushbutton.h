/*
 * pushbutton.h
 *
 *  Created on: May 5, 2017
 *      Author: UdL
 */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef PUSHBUTTON_H_
#define PUSHBUTTON_H_


#ifdef __cplusplus
 extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx.h"

typedef enum {
	BUTTON_SELECT	= 0,
	BUTTON_START	= 1,
	BUTTON_A 		= 2,
	BUTTON_B		= 3
} Button_TypeDef;

typedef enum {
	BUTTON_MODE_GPIO = 0,
	BUTTON_MODE_EXTI = 1
} ButtonMode_TypeDef;


#define BUTTONn                     4  // Number of user pushbuttons

#define BUTTON_SEL_PIN           	GPIO_Pin_11
#define BUTTON_SEL_GPIO_PORT     	GPIOD
#define BUTTON_SEL_GPIO_CLK      	RCC_AHB1Periph_GPIOD
#define BUTTON_SEL_EXTI_LINE     	EXTI_Line11
#define BUTTON_SEL_EXTI_PORT_SOURCE	EXTI_PortSourceGPIOD
#define BUTTON_SEL_EXTI_PIN_SOURCE  EXTI_PinSource11
#define BUTTON_SEL_EXTI_IRQn        EXTI15_10_IRQn
#define BUTTON_SEL_EXTI_IRQHandler	EXTI15_10_IRQHandler

#define BUTTON_STA_PIN           	GPIO_Pin_15
#define BUTTON_STA_GPIO_PORT     	GPIOA
#define BUTTON_STA_GPIO_CLK      	RCC_AHB1Periph_GPIOA
#define BUTTON_STA_EXTI_LINE     	EXTI_Line15
#define BUTTON_STA_EXTI_PORT_SOURCE	EXTI_PortSourceGPIOA
#define BUTTON_STA_EXTI_PIN_SOURCE  EXTI_PinSource15
#define BUTTON_STA_EXTI_IRQn        EXTI15_10_IRQn
#define BUTTON_STA_EXTI_IRQHandler	EXTI15_10_IRQHandler

#define BUTTON_A_PIN           		GPIO_Pin_8
#define BUTTON_A_GPIO_PORT     		GPIOA
#define BUTTON_A_GPIO_CLK      		RCC_AHB1Periph_GPIOA
#define BUTTON_A_EXTI_LINE     		EXTI_Line8
#define BUTTON_A_EXTI_PORT_SOURCE	EXTI_PortSourceGPIOA
#define BUTTON_A_EXTI_PIN_SOURCE  	EXTI_PinSource8
#define BUTTON_A_EXTI_IRQn        	EXTI9_5_IRQn
#define BUTTON_A_EXTI_IRQHandler	EXTI9_5_IRQHandler

#define BUTTON_B_PIN           		GPIO_Pin_2
#define BUTTON_B_GPIO_PORT     		GPIOD
#define BUTTON_B_GPIO_CLK      		RCC_AHB1Periph_GPIOD
#define BUTTON_B_EXTI_LINE     		EXTI_Line2
#define BUTTON_B_EXTI_PORT_SOURCE	EXTI_PortSourceGPIOD
#define BUTTON_B_EXTI_PIN_SOURCE  	EXTI_PinSource2
#define BUTTON_B_EXTI_IRQn        	EXTI2_IRQn
#define BUTTON_B_EXTI_IRQHandler	EXTI2_IRQHandler


/* Functions ------------------------------------------------------------------*/
void PushButtonInit(Button_TypeDef Button, ButtonMode_TypeDef Button_Mode);
uint8_t PushButtonGetState(Button_TypeDef Button);

#ifdef __cplusplus
}
#endif

#endif /* PUSHBUTTON_H_ */
