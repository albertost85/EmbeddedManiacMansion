/*
 * ts_stmpe811.h
 *
 *  Created on: Maig 1, 2014
 *      Author: Marcel
 *
 * Description: Touch Screen (TS) driver for the STMPE811QTR from STMicroelectronics
 *              It uses the I2C for the communication
 *
 */
#ifndef __TS_STMPE811_H
#define __TS_STMPE811_H

#ifdef __cplusplus
 extern "C" {
#endif

// Includes
#include "stm32f4xx.h"
#include ".\..\Systick\delay.h"

// I2Cx timeout waiting value (set for system running at 168 MHz).
#define TS_I2C_TIMEOUT		50000

// I2Cx clock speed configuration (in Hz)
#define TS_I2C_SPEED		30000

// Configuration for I2C Nº1
#define TS_I2Cx					I2C1
#define TS_I2Cx_CLK				RCC_APB1Periph_I2C1

#define TS_I2Cx_SCL_PIN         GPIO_Pin_8
#define TS_I2Cx_SCL_GPIO_PORT   GPIOB
#define TS_I2Cx_SCL_GPIO_CLK    RCC_AHB1Periph_GPIOB
#define TS_I2Cx_SCL_SOURCE      GPIO_PinSource8
#define TS_I2Cx_SCL_AF          GPIO_AF_I2C1

#define TS_I2Cx_SDA_PIN         GPIO_Pin_9
#define TS_I2Cx_SDA_GPIO_PORT   GPIOB
#define TS_I2Cx_SDA_GPIO_CLK    RCC_AHB1Periph_GPIOB
#define TS_I2Cx_SDA_SOURCE      GPIO_PinSource9
#define TS_I2Cx_SDA_AF          GPIO_AF_I2C1

// Struct definition for the state
typedef struct{
   uint16_t touchDetected;
   uint16_t x;
   uint16_t y;
   uint16_t z;
} TS_State;


// Communication result code
typedef enum {
	TS_Success 				= 0,
	TS_Chip_Not_Detected,
	TS_I2C_Timeout_Error,
} TS_ResCode;

// TS slave Address (with A0 to ground => 0x82)
typedef enum{
	TS_ADDR_WR    = 0x82,
	TS_ADDR_RD    = 0x82 | 0x01  // LSB to '1' indicating read operation
}TS_Address_TypeDef;


// TS data length types
typedef enum{
	TS_DATA_1Byte     = 0,
	TS_DATA_2Bytes    = 1
}TS_DataLength_TypeDef;

// LIST OF STMPE811QTR REGISTERS
typedef enum  {
	// Identification registers
	TS_REG_CHIP_ID 		= 0x00, // (16) Device identification (returns 0x811)
	TS_REG_ID_VER		= 0x02, // ( 8) Device version

	//General Control registers
	TS_REG_SYS_CTRL1	= 0x03, // ( 8) Reset control
	TS_REG_SYS_CTRL2	= 0x04, // ( 8) Clock control
	TS_REG_SPI_CFG		= 0x08, // ( 8) SPI interface configuration

	// Interrupt Control registers
	TS_REG_INT_CTRL		= 0x09, // ( 8) Interrupt control
	TS_REG_INT_EN		= 0x0A, // ( 8) Interrupt enable
	TS_REG_INT_STA		= 0x0B, // ( 8) Interrupt status
	TS_REG_GPIO_INT_EN	= 0x0C, // ( 8) GPIO interrupt enable
	TS_REG_GPIO_INT_STA	= 0x0D, // ( 8) GPIO interrupt status

	// GPIO Registers
	TS_REG_GPIO_SET_PIN	= 0x10, // ( 8) GPIO set pin
	TS_REG_GPIO_CLR_PIN	= 0x11, // ( 8) GPIO clear pin
	TS_REG_GPIO_MP_STA	= 0x12, // ( 8) GPIO monitor pin state
	TS_REG_GPIO_DIR		= 0x13, // ( 8) GPIO direction
	TS_REG_GPIO_ED		= 0x14, // ( 8) GPIO edge detect
	TS_REG_GPIO_RE		= 0x15, // ( 8) GPIO rising edge
	TS_REG_GPIO_FE		= 0x16, // ( 8) GPIO falling edge
	TS_REG_GPIO_AF		= 0x17, // ( 8) GPIO alternate function

	// ADC Registers
	TS_REG_ADC_INT_EN	= 0x0E, // ( 8) ADC interrupt enable
	TS_REG_ADC_INT_STA	= 0x0F, // ( 8) ADC interrupt status
	TS_REG_ADC_CTRL1	= 0x20, // ( 8) ADC control
	TS_REG_ADC_CTRL2	= 0x21, // ( 8) ADC control
	TS_REG_ADC_CAPT		= 0x22, // ( 8) To initialize the ADC data
	TS_REG_ADC_DATA_CH0	= 0x30, // (16) ADC channel 0 data
	TS_REG_ADC_DATA_CH1	= 0x32, // (16) ADC channel 1 data
	TS_REG_ADC_DATA_CH2	= 0x34, // (16) ADC channel 2 data
	TS_REG_ADC_DATA_CH3	= 0x36, // (16) ADC channel 3 data
	TS_REG_ADC_DATA_CH4	= 0x38, // (16) ADC channel 4 data
	TS_REG_ADC_DATA_CH5	= 0x3A, // (16) ADC channel 5 data
	TS_REG_ADC_DATA_CH6 = 0x3B, // (16) ADC channel 6 data
	TS_REG_ADC_DATA_CH7	= 0x3C, // (16) ADC channel 7 data

	// TouchScreen Registers
	TS_REG_TSC_CTRL		= 0x40, // ( 8) 4-wire touch screen setup
	TS_REG_TSC_CFG		= 0x41, // ( 8) Touch screen controller configuration
	TS_REG_WDM_TR_X		= 0x42, // (16) Window setup for top right X
	TS_REG_WDM_TR_Y		= 0x44, // (16) Window setup for top right Y
	TS_REG_WDM_BL_X		= 0x46, // (16) Window setup for bottom left X
	TS_REG_WDM_BL_Y		= 0x48, // (16) Window setup for bottom left Y
	TS_REG_FIFO_TH		= 0x4A, // ( 8) FIFO treshold level for generate the interrupt
	TS_REG_FIFO_STA		= 0x4B, // ( 8) Current status of the FIFO
	TS_REG_FIFO_SIZE	= 0x4C, // ( 8) Current filled FIFO level
	TS_REG_TSC_DATA_X	= 0x4D, // (16) Touch screen X data
	TS_REG_TSC_DATA_Y	= 0x4F, // (16) Touch screen Y data
	TS_REG_TSC_DATA_Z   = 0x51, // ( 8) Touch screen Z data
	TS_REG_TSC_DATA_XYZ	= 0x52, // (32) Touch screen XYZ data
	TS_REG_TSC_FRACT_Z	= 0x56, // ( 8) Touch screen fraction of Z
	TS_REG_TSC_DATA		= 0x57, // ( 8) Touch screen data
	TS_REG_TSC_I_DRIVE	= 0x58, // ( 8) Touch screen controller drive I
	TS_REG_TSC_SHIELD	= 0x59, // ( 8) Touch screen controller shield

	// Temperature Sensor registers
	TS_REG_TEMP_CTRL	= 0x60, // ( 8) Temperature sensor setup
	TS_REG_TEMP_DATA	= 0x61, // ( 8) Temperature data access
	TS_REG_TEMP_TH		= 0x62, // ( 8) Treshold for temperature interrupt
} TS_REG_TypeDef;


// Functionalities definitions
//	# TS_REG_SYS_CTRL2
#define FUN_ADC		0x01 // ADC Function
#define FUN_TSC		0x02 // TouchScreen Control Function
#define FUN_GPIO	0x04 // GPIO Function
#define FUN_TEMP	0x08 // Temperature Sensor Function
#define FUN_ALL     (uint8_t)(FUN_ADC_ON | FUN_TSC_ON | FUN_GPIO_ON | FUN_TEMP_ON)

// Global Interrupts definition
//	# TS_REG_INT_EN
//	# TS_REG_INT_STA
#define GIT_TOUCH_DET	0x01
#define GIT_FIFO_TH		0x02
#define GIT_FIFO_OFLOW	0x04
#define GIT_FIFO_FULL	0x08
#define GIT_FIFO_EMPTY	0x10
#define GIT_TEMP_SENS	0x20
#define GIT_ADC			0x40
#define GIT_GPIO		0x80
#define GIT_ALL			0xFF

// IO pins definitions:
// 	# TS_REG_GPIO_AF
// 	# TS_REG_GPIO_DIR
// 	# TS_REG_GPIO_FE
// 	# TS_REG_GPIO_RE
#define IO_Pin_0		0x01
#define IO_Pin_1		0x02
#define IO_Pin_2		0x04
#define IO_Pin_3		0x08
#define IO_Pin_4		0x10
#define IO_Pin_5		0x20
#define IO_Pin_6		0x40
#define IO_Pin_7		0x80
#define IO_Pin_ALL		0xFF
#define IO_TOUCH_YD		IO_Pin_1 // Input
#define IO_TOUCH_XD		IO_Pin_2 // Input
#define IO_TOUCH_YU		IO_Pin_3 // Input
#define IO_TOUCH_XU		IO_Pin_4 // Input
#define TOUCH_IO_ALL    (uint32_t)(IO_Pin_1 | IO_Pin_2 | IO_Pin_3 | IO_Pin_4)



/* F  U  N  C  T  I  O  N  S                          */
/******************************************************/

// Initialization functions
TS_ResCode TS_Init(void);
void TS_GPIOConfig(void);
void TS_InitI2C(void);
TS_ResCode TS_ControllerConfig(void);

// Configuration
TS_ResCode TS_FunctionsConfig(uint8_t Fun, FunctionalState NewState);
TS_ResCode TS_ITConfig(uint8_t Global_IT, FunctionalState NewState);
TS_ResCode TS_AFConfig(uint8_t IO_Pin, FunctionalState NewState);
TS_ResCode TS_Reset(void);

// Touch data functions
TS_ResCode TS_GetState(TS_State *status);
TS_ResCode TS_GetX(uint16_t *value);
TS_ResCode TS_GetY(uint16_t *value);
TS_ResCode TS_GetZ(uint16_t *value);

// I2C Communication functions
TS_ResCode TS_ReadRegister(TS_REG_TypeDef registerAddr, uint16_t *pRegisterValue, TS_DataLength_TypeDef nBytes);
TS_ResCode TS_WriteRegister(TS_REG_TypeDef registerAddr, uint16_t RegisterValue, TS_DataLength_TypeDef nBytes);

// I2C TimeOut function
TS_ResCode TS_TIMEOUT_Callback(void);



#ifdef __cplusplus
}
#endif

#endif /* ____TS_STMPE811_H */

 
