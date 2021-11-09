/*
 * game_arkanoid.c
 *	Funciones principales para controlar las acciones: update_highlighted, y update_buttons
 *	update_gui sólo afecta a la interfaz gráfica (mensajes, inventario, texto...)
 *
 *	funciones checkLimits, yramp son funciones auxiliares para calcular posiciones geométricas
 *
 *	Hay dos sistemas de coordenadas, uno relativo a la pantalla LCD, y otro llamado
 *	'absoluto' relativo a las dimensiones del escenario. La posición del muñeco es clave para decidir
 *	la relacion entre ambos.
 *
 *	Algunas opciones del juego tienen función propia, y otras no. Al final, todas se tratan partiendo de la función
 *	Action_Update
 *
 *	El tratamiento del flag para acabar el juego se da cuando funciona la máquina,
 *	y está contenido dentro de la función Talk_Update
 *
 *  Created on: May , 2018
 *  Author: Alberto Simón
 */

/* Includes ------------------------------------------------------------------*/
#include "game_mmmania.h"




/* Variables -----------------------------------------------------------------*/

GAME_ROOM actual_Room;
GAME_OBJECT room_Objects[6];
GAME_OBJECT emptyObject={0,0,0,0,0,0,0,0x00,OPT_NONE,{'\0'},0,};
GAME_OBJECT *highlightedObject;
GAME_OBJECT *clickedObject;

const uint8_t inventoryLength=4;
uint8_t firstInventory=0;
GAME_INVENTORY actualInventory[4];
GAME_INVENTORY emptyInventory={0,0,0,0,0,0,HIDDEN,OPT_NONE,{'\0'},};
GAME_INVENTORY *highlightedInventory;
GAME_INVENTORY *clickedInventory;


uint8_t stopIntro=0;
uint8_t	gameGoingOn; // Set to 0 when the game has to stop
uint8_t exitScreen;

uint8_t highlightedOption;
uint8_t selectedOption;
uint8_t pendingOption;


// Cursor position
const uint8_t CURSORSPEED = 50, CHARSPEED = 10, SCREENREFRESHTIME=80; // Update cursor every n ticks
int16_t CursorPos[2];
int16_t CursorOldPos[2];
uint16_t absCursorPos[2];
float32_t joyX, joyY;

//Values for touchscreen
uint16_t actTSx;
uint16_t actTSy;


// Character position
int16_t CharPos[2]={160,110};
int16_t ActionClick[2]; // Save last click of action button
uint8_t CharOrientation=0; // 0 means forward, 2 means right, 4 means backward, 8 means lefts
int16_t CharPosToGo[2]={160,110};
int16_t CharPatch[320][2];
uint8_t inMovement;
uint8_t isTalking;
uint16_t CharTimeMoveCounter;
uint8_t actionPending;

// Screen initial column is related to character position
int16_t screenInitCol;
uint32_t tScreenLastRefreshTime; // Debug variable to check timestamp of execution

// TIMER 6
uint32_t tCursorRefresh = 0, tScreenRefresh = 0, PositionRefreshFlag = 0, tGameTime=0, tCharRefresh=0;
uint8_t CursorRefreshFlag = 0, ScreenRefreshFlag = 0;

// Counters for animations
uint32_t graphicCounterMovement, graphicCounter;

// Counters and flags for talking
uint8_t somethingToSay; // Flag activated when is something to say
uint32_t graphicCounterTalking; // The value that graphics counter had when last talking sentence appeared
const uint8_t TALKINGSPEED = 20; // how many graphic ticks (not time ticks) last a sentece in screen
char bufferTalking[200];
uint8_t pBufferTalking; // pointer to last position buffer talking
char sentence[30]; // Sentence to say
uint8_t sentencelength;

// Related to what's going on
char guiWhatsGoingOn[29];
int lenGuiWhatsGoingOn;

// Buttons
// External flags to update
uint8_t flagButtonA=0;
uint8_t flagButtonB=0;
uint8_t flagButtonSelect=0;
uint8_t flagButtonStart=0;

// Old state of buttons to activate flags on edges
uint8_t oldStateButtonA=0;
uint8_t oldStateButtonB=0;
uint8_t oldStateButtonSelect=0;
uint8_t oldStateButtonStart=0;
uint8_t oldPushTouchScreen=0;

// Other flags
uint8_t flagEndingGame;

// Game main function
void MMMania_Run(){

	Screen_Select();
	Screen_Intro();


	// Fill inventory
	Inventory_Start();
	Inventory_Add(inCoolKey);
	Inventory_Add(inPackage);

	/* Game goes on */


	/* Game starting variables */
	highlightedOption = OPT_NONE;
	selectedOption	= OPT_NONE;
	CursorPos[0]=160;
	CursorPos[1]=120;
	CharPos[0]=160;
	CharPos[1]=110;
	CharOrientation=0;
	CharPosToGo[0]=160;
	CharPosToGo[1]=110;
	actual_Room = rmFloor3st;
	memcpy(room_Objects, obFloor3st, sizeof(obFloor3st));

	gameGoingOn=1;

	while(gameGoingOn) {
		Cursor_Update();
		Buttons_Update();
		Character_Update();
		Action_Update();
		Screen_Update(0);
		Talk_Update();
	}
}

void Cursor_Update() {
	if (CursorRefreshFlag) {

		TS_State tsdata;
		TS_GetState(&tsdata);
		TS_CalculatePos(tsdata.x, tsdata.y, &actTSx,&actTSy);

		if(tsdata.touchDetected)
		{
			CursorPos[0]=actTSx;
			CursorPos[1]=actTSy;
			oldPushTouchScreen=1;
			printf("TouchScreen, valores x=%d, y=%d, z=%d\r",tsdata.x,tsdata.y,tsdata.z); // Debug info
		}
		else if(!tsdata.touchDetected && oldPushTouchScreen)// Se levanta el dedo
		{
			flagButtonA=1; // Es como si se pulsase el botón A en las coordenadas del touchscreen. Anular?
			oldPushTouchScreen=0;
		}
		else // No ha habido cambos en el TouchScreen
		{
			Joystick_Read(&joyX, &joyY);
			if (joyX < -3.0 || joyX > 3.0 ){
				if((CursorPos[0] + (int16_t)joyX) <0)
				{
					CursorPos[0]=0;
				}
				else if((CursorPos[0]+(int16_t)joyX)>319)
				{
					CursorPos[0]=319;
				}
				else
				{
					CursorPos[0] +=(int16_t)joyX;
				}
			}
			if (joyY < -3.0 || joyY > 3.0 ){
				if((CursorPos[1] + (int16_t)joyY) <0)
					{
						CursorPos[1]=0;
					}
					else if((CursorPos[1]+(int16_t)joyY)>240)
					{
						CursorPos[1]=240;
					}
					else
					{
						CursorPos[1] +=(int16_t)joyY;
					}
			}
		}
		// Si está dentro del area activa, actualiza la posición absoluta del cursor
		if(CursorPos[1]>=20 && CursorPos[1]<148)
		{
			//absCursorPos es la posición del cursor en coordenadas de la habitación
			absCursorPos[0]=screenInitCol + CursorPos[0];
			absCursorPos[1]=CursorPos[1]-20;
		}
		// Actualiza la opción del menú seleccionada
		Update_Highlighted();
		CursorRefreshFlag = 0;
	}
}
void Character_Update(){
	if(PositionRefreshFlag){
		if(CharPos[0]!=CharPosToGo[0] || CharPos[1]!=CharPosToGo[1]){
			CharTimeMoveCounter +=1;
			CharPos[0]=CharPatch[CharTimeMoveCounter][0];
			CharPos[1]=CharPatch[CharTimeMoveCounter][1];
		}
		else{
			inMovement=0;
		}
		PositionRefreshFlag=0;
	}
}

void Buttons_Update(){

	if (flagButtonA){
		printf("Button A pressed in coordinates %d,%d !\r",CursorPos[0],CursorPos[1]);
		// Si está en la pantalla activa va a la posición
		if(CursorPos[1]>=20 && CursorPos[1]<148){
			// Si ya hay una opción seleccionada, se ejecuta en las coordenadas del click
			if(selectedOption!=OPT_NONE)
			{
				ActionClick[0]=absCursorPos[0];
				ActionClick[1]=absCursorPos[1];
				clickedObject=Check_Object(absCursorPos[0],absCursorPos[1]);
				pendingOption=selectedOption;
				selectedOption = OPT_NONE;
				actionPending = 1;
			}

			// Al final, haya acción o no, se mueve a las coordenadas de posición.
			// TODO: mejora: Sobreescribir las coordenadas del joystick por las del centro del objeto cuando se haga click en un objeto

			Start_Movement();
		}
		else if(CursorPos[0]>=220 && CursorPos[1]>=180)
		{ // Cursor in inventary
			clickedInventory=highlightedInventory;
			if(selectedOption==OPT_USE)
			{
				selectedOption=OPT_USEIN;
			}
			else
			{
				pendingOption=selectedOption;
				selectedOption = OPT_NONE;
				actionPending = 1;
			}
		}

		else{
			selectedOption=highlightedOption;
			clickedObject=&emptyObject;
		}
		flagButtonA=0;
	}
	if (flagButtonB){
		printf("Button B pressed!\r");
		// Si está en la pantalla activa, y no hay una opción previamente seleccionada, y hay alguna opción resaltada, la actualiza y se mueve allí
		if(CursorPos[1]>=20 && CursorPos[1]<148){
			if( highlightedOption != OPT_NONE)
			{
				pendingOption=highlightedOption;
				ActionClick[0]=absCursorPos[0];
				ActionClick[1]=absCursorPos[1];
				clickedObject=Check_Object(absCursorPos[0],absCursorPos[1]);
				actionPending = 1;
				Start_Movement();
			}
		}
		else if(CursorPos[0]>=220 && CursorPos[1]>=180)
		{ //  Cursor is in inventory
			selectedOption=highlightedOption;
			clickedInventory=highlightedInventory;
		}
		else
		{
			pendingOption=OPT_NONE;
			selectedOption=OPT_NONE;
		}
		graphicCounterTalking = 0; // This forces a new sentence in case is talking
		// isTalking = isTalking^0x01;  // Debug talk
		flagButtonB=0;
	}
	if (flagButtonSelect){
		printf("Button Select pressed!\r");
		if(gameGoingOn)
		{
			Tone_PlayRTTTL(Meloy_Tones[Tone_gameMMMfunny]);
			Tone_RepeatSong(RESET);
		}
		flagButtonSelect=0;
	}
	if (flagButtonStart){

		printf("Button Start pressed!\r");
		flagButtonStart=0;
		if(gameGoingOn)
		{
			Screen_Exit();
		}
	}
}

void Screen_Update(uint8_t options) {
	// No need to check if init is over agreeable value; function FlashImageShow2 makes it
	uint8_t i;

	// InitCol is an important parameter of the graphic displayment. Parse values between min and max
	screenInitCol= CharPos[0]-160;
	if(screenInitCol <0)
	{
		screenInitCol=0;
	}

	if (screenInitCol>actual_Room.width-320)
	{
		screenInitCol=actual_Room.width-320;
	}

	if (ScreenRefreshFlag) {
		uint32_t tDirtyTime=tGameTime;
		Screen_Load_Image2(&actual_Room,screenInitCol);
		for(i=0;i<=5;i++)
		{
			Screen_Load_Object(&room_Objects[i],screenInitCol,0);
		}
		Screen_Load_Image(CharPos[0]-screenInitCol-13, CharPos[1]-81, &bmWeirdEd[CharOrientation][((graphicCounter-graphicCounterMovement)%4 + 2)*inMovement] + (graphicCounter%2)*isTalking);
		if(options==0){Erase_Cursor();}

		FlashImageShow2(0, 20, &bmActiveScreen,0,320);

		//LCD_DrawFilledRectangle(0, 0, 320, 10, 0x0000);
		//LCD_DrawFilledRectangle(0, 138, 320, 102, 0x0000);
		if(options==0){
			Update_GUI();
			Draw_Cursor(CursorPos[0],CursorPos[1]);
		}
		graphicCounter = graphicCounter + 1;
		ScreenRefreshFlag=0;
		tScreenLastRefreshTime = tGameTime-tDirtyTime; // Debugging purpose only
	}
}


void MMMania_Callback(uint32_t tLocalTime) {

	// Cursor refresh flag
	if (tLocalTime - tCursorRefresh > CURSORSPEED) {
		tCursorRefresh = tLocalTime;
		CursorRefreshFlag = 1;

	}
	if (tLocalTime - tCharRefresh > CHARSPEED) {
		tCharRefresh = tLocalTime;
		PositionRefreshFlag = 1;
	}
	// Screen refresh flag
	if (tLocalTime - tScreenRefresh > SCREENREFRESHTIME) {
		tScreenRefresh = tLocalTime;
		ScreenRefreshFlag = 1;
	}

	if(oldStateButtonStart==SET && PushButtonGetState(BUTTON_START)==RESET)
	{
		flagButtonStart=1;
	}
	if(oldStateButtonSelect==SET && PushButtonGetState(BUTTON_SELECT)==RESET)
	{
		flagButtonSelect=1;
	}
	if(oldStateButtonA==SET && PushButtonGetState(BUTTON_A)==RESET)
	{
		flagButtonA=1;
	}
	if(oldStateButtonB==SET && PushButtonGetState(BUTTON_B)==RESET)
	{
		flagButtonB=1;
	}

	oldStateButtonStart = PushButtonGetState(BUTTON_START);
	oldStateButtonSelect = PushButtonGetState(BUTTON_SELECT);
	oldStateButtonA = PushButtonGetState(BUTTON_A);
	oldStateButtonB = PushButtonGetState(BUTTON_B);


	tGameTime=tLocalTime;
}




const uint8_t CursorLenght=20;
const uint8_t CursorWidth=0;
void Erase_Cursor(void)
{
	// Erases last cursor positions
	LCD_DrawLine((uint16_t)CursorOldPos[0]-CursorLenght/2,CursorOldPos[1] ,(uint16_t) CursorOldPos[0]+CursorLenght/2, CursorOldPos[1], CursorWidth,0x0000);
	LCD_DrawLine(CursorOldPos[0],(uint16_t)CursorOldPos[1]-CursorLenght/2 ,CursorOldPos[0],(uint16_t) CursorOldPos[1]+CursorLenght/2, CursorWidth,0x0000);
}
void Draw_Cursor(uint16_t x, uint16_t y)
{

	// Draws new cursor
	LCD_DrawLine((uint16_t)x-CursorLenght/2,y ,(uint16_t) x+CursorLenght/2, y, CursorWidth,0xFFFF);
	LCD_DrawLine(x,(uint16_t)y-CursorLenght/2 ,x,(uint16_t) y+CursorLenght/2, CursorWidth,0xFFFF);

	// Updates last coordinates where the cursor was drawn
	CursorOldPos[0]=x;
	CursorOldPos[1]=y;
}


void checkLimits(int16_t xIn,int16_t yIn,int16_t *xOut,int16_t *yOut)
{
	int16_t xMin = actual_Room.limitsWalk[0][0];
	int16_t yMax = actual_Room.limitsWalk[1][0];
	int16_t xMax = actual_Room.limitsWalk[0][7];
	int16_t yMin;

	if(xIn<=actual_Room.limitsWalk[0][0])
	{
		yMin = actual_Room.limitsWalk[1][0];
	}
	else if(xIn<=actual_Room.limitsWalk[0][1])
	{
		yMin = yRamp(xIn, actual_Room.limitsWalk[0][0],actual_Room.limitsWalk[1][0],actual_Room.limitsWalk[0][1],actual_Room.limitsWalk[1][1]);
	}
	else if(xIn<=actual_Room.limitsWalk[0][2])
	{
		yMin = yRamp(xIn, actual_Room.limitsWalk[0][1],actual_Room.limitsWalk[1][1],actual_Room.limitsWalk[0][2],actual_Room.limitsWalk[1][2]);
	}
	else if(xIn<=actual_Room.limitsWalk[0][3])
	{
		yMin = yRamp(xIn, actual_Room.limitsWalk[0][2],actual_Room.limitsWalk[1][2],actual_Room.limitsWalk[0][3],actual_Room.limitsWalk[1][3]);
	}
	else if(xIn<=actual_Room.limitsWalk[0][4])
	{
		yMin = yRamp(xIn, actual_Room.limitsWalk[0][3],actual_Room.limitsWalk[1][3],actual_Room.limitsWalk[0][4],actual_Room.limitsWalk[1][4]);
	}
	else if(xIn<=actual_Room.limitsWalk[0][5])
	{
		yMin = yRamp(xIn, actual_Room.limitsWalk[0][4],actual_Room.limitsWalk[1][4],actual_Room.limitsWalk[0][5],actual_Room.limitsWalk[1][5]);
	}
	else if(xIn<=actual_Room.limitsWalk[0][6])
	{
		yMin = yRamp(xIn, actual_Room.limitsWalk[0][5],actual_Room.limitsWalk[1][5],actual_Room.limitsWalk[0][6],actual_Room.limitsWalk[1][6]);
	}
	else if(xIn<=actual_Room.limitsWalk[0][7])
	{
		yMin = yRamp(xIn, actual_Room.limitsWalk[0][6],actual_Room.limitsWalk[1][6],actual_Room.limitsWalk[0][7],actual_Room.limitsWalk[1][7]);
	}
	else
	{
		yMin=actual_Room.limitsWalk[1][7];
	}
	if(xIn<xMin)
	{
		*xOut=xMin;
	}
	else if(xIn>xMax)
	{
		*xOut=xMax;
	}
	else
	{
		*xOut=xIn;
	}
	if(yIn<yMin)
	{
		*yOut=yMin;
	}
	else if(yIn>yMax)
	{
		*yOut=yMax;
	}
	else
	{
		*yOut=yIn;
	}
}
uint16_t yRamp( int16_t x, int16_t x_0, int16_t y_0, int16_t x_1, int16_t y_1)
{
	if(x<x_0)
		return y_0;
	else if(x>x_1)
		return y_1;
	else
		return y_0 + ((y_1-y_0)*(x-x_0))/(x_1-x_0) ;
}

void Start_Movement(){

	// Va a la zona de paseo más cercana (si el click es en una pared, pues en una zona proxima al inicio de la pared
	checkLimits(absCursorPos[0],absCursorPos[1],&CharPosToGo[0],&CharPosToGo[1]);

	int16_t distanceX = (int16_t) CharPosToGo[0] - CharPos[0];
	int16_t distanceY = (int16_t) CharPosToGo[1] - CharPos[1];

	graphicCounterMovement = graphicCounter;
	if(distanceX != 0 || distanceY != 0 )
	{
		inMovement = 1;
		// Moverse y hablar no es posible
		isTalking=0;

		// Guarda una linea Bresenham... como un array de 2x320
		int16_t i;
		int16_t xinc, yinc, cumul;
		int16_t x, y;

		CharTimeMoveCounter=0;

		x = CharPos[0];
		y = CharPos[1];

		// Line Bresenham
		xinc = (distanceX >= 0)? 1 : -1;
		yinc = (distanceY >= 0)? 1 : -1;
		distanceX = (distanceX > 0)? distanceX : -distanceX;
		distanceY = (distanceY > 0)? distanceY : -distanceY;
		CharPatch[0][0]=x;
		CharPatch[0][1]=y;

		if (distanceX > distanceY) {
			if(xinc>0){
				CharOrientation=1;
			}
			else{
				CharOrientation=3;
			}
			cumul = distanceX/2;
			for (i=1;i<=distanceX;i++) {
				x += xinc;
				cumul += distanceY;

				if ( cumul >= distanceX ) {
					cumul -= distanceX;
					y += yinc;
				}
				CharPatch[i][0]=x;
				CharPatch[i][1]=y;
			}

		} else {
			if(yinc>0){
				CharOrientation=0;
			}
			else{
				CharOrientation=2;
			}
			cumul = distanceY/2;
			for (i=1;i<=distanceY;i++) {
				y += yinc ;
				cumul += distanceX ;

				if (cumul >= distanceY) {
					cumul -= distanceY ;
					x += xinc ;
				}
				CharPatch[i][0]=x;
				CharPatch[i][1]=y;
			}
		}
		inMovement=1;
	}
	else
	{
		inMovement=0;
	}
}


void Update_GUI()
{
	const uint16_t defColour=0x0211;
	const uint16_t selColour=0xFE42;


	uint16_t colGive=defColour;
	uint16_t colOpen=defColour;
	uint16_t colClose=defColour;
	uint16_t colLook=defColour;
	uint16_t colTalk=defColour;
	uint16_t colPick=defColour;
	uint16_t colUse=defColour;
	uint16_t colPush=defColour;
	uint16_t colPull=defColour;

	char guiGive[5]="GIVE";
	char guiOpen[5]="OPEN";
	char guiClose[6]="CLOSE";
	char guiLook[7]="LOOKAT";
	char guiTalk[7]="TALKTO";
	char guiPick[7]="PICKUP";
	char guiUse[4]="USE";
	char guiPush[5]="PUSH";
	char guiPull[5]="PULL";

	char textHighlighted[10]="";


	switch (highlightedOption) {
	case OPT_GIVE:
		colGive=selColour;
		sprintf(textHighlighted,"Give");
		break;
	case OPT_OPEN:
		colOpen=selColour;
		sprintf(textHighlighted,"Open");
		break;
	case OPT_CLOSE:
		colClose=selColour;
		sprintf(textHighlighted,"Close");
		break;
	case OPT_LOOKAT:
		colLook=selColour;
		sprintf(textHighlighted,"Look at");
		break;
	case OPT_TALKTO:
		colTalk=selColour;
		sprintf(textHighlighted,"Talk to");
		break;
	case OPT_PICKUP:
		colPick=selColour;
		sprintf(textHighlighted,"Pick up");
		break;
	case OPT_USE:
		colUse=selColour;
		sprintf(textHighlighted,"Use");
		break;
	case OPT_PUSH:
		colPush=selColour;
		sprintf(textHighlighted,"Push");
		break;
	case OPT_PULL:
		colPull=selColour;
		sprintf(textHighlighted,"Pull");
		break;
	case OPT_WALKINTO:
		sprintf(textHighlighted,"Walk into");
		break;
	case OPT_WALK:
		sprintf(textHighlighted,"Walk to");
		break;
	case OPT_USEIN:
		colUse=selColour;
		sprintf(textHighlighted,"Use");
		break;
	default:
		break;
	}
	char textSelected[10]="";
	switch (selectedOption)
	{
	case OPT_GIVE:
		sprintf(textSelected,"Give");
		break;
	case OPT_OPEN:
		sprintf(textSelected,"Open");
		break;
	case OPT_CLOSE:
		sprintf(textSelected,"Close");
		break;
	case OPT_LOOKAT:
		sprintf(textSelected,"Look at");
		break;
	case OPT_TALKTO:
		colTalk=selColour;
		sprintf(textSelected,"Talk to");
		break;
	case OPT_PICKUP:
		sprintf(textSelected,"Pick up");
		break;
	case OPT_USE:
		sprintf(textSelected,"Use");
		break;
	case OPT_PUSH:
		sprintf(textSelected,"Push");
		break;
	case OPT_PULL:
		sprintf(textSelected,"Pull");
		break;
	case OPT_WALKINTO:
		sprintf(textSelected,"Walk into");
		break;
	case OPT_WALK:
		sprintf(textSelected,"Walk to");
		break;
	case OPT_USEIN:
		sprintf(textSelected,"Use %s in",clickedInventory->name);
		break;
	default:
		break;
	}
	LCD_DisplayString (0,180,guiGive,&Font20,colGive,0x0000,SET);
	LCD_DisplayString (0,200,guiOpen,&Font20,colOpen,0x0000,SET);
	LCD_DisplayString (0,220,guiClose,&Font20,colClose,0x0000,SET);
	LCD_DisplayString (75,180,guiPick,&Font20,colPick,0x0000,SET);
	LCD_DisplayString (75,200,guiLook,&Font20,colLook,0x0000,SET);
	LCD_DisplayString (75,220,guiTalk,&Font20,colTalk,0x0000,SET);
	LCD_DisplayString (162,180,guiUse,&Font20,colUse,0x0000,SET);
	LCD_DisplayString (162,200,guiPush,&Font20,colPush,0x0000,SET);
	LCD_DisplayString (162,220,guiPull,&Font20,colPull,0x0000,SET);

	// Erase last what's going on message
	LCD_DisplayString ((160-lenGuiWhatsGoingOn*11/2),150,guiWhatsGoingOn,&Font16,0x0000,0x0000,RESET);

	if(highlightedInventory->visibility==VISIBLE)
	{
		if(selectedOption==OPT_USEIN)
		{
			lenGuiWhatsGoingOn=sprintf(guiWhatsGoingOn,"%s",textSelected);
		}
		else
		{
			lenGuiWhatsGoingOn=sprintf(guiWhatsGoingOn,"%s %s",textSelected,highlightedInventory->name);
		}
	}
	else
	{
		lenGuiWhatsGoingOn=sprintf(guiWhatsGoingOn,"%s %s",textSelected,highlightedObject->name);
	}
    // Para hacer que el mensaje esté centrado, la mitad de la pantalla son 160, y cada caracter mide 11 puntos.
	LCD_DisplayString ((160-lenGuiWhatsGoingOn*11/2),150,guiWhatsGoingOn,&Font16,defColour,0x0000,SET);
	// Se muestra el inventario actual
	GAME_INVENTORY *InventorytoShow;
	uint8_t i;
	for(i=0;i<=4;i++)
	{
		InventorytoShow = &actualInventory[i];
		if(InventorytoShow->visibility==VISIBLE)
		{
			FlashImageShow(220+(i%2)*50, 180+(i/2)*30,(FLASH_BITMAP *)InventorytoShow);
		}
		else
		{
			LCD_DrawFilledRectangle(220+(i%2)*50, 180+(i/2)*30, 50, 30, 0x0000);
		}
	}

}

void Update_Highlighted(){
	highlightedInventory=&emptyInventory;
	highlightedObject=&emptyObject;
	if(selectedOption ==OPT_WALK || selectedOption ==OPT_WALKINTO ){selectedOption=OPT_NONE;}
	if(CursorPos[1]>=20 && CursorPos[1]<148)
	{
		//Devuelve la opción en función de lo que hay en pantalla

		highlightedObject=Check_Object(absCursorPos[0],absCursorPos[1]);

		if(selectedOption ==OPT_NONE || selectedOption ==OPT_WALKINTO ){selectedOption=OPT_WALK;}
		if(highlightedObject->typeObject==DOOR)
		{
			if(highlightedObject->visibility==OPEN)
			{
				highlightedOption=OPT_CLOSE;
				if(selectedOption==OPT_WALK){selectedOption=OPT_WALKINTO;}
			}
			else
			{
				highlightedOption=OPT_OPEN;
			}
		}
		else if(highlightedObject->typeObject==THRESHOLD)
		{
			if(selectedOption==OPT_WALK){selectedOption=OPT_WALKINTO;}
		}
		else if(highlightedObject->defaultAction!=OPT_NONE)
		{
			highlightedOption=highlightedObject->defaultAction;
		}
		else
		{
			highlightedOption=OPT_NONE;
		}
	}
	else if(CursorPos[1]>=177 && CursorPos[1]<197 && CursorPos[0]<220)
	{
		if(CursorPos[0]<75)
		{
			highlightedOption=OPT_GIVE;
		}
		else if(CursorPos[0]<160)
		{
			highlightedOption=OPT_PICKUP;
		}
		else
		{
			highlightedOption=OPT_USE;
		}
	}
	else if(CursorPos[1]>=197 && CursorPos[1]<217 && CursorPos[0]<220)
	{
		if(selectedOption ==OPT_WALK || selectedOption ==OPT_WALKINTO ){selectedOption=OPT_NONE;}
		if(CursorPos[0]<75)
		{
			highlightedOption=OPT_OPEN;
		}
		else if(CursorPos[0]<160)
		{
			highlightedOption=OPT_LOOKAT;
		}
		else
		{
			highlightedOption=OPT_PUSH;
		}
	}
	else if(CursorPos[1]>=217 && CursorPos[0]<220)
	{
		if(selectedOption ==OPT_WALK || selectedOption ==OPT_WALKINTO ){selectedOption=OPT_NONE;}
		if(CursorPos[0]<75)
		{
			highlightedOption=OPT_CLOSE;
		}
		else if(CursorPos[0]<160)
		{
			highlightedOption=OPT_TALKTO;
		}
		else
		{
			highlightedOption=OPT_PULL;
		}
	}
	else if(CursorPos[0]>=220 && CursorPos[1]>=180)
	{
		// Cursor is in inventory
		if(CursorPos[0]<270)
		{
			if(CursorPos[1]<210)
			{
				highlightedInventory = &actualInventory[0];
				highlightedOption = highlightedInventory->defaultAction;
			}
			else
			{
				highlightedInventory = &actualInventory[2];
				highlightedOption = highlightedInventory->defaultAction;
			}
		}
		else
		{
			if(CursorPos[1]<210)
			{
				highlightedInventory = &actualInventory[1];
				highlightedOption = highlightedInventory->defaultAction;
			}
			else
			{
				highlightedInventory = &actualInventory[3];
				highlightedOption = highlightedInventory->defaultAction;
			}
		}
	}
	else
	{
		//if(selectedOption ==OPT_WALK || selectedOption ==OPT_WALKINTO ){selectedOption=OPT_NONE;}
		highlightedOption=OPT_NONE;
	}
}


GAME_OBJECT *Check_Object(uint16_t x,uint16_t y)
{
	/* Devuelve un puntero con la dirección del objeto donde se ha hecho click */
	uint8_t i;
	GAME_OBJECT *objectAction;
	for(i=0;i<=5;i++)
	{
		objectAction=&room_Objects[i];
		if(x>=objectAction->col && x < (objectAction->col+objectAction->width) && y > objectAction->row && y < (objectAction->row+objectAction->height) && (objectAction->typeObject==DOOR || objectAction->visibility==VISIBLE) )
		{
			break;
		}
	}
	if(i<6)
	{
		return objectAction;
	}
	else
	{
		return &emptyObject;
	}
}

void Change_Room(void)
{

	// Clearing flags
	inMovement=0;
	isTalking=0;
	actionPending=0;
	highlightedOption = OPT_NONE;
	selectedOption	= OPT_NONE;

	if(clickedObject->param==0x3031)
	{
		// Store actual data into room state
		memcpy(obFloor3st,room_Objects,sizeof(room_Objects));

		// Load stored data from objects room
		memcpy(room_Objects, obRoom31, sizeof(obRoom31));

		// Defining new room
		actual_Room = rmRoom31;

		// Defining character position

		CharPos[0]=122;
		CharPos[1]=100;
		CharPosToGo[0]=122;
		CharPosToGo[1]=100;
	}
	else if(clickedObject->param==0x3130)
	{
		// Store actual data into room state
		memcpy(obRoom31,room_Objects,sizeof(room_Objects));

		// Load stored data from objects room
		memcpy(room_Objects, obFloor3st, sizeof(obFloor3st));

		// Defining new room
		actual_Room = rmFloor3st;

		// Defining character position

		CharPos[0]=138;
		CharPos[1]=116;
		CharPosToGo[0]=138;
		CharPosToGo[1]=116;
	}
	else if(clickedObject->param==0x3032)
	{
		// Store actual data into room state
		memcpy(obFloor3st,room_Objects,sizeof(room_Objects));

		// Load stored data from objects room
		memcpy(room_Objects, obRoom32, sizeof(obRoom32));

		// Defining new room
		actual_Room = rmRoom32;

		// Defining character position

		CharPos[0]=234;
		CharPos[1]=100;
		CharPosToGo[0]=234;
		CharPosToGo[1]=100;
	}
	else if(clickedObject->param==0x3230)
	{
		// Store actual data into room state
		memcpy(obRoom32,room_Objects,sizeof(room_Objects));

		// Load stored data from objects room
		memcpy(room_Objects, obFloor3st, sizeof(obFloor3st));

		// Defining new room
		actual_Room = rmFloor3st;

		// Defining character position

		CharPos[0]=261;
		CharPos[1]=116;
		CharPosToGo[0]=261;
		CharPosToGo[1]=116;
	}
	else if(clickedObject->param==0x3033)
	{
		// Store actual data into room state
		memcpy(obFloor3st,room_Objects,sizeof(room_Objects));

		// Load stored data from objects room
		memcpy(room_Objects, obRoom33, sizeof(obRoom33));

		// Defining new room
		actual_Room = rmRoom33;

		// Defining character position

		CharPos[0]=325;
		CharPos[1]=120;
		CharPosToGo[0]=325;
		CharPosToGo[1]=120;
	}
	else if(clickedObject->param==0x3330)
	{
		// Store actual data into room state
		memcpy(obRoom33,room_Objects,sizeof(room_Objects));

		// Load stored data from objects room
		memcpy(room_Objects, obFloor3st, sizeof(obFloor3st));

		// Defining new room
		actual_Room = rmFloor3st;

		// Defining character position

		CharPos[0]=380;
		CharPos[1]=116;
		CharPosToGo[0]=380;
		CharPosToGo[1]=116;
	}
	else if(clickedObject->param==0x3020)
	{
		sprintf(bufferTalking,"I don't want to go!\rThis seems so scaring!");
		somethingToSay=1;
	}
	else{
		sprintf(bufferTalking,"No way!");
		somethingToSay=1;
	}
}

void Talk_Update(void)
{
	/*// Counters and flags for talking
uint8_t somethingToSay; // Flag activated when is something to say
uint32_t graphicCounterTalking; // The value that graphics counter had when last talking sentence appeared
const uint8_t TALKINGSPEED = 10; // how many graphic ticks (not time ticks) last a sentece in screen
char bufferTalking[200];
uint8_t pBufferTalking; // pointer to last position buffer talking*/

	if (somethingToSay && ((graphicCounter - graphicCounterTalking) > TALKINGSPEED))
	{
		// There's something to say, and is time to do, so...
		if(inMovement!=1){isTalking=1;}
		// Update time variable
		graphicCounterTalking = graphicCounter;

		// Erase last sentence
		LCD_DisplayString ((160-sentencelength*11/2),2,sentence,&Font16,0x0000,0x0000,RESET);
		uint8_t i;

		for(i=0;i<=25;i++)
		{
			if(bufferTalking[pBufferTalking+i]=='\0')
			{
				sentence[i]='\0';
				pBufferTalking=0;
				somethingToSay=0;
				break;
			}
			else if(bufferTalking[pBufferTalking+i]=='\r')
			{
				sentence[i]='\0';
				pBufferTalking=pBufferTalking+i+1;
				break;
			}
			else
			{
				sentence[i]=bufferTalking[pBufferTalking+i];
				if(i==25) // Llega al final del espacio disponible
				{
					sentence[26]='.';
					sentence[27]='.';
					sentence[28]='.';
					sentence[29]='\0';
					pBufferTalking=pBufferTalking+i;
				}
			}
		}
		// Para hacer que el mensaje esté centrado, la mitad de la pantalla son 160, y cada caracter mide 11 puntos.
		sentencelength = i;
		// draw new sentence;
		LCD_DisplayString ((160-sentencelength*11/2),2,sentence,&Font16,0x2589,0x0000,SET);
	}// Si se ha eliminado el istalking porque se ha empezado el movimiento, habría un problema
	else if (somethingToSay==0 && (isTalking||inMovement) && ((graphicCounter - graphicCounterTalking) > TALKINGSPEED))
	{
		// Erase last sentence
		LCD_DisplayString ((160-sentencelength*11/2),2,sentence,&Font16,0x0000,0x0000,RESET);
		// Stops talking
		isTalking=0;
	}
	if(isTalking==0 && somethingToSay==0 && flagEndingGame==1)
	{
		Screen_Final();
	}
}

void Action_Update()
{
	if(actionPending==1 && inMovement==0)
	{

		if(pendingOption == OPT_CLOSE)
		{
			if(clickedObject->typeObject==DOOR)
			{
				clickedObject->visibility=CLOSED;
				Tone_PlayRTTTL(Meloy_Tones[Tone_gameMMMclosedoor]);
				Tone_RepeatSong(RESET);
			}
		}
		else if(pendingOption == OPT_OPEN)
		{
			if(clickedObject->typeObject==DOOR)
			{
				if(clickedObject->visibility==LOCKED)
				{
					sprintf(bufferTalking,"I can't open\rSeems locked");
					somethingToSay=1;
				}
				else
				{
				clickedObject->visibility=OPEN;
				Tone_PlayRTTTL(Meloy_Tones[Tone_gameMMMopendoor]);
				Tone_RepeatSong(RESET);
				}
			}
			else if(clickedInventory->param==0xA004)
			{   // Open the Farnell package
				sprintf(bufferTalking,"A ST microcontroller\rJust what i needed!");
				somethingToSay=1;
				*clickedInventory=inSTMicro;
			}
		}
		else if(pendingOption == OPT_WALKINTO)
		{
			Change_Room();
		}
		else if(pendingOption == OPT_USEIN)
		{
			Use_In();
		}
		else if(pendingOption== OPT_USE)
		{
			Use();
		}
		else if(pendingOption== OPT_PICKUP)
		{
			PickUp();
		}
		else if(pendingOption== OPT_LOOKAT)
		{
			LookAt();
		}
		pendingOption = OPT_NONE;
		clickedObject = &emptyObject;
		clickedInventory = &emptyInventory;
		actionPending =0;
	}
}

void Use()
{
	if(clickedObject->param==0x3120 || clickedObject->param==0x3121 || clickedObject->param==0x3122)
	{
		CharOrientation=0;
		sprintf(bufferTalking,"Can't use, is broken!");
		somethingToSay=1;
	}
	else if(clickedObject->param==0x3123)
	{
		CharOrientation=2;
		sprintf(bufferTalking,"Now is repaired, \rI don't need you anymore");
		somethingToSay=1;
		flagEndingGame=1;
	}
	if(clickedObject->param==0x3110)
	{
		CharOrientation=0;
		sprintf(bufferTalking,"Can't use, is broken!");
		somethingToSay=1;
	}
	if(clickedObject->param==0x3130)
	{
		CharOrientation=0;
		sprintf(bufferTalking,"Can't use, is broken!");
		somethingToSay=1;
	}
	if(clickedObject->param==0x3140)
	{
		CharOrientation=0;
		sprintf(bufferTalking,"Can't use, is broken!");
		somethingToSay=1;
	}
	if(clickedObject->param==0x3150)
	{
		CharOrientation=0;
		sprintf(bufferTalking,"Can't use, is broken!");
		somethingToSay=1;
	}
	else if(clickedObject->param==0x3310)
	{ // Radio
		CharOrientation=2;
		sprintf(bufferTalking,"I've no idea how\rto use this");
		somethingToSay=1;
	}
	else if(clickedObject->param==0x3320)
	{ // Radio
		CharOrientation=2;
		sprintf(bufferTalking,"Seems it works");
		somethingToSay=1;
	}

}

void Use_In()
{
	if(clickedInventory->param==0xA001) // DC SOURCE
	{
		if(clickedObject->param==0x3120) // Disco Crazy totally broken
		{
			clickedObject->param=0x3121;
			CharOrientation=0;
			sprintf(bufferTalking,"Cool! fits\r I still need a\r programmed chip");
			somethingToSay=1;
			Inventory_Remove(clickedInventory);
			//actualInventory[3]=emptyInventory;
		}
		else if(clickedObject->param==0x3122)
		{
			clickedObject->param=0x3123;
			CharOrientation=0;
			sprintf(bufferTalking,"Cool! fits\r Now is repaired!");
			somethingToSay=1;
			Inventory_Remove(clickedInventory);
			//actualInventory[3]=emptyInventory;
		}
		else if(clickedObject->param==0x3310) // Radio
		{
			room_Objects[1].visibility=VISIBLE;
			Inventory_Remove(clickedInventory);
			//actualInventory[3]=emptyInventory;
		}
		else
		{
			CharOrientation=0;
			sprintf(bufferTalking,"Doesn't work!");
			somethingToSay=1;
		}
	}
	else if(clickedInventory->param==0xA002)
	{ // Hamster
		CharOrientation=0;
		sprintf(bufferTalking,"It's alive!\rYou can't use it!");
		somethingToSay=1;
	}
	else if(clickedInventory->param==0xA003)
	{ // Key
		if(clickedObject->param==0x3033 || clickedObject->param==0x3330)
		{
			if(clickedObject->visibility==LOCKED)
			{
				clickedObject->visibility=OPEN;
				CharOrientation=0;
				sprintf(bufferTalking,"Door unlocked");
				somethingToSay=1;
			}
			else
			{
				CharOrientation=0;
				sprintf(bufferTalking,"Door is already unlocked");
				somethingToSay=1;
			}
		}
		else if(clickedObject->param==0x3032 || clickedObject->param==0x3230 || clickedObject->param==0x3031 || clickedObject->param==0x3130 || clickedObject->param==0x3034 || clickedObject->param==0x3430 || clickedObject->param==0x3035 || clickedObject->param==0x3530)
		{
			sprintf(bufferTalking,"Key doesn't fit here");
			somethingToSay=1;
		}
		else
		{
			sprintf(bufferTalking,"Not a door, idiot!");
			somethingToSay=1;
		}
	}
	else if(clickedInventory->param==0xA004)
	{ // Package
		CharOrientation=0;
		sprintf(bufferTalking,"You idiot!\rIt's... a package!\rWhy not try to open?");
		somethingToSay=1;
	}
	else if(clickedInventory->param==0xA005)
	{ // ST
		if(clickedObject->param==0x3120) // Disco Crazy totally broken
		{
			clickedObject->param=0x3122;
			CharOrientation=0;
			sprintf(bufferTalking,"Cool! fits\r I still need a\r DC Source");
			somethingToSay=1;
			Inventory_Remove(clickedInventory);
			//actualInventory[1]=emptyInventory;
		}
		else if(clickedObject->param==0x3121)
		{
			clickedObject->param=0x3123;
			CharOrientation=0;
			sprintf(bufferTalking,"Cool! fits\r Now is repaired!");
			Inventory_Remove(clickedInventory);
			//actualInventory[1]=emptyInventory;
		}
		else
		{
			CharOrientation=0;
			sprintf(bufferTalking,"Doesn't work!");
			somethingToSay=1;
		}
	}
}

void PickUp(void){
	if(clickedObject->param==0x3210) // Hamster
	{
		clickedObject->visibility=HIDDEN;
		Inventory_Add(inHamster);
		//actualInventory[2]=inHamster;
	}
	else if(clickedObject->param==0x3320) // DC source
	{
		clickedObject->visibility=HIDDEN;
		Inventory_Add(inDCSource);
		//actualInventory[3]=inDCSource;
	}
	else
	{
		CharOrientation=0;
		sprintf(bufferTalking,"Can't pick this up");
		somethingToSay=1;
	}
}

void LookAt(void){
	if(clickedObject->param==0x3110) // Pacman machine
	{
		CharOrientation=2;
		sprintf(bufferTalking,"Broken pacman machine");
		somethingToSay=1;
	}
	else if(clickedObject->param==0x3120) // Disco Crazy machine
	{
		CharOrientation=2;
		sprintf(bufferTalking,"Disco Crazy machine\r is broken :(\rI must repair");
		somethingToSay=1;
	}
	else if(clickedObject->param==0x3121) // Disco Crazy machine
	{
		CharOrientation=2;
		sprintf(bufferTalking,"Still broken\rIt needs a proper\rprogrammed chip");
		somethingToSay=1;
	}
	else if(clickedObject->param==0x3122) // Disco Crazy machine
	{
		CharOrientation=2;
		sprintf(bufferTalking,"Still broken,\rneeds a DC source");
		somethingToSay=1;
	}
	else if(clickedObject->param==0x3123) // Disco Crazy machine
	{
		CharOrientation=2;
		sprintf(bufferTalking,"Can't believe I repaired\rfear me, Garriga!\rstudent became a master!\r Ha Ha Ha");
		somethingToSay=1;
	}
	else if(clickedObject->param==0x3130) // Disco Crazy machine
	{
		CharOrientation=2;
		sprintf(bufferTalking,"Solar fox machine\r is broken");
		somethingToSay=1;
	}
	else if(clickedObject->param==0x3140) // Arcanoid
	{
		CharOrientation=2;
		sprintf(bufferTalking,"Broken Arkanoid machine");
		somethingToSay=1;
	}
	else if(clickedObject->param==0x3150) // Sierpe
	{
		CharOrientation=2;
		sprintf(bufferTalking,"Broken Snake machine");
		somethingToSay=1;
	}
	else if(clickedObject->param==0x3210) // hams
	{
		CharOrientation=2;
		sprintf(bufferTalking,"My cute hamster!\rWuzi wuzi wuzi");
		somethingToSay=1;
	}
	else if(clickedObject->param==0x3220) //
	{
		CharOrientation=1;
		sprintf(bufferTalking,"Can't see anything\rLot of 'boira'");
		somethingToSay=1;
	}
	else if(clickedObject->param==0x3230)
	{
		CharOrientation=2;
		sprintf(bufferTalking,"A diagram about how to\rexit EPS after hours");
		somethingToSay=1;
	}
	else if(clickedObject->param==0x3240)
	{
		CharOrientation=2;
		sprintf(bufferTalking,"Not really a UDL emblema\rJust a modified one");
		somethingToSay=1;
	}
	else if(clickedObject->param==0x3250) //
	{
		CharOrientation=0;
		sprintf(bufferTalking,"Can't believe i still\rkeep these Star Wars toys\rafter Disney ruined it");
		somethingToSay=1;
	}
	else if(clickedObject->param==0x3310)
	{
		CharOrientation=2;
		sprintf(bufferTalking,"Old fashioned radio");
		somethingToSay=1;
	}
	else if(clickedObject->param==0x3320)
	{
		CharOrientation=2;
		sprintf(bufferTalking,"DC power source\rseems working");
		somethingToSay=1;
	}
	else if(clickedObject->param==0x3330) //
	{
		CharOrientation=2;
		sprintf(bufferTalking,"Stupid Meteor");
		somethingToSay=1;
	}
	else if(clickedObject->param==0x3340) //
	{
		CharOrientation=2;
		sprintf(bufferTalking,"Mum!");
		somethingToSay=1;
	}
	else if(clickedObject->param==0x3020) // Threshold
	{
		CharOrientation=3;
		sprintf(bufferTalking,"Is dark and crappy!");
		somethingToSay=1;
	}
	else if(clickedInventory->param==0xA001) //
	{
		sprintf(bufferTalking,"My own source of power\rmmm no, just a converter");
		somethingToSay=1;
	}
	else if(clickedInventory->param==0xA002) //
	{
		sprintf(bufferTalking,"Very cozy in my pocket!\rHauva! Wuzi Wuzi!");
		somethingToSay=1;
	}
	else if(clickedInventory->param==0xA003) //
	{
		sprintf(bufferTalking,"Pretty cool key, huh?\rI wonder wich door opens");
		somethingToSay=1;
	}
	else if(clickedInventory->param==0xA004) //
	{
		sprintf(bufferTalking,"I've got a big package");
		somethingToSay=1;
	}
	else if(clickedInventory->param==0xA005) //
	{
		sprintf(bufferTalking,"ST Microcontroller\r32-bit Cortex M4 ");
		somethingToSay=1;
	}
}

//TODO inventory management functions
void Inventory_Start(void)
{
	uint8_t i;
	for(i=0;i<inventoryLength;i++)
	{
		actualInventory[i]=emptyInventory;
	}
}

uint8_t Inventory_Add(GAME_INVENTORY objectInventory)
{
	uint8_t i;
	for(i=0;i<inventoryLength;i++)
	{
		if(actualInventory[i].pData==0) // Inventario vacío
		{
			actualInventory[i]=objectInventory;
			return 0;
			break;
		}
	}
	return 1;
}

void Inventory_Remove(GAME_INVENTORY *objectInventory)
{
	uint8_t i,j;
	*objectInventory=emptyInventory;
	for(i=0;i<inventoryLength;i++)
	{
		if(actualInventory[i].pData==0) // Posición inventario vacío?
		{
			for(j=i+1;j<inventoryLength;j++)
			{
				if(!actualInventory[j].pData==0) // Posición inventario no vacío?
				{
					actualInventory[i]=actualInventory[j];
					actualInventory[j]=emptyInventory;
					j=inventoryLength; // End of search
				}
			}
		}
	}
}

void Screen_Select(void)
{


	uint8_t res;
	char titleChar[29];

	// FLAGS OFF
	flagButtonStart=0;
	flagButtonSelect=0;
	flagButtonA=0;
	flagButtonB=0;

	// Tittle screen
	res = BMP_Show("0:MMMania/initial.bmp", 0, 20, BMP_NORMAL);
	if (res != BMP_Success) {
		LCD_Clear(COLOR_BLACK);
		LCD_DisplayString(0,20, "Error reading an image!!", &Font16, COLOR_RED, COLOR_BLACK, 0);
	}
	sprintf(titleChar,"BROKEN ARCADE");
	LCD_DisplayString (50,150,titleChar,&Font24,0xFFFF,0x0000,SET);
	sprintf(titleChar,"A MANIAC MANSION GAME");
	LCD_DisplayString (45,175,titleChar,&Font16,0xFFFF,0x0000,SET);
	sprintf(titleChar,"START BUTTON TO INIT");
	LCD_DisplayString (45,202,titleChar,&Font16,0xFFFF,0x0000,SET);
	sprintf(titleChar,"SELECT TO CALIBRATE SCREEN");
	LCD_DisplayString (11,220,titleChar,&Font16,0xFFFF,0x0000,SET);
	while(!flagButtonStart){
		if(flagButtonSelect==1)
		{
			// Resetea el el flag y calibra la pantalla
			flagButtonSelect=0;
			TS_doCalibraion();

			// Vuelve a dibujar:
			res = BMP_Show("0:MMMania/initial.bmp", 0, 20, BMP_NORMAL);
			if (res != BMP_Success) {
				LCD_Clear(COLOR_BLACK);
				LCD_DisplayString(0,20, "Error reading an image!!", &Font16, COLOR_RED, COLOR_BLACK, 0);
			}
			sprintf(titleChar,"BROKEN ARCADE");
			LCD_DisplayString (50,150,titleChar,&Font24,0xFFFF,0x0000,SET);
			sprintf(titleChar,"A MANIAC MANSION GAME");
			LCD_DisplayString (45,175,titleChar,&Font16,0xFFFF,0x0000,SET);
			sprintf(titleChar,"START BUTTON TO INIT");
			LCD_DisplayString (45,202,titleChar,&Font16,0xFFFF,0x0000,SET);
			sprintf(titleChar,"SELECT TO CALIBRATE SCREEN");
			LCD_DisplayString (11,220,titleChar,&Font16,0xFFFF,0x0000,SET);

			// Por si acaso se ha pulsado mientras duraba la calibración:
			flagButtonStart=0;
		}
	}
	flagButtonStart=0;
	flagButtonA=0;
	flagButtonB=0;
	LCD_Clear(COLOR_BLACK);
}
void Screen_Intro(void)
{	// Here comes the intro
	highlightedOption = OPT_NONE;
	selectedOption	= OPT_NONE;

	uint8_t counterIntro;
	CursorPos[0]=0;
	CursorPos[1]=0;
	CharPos[0]=403;
	CharPos[1]=81;
	CharPosToGo[0]=403;
	CharPosToGo[1]=81;
	inMovement=0;
	isTalking=0;
	actionPending=0;
	actual_Room = rmExterior;

		memcpy(room_Objects, obExterior, sizeof(obExterior));

	// Música
	Tone_PlayRTTTL(Meloy_Tones[Tone_gameMMMstart]);
	Tone_RepeatSong(RESET);

	// Graphic conditions
	Erase_Cursor();
	screenInitCol= CharPos[0]-160;
	if(screenInitCol <0)
	{
		screenInitCol=0;
	}

	if (screenInitCol>actual_Room.width-320)
	{
		screenInitCol=actual_Room.width-320;
	}
	Screen_Load_Image2(&actual_Room,screenInitCol);
	FlashImageShow2(0, 20, &bmActiveScreen,0,320);
	Delay_ms(1000);
	room_Objects[0].visibility=VISIBLE;
	Tone_PlayRTTTL(Meloy_Tones[Tone_gameMMMopendoor]);
	Tone_RepeatSong(RESET);
	Screen_Load_Object(&room_Objects[0],screenInitCol,0);

	counterIntro=0;
	while(counterIntro<=17)
	{
		switch (counterIntro) {
			case 0:
				sprintf(bufferTalking,"I'm not a zombie Marcel\rMy name is Weird...\rWeird Ed, Ed for friends\rSo you call me Mr.Weird");
				somethingToSay=1;
				break;
			case 1:
				absCursorPos[0]=263;
				absCursorPos[1]=126;
				Start_Movement();
				break;
			case 2:
				absCursorPos[0]=48;
				absCursorPos[1]=126;
				Start_Movement();
				break;
			case 3:
				sprintf(bufferTalking,"There's mail, i wonder\rif my package has arrived");
				somethingToSay=1;
				break;
			case 4:
				room_Objects[3].visibility=HIDDEN;
				CharOrientation=0;
				sprintf(bufferTalking,"Let's open...");
				somethingToSay=1;
				break;
			case 5:
				CharOrientation=3;
				room_Objects[4].visibility=VISIBLE;
				sprintf(bufferTalking,"Finally!");
				somethingToSay=1;
				break;
			case 6:
				room_Objects[4].visibility=HIDDEN;
				room_Objects[5].visibility=VISIBLE;
				sprintf(bufferTalking,"Is a microcontroller");
				somethingToSay=1;
				break;
			case 7:
				room_Objects[5].visibility=HIDDEN;
				sprintf(bufferTalking,"...already programmed");
				somethingToSay=1;
				break;
			case 8:
				CharOrientation=0;
				sprintf(bufferTalking,"I'm a fan of arcade games\rand i want to repair\ran old arcade machine\rto play retro games\rI'll be happy to play\rDisco Crazy again\rbut i'm just a simple\rmechanical engineer\rI've no clue of this");
				somethingToSay=1;
				break;
			case 9:
				absCursorPos[0]=318;
				absCursorPos[1]=126;
				Start_Movement();
				break;
			case 10:
				absCursorPos[0]=357;
				absCursorPos[1]=81;
				Start_Movement();
				break;
			case 11:
				CharOrientation=0;
				sprintf(bufferTalking,"Before i forget...");
				somethingToSay=1;
				break;
			case 12:
				CharOrientation=1;
				room_Objects[1].visibility=VISIBLE;
				room_Objects[2].visibility=VISIBLE;
				sprintf(bufferTalking,"I'll need this key\rto look for other");
				somethingToSay=1;
				break;
			case 13:
				room_Objects[2].visibility=HIDDEN;
				CharOrientation=0;
				sprintf(bufferTalking,"electronic components");
				somethingToSay=1;
				break;
			case 14:
				room_Objects[1].visibility=HIDDEN;
				absCursorPos[0]=410;
				absCursorPos[1]=81;
				Start_Movement();
				break;
			case 15:

				break;
			case 16:
				CharOrientation=2;
				sprintf(bufferTalking,"Let's work");
				somethingToSay=1;
				break;
			case 17:
				Screen_Load_Image2(&actual_Room,screenInitCol);
				FlashImageShow2(0, 20, &bmActiveScreen,0,320);
				Tone_PlayRTTTL(Meloy_Tones[Tone_gameMMMclosedoor]);
				Tone_RepeatSong(RESET);
				break;
			default:
				break;
		}
		while(somethingToSay||isTalking||inMovement)
		{
			Buttons_Update();
			Character_Update();
			Screen_Update(1);
			Talk_Update();
		}
		counterIntro+=1;
	}
	Delay_ms(1000);
	Tone_StopPlaying();
}

void Screen_Exit(void)
{
	// Exit screen

	char titleChar[29];
	Erase_Cursor();
	LCD_DrawFilledRectangle(0, 0, 320, 20, 0x0000);
	LCD_DrawFilledRectangle(0, 148, 320, 92, 0x0000);
	sprintf(titleChar,"DO YOU WANT TO EXIT GAME?");
	LCD_DisplayString (17,150,titleChar,&Font16,0xFFFF,0x0000,SET);
	sprintf(titleChar,"START TO CONTINUE");
	LCD_DisplayString (65,202,titleChar,&Font16,0xFFFF,0x0000,SET);
	sprintf(titleChar,"SELECT TO EXIT");
	LCD_DisplayString (83,220,titleChar,&Font16,0xFFFF,0x0000,SET);
	while(!flagButtonStart && !flagButtonSelect){

	}
	if(flagButtonSelect==1)
	{
		flagButtonSelect=0;
		gameGoingOn=0;
		Tone_StopPlaying();
	}
	if(flagButtonStart==1)
	{
		flagButtonStart=0;
	}
	flagButtonA=0;
	flagButtonB=0;
	// Erases written screen
	LCD_DrawFilledRectangle(0, 148, 320, 92, 0x0000);
}

void Screen_Final(void)
{

	// Final screen
	Tone_PlayRTTTL(Meloy_Tones[Tone_gameMMMend]);
	Tone_RepeatSong(SET);
	Delay_ms(1000);
	uint8_t res;
	char titleChar[29];
	//
	res = BMP_Show("0:MMMania/initial.bmp", 0, 20, BMP_NORMAL);
	if (res != BMP_Success) {
		LCD_Clear(COLOR_BLACK);
		LCD_DisplayString(0,20, "Error reading an image!!", &Font16, COLOR_RED, COLOR_BLACK, 0);
	}
	sprintf(titleChar,"YOU'VE REACHED THE END");
	LCD_DisplayString (39,150,titleChar,&Font16,0xFFFF,0x0000,SET);
	Delay_ms(2000);
	sprintf(titleChar,"CREDITS:");
	LCD_DisplayString (121,170,titleChar,&Font16,0xFFFF,0x0000,SET);
	Delay_ms(2000);
	sprintf(titleChar,"System Integration III");
	LCD_DisplayString (33,188,titleChar,&Font16,0xFFFF,0x0000,SET);
	sprintf(titleChar,"Course 2018, EPS UDL");
	LCD_DisplayString (44,206,titleChar,&Font16,0xFFFF,0x0000,SET);
	Delay_ms(3000);
	sprintf(titleChar,"PRESS START TO EXIT");
	LCD_DisplayString (50,224,titleChar,&Font16,0xFFFF,0x0000,SET);
	while(!flagButtonStart){}
	flagButtonStart=0;
	LCD_Clear(COLOR_BLACK);
	gameGoingOn=0;
	Tone_StopPlaying();
}
