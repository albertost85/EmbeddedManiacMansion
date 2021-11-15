/*
 * image_bmp.c
 *
 *  Created on: Apr 30, 2014
 *    Modified: .
 *      Author: Marcel
 */

/* Includes ------------------------------------------------------------------*/
#include "image_bmp.h"
#include <string.h>

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/

/* Private function prototypes -----------------------------------------------*/
static void BMP_DisplayLine(IMAGEHDR *imageHeader, uint8_t *buff, BMP_Effect_TypeDef effect);
static void BMP_ComputeImagePitch(int32_t bpp, int32_t width, int32_t *pitch, int32_t *bytesperpixel);

/* Private functions ---------------------------------------------------------*/

// Decode, resize and display a BMP image
BMP_ResCode BMP_Show(char *filename, uint16_t col, uint16_t row, BMP_Effect_TypeDef effect) {
	FIL file;
	uint8_t buffer[INFOHEADSIZE];
	uint8_t buffline[BMP_MAX_PITCH];
	IMAGEHDR image_params;
	BMPFILEHEAD	bmpFH;
	BMPINFOHEAD bmpIH;
	BMPCOREHEAD bmpCH;
	uint16_t numOfReadBytes = 0;
	int32_t  headerSize;
	int32_t  h, compression;

	// Open the BMP file
	if (f_open(&file, filename, FA_READ) != FR_OK) {
		return BMP_File_Open_Error;
	}

	// Read BMP file header
	if (f_read(&file, &buffer, FILEHEADSIZE, (void *)&numOfReadBytes) != FR_OK) {
		f_close(&file);
		return BMP_File_Read_Error;
	}

	// Is it really a bmp file ?
	bmpFH.bfType[0] = buffer[0];
	bmpFH.bfType[1] = buffer[1];
	if ((bmpFH.bfType[0] | (uint16_t)(bmpFH.bfType[1] << 8)) != 0x4D42) /* 'BM' */ {
		f_close(&file);
		return BMP_File_Not_Bitmap; // Not bmp image file!
	}

	// BMP File size
	bmpFH.bfSize = (uint32_t)(buffer[2]| (buffer[3] << 8) | (buffer[4] << 16) | (buffer[5] << 24));

	// Offset position where start image data (pixel array)
	bmpFH.bfOffBits = (uint32_t)(buffer[10]| (buffer[11] << 8) | (buffer[12] << 16) | (buffer[13] << 24));

	// Read remaining bytes of header size (4 bytes value)
	if (f_read(&file, &headerSize, sizeof(uint32_t), (void *)&numOfReadBytes) != FR_OK) {
		f_close(&file);
		return BMP_File_Read_Error;
	}

	// Might be Windows or OS/2 header
	if (headerSize == COREHEADSIZE) {

		// Read OS/2 header:
		if (f_read(&file, &buffer, COREHEADSIZE - sizeof(uint32_t), (void *)&numOfReadBytes) != FR_OK) {
			f_close(&file);
			return BMP_File_Read_Error;
		}

		// Get data
		bmpCH.bcWidth    = (uint16_t)(buffer[0] + (uint16_t)(buffer[1]<<8));
		bmpCH.bcHeight   = (uint16_t)(buffer[2] + ((uint16_t)buffer[3]<<8));
		bmpCH.bcPlanes   = (uint16_t)(buffer[4] + ((uint16_t)buffer[5]<<8));
		bmpCH.bcBitCount = (uint16_t)(buffer[6] + ((uint16_t)buffer[7]<<8));

		image_params.width   = (int32_t)bmpCH.bcWidth;
		image_params.height  = (int32_t)bmpCH.bcHeight;
		image_params.bpp     = bmpCH.bcBitCount;

		if (image_params.bpp <= 8)
			image_params.palsize = 1 << image_params.bpp;
		else
			image_params.palsize = 0;

		compression = BI_RGB;

	} else {

		// Read Windows header:
		if (f_read(&file, &buffer, INFOHEADSIZE - sizeof(uint32_t), (void *)&numOfReadBytes) != FR_OK) {
			f_close(&file);
			return BMP_File_Read_Error;
		}

		bmpIH.BiWidth =  buffer[0] | (buffer[1] << 8) | (buffer[2] << 16) | (buffer[3] << 24);
		bmpIH.BiHeight = buffer[4] | (buffer[5] << 8) | (buffer[6] << 16) | (buffer[7] << 24);
		bmpIH.BiPlanes = buffer[8] | (buffer[9] << 8);
		bmpIH.BiBitCount =  buffer[10] | (buffer[11] << 8);
		bmpIH.BiCompression = buffer[12]| (buffer[13] << 8) | (buffer[14] << 16) | (buffer[15] << 24);
		bmpIH.BiSizeImage = buffer[16]| (buffer[17] << 8) | (buffer[18] << 16) | (buffer[19] << 24);
		bmpIH.BiXpelsPerMeter = buffer[20]| (buffer[21] << 8) | (buffer[22] << 16) | (buffer[23] << 24);
		bmpIH.BiYpelsPerMeter =  buffer[24]| (buffer[25] << 8) | (buffer[26] << 16) | (buffer[27] << 24);
		bmpIH.BiClrUsed = buffer[28]| (buffer[29] << 8) | (buffer[30] << 16) | (buffer[31] << 24);
		bmpIH.BiClrImportant =  buffer[32]| (buffer[33] << 8) | (buffer[34] << 16) | (buffer[35] << 24);

		image_params.width = (int32_t)bmpIH.BiWidth;
		image_params.height = (int32_t)bmpIH.BiHeight;
		image_params.bpp = bmpIH.BiBitCount;
		image_params.palsize = (int32_t)bmpIH.BiClrUsed;

		if (image_params.palsize > 256) {
			image_params.palsize = 0;
		} else if (image_params.palsize == 0 && image_params.bpp <= 8) {
			image_params.palsize = 1 << image_params.bpp;
		}

		compression = bmpIH.BiCompression;
	}
	image_params.compression = IMAGE_BGR; /* right side up, BGR order */
	image_params.planes = 1;

	// Only 16 and 24 bpp bitmaps
	if (image_params.bpp != 16 && image_params.bpp != 24) {
		f_close(&file);
		return BMP_File_Not_Compatible; // image loading error
	}

	// Compute byte line size (pitch=bytes per fila multiple de 32bits) and bytes per pixel (bytesperpixel)
	BMP_ComputeImagePitch(image_params.bpp, image_params.width, &image_params.pitch, &image_params.bytesperpixel);

	// Check maximum pitch (for buffer length limitation)
	if (image_params.pitch > BMP_MAX_PITCH) {
		f_close(&file);
		return BMP_File_TooBig; // image loading error (too big size)
	}

	// Check if there are enough pixels in the display matrix to show the image
	if (col+image_params.width > LCD_NumCols() || row+image_params.height > LCD_NumRows()) {
		f_close(&file);
		return BMP_File_Not_Enough_Space;
	}

	// Determine 16bpp 5/5/5 or 5/6/5 format
	if (image_params.bpp == 16) {
		uint32_t format = 0x7c00;  /* default is 5/5/5 */

		if (compression == BI_BITFIELDS) {
			uint8_t buf[4];
			if (f_read(&file, &buf, sizeof(uint32_t), (void *)&numOfReadBytes) != FR_OK) {
				f_close(&file);
				return BMP_File_Read_Error;
			}
			format = buf[0]| (buf[1] << 8) | (buf[2] << 16) | (buf[3] << 24);
		}
		if (format == 0x7c00) {
			image_params.compression |= IMAGE_555;
			// else it's 5/6/5 format, no flag required
		}

		// Ignore compressed format
		if ((compression != BI_BITFIELDS) && (compression != BI_RGB)) {
			f_close(&file);
			return BMP_File_Not_Compatible; // Don't work for compressed formats
		}
	}

	// Seek cursor to the begin of pixel data array for decode
	f_lseek(&file, bmpFH.bfOffBits);

	// Prepare LCD For Write
	LCD_SetEntryMode(LCD_ENTRY_MODE_BMP);
	LCD_SetWindow(col, row, image_params.width, image_params.height);
	LCD_SetCursor(col, row + image_params.height-1);
	LCD_WriteRAM_Prepare();

	// Write all pixel data to display
	uint32_t totalbytes;
	uint32_t inPos = 0;
	totalbytes = 0;
	h = image_params.height; // number of rows
	while(1) {

		// Reads sets of data of 320 bytes
		if (f_read(&file, &buffline[inPos], 320, (void *)&numOfReadBytes) != FR_OK) {
			f_close(&file);
			return BMP_File_Read_Error;
		}
		// Updates the input position
		inPos += numOfReadBytes;

		// Calculate the total bytes acumulated
		totalbytes = totalbytes + numOfReadBytes;

		while (inPos >= image_params.pitch) {
			BMP_DisplayLine(&image_params, buffline, effect);

			// Move the pending bytes to the begin of line buffer
			memcpy(&buffline[0], &buffline[image_params.pitch], inPos-image_params.pitch);
			inPos = inPos-image_params.pitch;

			h--;

		}

		if (h == 0) {
			break; // The image is entire read and showed
		}

		if (numOfReadBytes == 0) {
			f_close(&file);
			return BMP_File_Format_Error; // EOF until finish
		}

	}
	LCD_SetEntryMode(LCD_ENTRY_MODE_NORMAL);

	// Close file
	f_close(&file);

	return BMP_Success;
}

/**
  * @brief  compute image line size and bytes per pixel from bits per pixel and width
  * @param  bpp: bit per pixel
  * @param  width : image width
  * @param  pitch : image line width
  * @param  bytesperpixel : bytes per pixzl
  * @retval None
  */
static void BMP_ComputeImagePitch(int32_t bpp, int32_t width, int32_t *pitch, int32_t *bytesperpixel) {
	int32_t linesize;
	int32_t bytespp = 1;

	if (bpp == 1)
		linesize = PIX2BYTES(width);
	else if (bpp <= 4)
		linesize = PIX2BYTES(width << 2);
	else if (bpp <= 8)
		linesize = width;
	else if (bpp <= 16) {
		linesize = width * 2;
		bytespp = 2;
	} else if (bpp <= 24) {
		linesize = width * 3;
		bytespp = 3;
	} else {
		linesize = width * 4;
		bytespp = 4;
	}

	*pitch = (linesize + 3) & ~3;  //rows are DWORD right aligned (Son multiple de 32bits = 4 bytes)
	*bytesperpixel = bytespp;
}

// Display a line on the LCD screen
static void BMP_DisplayLine(IMAGEHDR *imageHeader, uint8_t *buff, BMP_Effect_TypeDef effect) {
	uint16_t i = 0;
	uint16_t pixel = 0;
	uint8_t  r_pix = 0, g_pix = 0, b_pix = 0;

	// Print each pixel in the line (row)
	for (i=0; i < imageHeader->pitch; i = i + imageHeader->bytesperpixel) {

		if (i >= (imageHeader->width*imageHeader->bytesperpixel)) {
			// Discard the Bytes of padding (for have lines of multiple ob 4bytes)
			// the bytes of padding normally have a value of 0x00
			continue;
		}

		// Get RGB in 8bits each (24 or 16 bits)
		if (imageHeader->bpp == 24) {
			b_pix = buff[i];
			g_pix = buff[i+1] ;
			r_pix = buff[i+2];

		} else if(imageHeader->bpp == 16) {
			pixel = (buff[i+1] << 8 | buff[i]);
			r_pix = BMP_PIXEL16_TO_R(pixel);
			g_pix = BMP_PIXEL16_TO_G(pixel);
			b_pix = BMP_PIXEL16_TO_B(pixel);
		}

		// Apply the effect
		switch(effect) {
		case BMP_NORMAL:
			pixel  = ASSEMBLE_RGB(r_pix, g_pix, b_pix);
			break;
      
		case BMP_BLUISH:
			pixel  = BMP_RGB_TO_PIXEL(b_pix,0,0);
			break;

		case BMP_GREENISH:
			pixel  = BMP_RGB_TO_PIXEL(0,g_pix,0);
			break;
      
		case BMP_REDISH:
			pixel  = BMP_RGB_TO_PIXEL(0,0,r_pix);
			break;
      
		case BMP_BLACK_WHITE:
			if((b_pix + g_pix + r_pix)/3 > BMP_BLACK_LEVEL) {
				pixel  = BMP_RGB_TO_PIXEL(0xFF,0xFF,0xFF);
			} else {
				pixel = BMP_RGB_TO_PIXEL(0,0,0);
			}
			break;
     
		default:
			pixel  = 0xFFFF;
			break;
		}
    
		LCD_WriteRAM(pixel);
	}
}

