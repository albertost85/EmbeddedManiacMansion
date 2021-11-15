/*
 * lcd_ssd2119.c
 *
 *  Created on: Feb 29, 2012
 *    Modified: Apr 28, 2014
 *      Author: Marcel
 */

#include "lcd_ssd2119.h"
#include "lcd_fonts.h"

// Orientacio del LCD
LCD_Orientation_TypeDef lcdOrientation = LCD_LANDSCAPE;
LCD_EntryMode_TypeDef lcdEntryMode = LCD_ENTRY_MODE_NORMAL;

/*************************************************************************************************
*   L L E G I R / E S C R I U R E    D A D E S
*************************************************************************************************/
// Writes to the selected LCD register.
void LCD_WriteReg(uint8_t LCD_Reg, uint16_t LCD_RegValue) {
  /* Write 16-bit Index, then Write Reg */
  LCD_REG = LCD_Reg;
  /* Write 16-bit Reg */
  LCD_RAM = LCD_RegValue;
}

// Reads the selected LCD Register.
uint16_t LCD_ReadReg(uint8_t LCD_Reg) {
  /* Write 16-bit Index (then Read Reg) */
  LCD_REG = LCD_Reg;
  /* Read 16-bit Reg */
  return (LCD_RAM);
}

//Prepare to write to the LCD RAM.
void LCD_WriteRAM_Prepare(void) {
  LCD_REG = SSD2119_RAM_DATA_REG;
}

//Writes to the LCD RAM.
void LCD_WriteRAM(uint16_t RGB_Code) {
  /* Write 16-bit GRAM Reg */
  LCD_RAM = RGB_Code;
}

// Reads the LCD RAM.
uint16_t LCD_ReadRAM(void) {
  /* Write 16-bit Index (then Read Reg) */
  LCD_REG = SSD2119_RAM_DATA_REG; /* Select GRAM Reg */
  /* Read 16-bit Reg */
  return LCD_RAM;
}

/*************************************************************************************************
*   I N I C I A L I T Z A C I O
*************************************************************************************************/
// HW LCD Reset
void LCD_Reset(void) {
	GPIO_ResetBits(LCD_RST_GPIO_PORT, LCD_RST_PIN);
	Delay_ms(200);
	GPIO_SetBits(LCD_RST_GPIO_PORT, LCD_RST_PIN);
	Delay_ms(50);
}


uint8_t LCD_Init(void) {

	// Configurar el GPIO
	LCD_CtrlLinesConfig();

	// Configurar la interficie paralela FSMC
	LCD_FSMCConfig();

	Delay_ms(5); // Delay 5ms

	// Reset
	LCD_Reset();

	// Llegir i comprovar el ID del LCD
	uint32_t lcdID;
	lcdID = LCD_ReadReg(0x00);
	switch(lcdID) {
		case 0x9919: /* The LCD is SSD2119 Controller */
			break;

		case 0x9320: /* The LCD is ILI9320 Controller */
		case 0x9325: /* The LCD is ILI9325 Controller */
		default:
			return 1; // Not supported!
	}

	// Initialize the SSD2119 LCD driver
	LCD_SSD2119Config();

	// Activa la iluminacio (PWM BackLight sempre al maxim: sortida digital)
	GPIO_SetBits(LCD_PWM_GPIO_PORT, LCD_PWM_PIN);

	return 0;
}

// Initialize the SSD2119 LCD driver
void LCD_SSD2119Config(void) {
	/* Enter sleep mode (if we are not already there).*/
	LCD_WriteReg(SSD2119_SLEEP_MODE_1_REG, 0x0001);

	/* Set initial power parameters. */
	LCD_WriteReg(SSD2119_PWR_CTRL_5_REG, 0x00B2);
	LCD_WriteReg(SSD2119_VCOM_OTP_1_REG, 0x0006);

	/* Start the oscillator.*/
	LCD_WriteReg(SSD2119_OSC_START_REG, 0x0001);

	/* Set pixel format and basic display orientation (scanning direction).*/
	LCD_WriteReg(SSD2119_OUTPUT_CTRL_REG, 0x30EF);
	LCD_WriteReg(SSD2119_LCD_DRIVE_AC_CTRL_REG, 0x0600);

	/* Exit sleep mode.*/
	LCD_WriteReg(SSD2119_SLEEP_MODE_1_REG, 0x0000);
	Delay_ms(5);

	/* Configure pixel color format and MCU interface parameters.*/
	LCD_WriteReg(SSD2119_ENTRY_MODE_REG, 0x6830);

	/* Set analog parameters */
	LCD_WriteReg(SSD2119_SLEEP_MODE_2_REG, 0x0999);
	LCD_WriteReg(SSD2119_ANALOG_SET_REG, 0x3800);

	/* Enable the display */
	LCD_WriteReg(SSD2119_DISPLAY_CTRL_REG, 0x0033);

	/* Set VCIX2 voltage to 6.1V.*/
	LCD_WriteReg(SSD2119_PWR_CTRL_2_REG, 0x0005);

	/* Configure gamma correction.*/
	LCD_WriteReg(SSD2119_GAMMA_CTRL_1_REG, 0x0000);
	LCD_WriteReg(SSD2119_GAMMA_CTRL_2_REG, 0x0303);
	LCD_WriteReg(SSD2119_GAMMA_CTRL_3_REG, 0x0407);
	LCD_WriteReg(SSD2119_GAMMA_CTRL_4_REG, 0x0301);
	LCD_WriteReg(SSD2119_GAMMA_CTRL_5_REG, 0x0301);
	LCD_WriteReg(SSD2119_GAMMA_CTRL_6_REG, 0x0403);
	LCD_WriteReg(SSD2119_GAMMA_CTRL_7_REG, 0x0707);
	LCD_WriteReg(SSD2119_GAMMA_CTRL_8_REG, 0x0400);
	LCD_WriteReg(SSD2119_GAMMA_CTRL_9_REG, 0x0a00);
	LCD_WriteReg(SSD2119_GAMMA_CTRL_10_REG, 0x1000);

	/* Configure Vlcd63 and VCOMl */
	LCD_WriteReg(SSD2119_PWR_CTRL_3_REG, 0x000A);
	LCD_WriteReg(SSD2119_PWR_CTRL_4_REG, 0x2E00);


	// Orientacio del LCD (R11h) and Normal Mode
	LCD_SetOrientation(LCD_LANDSCAPE);
	LCD_SetEntryMode(LCD_ENTRY_MODE_NORMAL);

	// Set GRAM area (R44h,R45h,R46h)
	LCD_SetWindow(0,0,LCD_NumCols(),LCD_NumRows());

	// Desactiva el scroll
	LCD_EnableScroll(0);

	//GRAM Horizontal/Vertical Address Set (R4Eh, R4Fh)
	LCD_SetCursor(0,0);

	Delay_ms(300);

	// Neteja Pantalla
	LCD_Clear(COLOR_BLACK);

}

// Configure the GPIO LCD Control pins utilitzant el bus FSMC
void LCD_CtrlLinesConfig(void) {
	  GPIO_InitTypeDef GPIO_InitStructure;

	  /* GPIOD Configuration:
	   * SRAM Data lines (PD14=D0, PD15=D1, PD0=D2, PD1=D3, PD8=D13, PD9=D14, PD10=D15)
	   * NOE and NWE configuration (PD4(NOE)=RD, PD5(NWE)=RW) */
	  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD, ENABLE);
	  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_8 | GPIO_Pin_9 |
	                                GPIO_Pin_10 | GPIO_Pin_14 | GPIO_Pin_15 | GPIO_Pin_4 |GPIO_Pin_5;
	  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
	  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	  GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_NOPULL;
	  GPIO_Init(GPIOD, &GPIO_InitStructure);
	  GPIO_PinAFConfig(GPIOD, GPIO_PinSource0, GPIO_AF_FSMC);
	  GPIO_PinAFConfig(GPIOD, GPIO_PinSource1, GPIO_AF_FSMC);
	  GPIO_PinAFConfig(GPIOD, GPIO_PinSource4, GPIO_AF_FSMC);
	  GPIO_PinAFConfig(GPIOD, GPIO_PinSource5, GPIO_AF_FSMC);
	  GPIO_PinAFConfig(GPIOD, GPIO_PinSource8, GPIO_AF_FSMC);
	  GPIO_PinAFConfig(GPIOD, GPIO_PinSource9, GPIO_AF_FSMC);
	  GPIO_PinAFConfig(GPIOD, GPIO_PinSource10, GPIO_AF_FSMC);
	  GPIO_PinAFConfig(GPIOD, GPIO_PinSource14, GPIO_AF_FSMC);
	  GPIO_PinAFConfig(GPIOD, GPIO_PinSource15, GPIO_AF_FSMC);

	  /* GPIOE Configuration:
	   * SRAM Data lines (PE7-PE15=D4-D12)
	   * NOE and NWE configuration (PD4(NOE)=RD, PD5(NWE)=RW) */
	  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOE, ENABLE);
	  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7 | GPIO_Pin_8 | GPIO_Pin_9 | GPIO_Pin_10 |
	                                GPIO_Pin_11 | GPIO_Pin_12 | GPIO_Pin_13 | GPIO_Pin_14 |
	                                GPIO_Pin_15;
	  GPIO_Init(GPIOE, &GPIO_InitStructure);
	  GPIO_PinAFConfig(GPIOE, GPIO_PinSource7 , GPIO_AF_FSMC);
	  GPIO_PinAFConfig(GPIOE, GPIO_PinSource8 , GPIO_AF_FSMC);
	  GPIO_PinAFConfig(GPIOE, GPIO_PinSource9 , GPIO_AF_FSMC);
	  GPIO_PinAFConfig(GPIOE, GPIO_PinSource10 , GPIO_AF_FSMC);
	  GPIO_PinAFConfig(GPIOE, GPIO_PinSource11 , GPIO_AF_FSMC);
	  GPIO_PinAFConfig(GPIOE, GPIO_PinSource12 , GPIO_AF_FSMC);
	  GPIO_PinAFConfig(GPIOE, GPIO_PinSource13 , GPIO_AF_FSMC);
	  GPIO_PinAFConfig(GPIOE, GPIO_PinSource14 , GPIO_AF_FSMC);
	  GPIO_PinAFConfig(GPIOE, GPIO_PinSource15 , GPIO_AF_FSMC);


	  /* GPIO RS Pin Config:
	   * SRAM Address line (FSMC_AXX)=RS) */
	  RCC_AHB1PeriphClockCmd(LCD_RS_GPIO_CLK, ENABLE);
	  GPIO_InitStructure.GPIO_Pin = LCD_RS_PIN;
	  GPIO_Init(LCD_RS_GPIO_PORT, &GPIO_InitStructure);
	  GPIO_PinAFConfig(LCD_RS_GPIO_PORT, LCD_RS_SOURCE, GPIO_AF_FSMC);


	  /* GPIO CS Pin Configuration:
	   * NEx configuration (NEx=CS) */
	  RCC_AHB1PeriphClockCmd(LCD_CS_GPIO_CLK, ENABLE);
	  GPIO_InitStructure.GPIO_Pin = LCD_CS_PIN;
	  GPIO_Init(LCD_CS_GPIO_PORT, &GPIO_InitStructure);
	  GPIO_PinAFConfig(LCD_CS_GPIO_PORT, LCD_CS_SOURCE, GPIO_AF_FSMC);

	  /* GPIO RESET Pin Configuration: */
	  RCC_AHB1PeriphClockCmd(LCD_RST_GPIO_CLK, ENABLE);
	  GPIO_InitStructure.GPIO_Pin = LCD_RST_PIN;
	  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
	  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	  GPIO_Init(LCD_RST_GPIO_PORT, &GPIO_InitStructure);

	  /* GPIO PWM BackLight Pin Configuration: */
	  RCC_AHB1PeriphClockCmd(LCD_PWM_GPIO_CLK, ENABLE);
	  GPIO_InitStructure.GPIO_Pin = LCD_PWM_PIN;
	  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
	  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	  GPIO_Init(LCD_PWM_GPIO_PORT, &GPIO_InitStructure);
 }


 // Configure the FSMC Parallel interface for LCD communication
 void LCD_FSMCConfig(void) {
	  FSMC_NORSRAMInitTypeDef  FSMC_NORSRAMInitStructure;
	  FSMC_NORSRAMTimingInitTypeDef  p;

	  /* Enable FSMC clock */
	  RCC_AHB3PeriphClockCmd(RCC_AHB3Periph_FSMC, ENABLE);

	  /*-- FSMC Configuration ------------------------------------------------------*/
	  /*----------------------- SRAM Bank 1 ----------------------------------------*/
	  /* FSMC_Bank1_NORSRAMx configuration */
	  p.FSMC_AddressSetupTime = 5;
	  p.FSMC_AddressHoldTime = 0;
	  p.FSMC_DataSetupTime = 9;
	  p.FSMC_BusTurnAroundDuration = 0;
	  p.FSMC_CLKDivision = 0;
	  p.FSMC_DataLatency = 0;
	  p.FSMC_AccessMode = FSMC_AccessMode_A;
	  /* Color LCD configuration ------------------------------------
	     LCD configured as follow:
	        - Data/Address MUX = Disable
	        - Memory Type = SRAM
	        - Data Width = 16bit
	        - Write Operation = Enable
	        - Extended Mode = Enable
	        - Asynchronous Wait = Disable */

	  FSMC_NORSRAMInitStructure.FSMC_Bank = FSMC_Bank1_NORSRAMx;
	  FSMC_NORSRAMInitStructure.FSMC_DataAddressMux = FSMC_DataAddressMux_Disable;
	  FSMC_NORSRAMInitStructure.FSMC_MemoryType = FSMC_MemoryType_SRAM;
	  FSMC_NORSRAMInitStructure.FSMC_MemoryDataWidth = FSMC_MemoryDataWidth_16b;
	  FSMC_NORSRAMInitStructure.FSMC_BurstAccessMode = FSMC_BurstAccessMode_Disable;
	  FSMC_NORSRAMInitStructure.FSMC_AsynchronousWait = FSMC_AsynchronousWait_Disable;
	  FSMC_NORSRAMInitStructure.FSMC_WaitSignalPolarity = FSMC_WaitSignalPolarity_Low;
	  FSMC_NORSRAMInitStructure.FSMC_WrapMode = FSMC_WrapMode_Disable;
	  FSMC_NORSRAMInitStructure.FSMC_WaitSignalActive = FSMC_WaitSignalActive_BeforeWaitState;
	  FSMC_NORSRAMInitStructure.FSMC_WriteOperation = FSMC_WriteOperation_Enable;
	  FSMC_NORSRAMInitStructure.FSMC_WaitSignal = FSMC_WaitSignal_Disable;
	  FSMC_NORSRAMInitStructure.FSMC_ExtendedMode = FSMC_ExtendedMode_Disable;
	  FSMC_NORSRAMInitStructure.FSMC_WriteBurst = FSMC_WriteBurst_Disable;
	  FSMC_NORSRAMInitStructure.FSMC_ReadWriteTimingStruct = &p;
	  FSMC_NORSRAMInitStructure.FSMC_WriteTimingStruct = &p;

	  FSMC_NORSRAMInit(&FSMC_NORSRAMInitStructure);

	  /* Enable FSMC NOR/SRAM Bank 1 Region x */
	  FSMC_NORSRAMCmd(FSMC_Bank1_NORSRAMx, ENABLE);
 }


// Posa el LCD amb color
void LCD_Clear(uint16_t color) {
	uint32_t index = 0;

	LCD_SetWindow(0,0,LCD_NumCols(),LCD_NumRows());
	LCD_SetCursor(0,0);

	LCD_WriteRAM_Prepare(); /* Prepare to write GRAM */

	for(index = 0; index < 76800; index++) {
		LCD_RAM = color;
	}
}

 /*************************************************************************************************
 *   P O S I C I N A M E N T    D E L    C U R S O R     I     F I N E S T R A
 *************************************************************************************************/
 //Posa el cursor a la posicio col (columna) i row (fila) comencant amb (0,0) dalt a la esquerra.
void LCD_SetCursor(uint16_t col, uint16_t row) {
 	uint16_t Xpos=0, Ypos=0;

 	if (lcdOrientation == LCD_LANDSCAPE) {
 		Xpos = col;
 		Ypos = row;

 	} else if (lcdOrientation == LCD_LANDSCAPE_FLIP) {
 		Xpos = 319-col;
 		Ypos = 239-row;

 	} else if (lcdOrientation == LCD_PORTAIT) {
 		Xpos = row;
 		Ypos = 239-col;

 	} else if (lcdOrientation == LCD_PORTAIT_FLIP) {
 		Xpos = 319-row;
 		Ypos = col;
 	}

 	LCD_WriteReg(SSD2119_X_RAM_ADDR_REG, Xpos); // Horizontal cursor position for GRAM (0-239)
 	LCD_WriteReg(SSD2119_Y_RAM_ADDR_REG, Ypos); // Vertical cursor position for GRAM   (0-319)
 }



 // Estableix la finestra de treball desde (col,row) que comenca a (0,0) i de tamany width*height
 // (col,row)
 //  * -----> width
 //  |
 //  |
 //  v height
 //
void LCD_SetWindow(uint16_t col, uint16_t row, uint16_t width, uint16_t height) {
 	uint16_t Xinici=0, Yinici=0, Xfinal=100, Yfinal=100;

 	if (lcdOrientation == LCD_LANDSCAPE) {
 		Xinici = col;
 		Xfinal = col+width-1;
 		Yinici = row;
 		Yfinal = row+height-1;

 	} else if (lcdOrientation == LCD_LANDSCAPE_FLIP) {
 		Xinici = 319-col-width+1;
 		Xfinal = 319-col;
 		Yinici = 239-row-height+1;
 		Yfinal = 239-row;


 	} else if (lcdOrientation == LCD_PORTAIT) {
 		Xinici = row;
 		Xfinal = row+height-1;
 		Yinici = 239-col-width+1;
 		Yfinal = 239-col;


 	} else if (lcdOrientation == LCD_PORTAIT_FLIP) {
 		Xinici = 319-row-height+1;
 		Xfinal = 319-row;
 		Yinici = col;
 		Yfinal = col+width-1;


 	}

	/* Set the display size and ensure that the GRAM window is set to allow
	access to the full display buffer.*/
	LCD_WriteReg(SSD2119_H_RAM_START_REG, Xinici);					// Horizontal starting position of GRAM
	LCD_WriteReg(SSD2119_H_RAM_END_REG, Xfinal);					// Horizontal end position GRAM
	LCD_WriteReg(SSD2119_V_RAM_POS_REG, (Yfinal << 8) | Yinici);	// Vertical starting/end position of GRAM

 }


/*************************************************************************************************
*   F U N C I O N S    -    O R I E N T A C I O
*************************************************************************************************/
// Retorna la orientacio establerta
LCD_Orientation_TypeDef LCD_GetOrientation(void) {
	return lcdOrientation;
}


// Orientacio del LCD, de 0 a 3  (0 i 1 Verticals, 2 i 3 Horitzontals)
void LCD_SetOrientation(LCD_Orientation_TypeDef mode) {

	lcdOrientation = mode;

	// Entry Mode (R11H) Screen rotation control (AM and ID, see datasheet)
	if (mode == LCD_LANDSCAPE) {
		if (lcdEntryMode == LCD_ENTRY_MODE_NORMAL) {
			LCD_WriteReg(SSD2119_ENTRY_MODE_REG, 0x6830); // AM=0 (Horizontal) and ID[1:0]=11
		} else { // BMP Mode
			LCD_WriteReg(SSD2119_ENTRY_MODE_REG, 0x6810); // AM=0 (Horizontal) and ID[1:0]=01
		}
	} else if (mode == LCD_LANDSCAPE_FLIP) {
		if (lcdEntryMode == LCD_ENTRY_MODE_NORMAL) {
			LCD_WriteReg(SSD2119_ENTRY_MODE_REG, 0x6800); // AM=0 (Horizontal) and ID[1:0]=00
		} else { // BMP Mode
			LCD_WriteReg(SSD2119_ENTRY_MODE_REG, 0x6820); // AM=0 (Horizontal) and ID[1:0]=10
		}

	} else if (mode == LCD_PORTAIT) {
		if (lcdEntryMode == LCD_ENTRY_MODE_NORMAL) {
			LCD_WriteReg(SSD2119_ENTRY_MODE_REG, 0x6818); // AM=1 (Vertical) and ID[1:0]=01
		} else { // BMP Mode
			LCD_WriteReg(SSD2119_ENTRY_MODE_REG, 0x6808); // AM=1 (Vertical) and ID[1:0]=00
		}

	} else if (mode == LCD_PORTAIT_FLIP) {
		if (lcdEntryMode == LCD_ENTRY_MODE_NORMAL) {
			LCD_WriteReg(SSD2119_ENTRY_MODE_REG, 0x6828); // AM=1 (Vertical) and ID[1:0]=10
		} else { // BMP Mode
			LCD_WriteReg(SSD2119_ENTRY_MODE_REG, 0x6838); // AM=1 (Vertical) and ID[1:0]=11
		}
	}

	LCD_SetWindow(0,0,LCD_NumCols(), LCD_NumRows());
	LCD_SetCursor(0,0);
}

// Entry Mode LCD
void LCD_SetEntryMode(LCD_EntryMode_TypeDef mode) {
	lcdEntryMode = mode;

	// Acualitza estat
	LCD_SetOrientation(lcdOrientation);

}

// Retorna el num de columnes
uint16_t LCD_NumCols(void) {
	if (lcdOrientation == LCD_LANDSCAPE || lcdOrientation == LCD_LANDSCAPE_FLIP)
		return 320;
	else
		return 240;
}

// Retorna el num de files
uint16_t LCD_NumRows(void) {
	if (lcdOrientation == LCD_LANDSCAPE || lcdOrientation == LCD_LANDSCAPE_FLIP)
		return 240;
	else
		return 320;
}




/*************************************************************************************************
*   F U N C I O N S    -    S C R O L L
*************************************************************************************************/
// Activa el Scroll (NOMES EN VERTICAL)
void LCD_EnableScroll(uint8_t ison) {
	if (ison > 0) {
		LCD_WriteReg(SSD2119_DISPLAY_CTRL_REG, 0x0033 | 0x200); // Enables the scroll
	} else {
		LCD_WriteReg(SSD2119_DISPLAY_CTRL_REG, 0x0033 & 0xFDFF); // Disables the scroll
	}

}


// Scroll (Offset dels pixels) (NOMES EN VERTICAL)
void LCD_SetScroll(uint16_t pixels_offset) {
	// Note: Not all ILI9325 imitations support HW vertical scrolling!
	// ST7781 - Not supported (ex. RFTechWorld 2.8" displays)
	// OTM3225A - Supported
	if (pixels_offset > LCD_NumRows()-1) pixels_offset = LCD_NumRows()-1;
	LCD_WriteReg(SSD2119_VSCROLL_CTRL_1_REG, pixels_offset);

}


/*************************************************************************************************
*   T E S T    L C D
*************************************************************************************************/
// Prova del LCD
void LCD_Test(void) {
	uint16_t fil, col;
	uint16_t color;
	uint16_t NCOL, NROW;

	NCOL = LCD_NumCols();
	NROW = LCD_NumRows();

	LCD_SetWindow(0, 0, NCOL, NROW); // Finestra activa
	LCD_SetCursor(0,0); // Posicio del cursor a dalt a la dreta

	LCD_WriteRAM_Prepare(); /* Prepare to write GRAM */

	for(fil=0;fil<NROW;fil++) {
		for(col=0;col<NCOL;col++) {
			if(col>(NCOL/8)*7) color=COLOR_PALEGRAY;
			else if(col>(NCOL/8)*6) color=COLOR_BLUE;
			else if(col>(NCOL/8)*5) color=COLOR_GREEN;
			else if(col>(NCOL/8)*4) color=COLOR_CYAN;
			else if(col>(NCOL/8)*3) color=COLOR_RED;
			else if(col>(NCOL/8)*2) color=COLOR_MAGENTA;
			else if(col>(NCOL/8)) color=COLOR_YELLOW;
			else color=COLOR_WHITE;

			LCD_RAM = color;

			//Delay_ms(1); //Per veure com ho fa
		}
	}

}


/*************************************************************************************************
*   F U N C I O N S    -    C O N V E R S I O N S
*************************************************************************************************/
// Pasa de (RGB 888 24bits) a (RGB 565 16bits)
uint16_t LCD_Get565Color(uint32_t rgb24bitsColor) {

	uint32_t r, g, b;
	uint16_t res;

	// Obtenim el valor dels colors
	b = (rgb24bitsColor>>0)  & 0xFF;
	g = (rgb24bitsColor>>8)  & 0xFF;
	r = (rgb24bitsColor>>16) & 0xFF;

	// Els escalem entre 0 i 31, o 63 (pel verd)
	b = b*31/255;
	g = g*63/255; //Son 6 bits => valor maxim es 63
	r = r*31/255;

	// Formem el color de 16 bits
	res = (r<<11) + (g<<5) + (b<<0);

	return res;
}



// Pasa de (RGB 565 16bits) a (RGB 888 24bits)
uint32_t LCD_Get888Color(uint16_t color16) {

	uint32_t r, g, b;
	uint32_t res;

	// Obtenim el valor dels colors
	b = (color16>>0)  & 0x1f;
	g = (color16>>5)  & 0x3f;
	r = (color16>>11) & 0x1f;

	// Els escalem entre 0 i 255
	b = b*255/31;
	g = g*255/63; //Son 6 bits => valor maxim es 63
	r = r*255/31;

	// Formem el color de 24 bits
	res = (r<<16) + (g<<8) + (b<<0);

	return res;

}



/*************************************************************************************************
*   E S C R I U R E    C A R A C T E R     I     S T R I N G
*************************************************************************************************/

// Display a Char with FontType
void LCD_DisplayChar(uint16_t col, uint16_t row, uint8_t ascii, sFONT *pFont, uint16_t fontcolor, uint16_t backcolor, uint16_t transparent) {
	const uint8_t *c;
	uint16_t height, width;
	uint32_t i = 0, j = 0;
	uint8_t  offset;
	uint8_t  *pchar;
	uint32_t line;

	// Pointer to char data
	c = &pFont->table[(ascii-' ') * pFont->Height * ((pFont->Width + 7)/8)];

	// Font size
	width = pFont->Width;
	height = pFont->Height;
	offset =  8 *((width + 7)/8) -  width;

	// Set GRAM area
	LCD_SetWindow(col, row, width, height);
	LCD_SetCursor(col, row);

	if (transparent == SET) {
		line = 0;
		for(i = 0; i < height; i++) {
			pchar = ((uint8_t *)c + (width + 7)/8 * i);

			switch(((width + 7)/8)) {
				case 1: line =  pchar[0]; break;
				case 2:	line =  (pchar[0]<< 8) | pchar[1]; break;
				case 3:
				default:
					line =  (pchar[0]<< 16) | (pchar[1]<< 8) | pchar[2];
					break;
			}

			for (j = 0; j < width; j++) {
				if(line & (1 << (width- j + offset- 1))) {
					LCD_DrawPixel((col + j), row, fontcolor);
				} else {
					// Nothing (transparent)
				}
			}
			row++;
		}
	} else {

		// Prepare to write GRAM
		LCD_WriteRAM_Prepare();

		// Write Data
		for(i = 0; i < height; i++) {
			pchar = ((uint8_t *)c + (width + 7)/8 * i);

			switch(((width + 7)/8)) {
				case 1: line =  pchar[0]; break;
				case 2:	line =  (pchar[0]<< 8) | pchar[1]; break;
				case 3:
				default:
					line =  (pchar[0]<< 16) | (pchar[1]<< 8) | pchar[2];
					break;
			}

			for (j = 0; j < width; j++) {
				if(line & (1 << (width- j + offset- 1))) {
					LCD_WriteRAM(fontcolor);
				} else {
					LCD_WriteRAM(backcolor);
				}
			}
			row++;
		}
	}
}


// Display a String with FontType
void LCD_DisplayString(uint16_t col, uint16_t row, char *s, sFONT *pFont, uint16_t fontcolor, uint16_t backcolor, uint16_t transparent) {
	uint16_t k=0;

	while (*s) {
		LCD_DisplayChar(col+k, row, *s, pFont, fontcolor, backcolor, transparent);
		k=k+pFont->Width;
		s++;
	}

}

// Write Character
void LCD_write_char(uint16_t col, uint16_t row, uint8_t car, uint16_t fontcolor, uint16_t backcolor, uint16_t transparent) {
	uint8_t avl;
	unsigned long n, i;

	LCD_SetWindow(col, row, 8, 16);
	LCD_SetCursor(col, row);

	// Imprimeix el caracter (tamany 16x8 pixels)
	if (transparent == 1) {

		for (i=0;i<16;i++) {
			// Agafem la info del caracter depenent del tipo de font
			avl=english_8x16_small[car-32][i];

			for (n=0;n<8;n++) {
				// Si bit 8= 1
				if(avl&0x80) {
					LCD_SetCursor(col+n, row+i);
					LCD_WriteReg(SSD2119_RAM_DATA_REG, fontcolor);
				}

				// Deplacament per fer canviar el bit 8 de avl
				avl<<=1;
			}
		}

	} else {

		// Prepare to write GRAM
		LCD_WriteRAM_Prepare();

		for (i=0;i<16;i++) {

			// Agadem la info del caracter depenent del tipo de font
			avl=english_8x16_small[car-32][i];

			for (n=0;n<8;n++) {

				// Si bit 8 = 1
				if(avl&0x80) LCD_WriteRAM(fontcolor);
				else LCD_WriteRAM(backcolor);

				// Deplacament per fer canviar el bit 8 de avl
				avl<<=1;
			}
		}

	}

}

// Write String
void LCD_write_string(uint16_t col, uint16_t row, uint8_t *s, uint16_t fontcolor, uint16_t backcolor, uint16_t transparent) {
	uint16_t k=0;

	while (*s) {
		LCD_write_char(col+k, row, *s, fontcolor, backcolor, transparent);
		k=k+8;
		s++;
	}

}



/*************************************************************************************************
*   F U N C I O N S    -    D I B U I X
*************************************************************************************************/
// Posa un pixel (col,row) a partir de (0,0) a un color.
void LCD_DrawPixel(uint16_t col, uint16_t row,uint16_t color) {
	LCD_SetCursor(col, row); // Set cursor position

	// Write Data to GRAM (R22h) only one value (un pixel)
	LCD_WriteReg(SSD2119_RAM_DATA_REG, color);

}


// Omple un quarat que comenca al pixel (col,row) a partir de (0,0),
// i de aple i llarg tants pixels com indica width i height
void LCD_DrawFilledRectangle(uint16_t col, uint16_t row, uint16_t width, uint16_t height, uint16_t color) {
	uint32_t i, numpixels = 0;

	numpixels = (uint16_t)width*(uint16_t)height;

	LCD_SetWindow(col, row, width, height);
	LCD_SetCursor(col, row);

	LCD_WriteRAM_Prepare(); // Prepare to write GRAM // Write Data to GRAM (R22h)

	for(i=0;i<numpixels;i++) {
		LCD_RAM = color;
	}

}


//Dibuixa un rectangle de gruix 1 pixel
void LCD_DrawRectangle(uint16_t col, uint16_t row, uint16_t width, uint16_t height, uint8_t linewidth, uint16_t color) {

	LCD_DrawFilledRectangle(col, row-linewidth, width, 1+linewidth*2, color);
	LCD_DrawFilledRectangle(col-linewidth, row, 1+linewidth*2, height, color);

	LCD_DrawFilledRectangle(col, (row+height-1)-linewidth, width,  1+linewidth*2, color);
	LCD_DrawFilledRectangle((col+width-1)-linewidth, row, 1+linewidth*2, height, color);

}


//LCD_DrawCircle
//Col y Row son el centre de la circunferencia
void LCD_DrawCircle(uint16_t col, uint16_t row, uint16_t radius, uint16_t color) {
	int32_t  D;/* Decision Variable */
	uint32_t  CurX;/* Current X Value */
	uint32_t  CurY;/* Current Y Value */

	// Posicio i finestra de treball
	LCD_SetWindow(col-radius, row-radius, (radius*2)+1, (radius*2)+1);
	LCD_SetCursor(col, row);

	D = 3 - (radius << 1);
	CurX = 0;
	CurY = radius;

	while (CurX <= CurY) {
		LCD_SetCursor(col + CurX, row + CurY);
		LCD_WriteRAM_Prepare(); /* Prepare to write GRAM */
		LCD_WriteRAM(color);
		LCD_SetCursor(col + CurX, row - CurY);
		LCD_WriteRAM_Prepare(); /* Prepare to write GRAM */
		LCD_WriteRAM(color);
		LCD_SetCursor(col - CurX, row + CurY);
		LCD_WriteRAM_Prepare(); /* Prepare to write GRAM */
		LCD_WriteRAM(color);
		LCD_SetCursor(col - CurX, row - CurY);
		LCD_WriteRAM_Prepare(); /* Prepare to write GRAM */
		LCD_WriteRAM(color);
		LCD_SetCursor(col + CurY, row + CurX);
		LCD_WriteRAM_Prepare(); /* Prepare to write GRAM */
		LCD_WriteRAM(color);
		LCD_SetCursor(col + CurY, row - CurX);
		LCD_WriteRAM_Prepare(); /* Prepare to write GRAM */
		LCD_WriteRAM(color);
		LCD_SetCursor(col - CurY, row + CurX);
		LCD_WriteRAM_Prepare(); /* Prepare to write GRAM */
		LCD_WriteRAM(color);
		LCD_SetCursor(col - CurY, row - CurX);
		LCD_WriteRAM_Prepare(); /* Prepare to write GRAM */
		LCD_WriteRAM(color);

		if (D < 0) {
			D += (CurX << 2) + 6;
		} else {
			D += ((CurX - CurY) << 2) + 10;
			CurY--;
		}
		CurX++;
	}
}


//LCD_DrawFilledCircle
//Col y Row son el centre de la circunferencia
void LCD_DrawFilledCircle(uint16_t col, uint16_t row, uint16_t radius, uint16_t color) {
	int32_t  D;    /* Decision Variable */
	uint32_t  CurX;/* Current X Value */
	uint32_t  CurY;/* Current Y Value */

	// Posicio i finestra de treball
	LCD_SetWindow(col-radius, row-radius, (radius*2)+1, (radius*2)+1);
	LCD_SetCursor(col, row);

	D = 3 - (radius << 1);

	CurX = 0;
	CurY = radius;

	while (CurX <= CurY) {
		if(CurY > 0) {
			LCD_DrawLine(col - CurX, row - CurY, col - CurX, row + CurY, 0, color);
			LCD_DrawLine(col + CurX, row - CurY, col + CurX, row + CurY, 0, color);
	    }
	    if(CurX > 0) {
	    	LCD_DrawLine(col - CurY, row - CurX, col - CurY, row + CurX, 0, color);
	    	LCD_DrawLine(col + CurY, row - CurX, col + CurY, row + CurX, 0, color);
	    }
	    if (D < 0) {
	      D += (CurX << 2) + 6;
	    }
	    else {
	      D += ((CurX - CurY) << 2) + 10;
	      CurY--;
	    }
	    CurX++;
	}
}


// LCD_DrawLine
void LCD_DrawLine(uint16_t col, uint16_t row, uint16_t endcol, uint16_t endrow, uint16_t linewidth, uint16_t color) {

	// Optimize horizontal or vertical line drawing
	if ( col == endcol ) {
		LCD_DrawFilledRectangle( col-linewidth, row, 1+linewidth*2, endrow-row+1, color);

	} else if (row == endrow) {
			LCD_DrawFilledRectangle( col, row-linewidth, endcol-col+1, 1+linewidth*2, color);

	} else {
		LCD_DrawLineBresenham(col, row, endcol, endrow, color);
	}
}

//LCD_DrawLineBresenham
void LCD_DrawLineBresenham(uint16_t col, uint16_t row, uint16_t endcol, uint16_t endrow, uint16_t color) {
	int32_t dx, dy;
	int32_t i;
	int32_t xinc, yinc, cumul;
	int32_t x, y;

	// Posicio i finestra de treball
	LCD_SetWindow(0,0,LCD_NumCols(),LCD_NumRows());
	LCD_SetCursor(col, row);

	x = col;
	y = row;
	dx = endcol - col;
	dy = endrow - row;


	xinc = (dx > 0)? 1 : -1;
	yinc = (dy > 0)? 1 : -1;
	dx = (dx > 0)? dx : -dx;
	dy = (dy > 0)? dy : -dy;

	LCD_DrawPixel( (uint16_t)x, (uint16_t)y, color) ;

	if (dx > dy) {
		cumul = dx/2;
		for (i=1;i<=dx;i++) {
			x += xinc;
			cumul += dy;

			if ( cumul >= dx ) {
				cumul -= dx;
				y += yinc;
			}
			LCD_DrawPixel( (uint16_t)x, (uint16_t)y, color ) ;
		}

	} else {
		cumul = dy/2;
		for (i=1;i<=dy;i++) {
			y += yinc ;
			cumul += dx ;

			if (cumul >= dy) {
				cumul -= dy ;
				x += xinc ;
			}

			LCD_DrawPixel( (uint16_t)x, (uint16_t)y, color ) ;
		}
	}

}


/*************************************************************************************************
*   F U N C I O N S    -    V E U R E    I M A T G E S
*************************************************************************************************/
// Grabar un buffer de dades a memoria
void LCD_WriteRAMBuffer(const uint16_t *pBuf, uint32_t size) {
	uint32_t addr;
	for (addr=0;addr<size;addr++) {
		LCD_WriteRAM(pBuf[addr]);
	}
}

// Dibuixar una imatge monocolor (pixels a 0 o 1)
// Tamany:  320*240/32bits = 2400 valors de uint32_t
void LCD_DrawMonoPict(const uint32_t *Pict) {
	uint32_t index = 0, i = 0;

	LCD_WriteRAM_Prepare(); /* Prepare to write GRAM */
	for(index = 0; index < 2400; index++) {
		for(i = 0; i < 32; i++) {
			if((Pict[index] & (1 << i)) == 0x00) {
				LCD_WriteRAM(COLOR_BLACK);
			} else {
				LCD_WriteRAM(COLOR_WHITE);
			}
		}
	}
}

// Displays a bitmap picture loaded in the internal Flash.
// FALTA PROVAR....
void LCD_DrawBMP(uint32_t BmpAddress) {
	uint32_t index = 0, size = 0;
	/* Read bitmap size */
	size = *(__IO uint16_t *) (BmpAddress + 2);
	size |= (*(__IO uint16_t *) (BmpAddress + 4)) << 16;
	/* Get bitmap data address offset */
	index = *(__IO uint16_t *) (BmpAddress + 10);
	index |= (*(__IO uint16_t *) (BmpAddress + 12)) << 16;
	size = (size - index)/2;
	BmpAddress += index;
	/* Set GRAM write direction and BGR = 1 */
	/* I/D=00 (Horizontal : decrement, Vertical : decrement) */
	/* AM=1 (address is updated in vertical writing direction) */
	//LCD_WriteReg(LCD_REG_3, 0x1008);

	LCD_WriteRAM_Prepare();
	for(index = 0; index < size; index++) {
		LCD_WriteRAM(*(__IO uint16_t *)BmpAddress);
		BmpAddress += 2;
	}

	/* Set GRAM write direction and BGR = 1 */
	/* I/D = 01 (Horizontal : increment, Vertical : decrement) */
	/* AM = 1 (address is updated in vertical writing direction) */
	//LCD_WriteReg(LCD_REG_3, 0x1018);
}


