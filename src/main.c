/****************************************************************************
**
**
**		PA 8   -->  A
**		PA 10  -->  B
**		PA 15  -->  SELECT
**		PD 11  -->  START
**
**		PB0	   -->	Joystick X
**		PB2	   -->	Joystick Y
**
**
**
*****************************************************************************/

// Includes

#include "stm32f4xx.h"
#include ".\Peripherals\joystick.h"
#include ".\Peripherals\led.h"
#include ".\Peripherals\pushbutton.h"
#include ".\SysTick\delay.h"
#include ".\Tone\tone.h"
#include ".\USART\usart.h"
#include ".\Random\random.h"
#include "stringcmd.h"
#include "arm_math.h"

// FatFs Library implementation header,
// disc I/O functions through SDIO write/read operations
#include "ff.h"
#include ".\SDCard\stm32f4_discovery_sdio_sd.h"

// LCD driver implementation
#include ".\LCD\lcd_ssd2119.h"
#include ".\LCD\image_bmp.h"
#include ".\LCD\image_flash.h"

// Touch screen (STMPE811QTR) driver and calibration functions
#include ".\LCD\ts_stmpe811.h"
#include ".\LCD\ts_calibration.h"

// GAMES
#include ".\Games\game_mmmania.h"

// General timer
void TIM12_Config(void);
uint32_t tLocalTime = 0;

// External function prototypes (defined in tiny_printf.c)
extern int printf(const char *fmt, ...);
extern int sprintf(char* str, const char *fmt, ...);

// File system object structure
static FATFS fatfs;



//Main program
int main(void) {
	TS_ResCode resTS;
	SD_Error Status = SD_OK;

	// Configures SysTick for delay functions
	Delay_Init();

	// Initialize PushButtons
	PushButtonInit(BUTTON_SELECT, BUTTON_MODE_GPIO);
	PushButtonInit(BUTTON_START, BUTTON_MODE_GPIO);
	PushButtonInit(BUTTON_A, BUTTON_MODE_GPIO);
	PushButtonInit(BUTTON_B, BUTTON_MODE_GPIO);

	// Initialize Joystick
	Joystick_Init();

	// USART2 Initialization
	USARTx_Init(USART_MODE_IRQ);

	// Tone Init in IRQ mode
	Tone_Init(TONE_MODE_IRQ);
	Tone_SetVolume(70);
	Tone_PlayRTTTL(Meloy_Tones[Tone_SmartAlert]);
	//while(Tone_isPlaying() == SET); // No wait


	// Initialize the LCD
	LCD_Init();
	LCD_SetOrientation(LCD_LANDSCAPE);

	// Initialize the TS (Touch Screen)
	resTS = TS_Init();
	if (resTS != TS_Success) {
		// Initialization ERROR!
		LCD_DisplayString(0,0, "TS Init Fail!", &Font12, COLOR_RED, COLOR_BLACK, 0);
		while(1);
	}

	// Calibration relationship parameters for portait orientation
	// xFactor, xOffset, yFactor and yOffset
	TS_SetCalParam((float)-0.0842, -331,(float)0.0646, 10);

	//TS_doCalibraion(); // Uncomment to do the touch calibration
	//TS_Test(); // Uncomment if we want to test the current touch calibration parameters

	// Check if SD card is present
	if(SD_Detect() != SD_PRESENT) {
		LCD_DisplayString(0,0, "Please, insert the SD...", &Font12, COLOR_RED, COLOR_BLACK, 0);

		// Wait the SD connection
		while(SD_Detect() != SD_PRESENT);
		Delay_ms(100);
	}

	// Initialize the SD Card (SDIO, DMAs, ...)
	if ((Status = SD_Init()) != SD_OK) {
		LCD_DisplayString(0,0, "Error in SD Card Init!!", &Font12, COLOR_RED, COLOR_BLACK, 0);
		while(1);
	}

	// Mount logical drive
	if (f_mount(&fatfs, "0:", 1) != FR_OK) {
		LCD_DisplayString(0,0, "Error mounting the FatFS!!", &Font12, COLOR_RED, COLOR_BLACK, 0);
		while(1);
	}
	/*
	// Random Peripheral Initialization, ni falta que hace
	// WARNING: The PLL48MHz clock must be activated
	Random_Init();
	*/
	// Config TIM12 general timer interrupt
	TIM12_Config();


	printf("Initialization OK!\r");



	while(1) {

		// START THE GAME:


		LCD_SetOrientation(LCD_LANDSCAPE);
		MMMania_Run();


	}

}





/**
  * @note   PCLK1 = HCLK/4
  * 		TIM12CLK = (PCLK1)x2 = (168 MHz/4)x2 = 84 MHz
  *
  * 		so CLK_INT = TIM12CLK/((Preescaler+1) * Clock_Division)
  *				TIM12 update time = (1/CLK_INT) * (Period+1)
  *				TIM12 update freq = CLK_INT / (Period+1)
  *
  *			Own Config:
  *			-----------
  *			Precaler = 199
  *			Period = 4199 => 4200 ticks (from 0 to 4199)
  *			CLK_INT = 84MHz/((199+1) * 1) = 420KHz
  *         TIM12 update freq = 420KHz / (419 + 1) = 1000 Hz => 1 ms
  */
void TIM12_Config(void) {
	TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
	NVIC_InitTypeDef NVIC_InitStructure;

	// TIM12 clock enable
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM12, ENABLE);

	// Configure the NVIC Priority Grouping to 2:
	//    - 2 bits for preemption prioryty
	//    - 2 bits for subpriority
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);

	// NVIC Configuration
	// Enable the TIM6 gloabal Interrupt with lower priority (with NVIC_PriorityGroup_2)
	NVIC_InitStructure.NVIC_IRQChannel = TIM8_BRK_TIM12_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x0F;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x0F;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);

	// Time base configuration
	TIM_TimeBaseStructure.TIM_Period = 419;
	TIM_TimeBaseStructure.TIM_Prescaler = 199;
	TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TimeBaseInit(TIM12, &TIM_TimeBaseStructure);

	// Set Counter to 0
	TIM_SetCounter(TIM12, 0);

	// Clear Interrupt pending bit
	TIM_ClearITPendingBit(TIM12, TIM_IT_Update);

	// Enable Interruption of peripheral TIM6 Update
	TIM_ITConfig(TIM12, TIM_IT_Update, ENABLE);

	// Enable Timer 12 (ALWAYS ON)
	TIM_Cmd(TIM12, ENABLE);
}


/**
  * @brief  This function handles TIM12 global interrupt request.
  * @param  None
  * @retval None
  */
void TIM8_BRK_TIM12_IRQHandler(void) {

	// Check if TIM12 Interrupt update is asserted
	if (TIM_GetITStatus(TIM12, TIM_IT_Update) != RESET) {

		// This code is executed when TIM12 counter has an overflow
		// (the counter value rise the Auto-reload specified value)
		// ...
		tLocalTime  += 1;

		// Game Timer Callbacks
		MMMania_Callback(tLocalTime);
		/*
		if (currentGame == GAME_ARKANOID) {
			Arkanoid_Callback(tLocalTime);
		} else if (currentGame == GAME_MINESWEEPER) {
			MW_Callback(tLocalTime);
		}
		*/
		// We need to clear line pending bit manually
		TIM_ClearITPendingBit(TIM12, TIM_IT_Update);
	}
}

