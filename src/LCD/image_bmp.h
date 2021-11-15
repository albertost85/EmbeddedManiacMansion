/*
 * image_bmp.h
 *
 *  Created on: Apr 30, 2014
 *    Modified: .
 *      Author: Marcel
 */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __IMAGE_BMP_H
#define __IMAGE_BMP_H

/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx.h"
#include  "ff.h"
#include  "lcd_ssd2119.h"

// BITMAPFILEHEADER
typedef struct {
	uint8_t bfType[2];
	uint32_t bfSize;
	uint16_t bfReserved1;
	uint16_t bfReserved2;
	uint32_t bfOffBits;
} BMPFILEHEAD;

// BITMAPINFOHEADER
typedef struct {
	uint32_t BiSize;
	uint32_t BiWidth;
	uint32_t BiHeight;
	uint16_t BiPlanes;
	uint16_t BiBitCount;
	uint32_t BiCompression;
	uint32_t BiSizeImage;
	uint32_t BiXpelsPerMeter;
	uint32_t BiYpelsPerMeter;
	uint32_t BiClrUsed;
	uint32_t BiClrImportant;
} BMPINFOHEAD;

// BITMAPCOREHEADER
typedef struct {
  uint32_t bcSize;
  uint16_t bcWidth;
  uint16_t bcHeight;
  uint16_t bcPlanes;
  uint16_t bcBitCount;
} BMPCOREHEAD;

// IMAGEHEADER
typedef struct {
  int32_t  width;         /* image width in pixels*/
  int32_t  height;        /* image height in pixels*/
  int32_t  planes;        /* # image planes*/
  int32_t  bpp;           /* bits per pixel (1, 4 or 8)*/
  int32_t  pitch;         /* bytes per line*/
  int32_t  bytesperpixel; /* bytes per pixel*/
  int32_t  compression;   /* compression algorithm*/
  int32_t  palsize;       /* palette size*/
  int32_t  transcolor; /* transparent color or -1 if none*/

} IMAGEHDR;


/* Exported constants --------------------------------------------------------*/
#define IMAGE_UPSIDEDOWN       0x01 /* compression flag: upside down image */
#define IMAGE_BGR              0x00 /* compression flag: BGR byte order */
#define IMAGE_RGB              0x02 /* compression flag: RGB not BGR bytes */
#define IMAGE_ALPHA_CHANNEL    0x04 /* compression flag: 32-bit w/alpha */
#define IMAGE_555              0x08 /* compression flag: 5/5/5 format */
#define READ_SIZE              4092

#define FILEHEADSIZE           14
#define INFOHEADSIZE           40
#define COREHEADSIZE           12

// Type defines of BMP Effect
typedef enum  {
	BMP_NORMAL		= 0,
	BMP_BLUISH		= 1,
	BMP_GREENISH	= 2,
	BMP_REDISH		= 3,
	BMP_BLACK_WHITE = 4
} BMP_Effect_TypeDef;

// Type defines of Compresion Mode
typedef enum  {
	BI_RGB		 = 0L,
	BI_RLE8		 = 1L,
	BI_RLE4		 = 2L,
	BI_BITFIELDS = 3L
} BMP_Compresion_TypeDef;

// Results code
typedef enum {
	BMP_Success = 0,

	BMP_File_Open_Error,
	BMP_File_Read_Error,
	BMP_File_Not_Bitmap,
	BMP_File_Not_Compatible,
	BMP_File_TooBig,
	BMP_File_Not_Enough_Space,
	BMP_File_Format_Error

} BMP_ResCode;

#define BMP_BLACK_LEVEL        		150  // For black/white effect
#define BMP_MAX_PITCH				3000 // Maximum of pich image (witdh * bytesPerPixel + padding to adjust 4bytes)

/* Exported macro ------------------------------------------------------------*/
// RGB888 to RGB565
#define BMP_RGB_TO_PIXEL(r,g,b)  ((uint16_t) (((r & 0x00F8) >> 3) | ((g & 0x00FC) << 3) | ((b & 0x00F8) << 8)))

// RGB565 to RGB888
#define BMP_PIXEL16_TO_B(pixel)       ((pixel & 0x1F) << 3)
#define BMP_PIXEL16_TO_G(pixel)       (((pixel >> 5) & 0x3F) << 2)
#define BMP_PIXEL16_TO_R(pixel)       (((pixel >> 11) & 0x1F) << 3)

#define PIX2BYTES(n)      (((n)+7)/8) // Rounds to multiple of 2

/* Exported functions ------------------------------------------------------- */
BMP_ResCode BMP_Show(char *filename, uint16_t col, uint16_t row, BMP_Effect_TypeDef effect);


#endif /* __IMAGE_BMP_H */
