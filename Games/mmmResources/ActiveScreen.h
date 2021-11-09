/*
 * ActiveScreen.h
 *
 *  Created on: 24 may. 2018
 *      Author: alber
 */

#ifndef GAMES_MMMRESOURCES_ACTIVESCREEN_H_
#define GAMES_MMMRESOURCES_ACTIVESCREEN_H_

#include <Games/mmmResources/Rooms.h> // Para el tipo GAME_ROOM
#include ".\..\..\LCD\image_flash.h" // PARA EL TIPO FLASH_BITMAP

uint8_t Screen_Load_Image(uint16_t col, uint16_t row, const FLASH_BITMAP *image);
uint8_t Screen_Load_Image2(const GAME_ROOM *image,uint16_t xInit);
uint8_t Screen_Load_Object(GAME_OBJECT *object,uint16_t xInit, int8_t deep);

#endif /* GAMES_MMMRESOURCES_ACTIVESCREEN_H_ */
