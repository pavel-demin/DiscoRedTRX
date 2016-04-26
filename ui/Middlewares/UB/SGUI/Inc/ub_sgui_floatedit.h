//--------------------------------------------------------------
// File     : ub_sgui_floatedit.h
//--------------------------------------------------------------

//--------------------------------------------------------------
#ifndef __STM32F4_UB_SGUI_FLOATEDIT_H
#define __STM32F4_UB_SGUI_FLOATEDIT_H


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
#define   MAX_FLOATEDIT_ANZ        100 


//--------------------------------------------------------------
// default einstellungen
//--------------------------------------------------------------
#define   SFLOATEDIT_DEF_FRAME_SIZE    1
#define   SFLOATEDIT_DEF_TEXT_COL    RGB_COL_BLACK
#define   SFLOATEDIT_DEF_FRAME_COL   RGB_COL_BLACK
#define   SFLOATEDIT_DEF_BACK_COL    SGUI_WINCOL
#define   SFLOATEDIT_DEF_FONT        &Arial_10x15


//--------------------------------------------------------------
// Typ vom Floatedit
//--------------------------------------------------------------
typedef enum {
  SFLOATEDIT_H =0,      // horizontal (up/down buttons)
  SFLOATEDIT_H2,        // horizontal (left/right buttons)
  SFLOATEDIT_V          // vertikal
}SFLOATEDIT_TYP_t;


//--------------------------------------------------------------
// Alignment vom Text
//--------------------------------------------------------------
typedef enum {
  SFLOATEDIT_ALIGN_CENTER =0,  // Mittig
  SFLOATEDIT_ALIGN_LEFT,       // links buendig
  SFLOATEDIT_ALIGN_RIGHT       // rechts buendig
}SFLOATEDIT_ALIGN_t; 

//--------------------------------------------------------------
// einzelne Bits vom status_byte
//--------------------------------------------------------------
#define   SFLOATEDIT_BIT_REDRAW   0x80 // dont change
#define   SFLOATEDIT_BIT_ARROW    0x40 // dont change
#define   SFLOATEDIT_BIT_PADDING  0x20 // dont change
#define   SFLOATEDIT_BIT_3DSTYLE  0x10 // dont change



//--------------------------------------------------------------
// Struktur von einem Objekt
//--------------------------------------------------------------
typedef struct {
  uint16_t window_nr;      // nr vom window [1...n]
  void (*fkt_ptr)(float);  // pointer auf funktions-handler
  SFLOATEDIT_TYP_t typ;    // typ vom Floatedit
  uint16_t xpos;           // xpos (in pixel)
  uint16_t ypos;           // ypos (in pixel)
  uint16_t width;          // breit (in pixel)
  uint16_t height;         // hoehe (in pixel)
  uint8_t frame_size;      // rahmendicke (in pixel)
  uint16_t col_text;       // farbe vom Text
  uint16_t col_frame;      // farbe vom Rahmen
  uint16_t col_back;       // farbe vom Hintergrund     
  float f_value;           // aktueller wert
  float f_minvalue;        // minimum wert
  float f_maxvalue;        // maximum wert
  float step_value;        // schrittweite
  uint8_t digits;          // anzahl der stellen  
  UB_Font *font;           // pointer auf einen Font
  SFLOATEDIT_ALIGN_t align;  // alignment vom Text
  uint8_t status;          // internes status-byte (nicht aendern !!)
}SFLOATEDIT_t;


//--------------------------------------------------------------
// interne Struktur, Liste aller Objekte
//--------------------------------------------------------------
typedef struct {
  bool update;                  // update flag
  uint16_t akt_anz;             // anzahl aller Objekte  
  SFLOATEDIT_t* ptr[MAX_FLOATEDIT_ANZ]; // liste aller Objekte
}SGUI_FLOATEDIT_t;
SGUI_FLOATEDIT_t SGUI_FLOATEDIT;


//----------Macro definition------------------------------------
#define WEAK __attribute__ ((weak))

//--------------------------------------------------------------
// interne Funktionen fuer SGUI
//--------------------------------------------------------------
void SGUI_FloatEditInit(void);
bool SGUI_FloatEditTouch(bool pressed, uint16_t x, uint16_t y, bool blocked);
void SGUI_FloatEditUpdate(void);


//--------------------------------------------------------------
// Globale Funktionen fuer den User
//--------------------------------------------------------------
SFLOATEDIT_t* SGUI_FloatEditCreate(uint16_t x, uint16_t y, uint16_t w, uint16_t h);
void SGUI_FloatEditSetTyp(SFLOATEDIT_t* ptr, SFLOATEDIT_TYP_t typ);
void SGUI_FloatEditSetStyle(SFLOATEDIT_t* ptr, STYLE_TYP_t style);
void SGUI_FloatEditSetNumFormat(SFLOATEDIT_t* ptr, uint8_t digits, bool padding);
void SGUI_FloatEditSetAlignment(SFLOATEDIT_t* ptr, SFLOATEDIT_ALIGN_t align); 
void SGUI_FloatEditSetArrowVisible(SFLOATEDIT_t* ptr, bool visible);
void SGUI_FloatEditSetFrameSize(SFLOATEDIT_t* ptr, uint8_t px);
void SGUI_FloatEditSetColor(SFLOATEDIT_t* ptr, uint16_t c1, uint16_t c2, uint16_t c3);
void SGUI_FloatEditSetFont(SFLOATEDIT_t* ptr, UB_Font* font);
void SGUI_FloatEditSetMinMax(SFLOATEDIT_t* ptr, float min, float max);
void SGUI_FloatEditSetStep(SFLOATEDIT_t* ptr, float step); 
void SGUI_FloatEditSetValue(SFLOATEDIT_t* ptr, float value);
float SGUI_FloatEditGetValue(SFLOATEDIT_t* ptr);
void SGUI_FloatEditSetHandler(SFLOATEDIT_t* ptr, void *fkt_ptr);







//--------------------------------------------------------------
#endif // __STM32F4_UB_SGUI_FLOATEDIT_H
