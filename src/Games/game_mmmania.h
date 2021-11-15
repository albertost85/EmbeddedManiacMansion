/*
 * game_arkanoid.h
 *
 *  Created on: May , 2017
 *      Author: Alberto Simon
 */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef GAME_ARKANOID_H_
#define GAME_ARKANOID_H_


#ifdef __cplusplus
	extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx.h"
#include ".\..\LCD\image_flash.h"
#include ".\..\LCD\image_bmp.h"
#include ".\..\LCD\ts_calibration.h"
#include ".\..\LCD\ts_stmpe811.h"
#include ".\..\Peripherals\pushbutton.h"
#include ".\..\Peripherals\joystick.h"
#include ".\..\Tone\tone.h"
#include <Games/mmmResources/ActiveScreen.h>
#include <Games/mmmResources/Rooms.h>


extern FLASH_BITMAP bmActiveScreen;
extern const FLASH_BITMAP bmWeirdEd[4][6];



// External function prototypes (defined in tiny_printf.c)
extern int printf(const char *fmt, ...);
extern int sprintf(char* str, const char *fmt, ...);





/* Functions ------------------------------------------------------------------*/

void MMMania_Run();
void MMMania_Callback(uint32_t tLocalTime);
void Erase_Cursor(void);
void Draw_Cursor(uint16_t x, uint16_t y);

GAME_OBJECT *Check_Object(uint16_t x,uint16_t y);

void Cursor_Update(void);
void Buttons_Update(void);
void Screen_Update(uint8_t options);
void Action_Update(void);
void Character_Update(void);
void Start_Movement(void);
void Update_GUI(void);
void Update_Highlighted(void);
void Change_Room(void);
void Talk_Update(void);
void Use(void);
void Use_In(void);
void PickUp(void);
void LookAt(void);
void checkLimits(int16_t xIn,int16_t yIn,int16_t *xOut,int16_t *yOut);
uint16_t yRamp( int16_t x, int16_t x_0, int16_t y_0, int16_t x_1, int16_t y_1);

// Inventory management functions
void Inventory_Start(void);
uint8_t Inventory_Add(GAME_INVENTORY objectInventory);
void Inventory_Remove(GAME_INVENTORY *objectInventory);

void Screen_Select(void);
void Screen_Intro(void);
void Screen_Exit(void);
void Screen_Final(void);
#ifdef __cplusplus
}
#endif

#endif /* GAME_ARKANOID_H_ */
