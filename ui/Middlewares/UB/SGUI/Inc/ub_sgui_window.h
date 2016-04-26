//--------------------------------------------------------------
// File     : ub_sgui_window.h
//--------------------------------------------------------------

//--------------------------------------------------------------
#ifndef __STM32F4_UB_SGUI_WIN_H
#define __STM32F4_UB_SGUI_WIN_H


//--------------------------------------------------------------
// Includes
//--------------------------------------------------------------
#include "stm32_ub_system.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "ub_sgui_screen.h"
#include "ub_sgui_text.h"



//--------------------------------------------------------------
// Anzahl wieviele Objekte maximal erzeugt werden können
//--------------------------------------------------------------
#define   MAX_WINDOW_ANZ        100



//--------------------------------------------------------------
// default einstellungen
//--------------------------------------------------------------
#define   SWINDOW_DEF_BACK_COL    SGUI_WINCOL



//--------------------------------------------------------------
// default einstellungen (Child)
//--------------------------------------------------------------
#define   SWINDOW_DEF_FRAME_SIZE  1
#define   SWINDOW_DEF_FRAME_COL   RGB_COL_BLACK



//--------------------------------------------------------------
// Typ vom Window
//--------------------------------------------------------------
typedef enum {
  SWINDOW_MAIN =0, // Main-Fenster
  SWINDOW_CHILD    // Child-Fenster
}SWIN_TYP_t;



//--------------------------------------------------------------
// Struktur von einem Objekt
//--------------------------------------------------------------
typedef struct {
  uint16_t window_nr;      // nr [1...n]
  SWIN_TYP_t typ;          // typ vom window
  STYLE_TYP_t style;       // styletyp (bei child)
  uint16_t xpos;           // xpos (in pixel)
  uint16_t ypos;           // ypos (in pixel)
  uint16_t width;          // breit (in pixel)
  uint16_t height;         // hoehe (in pixel)
  uint8_t frame_size;      // rahmendicke (bei child)
  uint16_t col_frame;      // farbe vom Rahmen (bei child)
  uint16_t col_back;       // farbe vom Hintergrund
  uint8_t status;          // internes status-byte (nicht aendern !!)
}SWINDOW_t;


//--------------------------------------------------------------
// interne Struktur, Liste aller Objekte
//--------------------------------------------------------------
typedef struct {  
  uint16_t aktiv_window;            // nr vom aktiven window
  uint16_t old_window;              // nr vom zuletzt aktiven Window
  SWINDOW_t* akt_win_ptr;           // aktueller Pointer auf Window
  bool update_window;               // update flag (aktuelles window)
  bool update_all;                  // update aller objekte
  uint8_t update_parent_mode;       // update (parent window)
  uint16_t akt_anz;                 // anzahl aller Objekte
  SWINDOW_t* ptr[MAX_WINDOW_ANZ];   // liste aller Objekte
  char buf[40];                     // buffer für strings (max 40 Zeichen pro Zeile)
  uint32_t ram_used;                // speicherbedarf (in bytes) 
}SGUI_WINDOW_t;
SGUI_WINDOW_t SGUI_WINDOW;



//--------------------------------------------------------------
// interne Funktionen fuer SGUI
//--------------------------------------------------------------
void SGUI_WindowInit(void);
void SGUI_WindowUpdate(void);
void SGUI_WindowFormatInt(int32_t value, uint8_t digits, bool padding);
void SGUI_WindowFormatFloat(float value, uint8_t digits, bool padding);



//--------------------------------------------------------------
// Globale Funktionen fuer den User
//--------------------------------------------------------------
SWINDOW_t* SGUI_WindowCreateMain(uint16_t window_nr);
SWINDOW_t* SGUI_WindowCreateChild(uint16_t window_nr, uint16_t x, uint16_t y, uint16_t w, uint16_t h);
void SGUI_WindowSetStyle(SWINDOW_t* ptr, STYLE_TYP_t style);
void SGUI_WindowSetFrameSize(SWINDOW_t* ptr, uint8_t px);
void SGUI_WindowSetColor(SWINDOW_t* ptr, uint16_t c1, uint16_t c2);
void SGUI_WindowShow(uint16_t window_nr);
void SGUI_WindowShowPrev(void);
uint16_t SGUI_WindowGetActivNr(void);



//--------------------------------------------------------------
#endif // __STM32F4_UB_SGUI_WIN_H
