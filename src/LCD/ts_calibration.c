/*
 * ts_calibration.c
 *
 *  Created on: Maig 1, 2014
 *      Author: Marcel
 */

#include "ts_calibration.h"
#include <string.h>

// Global variables for the conversion from ADC to LCD position
float	TS_xFact = 1; 	// pixels/ADC relation in x axis
float	TS_yFact = 1; 	// pixels/ADC relation in y axis
int16_t TS_xOffset = 0; // offset from 0 x position
int16_t TS_yOffset = 0; // offset from 0 y position

// External function prototypes (defined in tiny_printf.c)
extern int printf(const char *fmt, ...);
extern int sprintf(char* str, const char *fmt, ...);

// Do the calibration for obtain xFactor, xOffset, yFactor, yOffset
void TS_doCalibraion(void) {
	char text[250];
	TS_State tsdata;
	uint16_t k;
	Point_Struct ADCvalues[5], pointBase[5], p;
	float valor1, valor2;
	uint8_t isCalibrated = 0;

	// Calculate the base points
	pointBase[0].x = CURSOR_OFF;
	pointBase[0].y = CURSOR_OFF;
	pointBase[1].x = (LCD_NumCols()-1)-CURSOR_OFF;
	pointBase[1].y = CURSOR_OFF;
	pointBase[2].x = (LCD_NumCols()-1)-CURSOR_OFF;
	pointBase[2].y = (LCD_NumRows()-1)-CURSOR_OFF;
	pointBase[3].x = CURSOR_OFF;
	pointBase[3].y = (LCD_NumRows()-1)-CURSOR_OFF;
	pointBase[4].x = (LCD_NumCols())/2;
	pointBase[4].y = (LCD_NumRows())/2;

	// If the calibration fail repeat it
	while(!isCalibrated) {

		// Obtain the values from 4 points
		// in the extrems of the LCD
		LCD_Clear(COLOR_BLACK);
		for (k = 0; k<4; k++) {
			// Draw a Cross
			LCD_DrawLine(pointBase[k].x-CURSOR_LEN, pointBase[k].y, pointBase[k].x+CURSOR_LEN, pointBase[k].y, 0, COLOR_GREEN);
			LCD_DrawLine(pointBase[k].x, pointBase[k].y-CURSOR_LEN, pointBase[k].x, pointBase[k].y+CURSOR_LEN, 0, COLOR_GREEN);

			// Wait a touch screen pulsation
			do {
				 TS_GetState(&tsdata);
				 Delay_ms(25);
			} while(!tsdata.touchDetected);
			if (LCD_GetOrientation() == LCD_LANDSCAPE || LCD_GetOrientation() == LCD_LANDSCAPE_FLIP) {
				ADCvalues[k].x = tsdata.x;
				ADCvalues[k].y = tsdata.y;
			} else {
				ADCvalues[k].x = tsdata.y;
				ADCvalues[k].y = tsdata.x;
			}


			// Show data:
			sprintf(text, "P%d: x=%4d, y=%4d", k, tsdata.x, tsdata.y);
			LCD_DisplayString(6, CURSOR_OFF*2+16*k,text, &Font12, COLOR_BLUE, COLOR_BLACK, 0);

			// Draw a Cross
			LCD_DrawLine(pointBase[k].x-CURSOR_LEN, pointBase[k].y, pointBase[k].x+CURSOR_LEN, pointBase[k].y, 0, COLOR_BLUE);
			LCD_DrawLine(pointBase[k].x, pointBase[k].y-CURSOR_LEN, pointBase[k].x, pointBase[k].y+CURSOR_LEN, 0, COLOR_BLUE);

			Delay_ms(200);
		}

		//////////////////////////////////////////////////////////////
		// Calculate xFactor
		valor1 = (float)(((float)pointBase[1].x - (float)pointBase[0].x) + ((float)pointBase[2].x - (float)pointBase[3].x)) / 2.0;
		valor2 = (float)(((float)ADCvalues[1].x - (float)ADCvalues[0].x) + ((float)ADCvalues[2].x - (float)ADCvalues[3].x)) / 2.0;
		TS_xFact = valor1 / valor2;

		// Calculate yFactor
		valor1 = (float)(((float)pointBase[3].y - (float)pointBase[0].y) + ((float)pointBase[2].y - (float)pointBase[1].y)) / 2.0;
		valor2 = (float)(((float)ADCvalues[3].y - (float)ADCvalues[0].y) + ((float)ADCvalues[2].y - (float)ADCvalues[1].y)) / 2.0;
		TS_yFact = valor1 / valor2;

		// Calculate the xOffset
		valor1 = (((float)ADCvalues[0].x * TS_xFact - (float)pointBase[0].x) +
				  ((float)ADCvalues[1].x * TS_xFact - (float)pointBase[1].x)) / 2.0;
		TS_xOffset = (int16_t)valor1;

		// Calculate the yOffset
		valor1 = (((float)ADCvalues[0].y * TS_yFact - (float)pointBase[0].y)
				+ ((float)ADCvalues[1].y * TS_yFact - (float)pointBase[1].y)) / 2.0;
		TS_yOffset = (int16_t)valor1;
		//////////////////////////////////////////////////////////////



		// Check the parameters calculated
		// with central point
		LCD_DrawCircle(pointBase[4].x, pointBase[4].y, CURSOR_LEN*2, COLOR_GREEN);
		LCD_DrawLine(pointBase[4].x-CURSOR_LEN, pointBase[4].y, pointBase[4].x+CURSOR_LEN, pointBase[k].y, 0, COLOR_GREEN);
		LCD_DrawLine(pointBase[4].x, pointBase[4].y-CURSOR_LEN, pointBase[4].x, pointBase[4].y+CURSOR_LEN, 0, COLOR_GREEN);

		// Wait a touch screen pulsation
		// and get the center point ADC values
		do {
			 TS_GetState(&tsdata);
			 Delay_ms(25);
		} while(!tsdata.touchDetected);
		if (LCD_GetOrientation() == LCD_LANDSCAPE || LCD_GetOrientation() == LCD_LANDSCAPE_FLIP) {
			ADCvalues[4].x = tsdata.x;
			ADCvalues[4].y = tsdata.y;
		} else {
			ADCvalues[4].x = tsdata.y;
			ADCvalues[4].y = tsdata.x;
		}

		// Get the position in coordenates
		TS_CalculatePos(ADCvalues[4].x, ADCvalues[4].y, &p.x, &p.y);

		// Check if center point is inside a valid range
		if ((p.x > (pointBase[4].x + CAL_RANGE) || p.x < (pointBase[4].x - CAL_RANGE)) ||
			(p.y > (pointBase[4].y + CAL_RANGE) || p.y < (pointBase[4].y - CAL_RANGE))) {

			LCD_DisplayString(6, CURSOR_OFF*2+16*5,"CALIBRATION FAIL!!", &Font12, COLOR_RED, COLOR_BLACK, 0);

		} else {
			isCalibrated = 1;

			// Show the point
			LCD_DrawFilledCircle(p.x, p.y, 2, COLOR_CYAN);
			Delay_ms(2000);


			LCD_DisplayString(6, CURSOR_OFF*2+16*5,"CALIBRATION DONE!!", &Font12, COLOR_GREEN, COLOR_BLACK, 0);


			// Show the calibration results
			sprintf(text, "xFact:%+2.4f, xOffset:%+4d", TS_xFact, TS_xOffset);
			LCD_DisplayString(6, CURSOR_OFF*2+16*6, text, &Font12, COLOR_YELLOW, COLOR_BLACK, 0);
			sprintf(text, "yFact:%+2.4f, yOffset:%+4d", TS_yFact, TS_yOffset);
			LCD_DisplayString(6, CURSOR_OFF*2+16*7, text, &Font12, COLOR_YELLOW, COLOR_BLACK, 0);

		}

		// Wait a touch for continue
		do {
			 TS_GetState(&tsdata);
			 Delay_ms(25);
		} while(!tsdata.touchDetected);

	}

}


// Executes a test for check the calibration
void TS_Test(void) {
	char text[250];
	TS_State tsdata;
	Point_Struct p;
	uint16_t tmp;

	LCD_Clear(COLOR_BLACK);
	LCD_DisplayString(0, 16*0,"TESTING THE CALIBRATION:", &Font12, COLOR_GREEN, COLOR_BLACK, 0);


	while(1) {

		// Wait a touch screen pulsation
		do {
			 TS_GetState(&tsdata);
			 Delay_ms(25);
		} while(!tsdata.touchDetected);

		// Check the orientation of the LCD and correct the axes
		if (LCD_GetOrientation() != LCD_LANDSCAPE && LCD_GetOrientation() != LCD_LANDSCAPE_FLIP) {
			tmp = tsdata.x;
			tsdata.x = tsdata.y;
			tsdata.y = tmp;
		}

		// Convert the position in coordenates
		TS_CalculatePos(tsdata.x, tsdata.y, &p.x, &p.y);

		// Show the point
		LCD_DrawPixel(p.x, p.y, COLOR_CYAN);

		sprintf(text, "x:%3d, y:%3d", p.x, p.y);
		LCD_DisplayString(0, 16*1, text, &Font12, COLOR_BLUE, COLOR_BLACK, 0);
		sprintf(text, "ADCx:%4d, ADCy:%4d", tsdata.x, tsdata.y);
		LCD_DisplayString(0, 16*2, text, &Font12, COLOR_BLUE, COLOR_BLACK, 0);

	}



}


// Convert from ADC to LCD position with current calibration parameters
void TS_CalculatePos(uint16_t x, uint16_t y, uint16_t *col, uint16_t *row) {
	*col = ((int)(x * TS_xFact - TS_xOffset));
	*row = ((int)(y * TS_yFact - TS_yOffset));

	// Check the offsets
	if (*col > LCD_NumCols()-1) *col = LCD_NumCols()-1;
	if (*row > LCD_NumRows()-1) *row = LCD_NumRows()-1;

}


// Set the calibration parametres
void TS_SetCalParam(float xFact, int16_t xOff, float yFact, int16_t yOff) {
	TS_xFact 	= xFact;
	TS_xOffset 	= xOff;
	TS_yFact 	= yFact;
	TS_yOffset	= yOff;
}


// Get the current calibration parameters
void TS_GetCalParam(float *xFact, int16_t *xOff, float *yFact, int16_t *yOff) {
	*xFact = TS_xFact;
	*xOff = TS_xOffset;
	*yFact = TS_yFact;
	*yOff = TS_yOffset;
}


// Wait a touch pulse
void TS_WaitTouchDetection(uint16_t *x, uint16_t *y, uint16_t *z) {
	uint16_t tmp;
	TS_State tsdata;
	Point_Struct p;

	// Wait for touch pulse detection
	do {
		TS_GetState(&tsdata);
		 Delay_ms(25);
	} while(!tsdata.touchDetected);

	// Check the orientation of the LCD and correct the axes
	if (LCD_GetOrientation() != LCD_LANDSCAPE && LCD_GetOrientation() != LCD_LANDSCAPE_FLIP) {
		tmp = tsdata.x;
		tsdata.x = tsdata.y;
		tsdata.y = tmp;
	}

	// Get the position in coordenates
	TS_CalculatePos(tsdata.x, tsdata.y, &p.x, &p.y);

	// Uptades the values
	*x = p.x;
	*y = p.y;
	*z = tsdata.z;

}

void TS_WaitReleaseTouchDetection(uint16_t *x, uint16_t *y, uint16_t *z) {
	TS_State tsdata;

	// Wait a touch pulse
	TS_WaitTouchDetection(x, y, z);

	// Wait the release touch pulse detection
	do {
		TS_GetState(&tsdata);
		 Delay_ms(25);
	} while(tsdata.touchDetected == 1);

}

