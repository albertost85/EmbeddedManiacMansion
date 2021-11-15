/*
 * led.c
 *
 *  Created on: Oct 1, 2013
 *      Author: UdL
 */

/* Includes ------------------------------------------------------------------*/
#include "led.h"

/* Variables -----------------------------------------------------------------*/
GPIO_TypeDef*	LED_GPIO_PORT[LEDn] = {LED_GREEN_GPIO_PORT, LED_ORANGE_GPIO_PORT, LED_RED_GPIO_PORT, LED_BLUE_GPIO_PORT};
const uint16_t	LED_GPIO_PIN[LEDn]  = {LED_GREEN_PIN, LED_ORANGE_PIN, LED_RED_PIN, LED_BLUE_PIN};
const uint32_t	LED_GPIO_CLK[LEDn]  = {LED_GREEN_GPIO_CLK, LED_ORANGE_GPIO_CLK, LED_RED_GPIO_CLK, LED_BLUE_GPIO_CLK};


// Initialization of the LED
void LEDInit(Led_TypeDef Led) {
	  GPIO_InitTypeDef  GPIO_InitStructure;

	  /* Enable the GPIO_LED Clock */
	  RCC_AHB1PeriphClockCmd(LED_GPIO_CLK[Led], ENABLE);

	  /* Configure the GPIO_LED pin */
	  GPIO_InitStructure.GPIO_Pin = LED_GPIO_PIN[Led];
	  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	  GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_NOPULL;
	  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	  GPIO_Init(LED_GPIO_PORT[Led], &GPIO_InitStructure);
}

// Switch the LED On
void LEDOn(Led_TypeDef Led) {
	GPIO_SetBits(LED_GPIO_PORT[Led], LED_GPIO_PIN[Led]);
}

// Switch the LED Off
void LEDOff(Led_TypeDef Led) {
	GPIO_ResetBits(LED_GPIO_PORT[Led], LED_GPIO_PIN[Led]);
}

// Toggle the LED
void LEDToggle(Led_TypeDef Led) {
	GPIO_ToggleBits(LED_GPIO_PORT[Led], LED_GPIO_PIN[Led]);
}


// Initialization of ALL LEDS
void LEDInitAll(void) {
	uint8_t i;
	for (i=0; i<LEDn;i++) LEDInit(i);

}

// Switch ALL the LEDs On
void LEDOnAll(void) {
	uint8_t i;
	for (i=0; i<LEDn;i++) LEDOn(i);
}

// Switch ALL the LEDs Off
void LEDOffAll(void) {
	uint8_t i;
	for (i=0; i<LEDn;i++) LEDOff(i);
}

// Toggle ALL the LEDs
void LEDToggleAll(void) {
	uint8_t i;
	for (i=0; i<LEDn;i++) LEDToggle(i);
}





