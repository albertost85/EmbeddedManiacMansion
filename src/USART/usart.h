/*
 * usart.h
 *
 *  Created on: Feb 15, 2012
 *      Author: Marcel
 *
 * Modified on: Oct 28, 2013
 * Modified on: Feb 16, 2014
 *
 */

#ifndef USART_H_
#define USART_H_

#ifdef __cplusplus
 extern "C" {
#endif


#include "stm32f4xx.h"
#include <string.h>

typedef enum {
	USART_MODE_POLLING = 0,
	USART_MODE_IRQ 	   = 1
} USARTMode_TypeDef;


//----- PORT SERIE DEFINITIONS -----
/*
#define USARTx						USART2
#define USARTx_CLK					RCC_APB1Periph_USART2

#define USARTx_TX_PIN               GPIO_Pin_2
#define USARTx_TX_GPIO_PORT         GPIOA
#define USARTx_TX_GPIO_CLK          RCC_AHB1Periph_GPIOA
#define USARTx_TX_SOURCE            GPIO_PinSource2
#define USARTx_TX_AF                GPIO_AF_USART2

#define USARTx_RX_PIN               GPIO_Pin_3
#define USARTx_RX_GPIO_PORT         GPIOA
#define USARTx_RX_GPIO_CLK          RCC_AHB1Periph_GPIOA
#define USARTx_RX_SOURCE            GPIO_PinSource3
#define USARTx_RX_AF                GPIO_AF_USART2

#define USARTx_IRQn                 USART2_IRQn
#define USARTx_IRQHandler           USART2_IRQHandler
*/

#define USARTx						USART6
#define USARTx_CLK					RCC_APB2Periph_USART6

#define USARTx_TX_PIN               GPIO_Pin_6
#define USARTx_TX_GPIO_PORT         GPIOC
#define USARTx_TX_GPIO_CLK          RCC_AHB1Periph_GPIOC
#define USARTx_TX_SOURCE            GPIO_PinSource6
#define USARTx_TX_AF                GPIO_AF_USART6

#define USARTx_RX_PIN               GPIO_Pin_7
#define USARTx_RX_GPIO_PORT         GPIOC
#define USARTx_RX_GPIO_CLK          RCC_AHB1Periph_GPIOC
#define USARTx_RX_SOURCE            GPIO_PinSource7
#define USARTx_RX_AF                GPIO_AF_USART6

#define USARTx_IRQn                 USART6_IRQn
#define USARTx_IRQHandler           USART6_IRQHandler

// ---------------------------------


// Macro de valor absolut
#ifndef ABS
#define ABS(X)  ((X) > 0 ? (X) : -(X))
#endif

// USARTx receiving data buffer length
#define LEN_USARTx_BUFF		50


// Functions
void USARTx_task(void);
void USARTx_Init(USARTMode_TypeDef USART_Mode);
uint8_t USARTx_isDataRecived(void);
uint8_t USARTx_GetStr(char s[]);
void USARTx_PutStr(char s[]);
void USARTx_Put(uint16_t d);
void USARTx_PutNum(int32_t num);
void USARTx_PutUnsignedNum(uint32_t num);
void USARTx_PutFloat(float num, uint8_t precision);



#ifdef __cplusplus
}
#endif

#endif /* USART_H_ */
