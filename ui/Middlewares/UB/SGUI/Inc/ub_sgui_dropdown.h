//--------------------------------------------------------------
// File     : ub_sgui_dropdown.h
//--------------------------------------------------------------

//--------------------------------------------------------------
#ifndef __STM32F4_UB_SGUI_DROPDOWN_H
#define __STM32F4_UB_SGUI_DROPDOWN_H


//--------------------------------------------------------------
// Includes
//--------------------------------------------------------------
#include "stm32_ub_system.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "ub_sgui_window.h"
#include "ub_sgui_stringlist.h"







//--------------------------------------------------------------
// Anzahl wieviele Objekte maximal erzeugt werden können
//--------------------------------------------------------------
#define   MAX_DROPDOWN_ANZ        100 


//--------------------------------------------------------------
// default einstellungen
//--------------------------------------------------------------
#define   SDROPDOWN_DEF_FRAME_SIZE    1
#define   SDROPDOWN_DEF_TEXT_COL    RGB_COL_BLACK
#define   SDROPDOWN_DEF_FRAME_COL   RGB_COL_BLACK
#define   SDROPDOWN_DEF_BACK_COL    SGUI_WINCOL
#define   SDROPDOWN_DEF_FONT        &Arial_10x15



//--------------------------------------------------------------
// einzelne Bits vom status_byte
//--------------------------------------------------------------
#define   SDROPDOWN_BIT_REDRAW   0x80  // dont change
#define   SDROPDOWN_BIT_CHILD    0x40  // dont change
#define   SDROPDOWN_BIT_SIZE     0x20  // dont change
#define   SDROPDOWN_BIT_ARROW    0x10  // dont change
#define   SDROPDOWN_BIT_EXPAND   0x08  // dont change
#define   SDROPDOWN_BIT_SLIDER   0x04  // dont change
#define   SDROPDOWN_BIT_3DSTYLE  0x02  // dont change


//--------------------------------------------------------------
// Struktur von einem Objekt
//--------------------------------------------------------------
typedef struct {
  uint16_t window_nr;      // nr vom window [1...n]
  void (*fkt_ptr)(uint16_t);   // pointer auf funktions-handler
  uint16_t xpos;           // xpos (in pixel)
  uint16_t ypos;           // ypos (in pixel)
  uint16_t width;          // breit (in pixel)
  uint16_t height;         // hoehe (in pixel)
  uint8_t frame_size;      // rahmendicke (in pixel)
  uint16_t col_text;       // farbe vom Text
  uint16_t col_frame;      // farbe vom Rahmen
  uint16_t col_back;       // farbe vom Hintergrund  
  StringList_t *strlist;   // pointer auf stringlist
  uint16_t item_cnt;       // anzahl der items
  uint16_t max_ypos;       // max ypos
  uint16_t exp_height;     // expandierte height
  int16_t item_aktiv;      // aktives item
  int16_t item_first;      // erstes sichtbares item
  UB_Font *font;           // pointer auf einen Font
  uint8_t status;          // internes status-byte (nicht aendern !!)
}SDROPDOWN_t;


//--------------------------------------------------------------
// interne Struktur, Liste aller Objekte
//--------------------------------------------------------------
typedef struct {
  bool update;                  // update flag
  uint16_t akt_anz;             // anzahl aller Objekte  
  SDROPDOWN_t* ptr[MAX_DROPDOWN_ANZ]; // liste aller Objekte
}SGUI_DROPDOWN_t;
SGUI_DROPDOWN_t SGUI_DROPDOWN;

//----------Macro definition------------------------------------
#define WEAK __attribute__ ((weak))

//--------------------------------------------------------------
// interne Funktionen fuer SGUI
//--------------------------------------------------------------
void SGUI_DropdownInit(void);
bool SGUI_DropdownTouch(bool pressed, uint16_t x, uint16_t y, bool blocked);
void SGUI_DropdownUpdate(void);


//--------------------------------------------------------------
// Globale Funktionen fuer den User
//--------------------------------------------------------------
SDROPDOWN_t* SGUI_DropdownCreate(uint16_t x, uint16_t y, uint16_t w, uint16_t h);
void SGUI_DropdownSetStyle(SDROPDOWN_t* ptr, STYLE_TYP_t style);
void SGUI_DropdownSetArrowVisible(SDROPDOWN_t* ptr, bool visible);
void SGUI_DropdownSetFrameSize(SDROPDOWN_t* ptr, uint8_t px);
void SGUI_DropdownSetColor(SDROPDOWN_t* ptr, uint16_t c1, uint16_t c2, uint16_t c3);
void SGUI_DropdownSetFont(SDROPDOWN_t* ptr, UB_Font* font);
void SGUI_DropdownAddItem(SDROPDOWN_t* ptr, char *txt);
void SGUI_DropdownDeleteItem(SDROPDOWN_t* ptr, uint16_t pos);
void SGUI_DropdownInsertItem(SDROPDOWN_t* ptr, uint16_t pos, char *txt);
void SGUI_DropdownSetItem(SDROPDOWN_t* ptr, uint16_t pos, char *txt);
char* SGUI_DropdownGetItem(SDROPDOWN_t* ptr, uint16_t pos);
uint16_t SGUI_DropdownGetItemCnt(SDROPDOWN_t* ptr);
void SGUI_DropdownSetAktivItemNr(SDROPDOWN_t* ptr, uint16_t pos);
int16_t SGUI_DropdownGetAktivItemNr(SDROPDOWN_t* ptr);
void SGUI_DropdownSetExpand(SDROPDOWN_t* ptr, bool expand);
void SGUI_DropdownSetHandler(SDROPDOWN_t* ptr, void *fkt_ptr);







//--------------------------------------------------------------
#endif // __STM32F4_UB_SGUI_DROPDOWN_H
