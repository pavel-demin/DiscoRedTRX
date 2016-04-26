//--------------------------------------------------------------
// File     : ub_sgui_checkbox.h
//--------------------------------------------------------------

//--------------------------------------------------------------
#ifndef __STM32F4_UB_SGUI_CHKBOX_H
#define __STM32F4_UB_SGUI_CHKBOX_H


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
#define   MAX_CHECKBOX_ANZ        100


//--------------------------------------------------------------
// default einstellungen
//--------------------------------------------------------------
#define   SCHECKBOX_DEF_FRAME_SIZE  1
#define   SCHECKBOX_DEF_FRAME_COL   RGB_COL_BLACK
#define   SCHECKBOX_DEF_BACK_COL    SGUI_WINCOL


//--------------------------------------------------------------
// einzelne Bits vom status_byte
//--------------------------------------------------------------
#define   SCHECKBOX_BIT_AKTIV    0x01 // dont change
#define   SCHECKBOX_BIT_REDRAW   0x80 // dont change



//--------------------------------------------------------------
// Struktur von einem Objekt
//--------------------------------------------------------------
typedef struct {
  uint16_t window_nr;      // nr vom window [1...n]  
  void (*fkt_ptr)(bool);   // pointer auf funktions-handler
  STYLE_TYP_t style;       // styletyp
  uint16_t xpos;           // xpos (in pixel)
  uint16_t ypos;           // ypos (in pixel)
  uint16_t size;           // groesse (in pixel)
  uint8_t frame_size;      // rahmendicke (in pixel)
  uint16_t col_frame;      // farbe vom Rahmen
  uint16_t col_back;       // farbe vom Hintergrund  
  uint8_t status;          // internes status-byte (nicht aendern !!)
}SCHECKBOX_t;


//--------------------------------------------------------------
// interne Struktur, Liste aller Objekte
//--------------------------------------------------------------
typedef struct {
  bool update;               // update flag
  uint16_t akt_anz;          // anzahl aller Objekte 
  SCHECKBOX_t* ptr[MAX_CHECKBOX_ANZ];  // liste aller Objekte
}SGUI_CHECKBOX_t;
SGUI_CHECKBOX_t SGUI_CHECKBOX;


//----------Macro definition------------------------------------
#define WEAK __attribute__ ((weak))


//--------------------------------------------------------------
// interne Funktionen fuer SGUI
//--------------------------------------------------------------
void SGUI_CheckboxInit(void);
bool SGUI_CheckboxTouch(bool pressed, uint16_t x, uint16_t y, bool blocked);
void SGUI_CheckboxUpdate(void);


//--------------------------------------------------------------
// Globale Funktionen fuer den User
//--------------------------------------------------------------
SCHECKBOX_t* SGUI_CheckboxCreate(uint16_t x, uint16_t y, uint16_t s);
void SGUI_CheckboxSetStyle(SCHECKBOX_t* ptr, STYLE_TYP_t style);
void SGUI_CheckboxSetFrameSize(SCHECKBOX_t* ptr, uint8_t px);
void SGUI_CheckboxSetColor(SCHECKBOX_t* ptr, uint16_t c1, uint16_t c2);
void SGUI_CheckboxSetAktiv(SCHECKBOX_t* ptr, bool aktiv);
void SGUI_CheckboxSetHandler(SCHECKBOX_t* ptr, void *fkt_ptr);
bool SGUI_CheckboxIsAktiv(SCHECKBOX_t* ptr);


//--------------------------------------------------------------
#endif // __STM32F4_UB_SGUI_CHKBOX_H
