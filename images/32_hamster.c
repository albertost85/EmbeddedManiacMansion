/*********************************************************************
*                SEGGER Microcontroller GmbH & Co. KG                *
*        Solutions for real time microcontroller applications        *
*                           www.segger.com                           *
**********************************************************************
*                                                                    *
* C-file generated by                                                *
*                                                                    *
*        Bitmap Converter (ST) for emWin V5.32.                      *
*        Compiled Oct  8 2015, 11:58:22                              *
*                                                                    *
*        (c) 1998 - 2015 Segger Microcontroller GmbH & Co. KG        *
*                                                                    *
**********************************************************************
*                                                                    *
* Source file: 32_hamster                                            *
* Dimensions:  23 * 11                                               *
* NumColors:   16bpp: 65536                                          *
*                                                                    *
**********************************************************************
*/

#include <stdlib.h>

#include "GUI.h"

#ifndef GUI_CONST_STORAGE
  #define GUI_CONST_STORAGE const
#endif

extern GUI_CONST_STORAGE GUI_BITMAP bm32_hamster;

static GUI_CONST_STORAGE unsigned short _ac32_hamster[] = {
  0xF81F, 0xF81F, 0x8431, 0x8431, 0x8431, 0x8431, 0xF81F, 0x8431, 0x8431, 0x8431, 0x8431, 0xF81F, 0xF81F, 0xF81F, 0xF81F, 0xF81F, 0xF81F, 0xF81F, 0xF81F, 0x8431, 0xF81F, 0x0000, 0x7BD1,
  0x8432, 0x8432, 0x8432, 0x8432, 0x8432, 0x8432, 0x8432, 0x8432, 0x8432, 0x8432, 0x8432, 0x8432, 0x8432, 0x8432, 0x8432, 0x8432, 0x8432, 0x8432, 0x8432, 0x8432, 0x8432, 0x8432, 0x8432,
  0x8432, 0x8432, 0x8432, 0x8432, 0xB574, 0x8432, 0x8432, 0xB574, 0x8432, 0x8432, 0x8432, 0x8432, 0x8432, 0x71C2, 0x71C2, 0x71C2, 0x71C2, 0x8432, 0x8432, 0x8432, 0x8432, 0x8432, 0x8432,
  0x8432, 0x8432, 0x8432, 0xB574, 0xCE37, 0xB574, 0xB574, 0xBDB5, 0xB574, 0x0000, 0x8432, 0xB574, 0xB574, 0x71C2, 0x71C2, 0x71C2, 0x7202, 0x71C2, 0x71C2, 0x8432, 0x8432, 0x8432, 0x8432,
  0x8432, 0x8432, 0xB574, 0xD677, 0xEF3A, 0x7986, 0xD471, 0xE6F9, 0xDEB8, 0x0000, 0x0000, 0xD677, 0xCE37, 0xCE36, 0xC5F6, 0x7A43, 0x7A43, 0x7A43, 0x71C2, 0x71C2, 0x8432, 0x8432, 0x8432,
  0x8432, 0xB574, 0xC5F5, 0xD678, 0xE6F9, 0x528A, 0x528A, 0xE6F9, 0xDEB8, 0xDEB8, 0xDEB8, 0xD678, 0xD677, 0xCE37, 0xCE36, 0xC5F6, 0xC5F5, 0xBDB5, 0xBDB5, 0xB574, 0xB574, 0x8432, 0x8432,
  0x8432, 0xB34D, 0xC5F5, 0xCE36, 0xD678, 0xD678, 0xD678, 0xD678, 0xD677, 0xD678, 0xD678, 0xC5F6, 0xCE37, 0xCE37, 0xCE36, 0xC5F6, 0xC5F6, 0xC5F5, 0xBDB5, 0xBDB5, 0xB574, 0xB574, 0x8432,
  0x8432, 0x8432, 0xAD75, 0x528A, 0xCE36, 0xCE36, 0xC5F6, 0xCE36, 0xB574, 0xC5F6, 0xC5F6, 0xC5F5, 0xCE36, 0xCE36, 0xC5F6, 0xB574, 0x528A, 0x528A, 0xBDB5, 0xBDB5, 0xB574, 0xB574, 0x8432,
  0x8432, 0x8432, 0x528C, 0xB574, 0xBDB5, 0xBDB5, 0xBDB5, 0xB574, 0x528A, 0x528A, 0x528A, 0xBDB5, 0xBDB5, 0xB574, 0xBDB5, 0x528A, 0xBDB5, 0xBDB5, 0xB574, 0xB574, 0xB574, 0xB574, 0x8432,
  0x8432, 0x8432, 0xFD34, 0x528A, 0xAD75, 0x60C3, 0xB34D, 0x528A, 0x528A, 0xB574, 0xB574, 0xB574, 0xB574, 0xB574, 0x528A, 0x528A, 0xB574, 0xB574, 0xB574, 0xB574, 0x528A, 0x8432, 0x8432,
  0x8432, 0x8432, 0x8432, 0x8432, 0x8432, 0x8432, 0x8432, 0x8432, 0x8432, 0x8432, 0x8432, 0x8432, 0x8432, 0x8432, 0x8432, 0x8432, 0x8432, 0x8432, 0x8432, 0x8432, 0x8432, 0x8432, 0x8432
};

GUI_CONST_STORAGE GUI_BITMAP bm32_hamster = {
  23, // xSize
  11, // ySize
  46, // BytesPerLine
  16, // BitsPerPixel
  (unsigned char *)_ac32_hamster,  // Pointer to picture data
  NULL,  // Pointer to palette
  GUI_DRAW_BMPM565
};

/*************************** End of file ****************************/