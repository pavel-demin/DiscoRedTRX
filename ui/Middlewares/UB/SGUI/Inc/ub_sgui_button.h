//--------------------------------------------------------------
// File     : ub_sgui_button.h
//--------------------------------------------------------------

//--------------------------------------------------------------
#ifndef __STM32F4_UB_SGUI_BUTTON_H
#define __STM32F4_UB_SGUI_BUTTON_H


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
#define   MAX_BTN_ANZ        100


//--------------------------------------------------------------
// default einstellungen
//--------------------------------------------------------------
#define   SBUTTON_DEF_FRAME_SIZE  1
#define   SBUTTON_DEF_FRAME_COL   RGB_COL_BLACK
#define   SBUTTON_DEF_BACK_COL    SGUI_WINCOL
#define   SBUTTON_DEF_FONT        &Arial_13x19


//--------------------------------------------------------------
// Mode vom Button
//--------------------------------------------------------------
typedef enum {
  SBUTTON_PUSH =0,      // Taster
  SBUTTON_PUSHPULL      // Schalter (rastend)
}SBUTTON_MODE_t;




//--------------------------------------------------------------
// einzelne Bits vom status_byte
//--------------------------------------------------------------
#define   SBTN_BIT_REDRAW   0x80 // dont change
#define   SBTN_BIT_3DSTYLE  0x40 // dont change
#define   SBTN_BIT_AKTIV    0x01 // dont change



//--------------------------------------------------------------
// Struktur von einem Objekt
//--------------------------------------------------------------
typedef struct {
  uint16_t window_nr;      // nr vom window [1...n]
  void (*fkt_ptr)(bool);   // pointer auf funktions-handler
  SBUTTON_MODE_t mode;     // mode vom Button
  uint16_t xpos;           // xpos (in pixel)
  uint16_t ypos;           // ypos (in pixel)
  uint16_t width;          // breite (in pixel)
  uint16_t height;         // hoehe (in pixel)
  uint8_t frame_size;      // rahmendicke (in pixel)
  uint16_t col_frame;      // farbe vom Rahmen
  uint16_t col_back;       // farbe vom Button
  char *txt_ptr;           // pointer auf einen Text
  UB_Font *font;           // pointer auf einen Font
  uint8_t status;          // internes status-byte (nicht aendern !!)
}SBUTTON_t;


//--------------------------------------------------------------
// interne Struktur, Liste aller Objekte
//--------------------------------------------------------------
typedef struct {
  bool update;                 // update flag
  uint16_t akt_anz;            // anzahl aller Objekte  
  SBUTTON_t* ptr[MAX_BTN_ANZ]; // liste aller Objekte
}SGUI_BUTTON_t;
SGUI_BUTTON_t SGUI_BUTTON;




//----------Macro definition------------------------------------
#define WEAK __attribute__ ((weak))

//--------------------------------------------------------------
// interne Funktionen fuer SGUI
//--------------------------------------------------------------
void SGUI_ButtonInit(void);
bool SGUI_ButtonTouch(bool pressed, uint16_t x, uint16_t y, bool blocked);
void SGUI_ButtonUpdate(void);




//--------------------------------------------------------------
// Globale Funktionen fuer den User
//--------------------------------------------------------------
SBUTTON_t* SGUI_ButtonCreate(uint16_t x, uint16_t y, uint16_t w, uint16_t h);
void SGUI_ButtonSetMode(SBUTTON_t* ptr, SBUTTON_MODE_t mode);
void SGUI_ButtonSetStyle(SBUTTON_t* ptr, STYLE_TYP_t style);
void SGUI_ButtonSetFrameSize(SBUTTON_t* ptr, uint8_t px);
void SGUI_ButtonSetColor(SBUTTON_t* ptr, uint16_t c1, uint16_t c2);
void SGUI_ButtonSetText(SBUTTON_t* ptr, char* txt);
void SGUI_ButtonSetFont(SBUTTON_t* ptr, UB_Font* font);
void SGUI_ButtonSetAktiv(SBUTTON_t* ptr, bool aktiv);
void SGUI_ButtonSetHandler(SBUTTON_t* ptr, void *fkt_ptr);
bool SGUI_ButtonIsAktiv(SBUTTON_t* ptr);








//--------------------------------------------------------------
#endif // __STM32F4_UB_SGUI_BUTTON_H
