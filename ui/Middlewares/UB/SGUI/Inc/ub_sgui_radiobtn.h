//--------------------------------------------------------------
// File     : ub_sgui_radiobtn.h
//--------------------------------------------------------------

//--------------------------------------------------------------
#ifndef __STM32F4_UB_SGUI_RADIOBTN_H
#define __STM32F4_UB_SGUI_RADIOBTN_H


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
#define   MAX_RBTN_ANZ        100


//--------------------------------------------------------------
// default einstellungen
//--------------------------------------------------------------
#define   SRBTN_DEF_FRAME_SIZE  1
#define   SRBTN_DEF_FRAME_COL   RGB_COL_BLACK
#define   SRBTN_DEF_BACK_COL    SGUI_WINCOL 


//--------------------------------------------------------------
// einzelne Bits vom status_byte
//--------------------------------------------------------------
#define   SRBTN_BIT_AKTIV    0x01 // dont change
#define   SRBTN_BIT_REDRAW   0x80 // dont change


//--------------------------------------------------------------
// Struktur von einem Objekt
//--------------------------------------------------------------
typedef struct {
  uint16_t window_nr;      // nr vom window [1...n]
  void (*fkt_ptr)(bool);   // pointer auf funktions-handler  
  uint16_t group_nr;       // nr der gruppe
  STYLE_TYP_t style;       // styletyp
  uint16_t xpos;           // xpos (in pixel)
  uint16_t ypos;           // ypos (in pixel)
  uint16_t size;           // groesse (in pixel)
  uint8_t frame_size;      // rahmendicke (in pixel)
  uint16_t col_frame;      // farbe vom Rahmen
  uint16_t col_back;       // farbe vom Hintergrund
  uint8_t status;          // internes status-byte (nicht aendern !!)
}SRBTN_t;



//--------------------------------------------------------------
// interne Struktur, Liste aller Objekte
//--------------------------------------------------------------
typedef struct {
  bool update;               // update flag
  uint16_t akt_anz;          // anzahl aller Objekte  
  SRBTN_t* ptr[MAX_RBTN_ANZ];  // liste aller Objekte
}SGUI_RBTN_t;
SGUI_RBTN_t SGUI_RBTN;


//----------Macro definition------------------------------------
#define WEAK __attribute__ ((weak))

//--------------------------------------------------------------
// interne Funktionen fuer SGUI
//--------------------------------------------------------------
void SGUI_RadioButtonInit(void);
bool SGUI_RadioButtonTouch(bool pressed, uint16_t x, uint16_t y, bool blocked);
void SGUI_RadioButtonUpdate(void);



//--------------------------------------------------------------
// Globale Funktionen fuer den User
//--------------------------------------------------------------
SRBTN_t* SGUI_RadioButtonCreate(uint16_t x, uint16_t y, uint16_t s);
void SGUI_RadioButtonSetGroup(SRBTN_t* ptr, uint16_t group_nr);
void SGUI_RadioButtonSetStyle(SRBTN_t* ptr, STYLE_TYP_t style);
void SGUI_RadioButtonSetFrameSize(SRBTN_t* ptr, uint8_t px);
void SGUI_RadioButtonSetColor(SRBTN_t* ptr, uint16_t c1, uint16_t c2);
void SGUI_RadioButtonSetAktiv(SRBTN_t* ptr);
void SGUI_RadioButtonSetHandler(SRBTN_t* ptr, void *fkt_ptr);
bool SGUI_RadioButtonIsAktiv(SRBTN_t* ptr);


//--------------------------------------------------------------
#endif // __STM32F4_UB_SGUI_RADIOBTN_H
