//--------------------------------------------------------------
// File     : ub_sgui_selectbtn.h
//--------------------------------------------------------------

//--------------------------------------------------------------
#ifndef __STM32F4_UB_SGUI_SELECTBTN_H
#define __STM32F4_UB_SGUI_SELECTBTN_H


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
#define   MAX_SELECTBTN_ANZ        100 


//--------------------------------------------------------------
// default einstellungen
//--------------------------------------------------------------
#define   SSELECTBTN_DEF_FRAME_SIZE    1
#define   SSELECTBTN_DEF_TEXT_COL    RGB_COL_BLACK
#define   SSELECTBTN_DEF_FRAME_COL   RGB_COL_BLACK
#define   SSELECTBTN_DEF_BACK_COL    SGUI_WINCOL
#define   SSELECTBTN_DEF_FONT        &Arial_10x15


//--------------------------------------------------------------
// Typ vom Selectbutton
//--------------------------------------------------------------
typedef enum {
  SSELECTBTN_H =0,      // horizontal
  SSELECTBTN_V          // vertikal
}SSELECTBTN_TYP_t;



//--------------------------------------------------------------
// einzelne Bits vom status_byte
//--------------------------------------------------------------
#define   SSELECTBTN_BIT_REDRAW    0x80 // dont change
#define   SSELECTBTN_BIT_ARROW     0x40 // dont change
#define   SSELECTBTN_BIT_3DSTYLE   0x20 // dont change


//--------------------------------------------------------------
// Struktur von einem Objekt
//--------------------------------------------------------------
typedef struct {
  uint16_t window_nr;      // nr vom window [1...n]
  void (*fkt_ptr)(uint16_t);   // pointer auf funktions-handler
  SSELECTBTN_TYP_t typ;    // typ vom Selectbutton
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
  UB_Font *font;           // pointer auf einen Font
  uint8_t status;          // internes status-byte (nicht aendern !!)
}SSELECTBTN_t;


//--------------------------------------------------------------
// interne Struktur, Liste aller Objekte
//--------------------------------------------------------------
typedef struct {
  bool update;                  // update flag
  uint16_t akt_anz;             // anzahl aller Objekte  
  SSELECTBTN_t* ptr[MAX_SELECTBTN_ANZ]; // liste aller Objekte
}SGUI_SELECTBTN_t;
SGUI_SELECTBTN_t SGUI_SELECTBTN;


//----------Macro definition------------------------------------
#define WEAK __attribute__ ((weak))

//--------------------------------------------------------------
// interne Funktionen fuer SGUI
//--------------------------------------------------------------
void SGUI_SelectButtonInit(void);
bool SGUI_SelectButtonTouch(bool pressed, uint16_t x, uint16_t y, bool blocked);
void SGUI_SelectButtonUpdate(void);


//--------------------------------------------------------------
// Globale Funktionen fuer den User
//--------------------------------------------------------------
SSELECTBTN_t* SGUI_SelectButtonCreate(uint16_t x, uint16_t y, uint16_t w, uint16_t h);
void SGUI_SelectButtonSetTyp(SSELECTBTN_t* ptr, SSELECTBTN_TYP_t typ);
void SGUI_SelectButtonSetStyle(SSELECTBTN_t* ptr, STYLE_TYP_t style);
void SGUI_SelectButtonSetArrowVisible(SSELECTBTN_t* ptr, bool visible);
void SGUI_SelectButtonSetFrameSize(SSELECTBTN_t* ptr, uint8_t px);
void SGUI_SelectButtonSetColor(SSELECTBTN_t* ptr, uint16_t c1, uint16_t c2, uint16_t c3);
void SGUI_SelectButtonSetFont(SSELECTBTN_t* ptr, UB_Font* font);
void SGUI_SelectButtonAddItem(SSELECTBTN_t* ptr, char *txt);
void SGUI_SelectButtonDeleteItem(SSELECTBTN_t* ptr, uint16_t pos);
void SGUI_SelectButtonInsertItem(SSELECTBTN_t* ptr, uint16_t pos, char *txt);
void SGUI_SelectButtonSetItem(SSELECTBTN_t* ptr, uint16_t pos, char *txt);
char* SGUI_SelectButtonGetItem(SSELECTBTN_t* ptr, uint16_t pos);
uint16_t SGUI_SelectButtonGetItemCnt(SSELECTBTN_t* ptr);
void SGUI_SelectButtonSetAktivItemNr(SSELECTBTN_t* ptr, uint16_t pos);
int16_t SGUI_SelectButtonGetAktivItemNr(SSELECTBTN_t* ptr);
void SGUI_SelectButtonSetHandler(SSELECTBTN_t* ptr, void *fkt_ptr);








//--------------------------------------------------------------
#endif // __STM32F4_UB_SGUI_SELECTBTN_H
