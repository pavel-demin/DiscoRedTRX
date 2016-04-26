//--------------------------------------------------------------
// File     : ub_sgui_intedit.h
//--------------------------------------------------------------

//--------------------------------------------------------------
#ifndef __STM32F4_UB_SGUI_INTEDIT_H
#define __STM32F4_UB_SGUI_INTEDIT_H


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
#define   MAX_INTEDIT_ANZ        100 


//--------------------------------------------------------------
// default einstellungen
//--------------------------------------------------------------
#define   SINTEDIT_DEF_FRAME_SIZE    1
#define   SINTEDIT_DEF_TEXT_COL    RGB_COL_BLACK
#define   SINTEDIT_DEF_FRAME_COL   RGB_COL_BLACK
#define   SINTEDIT_DEF_BACK_COL    SGUI_WINCOL
#define   SINTEDIT_DEF_FONT        &Arial_10x15


//--------------------------------------------------------------
// Typ vom Intedit
//--------------------------------------------------------------
typedef enum {
  SINTEDIT_H =0,      // horizontal (up/down buttons)
  SINTEDIT_H2,        // horizontal (left/right buttons)
  SINTEDIT_V          // vertikal
}SINTEDIT_TYP_t;


//--------------------------------------------------------------
// Alignment vom Text
//--------------------------------------------------------------
typedef enum {
  SINTEDIT_ALIGN_CENTER =0,  // Mittig
  SINTEDIT_ALIGN_LEFT,       // links buendig
  SINTEDIT_ALIGN_RIGHT       // rechts buendig
}SINTEDIT_ALIGN_t; 

//--------------------------------------------------------------
// einzelne Bits vom status_byte
//--------------------------------------------------------------
#define   SINTEDIT_BIT_REDRAW   0x80 // dont change
#define   SINTEDIT_BIT_ARROW    0x40 // dont change
#define   SINTEDIT_BIT_PADDING  0x20 // dont change
#define   SINTEDIT_BIT_3DSTYLE  0x10 // dont change



//--------------------------------------------------------------
// Struktur von einem Objekt
//--------------------------------------------------------------
typedef struct {
  uint16_t window_nr;      // nr vom window [1...n]
  void (*fkt_ptr)(int32_t);   // pointer auf funktions-handler
  SINTEDIT_TYP_t typ;      // typ vom Intedit
  uint16_t xpos;           // xpos (in pixel)
  uint16_t ypos;           // ypos (in pixel)
  uint16_t width;          // breit (in pixel)
  uint16_t height;         // hoehe (in pixel)
  uint8_t frame_size;      // rahmendicke (in pixel)
  uint16_t col_text;       // farbe vom Text
  uint16_t col_frame;      // farbe vom Rahmen
  uint16_t col_back;       // farbe vom Hintergrund     
  int32_t i32_value;       // aktueller wert
  int32_t i32_minvalue;    // minimum wert
  int32_t i32_maxvalue;    // maximum wert
  uint16_t step_value;     // schrittweite
  uint8_t digits;          // anzahl der stellen  
  UB_Font *font;           // pointer auf einen Font
  SINTEDIT_ALIGN_t align;  // alignment vom Text
  uint8_t status;          // internes status-byte (nicht aendern !!)
}SINTEDIT_t;


//--------------------------------------------------------------
// interne Struktur, Liste aller Objekte
//--------------------------------------------------------------
typedef struct {
  bool update;                  // update flag
  uint16_t akt_anz;             // anzahl aller Objekte  
  SINTEDIT_t* ptr[MAX_INTEDIT_ANZ]; // liste aller Objekte
}SGUI_INTEDIT_t;
SGUI_INTEDIT_t SGUI_INTEDIT;


//----------Macro definition------------------------------------
#define WEAK __attribute__ ((weak))

//--------------------------------------------------------------
// interne Funktionen fuer SGUI
//--------------------------------------------------------------
void SGUI_IntEditInit(void);
bool SGUI_IntEditTouch(bool pressed, uint16_t x, uint16_t y, bool blocked);
void SGUI_IntEditUpdate(void);


//--------------------------------------------------------------
// Globale Funktionen fuer den User
//--------------------------------------------------------------
SINTEDIT_t* SGUI_IntEditCreate(uint16_t x, uint16_t y, uint16_t w, uint16_t h);
void SGUI_IntEditSetTyp(SINTEDIT_t* ptr, SINTEDIT_TYP_t typ);
void SGUI_IntEditSetStyle(SINTEDIT_t* ptr, STYLE_TYP_t style);
void SGUI_IntEditSetNumFormat(SINTEDIT_t* ptr, uint8_t digits, bool padding);
void SGUI_IntEditSetAlignment(SINTEDIT_t* ptr, SINTEDIT_ALIGN_t align); 
void SGUI_IntEditSetArrowVisible(SINTEDIT_t* ptr, bool visible);
void SGUI_IntEditSetFrameSize(SINTEDIT_t* ptr, uint8_t px);
void SGUI_IntEditSetColor(SINTEDIT_t* ptr, uint16_t c1, uint16_t c2, uint16_t c3);
void SGUI_IntEditSetFont(SINTEDIT_t* ptr, UB_Font* font);
void SGUI_IntEditSetMinMax(SINTEDIT_t* ptr, int32_t min, int32_t max);
void SGUI_IntEditSetStep(SINTEDIT_t* ptr, uint16_t step); 
void SGUI_IntEditSetValue(SINTEDIT_t* ptr, int32_t value);
int32_t SGUI_IntEditGetValue(SINTEDIT_t* ptr);
void SGUI_IntEditSetHandler(SINTEDIT_t* ptr, void *fkt_ptr);







//--------------------------------------------------------------
#endif // __STM32F4_UB_SGUI_INTEDIT_H
