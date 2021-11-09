/*
 * Rooms.h
 *
 *  Created on: 25 may. 2018
 *      Author: alber
 */
#include "stm32f4xx.h"
#ifndef GAME_IMAGES_ROOMS_H_
#define GAME_IMAGES_ROOMS_H_

typedef struct {
	uint16_t width;
	uint16_t height;
	uint16_t col;
	uint16_t row;
	int8_t deep; // 0 for background
	uint8_t visibility;
	uint8_t typeObject;
	uint16_t param;
	uint8_t defaultAction;
	char name[12];
	const uint8_t *pData;
} GAME_OBJECT;

typedef struct {
	uint16_t width;
	uint16_t height;
	uint16_t BytesPerLine;
	uint16_t BitsPerPixel;
	const uint8_t *pData;
	uint16_t param;
	uint8_t visibility;
	uint8_t defaultAction;
	char name[12];
} GAME_INVENTORY; // Mantiene propiedades del tipo flah image, para que sea posible hacer una conversión de tipos

typedef struct {
	uint16_t width;
	uint16_t height;
	uint16_t BytesPerLine;
	uint16_t BitsPerPixel;
	uint16_t limitsWalk[2][8];
	const uint8_t *pData;
} GAME_ROOM;

typedef enum {
	CLOSED		  		= 0,
	OPEN				= 1,
	LOCKED				= 2,
} Door_visibility;

typedef enum {
	HIDDEN		  		= 0,
	VISIBLE				= 1,
} Obj_visibility;

typedef enum {
	DEFAULT			=0,
	DOOR			=1,
	THRESHOLD		=3,
	KEY				=4,
	DECORATION		=5,
} Obj_special;

typedef enum {
	OPT_NONE		=0,
	OPT_GIVE		=1,
	OPT_OPEN		=2,
	OPT_CLOSE		=3,
	OPT_PICKUP		=4,
	OPT_LOOKAT		=5,
	OPT_TALKTO		=6,
	OPT_USE			=7,
	OPT_PUSH		=8,
	OPT_PULL		=9,
	OPT_WALK		=10,
	OPT_WALKINTO	=11,
	OPT_USEIN		=12,
} GAME_Options;

// Extern references of the flash images
extern const GAME_ROOM rmExterior;
extern const GAME_ROOM rmFloor3st;
extern const GAME_ROOM rmRoom31;
extern const GAME_ROOM rmRoom32;
extern const GAME_ROOM rmRoom33;

extern GAME_OBJECT obExterior[6];
extern GAME_OBJECT obFloor3st[6];
extern GAME_OBJECT obRoom31[6];
extern GAME_OBJECT obRoom32[6];
extern GAME_OBJECT obRoom33[6];

extern GAME_INVENTORY inDCSource;
extern GAME_INVENTORY inHamster;
extern GAME_INVENTORY inCoolKey;
extern GAME_INVENTORY inPackage;
extern GAME_INVENTORY inSTMicro;


#endif /* GAMES_IMAGES_ROOMS_H_ */
