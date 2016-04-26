//--------------------------------------------------------------
// File     : ub_sgui_gauge.h
//--------------------------------------------------------------

//--------------------------------------------------------------
#ifndef __STM32F4_UB_SGUI_GAUGE_H
#define __STM32F4_UB_SGUI_GAUGE_H


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
#define   MAX_GAUGE_ANZ        100


//--------------------------------------------------------------
// default einstellungen
//--------------------------------------------------------------
#define   SGAUGE_DEF_FRAME_SIZE  1
#define   SGAUGE_DEF_FRAME_COL   RGB_COL_BLACK
#define   SGAUGE_DEF_BACK_COL    SGUI_WINCOL


//--------------------------------------------------------------
// Typ vom Gauge
//--------------------------------------------------------------
typedef enum {
  SGAUGE_RL =0,      // right to left
  SGAUGE_LR,         // left to right
  SGAUGE_TB,         // top to bottom
  SGAUGE_BT          // bottom to top
}SGAUGE_TYP_t;


//--------------------------------------------------------------
// einzelne Bits vom status_byte
//--------------------------------------------------------------
#define   SGAUGE_BIT_REDRAW   0x80 // dont change


//--------------------------------------------------------------
// Struktur von einem Objekt
//--------------------------------------------------------------
typedef struct {
  uint16_t window_nr;      // nr vom window [1...n]  
  SGAUGE_TYP_t typ;        // typ vom Gauge
  STYLE_TYP_t style;       // styletyp
  uint16_t xpos;           // xpos (in pixel)
  uint16_t ypos;           // ypos (in pixel)
  uint16_t width;          // breite (in pixel)
  uint16_t height;         // hoehe (in pixel)
  uint8_t frame_size;      // rahmendicke (in pixel)
  uint16_t col_frame;      // farbe vom Rahmen
  uint16_t col_back;       // farbe vom Hintergrund
  int32_t min_value;       // minimum wert
  int32_t max_value;       // maximum wert
  int32_t akt_value;       // aktueller wert
  uint8_t status;          // internes status-byte (nicht aendern !!)
}SGAUGE_t;


//--------------------------------------------------------------
// interne Struktur, Liste aller Objekte
//--------------------------------------------------------------
typedef struct {
  bool update;                 // update flag
  uint16_t akt_anz;            // anzahl aller Objekte  
  SGAUGE_t* ptr[MAX_GAUGE_ANZ]; // liste aller Objekte
}SGUI_GAUGE_t;
SGUI_GAUGE_t SGUI_GAUGE;


//--------------------------------------------------------------
// interne Funktionen fuer SGUI
//--------------------------------------------------------------
void SGUI_GaugeInit(void);
void SGUI_GaugeUpdate(void);


//--------------------------------------------------------------
// Globale Funktionen fuer den User
//--------------------------------------------------------------
SGAUGE_t* SGUI_GaugeCreate(uint16_t x, uint16_t y, uint16_t w, uint16_t h);
void SGUI_GaugeSetTyp(SGAUGE_t* ptr, SGAUGE_TYP_t typ);
void SGUI_GaugeSetStyle(SGAUGE_t* ptr, STYLE_TYP_t style);
void SGUI_GaugeSetFrameSize(SGAUGE_t* ptr, uint8_t px);
void SGUI_GaugeSetColor(SGAUGE_t* ptr, uint16_t c1, uint16_t c2);
void SGUI_GaugeSetMinMax(SGAUGE_t* ptr, int32_t min, int32_t max);
void SGUI_GaugeSetValue(SGAUGE_t* ptr, int32_t value);
int32_t SGUI_GaugeGetValue(SGAUGE_t* ptr);


//--------------------------------------------------------------
#endif // __STM32F4_UB_SGUI_GAUGE_H
