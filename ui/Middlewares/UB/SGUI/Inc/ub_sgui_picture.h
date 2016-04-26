//--------------------------------------------------------------
// File     : ub_sgui_picture.h
//--------------------------------------------------------------

//--------------------------------------------------------------
#ifndef __STM32F4_UB_SGUI_PICTURE_H
#define __STM32F4_UB_SGUI_PICTURE_H


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
#define   MAX_PICTURE_ANZ        100


//--------------------------------------------------------------
// einzelne Bits vom status_byte
//--------------------------------------------------------------
#define   SPICTURE_BIT_REDRAW   0x80 // dont change
#define   SPICTURE_BIT_AKTIV    0x01 // dont change


//--------------------------------------------------------------
// Mode vom Picture
//--------------------------------------------------------------
typedef enum {
  SPICTURE_NOBTN =0,     // ohne Button-Funktion
  SPICTURE_PUSH,         // Taster
  SPICTURE_PUSHPULL      // Schalter (rastend)
}SPICTURE_MODE_t;




//--------------------------------------------------------------
// Struktur von einem Objekt
//--------------------------------------------------------------
typedef struct {
  uint16_t window_nr;      // nr vom window [1...n]
  void (*fkt_ptr)(bool);   // pointer auf funktions-handler
  SPICTURE_MODE_t mode;    // mode vom Picture
  uint16_t xpos;           // xpos (in pixel)
  uint16_t ypos;           // ypos (in pixel)
  uint16_t width;          // breit (in pixel)
  uint16_t height;         // hoehe (in pixel)
  UB_Image *img1;          // pointer auf ein Image
  UB_Image *img2;          // pointer auf ein Image
  uint8_t status;          // internes status-byte (nicht aendern !!)
}SPICTURE_t;


//--------------------------------------------------------------
// interne Struktur, Liste aller Objekte
//--------------------------------------------------------------
typedef struct {
  bool update;               // update flag
  uint16_t akt_anz;          // anzahl aller Objekte
  SPICTURE_t* ptr[MAX_PICTURE_ANZ];  // liste aller Objekte
}SGUI_PICTURE_t;
SGUI_PICTURE_t SGUI_PICTURE;



//----------Macro definition------------------------------------
#define WEAK __attribute__ ((weak))


//--------------------------------------------------------------
// interne Funktionen fuer SGUI
//--------------------------------------------------------------
void SGUI_PictureInit(void);
bool SGUI_PictureTouch(bool pressed, uint16_t x, uint16_t y, bool blocked);
void SGUI_PictureUpdate(void);


//--------------------------------------------------------------
// Globale Funktionen fuer den User
//--------------------------------------------------------------
SPICTURE_t* SGUI_PictureCreate(uint16_t x, uint16_t y, uint16_t w, uint16_t h);
void SGUI_PictureSetMode(SPICTURE_t* ptr, SPICTURE_MODE_t mode);
void SGUI_PictureSetImage1(SPICTURE_t* ptr, UB_Image *img);
void SGUI_PictureSetImage2(SPICTURE_t* ptr, UB_Image *img);
void SGUI_PictureSetAktiv(SPICTURE_t* ptr, bool aktiv);
void SGUI_PictureSetHandler(SPICTURE_t* ptr, void *fkt_ptr);
bool SGUI_PictureIsAktiv(SPICTURE_t* ptr);






//--------------------------------------------------------------
#endif // __STM32F4_UB_SGUI_PICTURE_H
