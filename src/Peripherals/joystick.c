/*
 * joystick.c
 *
 *  Created on: Oct 25, 2016
 *      Author: UdL
 */

/* Includes ------------------------------------------------------------------*/
#include "joystick.h"

/* Variables -----------------------------------------------------------------*/

/* Private functions declaration ---------------------------------------------*/
static uint16_t readADC(ADC_TypeDef* ADCx, uint8_t ADC_CH);


// Joystick Initialization
//   - Configure the GPIO in the ADC12_IN8 (PB0 pin) as analog input
//   - Configure the GPIO in the ADC12_IN9 (PB1 pin) as analog input
//   - Configure the ADC2-Channel8 as ADC single regular conversion mode
//   - Configure the ADC2-Channel9 as ADC single regular conversion mode
void Joystick_Init(void) {
	GPIO_InitTypeDef 	  GPIO_InitStructure;
	ADC_InitTypeDef       ADC_InitStructure;
	ADC_CommonInitTypeDef ADC_CommonInitStructure;

	// Enable GPIO clocks
	RCC_AHB1PeriphClockCmd(JOY_X_GPIO_CLK | JOY_Y_GPIO_CLK, ENABLE);

	// Setup GPIO potentiometer pins as analog mode.
	// GPIO_Speed, GPIO_OType not used in AN mode
	GPIO_InitStructure.GPIO_Pin = JOY_X_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AN;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_Init(JOY_X_GPIO_PORT, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin = JOY_Y_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AN;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_Init(JOY_Y_GPIO_PORT, &GPIO_InitStructure);



	// ADC Clock enable
	RCC_APB2PeriphClockCmd(JOY_ADC_CLK, ENABLE);

	// ADC Common Init
	//   - ADC1 and ADC2 operate independently
	//   - ADCCLK 84MHz/DIV2 = 42MHz
	//   - DMA dual/triple mode access Disabled
	//   - 5 cycles delay between two different ADC samples (only in dual/triple mode)
	ADC_CommonInitStructure.ADC_Mode = ADC_Mode_Independent;
	ADC_CommonInitStructure.ADC_Prescaler = ADC_Prescaler_Div2;
	ADC_CommonInitStructure.ADC_DMAAccessMode = ADC_DMAAccessMode_Disabled;
	ADC_CommonInitStructure.ADC_TwoSamplingDelay = ADC_TwoSamplingDelay_5Cycles;
	ADC_CommonInit(&ADC_CommonInitStructure);

	// ADC2 Init
	//   - 12Bits Resolution,
	//   - Disable the multi-chanel (scan) conversion, Only one channel sequence
	//   - Don't do continuous conversions - do them on demand
	//   - Start conversion by software, not an external trigger
	//   - Data align right -> put 12-bit result in the lower variable bits
	ADC_InitStructure.ADC_Resolution = ADC_Resolution_12b;
	ADC_InitStructure.ADC_ScanConvMode = DISABLE;
	ADC_InitStructure.ADC_ContinuousConvMode = DISABLE;
	ADC_InitStructure.ADC_ExternalTrigConvEdge = ADC_ExternalTrigConvEdge_None;
	ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Left;
	ADC_InitStructure.ADC_NbrOfConversion = 1;
	ADC_Init(JOY_ADC, &ADC_InitStructure);


	// Enable ADC
	ADC_Cmd(JOY_ADC, ENABLE);

}


// Get the ADC conversion value
static uint16_t readADC(ADC_TypeDef* ADCx, uint8_t ADC_CH) {
	uint32_t value, i;

	// Select the channel of the ADC
	ADC_RegularChannelConfig(ADCx, ADC_CH, 1, ADC_SampleTime_3Cycles);

	// Do an regular conversion
	// Average of 10 conversions
	value = 0;
	for (i=0; i<10; i++) {
		// Start the conversion (not necessary in continuous mode)
		ADC_SoftwareStartConv(ADCx);

		// Wait until conversion completion
		while(ADC_GetFlagStatus(ADCx, ADC_FLAG_EOC) == RESET);

		// Get conversion result
		value = value + (ADC_GetConversionValue(ADCx)>>4);

	}
	value = value /10;


	return value;
}

// Reads ADC conversion
void Joystick_ReadXY(uint16_t* xdata, uint16_t* ydata) {
	*xdata = readADC(JOY_ADC, JOY_X_ADC_CH);
	*ydata = readADC(JOY_ADC, JOY_Y_ADC_CH);
}

// Reads all joystick data with XY axes in millimeters (from -10 to 10mm)
void Joystick_Read(float32_t* xdata, float32_t* ydata) {
	uint16_t x, y;

	// Reads ADC values
	Joystick_ReadXY(&x,&y);

	// Converts the digital conversion to degrees
	// 0 V   (   0 ADC) => -10 mm
	// 1.5 V (2047 ADC) =>   0 mm (is at the center)
	// 3 V   (4095 ADC) => +10 mm
	*xdata =      (((float32_t)x)*(20.0f/4095.0f) - 10.0f);
	// Deleted the -1, non inverted joystick
	*ydata = (((float32_t)y)*(20.0f/4095.0f) - 10.0f);

}



