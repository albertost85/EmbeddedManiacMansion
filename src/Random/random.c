/*
 * random.c
 *
 *  Created on: Feb 21, 2017
 *      Author: UdL
 *
 *      WARNING: REMEMBER TO ACTIVATE THE PLL48MHZ CLOCK
 *
 */

/* Includes ------------------------------------------------------------------*/
#include "random.h"

/* Variables -----------------------------------------------------------------*/

/* Private functions declaration ---------------------------------------------*/

// Random Number Generator Initiallization
void Random_Init(void) {
	// Restart the hardware peripheral
	RNG_Cmd(DISABLE);
	RNG_DeInit();

	// Enable Peripheral Clock
	RCC_AHB2PeriphClockCmd(RCC_AHB2Periph_RNG, ENABLE);

	// Enable the RNG
	RNG_Cmd(ENABLE);
}


// Random get number
uint8_t Random_GetNum(uint32_t* number) {
	uint32_t timeout;

	// Wait flag data ready
	timeout = RANDOM_TIMEOUT;
	while(RNG_GetFlagStatus(RNG_FLAG_DRDY)== RESET){
		if((timeout--) == 0) return 0;
	}

	// Check flag of Clock Error Detection
	if (RNG_GetFlagStatus(RNG_FLAG_CECS) == SET) {
		RNG_ClearFlag(RNG_FLAG_CECS);
		Random_Init();
		return 0;
	}

	// Check flag of Seed Error Detection
	if(RNG_GetFlagStatus(RNG_FLAG_SECS)== SET){
		RNG_ClearFlag(RNG_FLAG_SECS);
		Random_Init();
		return 0;
	}

	// Get a 32bit Random number
	*number = RNG_GetRandomNumber();

	return 1;
}


