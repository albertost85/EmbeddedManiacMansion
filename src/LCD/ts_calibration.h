/*
 * ts_calibration.h
 *
 *  Created on: Maig 1, 2014
 *      Author: Marcel
 *
 * Description: Touch Screen (TS) calibration functions for convert ADC values to LCD positions
 *
 *
 */
#ifndef __TS_CALIBRATION_H
#define __TS_CALIBRATION_H

#ifdef __cplusplus
 extern "C" {
#endif

// Includes
#include "stm32f4xx.h"
#include ".\..\Systick\delay.h"
#include ".\LCD\lcd_ssd2119.h"
#include ".\LCD\ts_stmpe811.h"

// Structure of a point
typedef struct{
	uint16_t x;
	uint16_t y;
}Point_Struct;


// Defines for the calibration
#define	CURSOR_LEN		10
#define	CURSOR_OFF		30
#define	CAL_RANGE		10


/* F  U  N  C  T  I  O  N  S                          */
/******************************************************/

// Do the calibration for obtain xFactor, xOffset, yFactor, yOffset
void TS_doCalibraion(void);

// Executes a test for check the calibration
void TS_Test(void);

// Convert from ADC to LCD position with current calibration parameters
void TS_CalculatePos(uint16_t x, uint16_t y, uint16_t *col, uint16_t *row);

// Set the calibration parametres
void TS_SetCalParam(float xFact, int16_t xOff, float yFact, int16_t yOff);

// Get the current calibration parameters
void TS_GetCalParam(float *xFact, int16_t *xOff, float *yFact, int16_t *yOff);

// Wait for a touch screen pulsation and return the screen coordenades
void TS_WaitTouchDetection(uint16_t *x, uint16_t *y, uint16_t *z);

// Wait for a touch an Release of screen pulsation and return the screen coordenades
void TS_WaitReleaseTouchDetection(uint16_t *x, uint16_t *y, uint16_t *z);


#ifdef __cplusplus
}
#endif

#endif /* ____TS_CALIBRATION_H */

 
