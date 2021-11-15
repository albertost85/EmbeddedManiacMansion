/*
 * lcd_ili9325.h
 *
 *  Created on: Feb 29, 2012
 *    Modified: Apr 28, 2014
 *      Author: Marcel
 */

#ifndef LCD_ILI9325_H_
#define LCD_ILI9325_H_

#ifdef __cplusplus
 extern "C" {
#endif

#include "stm32f4xx.h"
#include "..\SysTick\delay.h"
#include ".\Fonts\fonts.h"

// ORGANITZACIO DE LA NOR/SRAM PEL FSMC
// -----------------------------------
// El Bank 1 esta dividit en 4 regions:
// NE1 0x60000000
// NE2 0x64000000
// NE3 0x68000000
// NE4 0x6C000000
// Hi ha 26 linies de direccio A0-A25
// Hi han 16 bits de dades D0-D15
// Cada regio te 2^26 posicions = 67108864 = 0x4000000h
// Cada posicio es de 8 bits => memoria de cada regio = 2^26*8 bits = 64 Mbytes per cada regio
// RECORDAR:
// - Si utilitzem  8 bits de dades, el direccionament es calcula ADDR[25:0]
// - Si utilitzem 16 bits de dades, el direccionament es calcula (ADDR[25:1] >> 1), el A0 sempre sera 0.

// CONFIGURAR EL LCD PER A QUE FUNCIONI AMB EL FSMC
// -----------------------------------------------
// D0-D15 => FSMC_D0-FSMC_D15 (PD14, PD15, PD0, PD1, PE7, PE8, PE9, PE10,
//                             PE11, PE13, PE13, PE14, PE15, PD8, PD9, PD10)
// WR     => FSMC_NWE (PD5) (Negate Write Enable)
// RD     => FSMC_NOE (PD4) (Negate Output Enable)
// CS     => FSMC_NEx (Depen de la regio que utilitzem: NE1=PD7, NE2=PG9, NE3=PG10, NE4=PG12) (Negate Enable Region X)
// RS(DC) => FSMC_Axx (Es fa anar un bit d'adreca: A0=PF0 o A16=PD11 o A19=PE3, ... recorda que pel de 100pins A0-A15 no estan)
// RST    => El HW Reset del LCD utilitzar qualsevol pin lluire
// BKL    => Blacklight ON/OFF utilitzar qualsevol pin lliure (Compatible amb una sortida PWM per regular el brightness)
//
//

// Posicions de memoria de control del LCD
// Si utilitzem la Regio 3 (NE3) del Bank 1 i el bit adreca A0 per al RS
#define Bank1_REG3_ADR0          ((uint32_t)(0x60000000 | 0x08000000))
#define Bank1_REG3_ADR2          ((uint32_t)(0x60000000 | 0x08000000 | 0x00000002)) // (la adreca 0x2  posara el A0 a 1 ==> RS=1)

// Si utilitzem la Regio 1 (NE1) del Bank 1 i el bit adreca A16 per al RS, o el A19
#define Bank1_REG1_ADR0          ((uint32_t)(0x60000000)) // (la adreca 0x00 posara el A16 a 0 ==> RS=0)
#define Bank1_REG1_ADR20000      ((uint32_t)(0x60000000 | 0x00020000)) // (la adreca 0x20000  posara el A16 a 1 ==> RS=1)
#define Bank1_REG1_ADR100000     ((uint32_t)(0x60000000 | 0x00100000)) // (la adreca 0x100000 posara el A19 a 1 ==> RS=1)

// Variables per enviar/rebre dades
#define LCD_REG           *(__IO uint16_t *) (Bank1_REG1_ADR0)
#define LCD_RAM           *(__IO uint16_t *) (Bank1_REG1_ADR100000)

// Definir la regio del bank 1 que s'utiliza
#define FSMC_Bank1_NORSRAMx  FSMC_Bank1_NORSRAM1 // Bank1 - Region 1 (NE1 per activar/desactivar)

// LCD RS(DC) Pin. Contected to FSMC_A19 (PE3)
#define LCD_RS_PIN               GPIO_Pin_3
#define LCD_RS_GPIO_PORT         GPIOE
#define LCD_RS_GPIO_CLK          RCC_AHB1Periph_GPIOE
#define LCD_RS_SOURCE            GPIO_PinSource3

// LCD CS Pin. Contected to NE1 (PD7)
#define LCD_CS_PIN               GPIO_Pin_7
#define LCD_CS_GPIO_PORT         GPIOD
#define LCD_CS_GPIO_CLK          RCC_AHB1Periph_GPIOD
#define LCD_CS_SOURCE            GPIO_PinSource7

// LCD Reset Pin. Conected to PD3
#define LCD_RST_PIN               GPIO_Pin_3
#define LCD_RST_GPIO_PORT         GPIOD
#define LCD_RST_GPIO_CLK          RCC_AHB1Periph_GPIOD

// PWM BackLight Pin. Conected to PD13
#define LCD_PWM_PIN               GPIO_Pin_13
#define LCD_PWM_GPIO_PORT         GPIOD
#define LCD_PWM_GPIO_CLK          RCC_AHB1Periph_GPIOD


// Type defines of LCD orientation
typedef enum  {
	LCD_LANDSCAPE		= 0, // Horitzontal
	LCD_LANDSCAPE_FLIP	= 1, // Horitzontal-Girada
	LCD_PORTAIT			= 2, // Vertical
	LCD_PORTAIT_FLIP	= 3, // Vertical-Girada
} LCD_Orientation_TypeDef;

// Mode of entry data in GDDRAM
typedef enum  {
	LCD_ENTRY_MODE_BMP    = 0,
	LCD_ENTRY_MODE_NORMAL = 1
} LCD_EntryMode_TypeDef;

// LCD COLORS
#define COLOR_BLACK		0x0000
#define COLOR_BLUE		0x001F
#define COLOR_BLUE2 	0x051F
#define COLOR_RED 		0xF800
#define COLOR_GREEN		0x07E0
#define COLOR_CYAN		0x07FF
#define COLOR_MAGENTA 	0xF81F
#define COLOR_YELLOW	0xFFE0
#define COLOR_WHITE		0xFFFF
#define COLOR_GRAY    	0xF7DE

// Grayscale Colors
#define COLOR_DARKGRAY      0x18E3    //  30  30  30
#define COLOR_DARKERGRAY    0x528A    //  80  80  80
#define COLOR_MEDIUMGRAY    0x8410    // 128 128 128
#define COLOR_LIGHTGRAY     0xCE59    // 200 200 200
#define COLOR_PALEGRAY      0xE71C    // 225 225 225

// SSD2119 Registers
#define SSD2119_DEVICE_CODE_READ_REG  0x00
#define SSD2119_OSC_START_REG         0x00
#define SSD2119_OUTPUT_CTRL_REG       0x01
#define SSD2119_LCD_DRIVE_AC_CTRL_REG 0x02
#define SSD2119_PWR_CTRL_1_REG        0x03
#define SSD2119_DISPLAY_CTRL_REG      0x07
#define SSD2119_FRAME_CYCLE_CTRL_REG  0x0B
#define SSD2119_PWR_CTRL_2_REG        0x0C
#define SSD2119_PWR_CTRL_3_REG        0x0D
#define SSD2119_PWR_CTRL_4_REG        0x0E
#define SSD2119_GATE_SCAN_START_REG   0x0F
#define SSD2119_SLEEP_MODE_1_REG      0x10
#define SSD2119_ENTRY_MODE_REG        0x11
#define SSD2119_SLEEP_MODE_2_REG      0x12
#define SSD2119_GEN_IF_CTRL_REG       0x15
#define SSD2119_PWR_CTRL_5_REG        0x1E
#define SSD2119_RAM_DATA_REG          0x22
#define SSD2119_FRAME_FREQ_REG        0x25
#define SSD2119_ANALOG_SET_REG        0x26
#define SSD2119_VCOM_OTP_1_REG        0x28
#define SSD2119_VCOM_OTP_2_REG        0x29
#define SSD2119_GAMMA_CTRL_1_REG      0x30
#define SSD2119_GAMMA_CTRL_2_REG      0x31
#define SSD2119_GAMMA_CTRL_3_REG      0x32
#define SSD2119_GAMMA_CTRL_4_REG      0x33
#define SSD2119_GAMMA_CTRL_5_REG      0x34
#define SSD2119_GAMMA_CTRL_6_REG      0x35
#define SSD2119_GAMMA_CTRL_7_REG      0x36
#define SSD2119_GAMMA_CTRL_8_REG      0x37
#define SSD2119_GAMMA_CTRL_9_REG      0x3A
#define SSD2119_GAMMA_CTRL_10_REG     0x3B
#define SSD2119_VSCROLL_CTRL_1_REG    0x41
#define SSD2119_VSCROLL_CTRL_2_REG    0x42
#define SSD2119_V_RAM_POS_REG         0x44
#define SSD2119_H_RAM_START_REG       0x45
#define SSD2119_H_RAM_END_REG         0x46
#define SSD2119_X_RAM_ADDR_REG        0x4E
#define SSD2119_Y_RAM_ADDR_REG        0x4F

 // Transforma de RGB888 (24bits) a RGB565 (16bits)
#define ASSEMBLE_RGB(R ,G, B)    ((((R)& 0xF8) << 8) | (((G) & 0xFC) << 3) | (((B) & 0xF8) >> 3))

// Funcions de communicacio
void LCD_WriteReg(uint8_t LCD_Reg, uint16_t LCD_RegValue);
uint16_t LCD_ReadReg(uint8_t LCD_Reg);
void LCD_WriteRAM_Prepare(void);
void LCD_WriteRAM(uint16_t RGB_Code);
uint16_t LCD_ReadRAM(void);

// Funcions inici LCD
void LCD_Reset(void);
uint8_t LCD_Init(void);
void LCD_SSD2119Config(void);
void LCD_CtrlLinesConfig(void);
void LCD_FSMCConfig(void);
void LCD_Clear(uint16_t color);

// Posa el cursor en una posicio. Estableix finestra per les dades
void LCD_SetCursor(uint16_t col, uint16_t row);
void LCD_SetWindow(uint16_t col, uint16_t row, uint16_t width, uint16_t height);

// Funcions per l'orientacio de la pantalla
LCD_Orientation_TypeDef LCD_GetOrientation(void);
void LCD_SetOrientation(LCD_Orientation_TypeDef mode);
void LCD_SetEntryMode(LCD_EntryMode_TypeDef mode);
uint16_t LCD_NumCols(void);
uint16_t LCD_NumRows(void);

// Funcions de control del scroll
void LCD_EnableScroll(uint8_t ison);
void LCD_SetScroll(uint16_t pixels_offset);

// Prova del LCD
void LCD_Test(void);

// Funcions per convertir de (RGB 565 16bits) a (RGB 888 24bits) i viceversa
uint16_t LCD_Get565Color(uint32_t rgb24bitsColor);
uint32_t LCD_Get888Color(uint16_t color16);

// Imprimeix un caracter o String
// OPCIO1:
void LCD_write_char(uint16_t col, uint16_t row, uint8_t car, uint16_t fontcolor, uint16_t backcolor, uint16_t transparent);
void LCD_write_string(uint16_t col, uint16_t row, uint8_t *s, uint16_t fontcolor, uint16_t backcolor, uint16_t transparent);
// OPCIO2:
void LCD_DisplayChar(uint16_t col, uint16_t row, uint8_t ascii, sFONT *pFont, uint16_t fontcolor, uint16_t backcolor, uint16_t transparent);
void LCD_DisplayString(uint16_t col, uint16_t row, char *s, sFONT *pFont, uint16_t fontcolor, uint16_t backcolor, uint16_t transparent);

// Funcions de Dibuixar
void LCD_DrawPixel(uint16_t col, uint16_t row, uint16_t color);
void LCD_DrawFilledRectangle(uint16_t col, uint16_t row, uint16_t width, uint16_t height, uint16_t color);
void LCD_DrawRectangle(uint16_t col, uint16_t row, uint16_t width, uint16_t height, uint8_t linewidth, uint16_t color);
void LCD_DrawCircle(uint16_t col, uint16_t row, uint16_t radius, uint16_t color);
void LCD_DrawFilledCircle(uint16_t col, uint16_t row, uint16_t radius, uint16_t color);
void LCD_DrawLine(uint16_t col, uint16_t row, uint16_t endcol, uint16_t endrow, uint16_t linewidth, uint16_t color);
void LCD_DrawLineBresenham(uint16_t col, uint16_t row, uint16_t endcol, uint16_t endrow, uint16_t color);

// Funcions de imatges
void LCD_WriteRAMBuffer(const uint16_t *pBuf, uint32_t size);
void LCD_DrawMonoPict(const uint32_t *Pict);
void LCD_DrawBMP(uint32_t BmpAddress);

#ifdef __cplusplus
}
#endif

#endif /* LCD_ILI9325_H_ */
