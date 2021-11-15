/*
 * tone.h
 *
 *  Created on: Nov 11, 2016
 *         Mod: May 25, 2017
 *      Author: Marcel
 *
 *      Information:
 *      https://en.wikipedia.org/wiki/Ring_Tone_Transfer_Language
 *      https://codebender.cc/example/Tone/RTTTL#RTTTL.ino
 *      http://www.vaughns-1-pagers.com/music/musical-note-frequencies.htm
 *      http://www.convertyourtone.com/ringtones.html
 *
 *      MP/WAV to MIDI:
 *      http://www.ofoct.com/audio-converter/convert-wav-or-mp3-ogg-aac-wma-to-midi.html
 *
 *      MIDI to RTTL:
 *      http://midi.mathewvp.com/midi2RTTL.php
 *
 */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __TONE_H
#define __TONE_H

#ifdef __cplusplus
 extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx.h"
#include ".\..\SysTick\delay.h"

// Notes Frequency definition (in Hz)
#define NOTE_B0  31
#define NOTE_C1  33
#define NOTE_CS1 35
#define NOTE_D1  37
#define NOTE_DS1 39
#define NOTE_E1  41
#define NOTE_F1  44
#define NOTE_FS1 46
#define NOTE_G1  49
#define NOTE_GS1 52
#define NOTE_A1  55
#define NOTE_AS1 58
#define NOTE_B1  62
#define NOTE_C2  65
#define NOTE_CS2 69
#define NOTE_D2  73
#define NOTE_DS2 78
#define NOTE_E2  82
#define NOTE_F2  87
#define NOTE_FS2 93
#define NOTE_G2  98
#define NOTE_GS2 104
#define NOTE_A2  110
#define NOTE_AS2 117
#define NOTE_B2  123
#define NOTE_C3  131
#define NOTE_CS3 139
#define NOTE_D3  147
#define NOTE_DS3 156
#define NOTE_E3  165
#define NOTE_F3  175
#define NOTE_FS3 185
#define NOTE_G3  196
#define NOTE_GS3 208
#define NOTE_A3  220
#define NOTE_AS3 233
#define NOTE_B3  247
#define NOTE_C4  262
#define NOTE_CS4 277
#define NOTE_D4  294
#define NOTE_DS4 311
#define NOTE_E4  330
#define NOTE_F4  349
#define NOTE_FS4 370
#define NOTE_G4  392
#define NOTE_GS4 415
#define NOTE_A4  440
#define NOTE_AS4 466
#define NOTE_B4  494
#define NOTE_C5  523
#define NOTE_CS5 554
#define NOTE_D5  587
#define NOTE_DS5 622
#define NOTE_E5  659
#define NOTE_F5  698
#define NOTE_FS5 740
#define NOTE_G5  784
#define NOTE_GS5 831
#define NOTE_A5  880
#define NOTE_AS5 932
#define NOTE_B5  988
#define NOTE_C6  1047
#define NOTE_CS6 1109
#define NOTE_D6  1175
#define NOTE_DS6 1245
#define NOTE_E6  1319
#define NOTE_F6  1397
#define NOTE_FS6 1480
#define NOTE_G6  1568
#define NOTE_GS6 1661
#define NOTE_A6  1760
#define NOTE_AS6 1865
#define NOTE_B6  1976
#define NOTE_C7  2093
#define NOTE_CS7 2217
#define NOTE_D7  2349
#define NOTE_DS7 2489
#define NOTE_E7  2637
#define NOTE_F7  2794
#define NOTE_FS7 2960
#define NOTE_G7  3136
#define NOTE_GS7 3322
#define NOTE_A7  3520
#define NOTE_AS7 3729
#define NOTE_B7  3951
#define NOTE_C8  4186
#define NOTE_CS8 4435
#define NOTE_D8  4699
#define NOTE_DS8 4978

#define isdigit(n) (n >= '0' && n <= '9')

#define OCTAVE_OFFSET 0

// Tone playing modes
typedef enum {
	TONE_MODE_BLOCK = 0,
 	TONE_MODE_IRQ = 1
} ToneMode_TypeDef;


// Melody Identification
typedef enum {
	Tone_TheSimpsons = 0,
	Tone_Indiana,
	Tone_TakeOnMe,
	Tone_Entertainer,
	Tone_Muppets,
	Tone_Xfiles,
	Tone_Looney,
	Tone_20thCenFox,
	Tone_OneBond,
	Tone_MASH,
	Tone_StarWars,
	Tone_GoodBad,
	Tone_TopGun,
	Tone_ATeam,
	Tone_Flinstones,
	Tone_Jeopardy,
	Tone_Gadget,
	Tone_Smurfs,
	Tone_MahnaMahna,
	Tone_LeisureSuit,
	Tone_MissionImp,
	Tone_AbbaMammaMia,
	Tone_SuperMarioBross,
	Tone_SuperMarioBross_Under,
	Tone_Pacman,
	Tone_Tetris,
	Tone_Arkanoid,
	Tone_Hawaii50,
	Tone_Time,
	Tone_VanHalenEruption,
	Tone_TubularBells,
	Tone_Olympics,
	Tone_Wolves,
	Tone_Blue,
	Tone_Be,
	Tone_Urgent,
	Tone_Fido,
	Tone_Intel,
	Tone_Mosaic,
	Tone_Triple,
	Tone_SmartAlert,
	Tone_musicSM3,
	Tone_musicStart,
	Tone_musicTetrisTheme,
	Tone_musicTetrisC,
	Tone_musicTetrisB,
	Tone_musicSoft,
	Tone_musicArkanoid,
	Tone_musicArkanoid2,
	Tone_musicScore,
	Tone_gameShowItem,
	Tone_gameShoot,
	Tone_gameFisnish,
	Tone_gameComplete,
	Tone_gameExplosion,
	Tone_gameSMBGameOver,
	Tone_gameSMBDeath,
	Tone_gameMMMstart,
	Tone_gameMMMend,
	Tone_gameMMMopendoor,
	Tone_gameMMMclosedoor,
	Tone_gameMMMfunny,
} ToneMelody_TypeDef;

// Number of melodies
#define nMelodies	62

extern const char *Meloy_Tones[nMelodies];


// GPIO Definition
/*
// Piezo PWM at PB3 (TM2-CH2)
#define TONE_PIN        	GPIO_Pin_3
#define TONE_GPIO_PORT		GPIOB
#define TONE_GPIO_CLK       RCC_AHB1Periph_GPIOB
#define TONE_GPIO_AF		GPIO_AF_TIM2
#define TONE_PIN_SOURCE		EXTI_PinSource3
#define TONE_IRQn       	TIM2_IRQn
#define TONE_IRQHandler 	TIM2_IRQHandler
#define TONE_TIM			TIM2
#define TONE_TIM_CLK		RCC_APB1Periph_TIM2
#define TONE_TIM_CCR		CCR2
#define TONE_TIM_CH_INIT	TIM_OC2Init
*/
// Piezo PWM at PA10 (TM1-CH3)
#define TONE_PIN        	GPIO_Pin_10
#define TONE_GPIO_PORT		GPIOA
#define TONE_GPIO_CLK       RCC_AHB1Periph_GPIOA
#define TONE_GPIO_AF		GPIO_AF_TIM1
#define TONE_PIN_SOURCE		EXTI_PinSource10
#define TONE_IRQn       	TIM1_UP_TIM10_IRQn
#define TONE_IRQHandler 	TIM1_UP_TIM10_IRQHandler
#define TONE_TIM			TIM1
#define TONE_TIM_CLK		RCC_APB2Periph_TIM1
#define TONE_TIM_CCR		CCR3
#define TONE_TIM_CH_INIT	TIM_OC3Init


 // Functions
///////////////////////

// Configures Piezo PWM
void Tone_Init(ToneMode_TypeDef mode);

// Change the duty, volume from 0 to 100
void Tone_SetVolume(uint32_t volume);

// Play a Note
void Tone_PlayNote(uint32_t freq, uint32_t duration);

// Do Beep & DoubleBeep
void Tone_doBeep(void);
void Tone_doDblBeep(void);

// Returns if is playing
uint8_t Tone_isPlaying(void);

// Repeats the song when it is finished
void Tone_RepeatSong(uint8_t state);

// Return current melody name
void Tone_GetCurrentSongName(char *songName);

// Stops the current song
void Tone_StopPlaying(void);

// Plays RTTL melody
void Tone_PlayRTTTL(const char *song);

// Test notes for check correct frequency
void Tone_TestNotes(void);

#ifdef __cplusplus
}
#endif

#endif /* __TONE_H */

