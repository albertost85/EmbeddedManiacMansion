/*
 * pushbutton.c
 *
 *  Created on: May 5, 2017
 *      Author: UdL
 */

/* Includes ------------------------------------------------------------------*/
#include "pushbutton.h"

/* Variables -----------------------------------------------------------------*/
GPIO_TypeDef*  BUTTON_PORT[BUTTONn] = {BUTTON_SEL_GPIO_PORT, BUTTON_STA_GPIO_PORT, BUTTON_A_GPIO_PORT, BUTTON_B_GPIO_PORT};
const uint16_t BUTTON_PIN[BUTTONn] =  {BUTTON_SEL_PIN, BUTTON_STA_PIN, BUTTON_A_PIN, BUTTON_B_PIN};
const uint32_t BUTTON_CLK[BUTTONn] = {BUTTON_SEL_GPIO_CLK, BUTTON_STA_GPIO_CLK, BUTTON_A_GPIO_CLK, BUTTON_B_GPIO_CLK};
const uint16_t BUTTON_EXTI_LINE[BUTTONn] = {BUTTON_SEL_EXTI_LINE, BUTTON_STA_EXTI_LINE, BUTTON_A_EXTI_LINE, BUTTON_B_EXTI_LINE};
const uint8_t  BUTTON_PORT_SOURCE[BUTTONn] = {BUTTON_SEL_EXTI_PORT_SOURCE, BUTTON_STA_EXTI_PORT_SOURCE, BUTTON_A_EXTI_PORT_SOURCE, BUTTON_B_EXTI_PORT_SOURCE};
const uint8_t  BUTTON_PIN_SOURCE[BUTTONn] = {BUTTON_SEL_EXTI_PIN_SOURCE, BUTTON_STA_EXTI_PIN_SOURCE, BUTTON_A_EXTI_PIN_SOURCE, BUTTON_B_EXTI_PIN_SOURCE };
const uint8_t  BUTTON_IRQn[BUTTONn] = {BUTTON_SEL_EXTI_IRQn, BUTTON_STA_EXTI_IRQn, BUTTON_A_EXTI_IRQn, BUTTON_B_EXTI_IRQn};


// Initialization of PushButton
void PushButtonInit(Button_TypeDef Button, ButtonMode_TypeDef Button_Mode) {
	GPIO_InitTypeDef GPIO_InitStructure;
	EXTI_InitTypeDef EXTI_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;

	/* Enable the BUTTON Clock */
	RCC_AHB1PeriphClockCmd(BUTTON_CLK[Button], ENABLE);

	/* Configure Button pin as input */
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_InitStructure.GPIO_Pin = BUTTON_PIN[Button];
	GPIO_Init(BUTTON_PORT[Button], &GPIO_InitStructure);

	if (Button_Mode == BUTTON_MODE_EXTI) {
		/* Enable the SYSCFG Clock */
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE);

		/* Connect Button EXTI Line to Button GPIO Pin */
		SYSCFG_EXTILineConfig(BUTTON_PORT_SOURCE[Button], BUTTON_PIN_SOURCE[Button]);

		/* Configure Button EXTI line */
		EXTI_InitStructure.EXTI_Line = BUTTON_EXTI_LINE[Button];
		EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
		EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising;
		EXTI_InitStructure.EXTI_LineCmd = ENABLE;
		EXTI_Init(&EXTI_InitStructure);

		/* Enable and set Button EXTI Interrupt to the lowest priority */
		NVIC_InitStructure.NVIC_IRQChannel = BUTTON_IRQn[Button];
		NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x0F;
		NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x0F;
		NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;

		NVIC_Init(&NVIC_InitStructure);
	}
}

// Get the Pushbutton state
uint8_t PushButtonGetState(Button_TypeDef Button) {
  return !GPIO_ReadInputDataBit(BUTTON_PORT[Button], BUTTON_PIN[Button]);
}


/*
// This function handles EXTIx interrupt request.
void BUTTON_SELSTA_EXTI_IRQHandler(void) {

	//Check if EXTIx is asserted
	if(EXTI_GetITStatus(BUTTON_SEL_EXTI_LINE) != RESET) {

		// Do code here
		// ...
		flagButtonSelect=1;

		// We need to clear line pending bit manually
		EXTI_ClearITPendingBit(BUTTON_SEL_EXTI_LINE);
	}
	//Check if EXTIx is asserted
	if(EXTI_GetITStatus(BUTTON_STA_EXTI_LINE) != RESET) {

		// Do code here
		// ...
		flagButtonStart=1;

		// We need to clear line pending bit manually
		EXTI_ClearITPendingBit(BUTTON_STA_EXTI_LINE);
	}
}



// This function handles EXTIx interrupt request.
void BUTTON_SEL_EXTI_IRQHandler(void) {

	//Check if EXTIx is asserted
	if(EXTI_GetITStatus(BUTTON_SEL_EXTI_LINE) != RESET) {

		// Do code here
		// ...
		//LEDToggle(LED_GREEN);

		// We need to clear line pending bit manually
		EXTI_ClearITPendingBit(BUTTON_SEL_EXTI_LINE);
	}
}

// This function handles EXTIx interrupt request.
void BUTTON_STA_EXTI_IRQHandler(void) {

	//Check if EXTIx is asserted
	if(EXTI_GetITStatus(BUTTON_STA_EXTI_LINE) != RESET) {

		// Do code here
		// ...
		//LEDToggle(LED_GREEN);

		// We need to clear line pending bit manually
		EXTI_ClearITPendingBit(BUTTON_STA_EXTI_LINE);
	}
}

// This function handles EXTIx interrupt request.
void BUTTON_A_EXTI_IRQHandler(void) {

	//Check if EXTIx is asserted
	if(EXTI_GetITStatus(BUTTON_A_EXTI_LINE) != RESET) {

		// Do code here
		// ...
		//LEDToggle(LED_GREEN);

		// We need to clear line pending bit manually
		EXTI_ClearITPendingBit(BUTTON_A_EXTI_LINE);
	}
}

// This function handles EXTIx interrupt request.
void BUTTON_B_EXTI_IRQHandler(void) {

	//Check if EXTIx is asserted
	if(EXTI_GetITStatus(BUTTON_B_EXTI_LINE) != RESET) {

		// Do code here
		// ...
		//LEDToggle(LED_GREEN);

		// We need to clear line pending bit manually
		EXTI_ClearITPendingBit(BUTTON_B_EXTI_LINE);
	}
}

*/






