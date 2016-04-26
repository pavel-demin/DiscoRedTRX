//--------------------------------------------------------------
// File     : ub_sgui_graph.h
//--------------------------------------------------------------

//--------------------------------------------------------------
#ifndef __STM32F4_UB_SGUI_GRAPH_H
#define __STM32F4_UB_SGUI_GRAPH_H


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
#define   MAX_GRAPH_ANZ        100


//--------------------------------------------------------------
// Maximale anzahl der Kanäle pro Graph
//--------------------------------------------------------------
#define   MAX_GRAPHCH_ANZ         4

//--------------------------------------------------------------
// Maximale anzahl der Daten pro Kanal
//--------------------------------------------------------------
#define   MAX_GRAPHDATA_ANZ         400


//--------------------------------------------------------------
// Typ vom Datenarray
//--------------------------------------------------------------
typedef uint8_t* DataArray_t; 


//--------------------------------------------------------------
// default einstellungen
//--------------------------------------------------------------
#define   SGRAPH_DEF_BACK_COL    SGUI_WINCOL
#define   SGRAPH_DEF_FRAME_SIZE  1
#define   SGRAPH_DEF_FRAME_COL   RGB_COL_BLACK
#define   SGRAPH_DEF_CH_COL      RGB_COL_BLUE




//--------------------------------------------------------------
// einzelne Bits vom status_byte
//--------------------------------------------------------------
#define   SGRAPH_BIT_REDRAW   0x80 // dont change



//--------------------------------------------------------------
// Settings eines Kanals
//--------------------------------------------------------------
typedef struct {
  bool visible;            // sichtbar
  uint16_t col;            // farbe  
}SGRAPH_CH_t;

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
  uint8_t ch_cnt;          // anzahl der Kanäle
  uint16_t item_cnt;       // anzahl der Daten pro Kanal
  DataArray_t *data;       // pointer auf Datenarray
  SGRAPH_CH_t ch[MAX_GRAPHCH_ANZ];  // Settings der Kanäle
  uint8_t status;          // internes status-byte (nicht aendern !!)
}SGRAPH_t;


//--------------------------------------------------------------
// interne Struktur, Liste aller Objekte
//--------------------------------------------------------------
typedef struct {
  bool update;                  // update flag
  uint16_t akt_anz;             // anzahl aller Objekte  
  SGRAPH_t* ptr[MAX_GRAPH_ANZ]; // liste aller Objekte
}SGUI_GRAPH_t;
SGUI_GRAPH_t SGUI_GRAPH;



//--------------------------------------------------------------
// interne Funktionen fuer SGUI
//--------------------------------------------------------------
void SGUI_GraphInit(void);
void SGUI_GraphUpdate(void);




//--------------------------------------------------------------
// Globale Funktionen fuer den User
//--------------------------------------------------------------
SGRAPH_t* SGUI_GraphCreate(uint16_t x, uint16_t y, uint16_t w, uint16_t h);
void SGUI_GraphSetStyle(SGRAPH_t* ptr, STYLE_TYP_t style);
void SGUI_GraphSetFrameSize(SGRAPH_t* ptr, uint8_t px);
void SGUI_GraphSetColor(SGRAPH_t* ptr, uint16_t c1, uint16_t c2);
void SGUI_GraphCreateDataArray(SGRAPH_t* ptr, uint8_t ch, uint16_t cnt);
void SGUI_GraphSetCHValue(SGRAPH_t* ptr, uint8_t ch, uint16_t pos, uint8_t value);
uint8_t SGUI_GraphGetCHValue(SGRAPH_t* ptr, uint8_t ch, uint16_t pos);
void SGUI_GraphSetCHVisible(SGRAPH_t* ptr, uint8_t ch, bool visible);
void SGUI_GraphSetCHColor(SGRAPH_t* ptr, uint8_t ch, uint16_t c);





//--------------------------------------------------------------
#endif // __STM32F4_UB_SGUI_GRAPH_H
