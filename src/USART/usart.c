/*
 * usart.c
 *
 *  Created on: Feb 15, 2012
 *      Author: Marcel
 *
 * Modified on: Oct 28, 2013
 * Modified on: Feb 16, 2014
 *
 */

#include "usart.h"


// Variables del USARTx per guardar les dades rebudes
static uint8_t USARTx_isRecived;
static char USARTx_buff[LEN_USARTx_BUFF];
static uint8_t USARTx_buffpos;


// Initialize USART
void USARTx_Init(USARTMode_TypeDef USART_Mode) {
	GPIO_InitTypeDef GPIO_InitStructure;
	USART_ClockInitTypeDef USART_ClockInitStruct;
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;

	// Start the control variables
	USARTx_isRecived = RESET;
	USARTx_buffpos = 0;


	///   Configure the GPIO
	/////////////////////////////////////////////////////////

	// Enable GPIO clock
	RCC_AHB1PeriphClockCmd(USARTx_TX_GPIO_CLK | USARTx_RX_GPIO_CLK, ENABLE);

	// Changes the mapping of Tx/Rx pins to USARTx.
	GPIO_PinAFConfig(USARTx_TX_GPIO_PORT, USARTx_TX_SOURCE, USARTx_TX_AF);
	GPIO_PinAFConfig(USARTx_RX_GPIO_PORT, USARTx_RX_SOURCE, USARTx_RX_AF);

	// Setup GPIO Tx pin.
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_InitStructure.GPIO_Pin = USARTx_TX_PIN;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(USARTx_TX_GPIO_PORT, &GPIO_InitStructure);

	// Setup GPIO Rx pin.
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_InitStructure.GPIO_Pin = USARTx_RX_PIN;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(USARTx_RX_GPIO_PORT, &GPIO_InitStructure);



	///   Configure the USARTx Peripheral
	/////////////////////////////////////////////////////////

	// Enable UART clock
	RCC_APB2PeriphClockCmd(USARTx_CLK, ENABLE); // XXX: RCC_APB1PeriphClockCmd => For USART2, RCC_APB2PeriphClockCmd => For USART6

	// Initializes the USARTx peripheral Clock with default values (SCLK Disabled)
	USART_ClockStructInit(&USART_ClockInitStruct);
	USART_ClockInit(USARTx, &USART_ClockInitStruct);


	// USARTx configured as follow:
	//	- BaudRate = 115200 baud
	//	- Word Length = 8 Bits
	//	- One Stop Bit
	//	- No parity
	//	- Hardware flow control disabled (RTS and CTS signals)
	//	- Receive and transmit enabled
	USART_StructInit(&USART_InitStructure);
	USART_InitStructure.USART_BaudRate = 115200;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	USART_InitStructure.USART_StopBits = USART_StopBits_1;
	USART_InitStructure.USART_Parity = USART_Parity_No;
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
	USART_Init(USARTx, &USART_InitStructure);

	// Enable the USART interrupt request.
	if (USART_Mode == USART_MODE_IRQ) {
		// Enable the USARTx Interrupt with low priority
		NVIC_InitStructure.NVIC_IRQChannel = USARTx_IRQn;
		NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x0F;
		NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x0F;
		NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
		NVIC_Init(&NVIC_InitStructure);

		// Clears the USARTx's interrupt pending bits.
		USART_ClearITPendingBit(USARTx, USART_IT_RXNE);

		// Enable the USARTx Receive interrupt: this interrupt is generated when the
		// USARTx receive data register is not empty
		USART_ITConfig(USARTx, USART_IT_RXNE, ENABLE);

	}

	// Clears the USARTx's flags.
	USART_ClearFlag(USARTx, USART_FLAG_TC);
	USART_ClearFlag(USARTx, USART_FLAG_RXNE);

	// Enable USARTx
	USART_Cmd(USARTx, ENABLE);
}

// Update new recieved data and store it to buffer
void USARTx_task(void){
	uint16_t c;

	// If there es a string pending to read, then discard new data
	if (USARTx_isRecived == SET) return;

	// Check if new byte arrived
	if (USART_GetFlagStatus(USARTx, USART_FLAG_RXNE) == SET) {
		c = USART_ReceiveData(USARTx);
		USARTx_buff[USARTx_buffpos] = c;

		// if is a return, '/r' = char(13) then indicates there is a new string
		if (c == 13) {
			USARTx_buff[USARTx_buffpos] = '\0';
			USARTx_isRecived = SET;
			USARTx_buffpos = 0;
		} else if (USARTx_buffpos < LEN_USARTx_BUFF-1) USARTx_buffpos++;
	}
}


// Check flag if is recived data
uint8_t USARTx_isDataRecived(void) {
	return USARTx_isRecived;
}


// Get string from buffer
uint8_t USARTx_GetStr(char s[]) {
	if (USARTx_isRecived == RESET) return 0;
	strcpy(s, USARTx_buff);
	USARTx_isRecived = RESET;
	return 1;
}


// Send String
void USARTx_PutStr(char s[]) {
	int16_t i = 0;
	while(s[i]!='\0') {
		USARTx_Put(s[i]);
		i++;
	}
}


// Put char
void USARTx_Put(uint16_t d) {
	USART_SendData(USARTx, d);

	// Wait while the byte is transmitting
	while(!USART_GetFlagStatus( USARTx, USART_FLAG_TXE ));
	//while (USART_GetFlagStatus(USARTx, USART_FLAG_TC) == RESET); // Es el mateix que l'anterior
}


// Put Number
void USARTx_PutNum(int32_t num) {
	uint8_t i=0, j=0;
	char str[128], tmp_str[128];
	int32_t absnum;

	// Numero en valor absolut
	absnum = (num>0?num:-num);

	// Obtenim els digits, de menor a major pes
	do {
		int lastDigit = absnum%10;
		tmp_str[i++]='0'+lastDigit;
		absnum = absnum/10;
	} while( absnum>0);

	// Comprovem i posem signe
	if (num < 0) tmp_str[i++] = '-';

	// Capgirem el numero
	while(i>0) str[j++]=tmp_str[--i];
	str[j]='\0';

	// Enviem el String
	USARTx_PutStr(str);
}


// Put Unsigned Number
void USARTx_PutUnsignedNum(uint32_t num) {
	uint8_t i=0, j=0;
	char str[128], tmp_str[128];

	// Obtenim els digits, de menor a major pes
	do {
		int lastDigit = num%10;
		tmp_str[i++]='0'+lastDigit;
		num = num/10;
	} while( num>0);

	// Capgirem el numero
	while(i>0) str[j++]=tmp_str[--i];
	str[j]='\0';

	// Enviem el String
	USARTx_PutStr(str);
}


// Put Floating Number
void USARTx_PutFloat(float num, uint8_t precision) {
	uint8_t i, j;
	char str[128], tmp_str[128];
	uint32_t parteEntera, parteDecimal;
	float mul10=1;

	for(i=0;i<precision;i++) mul10*=10;
	parteEntera=ABS(num);
	parteDecimal=ABS((num-(int32_t)num)*mul10);

	// Posem digits part entera, de menor a major pes
	i=0;
	do {
		tmp_str[i++]=(parteEntera%10)+'0';
		parteEntera = parteEntera/10;
	} while(parteEntera>0);

	// Comprovem i posem signe
	if (num < 0) tmp_str[i++] = '-';

	// Capgirem el numero
	j=0;
	while(i>0) str[j++]=tmp_str[--i];
	str[j]='\0';


	// Si te presicio la posem
	if(precision>0) {
		uint8_t posDecimal = j;

		// Posem digits part decimal, de menor a major pes
		i=0;
		for(j=0;j<precision;j++) {
			tmp_str[i++]=(parteDecimal%10)+'0';
			parteDecimal = parteDecimal/10;
		}

		// Fiquem el punt
		tmp_str[i++] = '.';

		// Capgirem i posem els decimals + el punt
		j=posDecimal;
		while(i>0) str[j++]=tmp_str[--i];
		str[j]='\0';
	}

	// Enviem el String
	USARTx_PutStr(str);
}

// This function handles USART interrupt request.
void USARTx_IRQHandler(void) {

	//wait until previous byte is received
	if(USART_GetITStatus(USARTx, USART_IT_RXNE) == SET) {

		// Read the byte
		USARTx_task();

		// IMPORTANT TO DO THAT!!, Clear the interrupt flag.
		USART_ClearFlag(USARTx, USART_IT_RXNE);
	}
}

