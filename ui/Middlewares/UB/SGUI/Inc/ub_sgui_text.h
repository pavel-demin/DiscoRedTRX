//--------------------------------------------------------------
// File     : ub_sgui_text.h
//--------------------------------------------------------------

//--------------------------------------------------------------
#ifndef __STM32F4_UB_SGUI_TEXT_H
#define __STM32F4_UB_SGUI_TEXT_H


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
#define   MAX_TEXT_ANZ        100


//--------------------------------------------------------------
// default einstellungen
//--------------------------------------------------------------
#define   STEXT_DEF_TEXT_COL    RGB_COL_BLACK
#define   STEXT_DEF_BACK_COL    SGUI_WINCOL
#define   STEXT_DEF_FONT        &Arial_10x15



//--------------------------------------------------------------
// einzelne Bits vom status_byte
//--------------------------------------------------------------
#define   STEXT_BIT_REDRAW   0x80 // dont change
#define   STEXT_BIT_PADDING  0x40 // dont change


//--------------------------------------------------------------
// Typ vom Text
//--------------------------------------------------------------
typedef enum {
  STEXT_ASCII =0, // Ascii-Zeichen
  STEXT_I32,      // integer 32bit
  STEXT_FLOAT     // float
}STEXT_TYP_t;


//--------------------------------------------------------------
// Struktur von einem Objekt
//--------------------------------------------------------------
typedef struct {
  uint16_t window_nr;      // nr vom window [1...n]
  STEXT_TYP_t typ;         // typ vom Text
  uint16_t xpos;           // xpos (in pixel)
  uint16_t ypos;           // ypos (in pixel)
  uint16_t col_text;       // farbe vom Text
  uint16_t col_back;       // farbe vom Hintergrund
  char *txt_ptr;           // pointer auf einen Text (falls STEXT_ASCII)
  int32_t i32_value;       // value (falls STEXT_I32)
  float f_value;           // value (falls STEXT_FLOAT)
  uint8_t digits;          // anzahl der stellen (falls STEXT_I32 oder STEXT_FLOAT)
  UB_Font *font;           // pointer auf einen Font
  uint8_t status;          // internes status-byte (nicht aendern !!)
}STEXT_t;


//--------------------------------------------------------------
// interne Struktur (aktuelle Einstellungen)
//--------------------------------------------------------------
typedef struct {
  uint16_t cursor_oldx;    // alte xpos (fuer linefeed)
  uint16_t cursor_x;       // xpos
  uint16_t cursor_y;       // ypos
  uint16_t col_text;       // farbe vom Text
  uint16_t col_back;       // farbe vom Hintergrund  
  UB_Font *font;           // default font
}STXT_DEF_t;


//--------------------------------------------------------------
// interne Struktur, Liste aller Objekte
//--------------------------------------------------------------
typedef struct {
  bool update;                  // update flag  
  uint16_t akt_anz;             // anzahl aller Objekte  
  STEXT_t* ptr[MAX_TEXT_ANZ];   // liste aller Objekte  
  STXT_DEF_t akt;               // aktuelle settings  
}SGUI_TEXT_t;
SGUI_TEXT_t SGUI_TEXT;

//--------------------------------------------------------------
// interne Funktionen fuer SGUI
//--------------------------------------------------------------
void SGUI_TextInit(void);
void SGUI_TextUpdate(void);




//--------------------------------------------------------------
// Globale Funktionen fuer den User
//--------------------------------------------------------------
void SGUI_TextSetCursor(uint16_t x, uint16_t y);
void SGUI_TextCursorLinefeed(void);
void SGUI_TextSetDefFont(UB_Font *font);
void SGUI_TextSetDefColor(uint16_t c1, uint16_t c2);
void SGUI_TextPrintString(char *txt_ptr);
void SGUI_TextPrintInt(int32_t value, uint8_t digits, bool padding);
void SGUI_TextPrintFloat(float value, uint8_t digits, bool padding);
//--------------------------------------------------------------
STEXT_t* SGUI_TextCreateString(char *txt_ptr);
void SGUI_TextSetString(STEXT_t* ptr, char *txt_ptr);
//--------------------------------------------------------------
STEXT_t* SGUI_TextCreateInt(int32_t value, uint8_t digits, bool padding);
void SGUI_TextSetInt(STEXT_t* ptr, int32_t value);
//--------------------------------------------------------------
STEXT_t* SGUI_TextCreateFloat(float value, uint8_t digits, bool padding);
void SGUI_TextSetFloat(STEXT_t* ptr, float value);
//--------------------------------------------------------------
void SGUI_TextSetColor(STEXT_t* ptr, uint16_t c1, uint16_t c2);
void SGUI_TextSetFont(STEXT_t* ptr, UB_Font* font);



//--------------------------------------------------------------
#endif // __STM32F4_UB_SGUI_TEXT_H
