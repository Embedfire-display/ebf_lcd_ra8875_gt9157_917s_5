#ifndef	_APP_H_
#define	_APP_H_

#include "includes.h"
#include "GUI.h"
#include "DIALOG.h"
#include  "stm32f10x.h"

#define APPBKCOLOR  	(GUI_LIGHTYELLOW+0x600000)
#define APPTEXTCOLOR    GUI_MAGENTA

extern uint8_t app_prevent_refresh;

extern uint8_t OnICON0;
extern void OnICON0Clicked(void);

extern uint8_t OnICON1;
extern void OnICON1Clicked(void);

extern uint8_t OnICON2;
extern void OnICON2Clicked(void);

extern uint8_t OnICON3;
extern void OnICON3Clicked(void);

extern uint8_t OnICON4;
extern void OnICON4Clicked(void);

//extern uint8_t OnICON5;
//extern void OnICON5Clicked(void);

//extern uint8_t OnICON6;
//extern void OnICON6Clicked(void);

//extern uint8_t OnICON7;
//extern void OnICON7Clicked(void);

//extern uint8_t OnICON8;
//extern void OnICON8Clicked(void);

//extern uint8_t OnICON9;
//extern void OnICON9Clicked(void);

//extern uint8_t OnICON10;
//extern void OnICON10Clicked(void);

//extern uint8_t OnICON11;
//extern void OnICON11Clicked(void);

//extern uint8_t OnICON12;
//extern void OnICON12Clicked(void);

//extern uint8_t OnICON13;
//extern void OnICON13Clicked(void);

//extern uint8_t OnICON14;
//extern void OnICON14Clicked(void);

//extern uint8_t OnICON15;
//extern void OnICON15Clicked(void);


/*
*********************************************************************************************************
*                                         FUNCTION PROTOTYPES
*********************************************************************************************************
*/
void emWinMainApp(void);
#endif	//_APP_H_

