/*
 * delay.c
 *
 *  Created on: Feb 5, 2012
 *      Author: Marcel
 *
 *  Modified on: Feb 16, 2014
 *
 */

#include "delay.h"

static __IO uint32_t timingDelay;
static __IO uint32_t delay_ticks_1us;
static __IO uint32_t delay_ticks_1ms;

// Configures the SysTick for the delay time functions
void Delay_Init(void) {

	RCC_ClocksTypeDef RCC_Clocks;

    // Especificar els ticks per fer una IT en 1ms o en 1us
    // ----------------------------------------------------
    	// Obtenim la freq. dels clocks
    	RCC_GetClocksFreq(&RCC_Clocks);

    	// Posem els ticks que es necessiten pel clock configurat
    	// SystemCoreClock es el mateix que  RCC_ClocksStatus.HCLK_Frequency = 168000000 Hz
    	delay_ticks_1us = RCC_Clocks.HCLK_Frequency / 1000000;
    	delay_ticks_1ms = RCC_Clocks.HCLK_Frequency / 1000;
    // ----------------------------------------------------

    // Configure the SysTick IRQ priority to the lowest value (0x0F).
    // Take care with using Delay function inside another function:
    // Then you have to increment the preemption priority of the systick!
    NVIC_SetPriority (SysTick_IRQn, (1<<__NVIC_PRIO_BITS) - 1);

	//Disable SysTick IRQ (Bit 0 i 1 a valor 0)
	SysTick->CTRL &= (~SysTick_CTRL_TICKINT_Msk & ~SysTick_CTRL_ENABLE_Msk);

    //SysTick clock source:  AHB clock selected as SysTick clock source (HCLK)
    SysTick_CLKSourceConfig(SysTick_CLKSource_HCLK);

}

// Funcio que es crida desde la interrupcio SysTick_Handler(void) de stm32f4xxit.c
void Delay_TimingDecrement(void) {
  if (timingDelay != 0x00) {
    timingDelay--;
  }
}


// Delay de x ms
void Delay_ms(uint32_t nms) {
	// Adjust the SysTick time base
	SysTick->LOAD  = (delay_ticks_1ms & SysTick_LOAD_RELOAD_Msk) - 1;

	// Reset the SysTick Counter register
	SysTick->VAL = 0;

	// Start counter num SysTick interrupts
	timingDelay = nms;

	//Enable SysTick IRQ and SysTick Timer (Bit 0, 1 i 2 a valor 1)
	SysTick->CTRL = SysTick_CTRL_CLKSOURCE_Msk | 			// SysTick Timer
					SysTick_CTRL_TICKINT_Msk   |			// SysTick IRQ
					SysTick_CTRL_ENABLE_Msk;                // Function successful

	// Wait the time specified
	while(timingDelay != 0);

	//Disable SysTick IRQ (Bit 0 i 1 a valor 0)
	SysTick->CTRL &= (~SysTick_CTRL_TICKINT_Msk & ~SysTick_CTRL_ENABLE_Msk);

}

// Delay de x us
void Delay_us(uint32_t nus) {
	// Adjust the SysTick time base
	SysTick->LOAD  = (delay_ticks_1us & SysTick_LOAD_RELOAD_Msk) - 1;

	// Reset the SysTick Counter register
	SysTick->VAL = 0;

	// Start counter num SysTick interrupts
	timingDelay = nus;
	if (timingDelay > 1)  timingDelay = timingDelay - 1; // Ajusta el que esperd executant aquestes instruccions (1us)

	//Enable SysTick IRQ and SysTick Timer (Bit 0, 1 i 2 a valor 1)
	SysTick->CTRL = SysTick_CTRL_CLKSOURCE_Msk | 			// SysTick Timer
					SysTick_CTRL_TICKINT_Msk   |			// SysTick IRQ
					SysTick_CTRL_ENABLE_Msk;               // Function successful

	// Wait the time specified
	while(timingDelay != 0);

	//Disable SysTick IRQ (Bit 0 i 1 a valor 0)
	SysTick->CTRL &= (~SysTick_CTRL_TICKINT_Msk & ~SysTick_CTRL_ENABLE_Msk);
}



