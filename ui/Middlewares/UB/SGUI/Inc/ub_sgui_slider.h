//--------------------------------------------------------------
// File     : ub_sgui_slider.h
//--------------------------------------------------------------

//--------------------------------------------------------------
#ifndef __STM32F4_UB_SGUI_SLIDER_H
#define __STM32F4_UB_SGUI_SLIDER_H


//--------------------------------------------------------------
// Includes
//--------------------------------------------------------------
#include "stm32_ub_system.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "ub_sgui_window.h"


//--------------------------------------------------------------
// Anzahl wieviele Objekte maximal erzeugt werden können
//--------------------------------------------------------------
#define   MAX_SLIDER_ANZ        100


//--------------------------------------------------------------
// default einstellungen
//--------------------------------------------------------------
#define   SSLIDER_DEF_FRAME_SIZE  1
#define   SSLIDER_DEF_FRAME_COL   RGB_COL_BLACK
#define   SSLIDER_DEF_BACK_COL    SGUI_WINCOL


//--------------------------------------------------------------
// Typ vom SLIDER
//--------------------------------------------------------------
typedef enum {
  SSLIDER_H =0,      // horizontal
  SSLIDER_V,         // vertikal
}SSLIDER_TYP_t;


//--------------------------------------------------------------
// einzelne Bits vom status_byte
//--------------------------------------------------------------
#define   SSLIDER_BIT_REDRAW   0x80 // dont change
#define   SSLIDER_BIT_ARROW    0x40  // dont change
#define   SSLIDER_BIT_3DSTYLE  0x20 // dont change


//--------------------------------------------------------------
// Struktur von einem Objekt
//--------------------------------------------------------------
typedef struct {
  uint16_t window_nr;      // nr vom window [1...n]  
  void (*fkt_ptr)(int32_t);   // pointer auf funktions-handler
  SSLIDER_TYP_t typ;       // typ vom SLIDER  
  uint16_t xpos;           // xpos (in pixel)
  uint16_t ypos;           // ypos (in pixel)
  uint16_t width;          // breite (in pixel)
  uint16_t height;         // hoehe (in pixel)
  uint8_t frame_size;      // rahmendicke (in pixel)
  uint16_t col_frame;      // farbe vom Rahmen
  uint16_t col_back;       // farbe vom Hintergrund
  int32_t min_value;       // minimum wert
  int32_t max_value;       // maximum wert
  int32_t akt_value;       // aktueller wert
  uint16_t step_value;     // schrittweite
  uint8_t status;          // internes status-byte (nicht aendern !!)
}SSLIDER_t;


//--------------------------------------------------------------
// interne Struktur, Liste aller Objekte
//--------------------------------------------------------------
typedef struct {
  bool update;                 // update flag
  uint16_t akt_anz;            // anzahl aller Objekte  
  SSLIDER_t* ptr[MAX_SLIDER_ANZ]; // liste aller Objekte
}SGUI_SLIDER_t;
SGUI_SLIDER_t SGUI_SLIDER;


//----------Macro definition------------------------------------
#define WEAK __attribute__ ((weak))


//--------------------------------------------------------------
// interne Funktionen fuer SGUI
//--------------------------------------------------------------
void SGUI_SliderInit(void);
bool SGUI_SliderTouch(bool pressed, uint16_t x, uint16_t y, bool blocked);
void SGUI_SliderUpdate(void);


//--------------------------------------------------------------
// Globale Funktionen fuer den User
//--------------------------------------------------------------
SSLIDER_t* SGUI_SliderCreate(uint16_t x, uint16_t y, uint16_t w, uint16_t h);
void SGUI_SliderSetTyp(SSLIDER_t* ptr, SSLIDER_TYP_t typ);
void SGUI_SliderSetStyle(SSLIDER_t* ptr, STYLE_TYP_t style);
void SGUI_SliderSetArrowVisible(SSLIDER_t* ptr, bool visible);
void SGUI_SliderSetFrameSize(SSLIDER_t* ptr, uint8_t px);
void SGUI_SliderSetColor(SSLIDER_t* ptr, uint16_t c1, uint16_t c2);
void SGUI_SliderSetMinMax(SSLIDER_t* ptr, int32_t min, int32_t max);
void SGUI_SliderSetStep(SSLIDER_t* ptr, uint16_t step);
void SGUI_SliderSetValue(SSLIDER_t* ptr, int32_t value);
int32_t SGUI_SliderGetValue(SSLIDER_t* ptr);
void SGUI_SliderSetHandler(SSLIDER_t* ptr, void *fkt_ptr);


//--------------------------------------------------------------
#endif // __STM32F4_UB_SGUI_SLIDER_H
