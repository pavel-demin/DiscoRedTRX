//--------------------------------------------------------------
// File     : ub_sgui_led.h
//--------------------------------------------------------------

//--------------------------------------------------------------
#ifndef __STM32F4_UB_SGUI_LED_H
#define __STM32F4_UB_SGUI_LED_H


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
#define   MAX_LED_ANZ        100


//--------------------------------------------------------------
// default einstellungen
//--------------------------------------------------------------
#define   SLED_DEF_FRAME_SIZE  1
#define   SLED_DEF_FRAME_COL   RGB_COL_BLACK
#define   SLED_DEF_COL_ON      0x07E0
#define   SLED_DEF_COL_OFF     0x03E0



//--------------------------------------------------------------
// einzelne Bits vom status_byte
//--------------------------------------------------------------
#define   SLED_BIT_AKTIV    0x01 // dont change
#define   SLED_BIT_REDRAW   0x80 // dont change



//--------------------------------------------------------------
// Typ der LED
//--------------------------------------------------------------
typedef enum {
  SLED_ROUND =0,      // runde led
  SLED_RECT           // rechteckige led
}SLED_TYP_t;


//--------------------------------------------------------------
// Struktur von einem Objekt
//--------------------------------------------------------------
typedef struct {
  uint16_t window_nr;      // nr vom window [1...n]
  SLED_TYP_t typ;          // typ der led
  STYLE_TYP_t style;       // styletyp
  uint16_t xpos;           // xpos (in pixel)
  uint16_t ypos;           // ypos (in pixel)
  uint16_t size;           // groesse (in pixel)
  uint8_t frame_size;      // rahmendicke (in pixel)
  uint16_t col_frame;      // farbe vom Rahmen
  uint16_t col_off;        // farbe der LED (AUS)
  uint16_t col_on;         // farbe der LED (EIN)
  uint8_t status;          // internes status-byte (nicht aendern !!)
}SLED_t;



//--------------------------------------------------------------
// interne Struktur, Liste aller Objekte
//--------------------------------------------------------------
typedef struct {
  bool update;               // update flag
  uint16_t akt_anz;          // anzahl aller Objekte  
  SLED_t* ptr[MAX_LED_ANZ];  // liste aller Objekte
}SGUI_LED_t;
SGUI_LED_t SGUI_LED;


//--------------------------------------------------------------
// interne Funktionen fuer SGUI
//--------------------------------------------------------------
void SGUI_LedInit(void);
void SGUI_LedUpdate(void);


//--------------------------------------------------------------
// Globale Funktionen fuer den User
//--------------------------------------------------------------
SLED_t* SGUI_LedCreate(uint16_t x, uint16_t y, uint16_t s);
void SGUI_LedSetTyp(SLED_t* ptr, SLED_TYP_t typ); 
void SGUI_LedSetStyle(SLED_t* ptr, STYLE_TYP_t style);
void SGUI_LedOn(SLED_t* ptr);
void SGUI_LedOff(SLED_t* ptr);
void SGUI_LedToggle(SLED_t* ptr);
void SGUI_LedSetAktiv(SLED_t* ptr, bool aktiv);
void SGUI_LedSetFrameSize(SLED_t* ptr, uint8_t px);
void SGUI_LedSetColor(SLED_t* ptr, uint16_t c1, uint16_t c2, uint16_t c3);
bool SGUI_LedIsAktiv(SLED_t* ptr);


//--------------------------------------------------------------
#endif // __STM32F4_UB_SGUI_LED_H
