//--------------------------------------------------------------
// File     : ub_sgui_panel.h
//--------------------------------------------------------------

//--------------------------------------------------------------
#ifndef __STM32F4_UB_SGUI_PANEL_H
#define __STM32F4_UB_SGUI_PANEL_H


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
#define   MAX_PANEL_ANZ        100



//--------------------------------------------------------------
// default einstellungen
//--------------------------------------------------------------
#define   SPANEL_DEF_BACK_COL    SGUI_WINCOL
#define   SPANEL_DEF_FRAME_SIZE  1
#define   SPANEL_DEF_FRAME_COL   RGB_COL_BLACK



//--------------------------------------------------------------
// einzelne Bits vom status_byte
//--------------------------------------------------------------
#define   SPANEL_BIT_REDRAW   0x80 // dont change



//--------------------------------------------------------------
// Struktur von einem Objekt
//--------------------------------------------------------------
typedef struct {
  uint16_t window_nr;      // nr vom window [1...n]
  STYLE_TYP_t style;       // styletyp
  uint16_t xpos;           // xpos (in pixel)
  uint16_t ypos;           // ypos (in pixel)
  uint16_t width;          // breit (in pixel)
  uint16_t height;         // hoehe (in pixel)
  uint8_t frame_size;      // rahmendicke
  uint16_t col_frame;      // farbe vom Rahmen
  uint16_t col_back;       // farbe vom Hintergrund
  uint8_t status;          // internes status-byte (nicht aendern !!)
}SPANEL_t;


//--------------------------------------------------------------
// interne Struktur, Liste aller Objekte
//--------------------------------------------------------------
typedef struct {
  bool update;                  // update flag
  uint16_t akt_anz;             // anzahl aller Objekte  
  SPANEL_t* ptr[MAX_PANEL_ANZ]; // liste aller Objekte
}SGUI_PANEL_t;
SGUI_PANEL_t SGUI_PANEL;



//--------------------------------------------------------------
// interne Funktionen fuer SGUI
//--------------------------------------------------------------
void SGUI_PanelInit(void);
void SGUI_PanelUpdate(void);




//--------------------------------------------------------------
// Globale Funktionen fuer den User
//--------------------------------------------------------------
SPANEL_t* SGUI_PanelCreate(uint16_t x, uint16_t y, uint16_t w, uint16_t h);
void SGUI_PanelSetStyle(SPANEL_t* ptr, STYLE_TYP_t style);
void SGUI_PanelSetFrameSize(SPANEL_t* ptr, uint8_t px);
void SGUI_PanelSetColor(SPANEL_t* ptr, uint16_t c1, uint16_t c2);





//--------------------------------------------------------------
#endif // __STM32F4_UB_SGUI_PANEL_H
