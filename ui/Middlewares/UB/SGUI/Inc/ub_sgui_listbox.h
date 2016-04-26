//--------------------------------------------------------------
// File     : ub_sgui_listbox.h
//--------------------------------------------------------------

//--------------------------------------------------------------
#ifndef __STM32F4_UB_SGUI_LISTBOX_H
#define __STM32F4_UB_SGUI_LISTBOX_H


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
#define   MAX_LISTBOX_ANZ        100 


//--------------------------------------------------------------
// default einstellungen
//--------------------------------------------------------------
#define   SLISTBOX_DEF_FRAME_SIZE   1
#define   SLISTBOX_DEF_SLIDER_SIZE  20
#define   SLISTBOX_DEF_TEXT_COL     RGB_COL_BLACK
#define   SLISTBOX_DEF_FRAME_COL    RGB_COL_BLACK
#define   SLISTBOX_DEF_BACK_COL     SGUI_WINCOL
#define   SLISTBOX_DEF_FONT         &Arial_10x15



//--------------------------------------------------------------
// einzelne Bits vom status_byte
//--------------------------------------------------------------
#define   SLISTBOX_BIT_REDRAW   0x80 // dont change
#define   SLISTBOX_BIT_SLIDER   0x40 // dont change
#define   SLISTBOX_BIT_3DSTYLE  0x20 // dont change


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
  int16_t item_aktiv;      // aktives item
  uint8_t slider_size;     // groesse vom slider
  int16_t item_first;      // erstes sichtbares item
  UB_Font *font;           // pointer auf einen Font
  uint8_t status;          // internes status-byte (nicht aendern !!)
}SLISTBOX_t;


//--------------------------------------------------------------
// interne Struktur, Liste aller Objekte
//--------------------------------------------------------------
typedef struct {
  bool update;                  // update flag
  uint16_t akt_anz;             // anzahl aller Objekte  
  SLISTBOX_t* ptr[MAX_LISTBOX_ANZ]; // liste aller Objekte
}SGUI_LISTBOX_t;
SGUI_LISTBOX_t SGUI_LISTBOX;


//----------Macro definition------------------------------------
#define WEAK __attribute__ ((weak))

//--------------------------------------------------------------
// interne Funktionen fuer SGUI
//--------------------------------------------------------------
void SGUI_ListboxInit(void);
bool SGUI_ListboxTouch(bool pressed, uint16_t x, uint16_t y, bool blocked);
void SGUI_ListboxUpdate(void);


//--------------------------------------------------------------
// Globale Funktionen fuer den User
//--------------------------------------------------------------
SLISTBOX_t* SGUI_ListboxCreate(uint16_t x, uint16_t y, uint16_t w, uint16_t h);
void SGUI_ListboxSetStyle(SLISTBOX_t* ptr, STYLE_TYP_t style);
void SGUI_ListboxSetFrameSize(SLISTBOX_t* ptr, uint8_t px);
void SGUI_ListboxSetSliderSize(SLISTBOX_t* ptr, uint8_t px);
void SGUI_ListboxSetSliderVisible(SLISTBOX_t* ptr, bool visible);
void SGUI_ListboxSetColor(SLISTBOX_t* ptr, uint16_t c1, uint16_t c2, uint16_t c3);
void SGUI_ListboxSetFont(SLISTBOX_t* ptr, UB_Font* font);
void SGUI_ListboxAddItem(SLISTBOX_t* ptr, char *txt);
void SGUI_ListboxDeleteItem(SLISTBOX_t* ptr, uint16_t pos);
void SGUI_ListboxInsertItem(SLISTBOX_t* ptr, uint16_t pos, char *txt);
void SGUI_ListboxSetItem(SLISTBOX_t* ptr, uint16_t pos, char *txt);
char* SGUI_ListboxGetItem(SLISTBOX_t* ptr, uint16_t pos);
uint16_t SGUI_ListboxGetItemCnt(SLISTBOX_t* ptr);
void SGUI_ListboxSetAktivItemNr(SLISTBOX_t* ptr, int16_t pos);
int16_t SGUI_ListboxGetAktivItemNr(SLISTBOX_t* ptr);
void SGUI_ListboxSetFirstItemNr(SLISTBOX_t* ptr, uint16_t pos);
void SGUI_ListboxSetHandler(SLISTBOX_t* ptr, void *fkt_ptr);







//--------------------------------------------------------------
#endif // __STM32F4_UB_SGUI_LISTBOX_H
