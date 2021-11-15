/*
 * ts_stmpe811.c
 *
 *  Created on: Maig 1, 2014
 *      Author: Marcel
 */

#include "ts_stmpe811.h"


// TouchScreen Initialization
TS_ResCode TS_Init(void) {
	uint16_t chipID;
	TS_ResCode res = TS_Success;

	// Configure GPIO
	TS_GPIOConfig();

	// Configure I2Cx
	TS_InitI2C();

	// Check the communication with the chip
	res = TS_ReadRegister(TS_REG_CHIP_ID, &chipID, TS_DATA_2Bytes);
	if (res != TS_Success || chipID != 0x0811) // The STMPE811QTR chipset has the ID of 0x0811
		return TS_Chip_Not_Detected;

	// Generate Software Reset
	res |= TS_Reset();

	// Configure the TouchScreen Controller
	res |= TS_ControllerConfig();

	return res;
}


// I2Cx GIPO (SCL + SDA) Configuration
void TS_GPIOConfig(void) {
	GPIO_InitTypeDef GPIO_InitStructure;

	// Enable GPIO clock
	RCC_AHB1PeriphClockCmd(TS_I2Cx_SCL_GPIO_CLK | TS_I2Cx_SDA_GPIO_CLK, ENABLE);

	// Connect I2Cx pins to AFx
	GPIO_PinAFConfig(TS_I2Cx_SCL_GPIO_PORT, TS_I2Cx_SCL_SOURCE, TS_I2Cx_SCL_AF);
	GPIO_PinAFConfig(TS_I2Cx_SDA_GPIO_PORT, TS_I2Cx_SDA_SOURCE, TS_I2Cx_SDA_AF);

	// Setup GPIO SCL pin.
	GPIO_InitStructure.GPIO_Pin = TS_I2Cx_SCL_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_OD;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(TS_I2Cx_SCL_GPIO_PORT, &GPIO_InitStructure);

	// Setup GPIO SDA pin.
	GPIO_InitStructure.GPIO_Pin = TS_I2Cx_SDA_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_OD;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(TS_I2Cx_SDA_GPIO_PORT, &GPIO_InitStructure);

}


// I2Cx Initialization
void TS_InitI2C(void) {
	I2C_InitTypeDef  I2C_InitStruct;

	// I2Cx clock enable
	RCC_APB1PeriphClockCmd(TS_I2Cx_CLK, ENABLE);

	// I2C DeInit
	I2C_DeInit(TS_I2Cx);

	// Initialize the I2C peripheral with selected parameters
	//  - Mode: I2C
	//  - Duty Cycle = 2  (not used in standard speed)
	//  - Own address = 0xFE (not important, we are the master)
	//  - Acknowledge = ON
	//  - 7-bit for address
	//  - Clock Speed = 30000 Hz => Works in standard speed (tlow=thigh)
	I2C_InitStruct.I2C_Mode = I2C_Mode_I2C;
	I2C_InitStruct.I2C_DutyCycle = I2C_DutyCycle_2;
	I2C_InitStruct.I2C_OwnAddress1 = 0xFE;
	I2C_InitStruct.I2C_Ack = I2C_Ack_Enable;
	I2C_InitStruct.I2C_AcknowledgedAddress = I2C_AcknowledgedAddress_7bit;
	I2C_InitStruct.I2C_ClockSpeed = TS_I2C_SPEED;
	I2C_Init(TS_I2Cx, &I2C_InitStruct);

	// Enable the I2C peripheral
	I2C_Cmd(TS_I2Cx, ENABLE);
}

// Configure the TS controller main registers
TS_ResCode TS_ControllerConfig(void) {
	TS_ResCode res = TS_Success;

	// Enable the GPIO, Touch Screen and ADC functionalities
	res |= TS_FunctionsConfig(FUN_GPIO | FUN_TSC | FUN_ADC, ENABLE);

	// Enable the TouchScreen detection and FIFO threshold global interrupts
	res |= TS_ITConfig(GIT_TOUCH_DET | GIT_FIFO_TH, ENABLE);

	// Select Sample Time of 80 clocks, 12 bit ADC with internal reference
	res |= TS_WriteRegister(TS_REG_ADC_CTRL1, (uint16_t) 0x49, TS_DATA_1Byte);

	Delay_ms(50);

	// Select the ADC clock speed: 3.25 MHz
	res |= TS_WriteRegister(TS_REG_ADC_CTRL2, (uint16_t) 0x01, TS_DATA_1Byte);

	//Select TSC pins in non default mode
	res |= TS_AFConfig(TOUCH_IO_ALL, DISABLE);

	// Select 4 samples, 500us delay, 500us settling time
	res |= TS_WriteRegister(TS_REG_TSC_CFG, (uint16_t) 0x9A, TS_DATA_1Byte);

	// Select single point reading
	res |= TS_WriteRegister(TS_REG_FIFO_TH, (uint16_t) 0x01, TS_DATA_1Byte);

	// Write 0x01 to clear the FIFO memory content.
	res |= TS_WriteRegister(TS_REG_FIFO_STA, (uint16_t) 0x01, TS_DATA_1Byte);

	// Write 0x00 to put the FIFO back into operation mode
	res |= TS_WriteRegister(TS_REG_FIFO_STA, (uint16_t) 0x00, TS_DATA_1Byte);

	// Sset the data format for Z value: 7 fractional part and 1 whole part
	res |= TS_WriteRegister(TS_REG_TSC_FRACT_Z, (uint16_t) 0x01, TS_DATA_1Byte);

	// set the driving capability of the device for TSC pins: 50mA
	res |= TS_WriteRegister(TS_REG_TSC_I_DRIVE, (uint16_t) 0x01, TS_DATA_1Byte);

	// Use no tracking index, touchscreen controller operation mode (XYZ) and enable the TSC
	res |= TS_WriteRegister(TS_REG_TSC_CTRL, (uint16_t) 0x01, TS_DATA_1Byte);

	// Clear all the status pending bits
	res |= TS_WriteRegister(TS_REG_INT_STA, (uint16_t) 0xFF, TS_DATA_1Byte);

	return res;

}


// Enable/Disable Functionalities
//	FUN_ADC  : ADC Function
//	FUN_TSC  : TouchScreen Control Function
//	FUN_GPIO : GPIO Function
//  FUN_TS 	 : Temperature Sensor Function
//  FUN_ALL  : All functions
TS_ResCode TS_FunctionsConfig(uint8_t Fun, FunctionalState NewState) {
	TS_ResCode res = TS_Success;
	uint8_t tmp = 0;

	// Get the register value
	res |= TS_ReadRegister(TS_REG_SYS_CTRL2, (uint16_t*)&tmp, TS_DATA_1Byte);

	if (NewState != DISABLE) {
		// Set the Functionalities to be Enabled
		tmp &= ~(uint8_t)Fun;
	} else {
		// Set the Functionalities to be Disabled
		tmp |= (uint8_t)Fun;
	}

	// Set the register value
	res |= TS_WriteRegister(TS_REG_SYS_CTRL2, (uint16_t)tmp, TS_DATA_1Byte);

	// return the result
	return res;
}

// Enable/Disable Global interrupts
//	GIT_TOUCH_DET	: Touch is detected
//	GIT_FIFO_TH		: FIFO is equal or above threshold value
//	GIT_FIFO_OFLOW	: FIFO is overflowed
//	GIT_FIFO_FULL	: FIFO is full
//	GIT_FIFO_EMPTY	: FIFO is empty
//	GIT_TEMP_SENS	: Temperature threshold triggering
//	GIT_ADC			: Any enabled ADC interrupts
//	GIT_GPIO		: Any enabled GPIO interrupts
TS_ResCode TS_ITConfig(uint8_t Global_IT, FunctionalState NewState) {
	TS_ResCode res = TS_Success;
	uint8_t tmp = 0;

	// Get the register value
	res |= TS_ReadRegister(TS_REG_INT_EN, (uint16_t*)&tmp, TS_DATA_1Byte);

	if (NewState != DISABLE) {
		tmp |= (uint8_t)Global_IT;
	} else {
		tmp &= ~(uint8_t)Global_IT;
	}

	// Set the register value
	res |= TS_WriteRegister(TS_REG_INT_EN, (uint16_t)tmp, TS_DATA_1Byte);

	// return the result
	return res;
}

// Enable/Disable Global interrupts
//	IO_Pin_x     : Where x can be from 0 to 7.
//	IO_Pin_ALL   : All 8 pins
//  TOUCH_IO_ALL : All pins of TSC
TS_ResCode TS_AFConfig(uint8_t IO_Pin, FunctionalState NewState) {
	TS_ResCode res = TS_Success;
	uint8_t tmp = 0;

	// Get the register value
	res |= TS_ReadRegister(TS_REG_GPIO_AF, (uint16_t*)&tmp, TS_DATA_1Byte);

	if (NewState != DISABLE) {
		tmp |= (uint8_t)IO_Pin;
	} else {
		tmp &= ~(uint8_t)IO_Pin;
	}

	// Set the register value
	res |= TS_WriteRegister(TS_REG_GPIO_AF, (uint16_t)tmp, TS_DATA_1Byte);

	// return the result
	return res;
}

// Software Reset
TS_ResCode TS_Reset(void) {
	TS_ResCode res = TS_Success;

	// Power Down the chip
	res |= TS_WriteRegister(TS_REG_SYS_CTRL1, (uint16_t) 0x02, TS_DATA_1Byte);

	// Wait for a delay to insure registers erasing
	Delay_ms(50);

	// Power On after the power off => all registers are reinitialized
	res |= TS_WriteRegister(TS_REG_SYS_CTRL1, (uint16_t) 0x00, TS_DATA_1Byte);

	return res;
}


// Get TS state
TS_ResCode TS_GetState(TS_State *status) {
	TS_ResCode res = TS_Success;
	uint8_t tmp;
	uint16_t xDiff, yDiff , x , y;
	static uint16_t _x = 0, _y = 0;

	// Check if the Touch detect event happened
	res = TS_ReadRegister(TS_REG_TSC_CTRL, (uint16_t*)&tmp, TS_DATA_1Byte);
	if (res != TS_Success) return res;
	status->touchDetected = (tmp & 0x80)? 1 : 0;

	if (status->touchDetected) {
		res |= TS_GetX(&x);
		res |= TS_GetY(&y);

		// Check if the diference between previos value is more than 5
		// and the updates the value
		xDiff = x > _x? (x - _x): (_x - x);
		yDiff = y > _y? (y - _y): (_y - y);
		if (xDiff + yDiff > 5) {
			_x = x;
			_y = y;
		}
	}

	// Store the positions
	status->x = _x;
	status->y = _y;

	// Get the Z position
	TS_GetZ(&(status->z));

	// Clear the interrupt pending bit and enable the FIFO again
	res |= TS_WriteRegister(TS_REG_FIFO_STA, (uint16_t) 0x01, TS_DATA_1Byte);
	res |= TS_WriteRegister(TS_REG_FIFO_STA, (uint16_t) 0x00, TS_DATA_1Byte);

	// Return the operation result
	return res;
}

// Read ADC of Y position
TS_ResCode TS_GetX(uint16_t *value) {
	TS_ResCode res = TS_Success;
	uint32_t i, sum;
	uint16_t tmp;

	// Average of 16 captures
	sum = 0;
	for(i=0; i<16; i++) {
		res |= TS_ReadRegister(TS_REG_TSC_DATA_X, &tmp, TS_DATA_2Bytes);
		sum += tmp;
	}
	sum >>= 4;

	// uptades the value obtained
	if (res == TS_Success) {
		*value = sum;
	}

	return res;
}

// Read ADC of Y position
TS_ResCode TS_GetY(uint16_t *value) {
	TS_ResCode res = TS_Success;
	uint32_t i, sum;
	uint16_t tmp;

	// Average of 16 captures
	sum = 0;
	for(i=0; i<16; i++) {
		res |= TS_ReadRegister(TS_REG_TSC_DATA_Y, &tmp, TS_DATA_2Bytes);
		sum += tmp;
	}
	sum >>= 4;

	// uptades the value obtained
	if (res == TS_Success) {
		*value = sum;
	}

	return res;
}

// Read ADC of Z position
TS_ResCode TS_GetZ(uint16_t *value) {
	TS_ResCode res = TS_Success;
	uint8_t tmp;

	res |= TS_ReadRegister(TS_REG_TSC_DATA_Z, (uint16_t*)&tmp, TS_DATA_1Byte);

	// uptades the value obtained
	if (res == TS_Success) {
		*value = tmp;
	}

	return res;
}

// Read a 8-bit or 16-bit register through I2Cx, see datasheet
TS_ResCode TS_ReadRegister(TS_REG_TypeDef registerAddr, uint16_t *pRegisterValue, TS_DataLength_TypeDef nBytes) {
	uint32_t timeout;
	uint8_t data[2];

	// Wait while the bus is busy
	timeout = TS_I2C_TIMEOUT;
	while(I2C_GetFlagStatus(TS_I2Cx, I2C_FLAG_BUSY)) {
		if((timeout--) == 0) return TS_TIMEOUT_Callback();
	}

	// SEND START CONDITION
	I2C_GenerateSTART(TS_I2Cx, ENABLE);

	// Test on EV5 and clear it
	timeout = TS_I2C_TIMEOUT;
	while (!I2C_CheckEvent(TS_I2Cx, I2C_EVENT_MASTER_MODE_SELECT)) {
		if((timeout--) == 0) return TS_TIMEOUT_Callback();
	}

	// SEND I2C ADDRESS + WRITE BIT + READ ACK (specify master as transmitter)
	I2C_Send7bitAddress(TS_I2Cx, TS_ADDR_WR, I2C_Direction_Transmitter);

	// Test on EV6 and clear it
	timeout = TS_I2C_TIMEOUT;
	while (!I2C_CheckEvent(TS_I2Cx, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED)) {
		if((timeout--) == 0) return TS_TIMEOUT_Callback();
	}

	// SEND REGISTER + READ ACK
	I2C_SendData(TS_I2Cx, registerAddr);

	// Test on EV8 and clear it
	timeout = TS_I2C_TIMEOUT;
	while(!I2C_CheckEvent(TS_I2Cx, I2C_EVENT_MASTER_BYTE_TRANSMITTED)) {
		if((timeout--) == 0) return TS_TIMEOUT_Callback();
	}


	// SEND RE-START CONDITION
	I2C_GenerateSTART(TS_I2Cx, ENABLE);

	// Test on EV5 and clear it
	timeout = TS_I2C_TIMEOUT;
	while (!I2C_CheckEvent(TS_I2Cx, I2C_EVENT_MASTER_MODE_SELECT)) {
		if((timeout--) == 0) return TS_TIMEOUT_Callback();
	}


	// SEND I2C ADDRESS + READ BIT + READ ACK (specify master as receiver)
	I2C_Send7bitAddress(TS_I2Cx, TS_ADDR_RD, I2C_Direction_Receiver);

	// Wait on ADDR flag to be set. Test on EV6 and clear it
	timeout = TS_I2C_TIMEOUT;
	while(!I2C_CheckEvent(TS_I2Cx, I2C_EVENT_MASTER_RECEIVER_MODE_SELECTED)) {
		if((timeout--) == 0) return TS_TIMEOUT_Callback();
	}

	data[1] = 0;
	if (nBytes == TS_DATA_2Bytes) {

		// WAIT 8-bits + ACK
		while(!I2C_CheckEvent(TS_I2Cx, I2C_EVENT_MASTER_BYTE_RECEIVED));

		// Read the received data (Most Significant Byte)
		data[1] = I2C_ReceiveData(TS_I2Cx);
	}


	// Disable Acknowledgment for next NACK
	I2C_AcknowledgeConfig(TS_I2Cx, DISABLE);

	// WAIT 8-bits + NACK (None ACK).
	while(!I2C_CheckEvent(TS_I2Cx, I2C_EVENT_MASTER_BYTE_RECEIVED));

	// Read the received data (Least Significant Byte)
	data[0] = I2C_ReceiveData(TS_I2Cx);

	// SEND STOP CONDITION
	I2C_GenerateSTOP(TS_I2Cx, ENABLE);


	// Re-Enable Acknowledgment to be ready for another reception
	I2C_AcknowledgeConfig(TS_I2Cx, ENABLE);


	// Clear AF (Acknowledge failure flag) flag for next communication
	I2C_ClearFlag(TS_I2Cx, I2C_FLAG_AF);


	// Calculate the result
	*pRegisterValue =  (data[1] << 8) & 0xFF00;
	*pRegisterValue = *pRegisterValue + data[0];

	return TS_Success;
}


// Write a 8-bit or 16-bit register through I2Cx, see datasheet
TS_ResCode TS_WriteRegister(TS_REG_TypeDef registerAddr, uint16_t RegisterValue, TS_DataLength_TypeDef nBytes) {
	uint32_t timeout;
	uint8_t data[2];

	// Wait while the bus is busy
	timeout = TS_I2C_TIMEOUT;
	while(I2C_GetFlagStatus(TS_I2Cx, I2C_FLAG_BUSY)) {
		if((timeout--) == 0) return TS_TIMEOUT_Callback();
	}

	// SEND START CONDITION
	I2C_GenerateSTART(TS_I2Cx, ENABLE);

	// Test on EV5 and clear it
	timeout = TS_I2C_TIMEOUT;
	while (!I2C_CheckEvent(TS_I2Cx, I2C_EVENT_MASTER_MODE_SELECT)) {
		if((timeout--) == 0) return TS_TIMEOUT_Callback();
	}

	// SEND I2C ADDRESS + WRITE BIT + READ ACK (specify master as transmitter)
	I2C_Send7bitAddress(TS_I2Cx, TS_ADDR_WR, I2C_Direction_Transmitter);

	// Test on EV6 and clear it
	timeout = TS_I2C_TIMEOUT;
	while (!I2C_CheckEvent(TS_I2Cx, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED)) {
		if((timeout--) == 0) return TS_TIMEOUT_Callback();
	}

	// SEND REGISTER + READ ACK
	I2C_SendData(TS_I2Cx, registerAddr);

	// Test on EV8 and clear it
	timeout = TS_I2C_TIMEOUT;
	while(!I2C_CheckEvent(TS_I2Cx, I2C_EVENT_MASTER_BYTE_TRANSMITTED)) {
		if((timeout--) == 0) return TS_TIMEOUT_Callback();
	}

	// Calculate the data bytes
	data[1] = (RegisterValue >> 8);
	data[0] = (RegisterValue & 0x00FF);

	if (nBytes == TS_DATA_2Bytes) {

		// SEND DATA (Most Significant Byte) + READ ACK
		I2C_SendData(TS_I2Cx, data[1]);

		// Test on EV8 and clear it
		timeout = TS_I2C_TIMEOUT;
		while(!I2C_CheckEvent(TS_I2Cx, I2C_EVENT_MASTER_BYTE_TRANSMITTED)) {
			if((timeout--) == 0) return TS_TIMEOUT_Callback();
		}
	}


	// SEND DATA (Least Significant Byte) + READ ACK
	I2C_SendData(TS_I2Cx, data[0]);

	// Test on EV8 and clear it
	timeout = TS_I2C_TIMEOUT;
	while(!I2C_CheckEvent(TS_I2Cx, I2C_EVENT_MASTER_BYTE_TRANSMITTED)) {
		if((timeout--) == 0) return TS_TIMEOUT_Callback();
	}


	// SEND STOP CONDITION
	I2C_GenerateSTOP(TS_I2Cx, ENABLE);


	// Re-Enable Acknowledgment to be ready for another reception
	I2C_AcknowledgeConfig(TS_I2Cx, ENABLE);


	// Clear AF (Acknowledge failure flag) flag for next communication
	I2C_ClearFlag(TS_I2Cx, I2C_FLAG_AF);


	return TS_Success;
}

// Allows I2C error recovery and return to normal communication
TS_ResCode TS_TIMEOUT_Callback(void) {

	I2C_GenerateSTOP(TS_I2Cx, ENABLE);
	I2C_SoftwareResetCmd(TS_I2Cx, ENABLE);
	I2C_SoftwareResetCmd(TS_I2Cx, DISABLE);

	// I2Cx peripheral re-configuration
	TS_InitI2C();

	/* At this stage the I2C error should be recovered and device can communicate
	   again (except if the error source still exist).
	   User can implement mechanism (ex. test on max trial number) to manage situation
	   when the I2C can't recover from current error. */

	// Indicate Error
	return TS_I2C_Timeout_Error;
}


