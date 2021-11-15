/*
 * image_flash.c
 *
 *  Created on: Mar 29, 2017
 *    Modified: .
 *      Author: Marcel
 */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __IMAGE_FLASH_H
#define __IMAGE_FLASH_H

/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx.h"
#include  "lcd_ssd2119.h"


typedef struct {
	uint16_t width;
	uint16_t height;
	uint16_t BytesPerLine;
	uint16_t BitsPerPixel;
	const uint8_t *pData;
} FLASH_BITMAP;

/* Exported functions ------------------------------------------------------- */
uint8_t FlashImageShow(uint16_t col, uint16_t row, const FLASH_BITMAP *image);
uint8_t FlashImageShow2(uint16_t col, uint16_t row, const FLASH_BITMAP *image,uint16_t xInit,uint16_t width);
uint8_t FlashImageShow3(uint16_t col, uint16_t row, const FLASH_BITMAP *image);


#endif /* __IMAGE_FLASH_H */
