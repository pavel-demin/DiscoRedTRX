//--------------------------------------------------------------
// File     : ub_sgui_label.h
//--------------------------------------------------------------

//--------------------------------------------------------------
#ifndef __STM32F4_UB_SGUI_LABEL_H
#define __STM32F4_UB_SGUI_LABEL_H


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
#define   MAX_LABEL_ANZ        100


//--------------------------------------------------------------
// default einstellungen
//--------------------------------------------------------------
#define   SLABEL_DEF_TEXT_COL    RGB_COL_BLACK
#define   SLABEL_DEF_BACK_COL    SGUI_WINCOL
#define   SLABEL_DEF_FONT        &Arial_13x19
#define   SLABEL_DEF_FRAME_SIZE  1
#define   SLABEL_DEF_FRAME_COL   RGB_COL_BLACK



//--------------------------------------------------------------
// einzelne Bits vom status_byte
//--------------------------------------------------------------
#define   SLABEL_BIT_REDRAW   0x80 // dont change
#define   SLABEL_BIT_PANEL    0x40 // dont change 
#define   SLABEL_BIT_PADDING  0x20 // dont change



//--------------------------------------------------------------
// Typ vom Label
//--------------------------------------------------------------
typedef enum {
  SLABEL_ASCII =0, // Ascii-Zeichen
  SLABEL_I32,      // integer 32bit
  SLABEL_FLOAT     // float
}SLABEL_TYP_t;


//--------------------------------------------------------------
// Alignment vom Text
//--------------------------------------------------------------
typedef enum {
  SLABEL_ALIGN_CENTER =0,  // Mittig
  SLABEL_ALIGN_LEFT,       // links buendig
  SLABEL_ALIGN_RIGHT       // rechts buendig
}SLABEL_ALIGN_t;



//--------------------------------------------------------------
// Struktur von einem Objekt
//--------------------------------------------------------------
typedef struct {
  uint16_t window_nr;      // nr vom window [1...n]
  STYLE_TYP_t style;       // styletyp
  SLABEL_TYP_t typ;        // typ vom Label
  uint16_t xpos;           // xpos (in pixel)
  uint16_t ypos;           // ypos (in pixel)
  uint16_t width;          // breit (in pixel)
  uint16_t height;         // hoehe (in pixel)
  uint8_t frame_size;      // rahmendicke
  uint16_t col_frame;      // farbe vom Rahmen  
  uint16_t col_text;       // farbe vom Text
  uint16_t col_back;       // farbe vom Hintergrund
  char *txt_ptr;           // pointer auf einen Text (falls SLABEL_ASCII)
  int32_t i32_value;       // value (falls SLABEL_I32)
  float f_value;           // value (falls SLABEL_FLOAT)
  uint8_t digits;          // anzahl der stellen (falls SLABEL_I32 oder SLABEL_FLOAT)
  UB_Font *font;           // pointer auf einen Font
  SLABEL_ALIGN_t align;    // alignment vom Text
  uint8_t status;          // internes status-byte (nicht aendern !!)
}SLABEL_t;


//--------------------------------------------------------------
// interne Struktur, Liste alle Objekte
//--------------------------------------------------------------
typedef struct {
  bool update;                   // update flag
  uint16_t akt_anz;              // anzahl aller Objekte  
  SLABEL_t* ptr[MAX_LABEL_ANZ];  // liste aller Objekte
}SGUI_LABEL_t;
SGUI_LABEL_t SGUI_LABEL;


//--------------------------------------------------------------
// interne Funktionen fuer SGUI
//--------------------------------------------------------------
void SGUI_LabelInit(void);
void SGUI_LabelUpdate(void);


//--------------------------------------------------------------
// Globale Funktionen fuer den User
//--------------------------------------------------------------
SLABEL_t* SGUI_LabelCreate(uint16_t x, uint16_t y, uint16_t w, uint16_t h);
void SGUI_LabelSetStyle(SLABEL_t* ptr, STYLE_TYP_t style);
void SGUI_LabelSetPanelVisible(SLABEL_t* ptr, bool visible);
void SGUI_LabelSetFrameSize(SLABEL_t* ptr, uint8_t px);
void SGUI_LabelSetColor(SLABEL_t* ptr, uint16_t c1, uint16_t c2, uint16_t c3);
void SGUI_LabelSetText(SLABEL_t* ptr, char* txt);
void SGUI_LabelSetInt(SLABEL_t* ptr, int32_t value);
void SGUI_LabelSetFloat(SLABEL_t* ptr, float value);
void SGUI_LabelSetNumFormat(SLABEL_t* ptr, uint8_t digits, bool padding);
void SGUI_LabelSetFont(SLABEL_t* ptr, UB_Font* font);
void SGUI_LabelSetAlignment(SLABEL_t* ptr, SLABEL_ALIGN_t align);


//--------------------------------------------------------------
#endif // __STM32F4_UB_SGUI_LABEL_H
