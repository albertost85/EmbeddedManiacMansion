/*
 * image_flash.c
 *
 *  Created on: Mar 29, 2017
 *    Modified: .
 *      Author: Marcel
 */

/* Includes ------------------------------------------------------------------*/
#include "image_flash.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/

/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/


uint8_t FlashImageShow(uint16_t col, uint16_t row, const FLASH_BITMAP *image) {

	// Check if there are enough pixels in the display matrix to show the image
	if (col+image->width > LCD_NumCols() || row+image->height > LCD_NumRows()) {
		return 1;
	}

	// Prepare LCD For Write
	LCD_SetWindow(col, row, image->width, image->height);
	LCD_SetCursor(col,row);
	LCD_WriteRAM_Prepare();

	// Write entire buffer
	LCD_WriteRAMBuffer((uint16_t*)image->pData, image->width*image->height);

	return 0;

}


uint8_t FlashImageShow2(uint16_t col, uint16_t row, const FLASH_BITMAP *image,uint16_t xInit,uint16_t width) {
	uint16_t x,y;

	// Check if there are enough pixels in the display matrix to show the image
	if (col+width > LCD_NumCols() || row+image->height > LCD_NumRows()) {
		return 1;
	}
	// Parse xInit between max limit
	if(xInit > (image->width-width))
	{
		xInit = image->width-width;
	}
	// Prepare LCD For Write
	LCD_SetWindow(col, row, width, image->height);
	LCD_SetCursor(col,row);
	LCD_WriteRAM_Prepare();

	const uint16_t *pBuf = (uint16_t*)image->pData;
	for (y=0;y<image->height;y++) {
		for (x=xInit;x<xInit+width;x++) {
			LCD_WriteRAM(*(pBuf+(y*image->width)+x));
		}
	}

	/*
	uint32_t addr;
	const uint16_t *pBuf = (uint16_t*)image->pData;
	for (addr=0;addr<size;addr++) {
		if((addr%image->width>=xInit) && (addr%image->width<=(xInit+width))){
			LCD_WriteRAM(pBuf[addr]);
		}
	}
*/
	return 0;

}


uint8_t FlashImageShow3(uint16_t col, uint16_t row, const FLASH_BITMAP *image) {
	uint16_t x,y;

	// Check if there are enough pixels in the display matrix to show the image
	if (col+image->width > LCD_NumCols() || row+image->height > LCD_NumRows()) {
		return 1;
	}

	// Prepare LCD For Write
	LCD_SetWindow(col, row, image->width, image->height);
	LCD_SetCursor(col,row);

	const uint16_t *pBuf = (uint16_t*)image->pData;
	for (y=0;y<image->height;y++) {
		for (x=0;x<image->width;x++) {
			if (*(pBuf+(y*image->width)+x) != 0xF81F) {
			LCD_SetCursor(col+x, row+y);
			LCD_WriteReg(SSD2119_RAM_DATA_REG, *(pBuf+(y*image->width)+x));
			}
		}
	}
	return 0;
}
