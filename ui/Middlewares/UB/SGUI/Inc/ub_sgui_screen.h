//--------------------------------------------------------------
// File     : ub_sgui_screen.h
//--------------------------------------------------------------

//--------------------------------------------------------------
#ifndef __STM32F4_UB_SGUI_SCREEN_H
#define __STM32F4_UB_SGUI_SCREEN_H


//--------------------------------------------------------------
// Includes
//--------------------------------------------------------------
#include "stm32_ub_system.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "stm32_ub_graphic2d.h"
#include "stm32_ub_font.h"
#include <stdbool.h>

 
//--------------------------------------------------------------
// Screen Ausrichtung
// 0 = portrait
// 1 = landscape
//--------------------------------------------------------------
//#define   SGUI_SCREEN_ORIENTATION  0   // 0=portrait
#define   SGUI_SCREEN_ORIENTATION  1   // 1=landscape




//--------------------------------------------------------------
// Farb Einstellungen (Hintergrundfarbe)
//--------------------------------------------------------------
#define   SGUI_WINCOL   0xD69A // grau 




//--------------------------------------------------------------
// Farb Einstellungen (Rahmenfarben beim 3D-Style)
//--------------------------------------------------------------
#define   SGUI_3DCOL_1   0xFFFF
#define   SGUI_3DCOL_2   0xEF5D
#define   SGUI_3DCOL_3   0x8410
#define   SGUI_3DCOL_4   0x0000
#define   SGUI_3D_FRAMESIZE   2  // 3D-Rahmen = 2 pixel breit


//--------------------------------------------------------------
// Display size
//--------------------------------------------------------------

#if SGUI_SCREEN_ORIENTATION==0
  // portrait
  #define   SGUI_SCREEN_MAXX   LCD_MAXY  // X=272
  #define   SGUI_SCREEN_MAXY   LCD_MAXX  // Y=480
#else
  // landscape
  #define   SGUI_SCREEN_MAXX   LCD_MAXX  // X=480
  #define   SGUI_SCREEN_MAXY   LCD_MAXY  // Y=272
#endif



//--------------------------------------------------------------
// Style-Typ
//--------------------------------------------------------------
typedef enum {
  STYLE_FLAT  =0, // ohne 3D-Effekt
  STYLE_RAISED,   // erhaben
  STYLE_LOWERED   // vertieft
}STYLE_TYP_t;



//--------------------------------------------------------------
// typen vom Pfeil-Button
//--------------------------------------------------------------
typedef enum {
  SARROW_UP =0, // Pfeil nach oben
  SARROW_DOWN,  // Pfeil nach unten
  SARROW_LEFT,  // Pfeil nach links
  SARROW_RIGHT  // Pfeil nach rechts
}SSCREEN_ARROW_t;



//--------------------------------------------------------------
// interne Funktionen fuer SGUI
//--------------------------------------------------------------
void SGUI_ScreenDrawArrowBtn(uint16_t x, uint16_t y, uint16_t s, SSCREEN_ARROW_t m, uint16_t c1, uint16_t c2);
void SGUI_ScreenDraw3DFrame(uint16_t x, uint16_t y, uint16_t w, uint16_t h, STYLE_TYP_t style);
void SGUI_ScreenDraw3DCircle(uint16_t x, uint16_t y, uint16_t r, STYLE_TYP_t style);
void SGUI_ScreenDrawImage(UB_Image *img, uint16_t x, uint16_t y, uint16_t w, uint16_t h);




//--------------------------------------------------------------
// Globale Funktionen fuer den User
//--------------------------------------------------------------
void SGUI_ScreenDrawPixel(uint16_t xp, uint16_t yp, uint16_t c);
void SGUI_ScreenDrawLine(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint16_t c);
void SGUI_ScreenDrawFullRect(uint16_t xp, uint16_t yp, uint16_t w, uint16_t h, uint16_t c);
void SGUI_ScreenDrawFullCircle(uint16_t xp, uint16_t yp, uint16_t r, uint16_t c);
void SGUI_ScreenStraight(uint16_t xp, uint16_t yp, uint16_t l, LCD_DIR_t d, uint16_t c);
void SGUI_ScreenDrawString(uint16_t xp, uint16_t yp,char *ptr, UB_Font *font, uint16_t vg, uint16_t bg);




//--------------------------------------------------------------
#endif // __STM32F4_UB_SGUI_SCREEN_H
