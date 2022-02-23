/*
 * led.h
 *
 *  Created on: Oct 1, 2013
 *      Author: UdL
 */
//
/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef LED_H_
#define LED_H_


#ifdef __cplusplus
 extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx.h"

typedef enum  {
	LED_GREEN	= 0,
	LED_ORANGE	= 1,
	LED_RED		= 2,
	LED_BLUE	= 3
} Led_TypeDef;

#define LEDn                             	4  // Number of LEDS

#define LED_GREEN_PIN                       GPIO_Pin_12
#define LED_GREEN_GPIO_PORT                 GPIOD
#define LED_GREEN_GPIO_CLK                  RCC_AHB1Periph_GPIOD

#define LED_ORANGE_PIN                      GPIO_Pin_13
#define LED_ORANGE_GPIO_PORT                GPIOD
#define LED_ORANGE_GPIO_CLK                 RCC_AHB1Periph_GPIOD

#define LED_RED_PIN                         GPIO_Pin_14
#define LED_RED_GPIO_PORT                   GPIOD
#define LED_RED_GPIO_CLK                    RCC_AHB1Periph_GPIOD

#define LED_BLUE_PIN                        GPIO_Pin_15
#define LED_BLUE_GPIO_PORT                  GPIOD
#define LED_BLUE_GPIO_CLK                   RCC_AHB1Periph_GPIOD




/* Functions ------------------------------------------------------------------*/
void LEDInit(Led_TypeDef Led);
void LEDOn(Led_TypeDef Led);
void LEDOff(Led_TypeDef Led);
void LEDToggle(Led_TypeDef Led);
void LEDInitAll(void);
void LEDOnAll(void);
void LEDOffAll(void);
void LEDToggleAll(void);


#ifdef __cplusplus
}
#endif

#endif /* LED_H_ */
