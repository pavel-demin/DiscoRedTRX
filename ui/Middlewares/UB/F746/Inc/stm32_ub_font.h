//--------------------------------------------------------------
// File     : stm32_ub_Font.h
//--------------------------------------------------------------

//--------------------------------------------------------------
#ifndef __STM32F7_UB_FONT_H
#define __STM32F7_UB_FONT_H

//--------------------------------------------------------------
// Includes
//--------------------------------------------------------------
#include "stm32_ub_system.h"



//--------------------------------------------------------------
// Struktur von einem monospaced Font (max 16 Pixel Breite)
//-------------------------------------------------------------- 
typedef struct UB_Font_t {
  const uint16_t *table; // Tabelle mit den Daten
  uint16_t width;        // Breite eines Zeichens (in Pixel)
  uint16_t height;       // Hoehe eines Zeichens  (in Pixel)
}UB_Font;


//--------------------------------------------------------------
// Struktur von einem monospaced Font (max 32 Pixel Breite)
//--------------------------------------------------------------
typedef struct UB_Font32_t {
  const uint32_t *table; // Tabelle mit den Daten
  uint16_t width;        // Breite eines Zeichens (in Pixel)
  uint16_t height;       // Hoehe eines Zeichens  (in Pixel)
}UB_Font32;


//--------------------------------------------------------------
// Struktur von einem proportional Font (max 16 Pixel Breite)
//--------------------------------------------------------------
typedef struct UB_pFont_t {
  const uint16_t *table; // Tabelle mit den Daten
  uint16_t height;       // Hoehe eines Zeichens  (in Pixel)
  uint16_t first_char;   // erstes Zeichen  (Ascii-Nr)
  uint16_t last_char;    // letztes Zeichen (Ascii-Nr)
}UB_pFont;


//--------------------------------------------------------------
// Struktur von einem proportional Font (max 32 Pixel Breite)
//--------------------------------------------------------------
typedef struct UB_pFont32_t {
  const uint32_t *table; // Tabelle mit den Daten
  uint16_t height;       // Hoehe eines Zeichens  (in Pixel)
  uint16_t first_char;   // erstes Zeichen  (Ascii-Nr)
  uint16_t last_char;    // letztes Zeichen (Ascii-Nr)
}UB_pFont32;


//--------------------------------------------------------------
// Aktivierung der verschiedenen Fonts
// (unbenutzte einfach auskommentieren)
// fuer alle benutzten Fonts muss das entsprechende C-File
// in der CooCox-IDE hinzugefügt werden
//--------------------------------------------------------------
// monospaced
extern UB_Font Arial_7x10; 
extern UB_Font Arial_8x13;
extern UB_Font Arial_10x15;
extern UB_Font Arial_11x18;
extern UB_Font Arial_13x19;
extern UB_Font Arial_14x22;
extern UB_Font Arial_16x25;
extern UB_Font32 Arial_18x27;
// proportional
extern UB_pFont pArial_10;
extern UB_pFont pArial_13;
extern UB_pFont pArial_16;
extern UB_pFont pComic_12;
extern UB_pFont pComic_16;
extern UB_pFont pTimes_12;
extern UB_pFont pTimes_15;
extern UB_pFont32 pArial_21;
extern UB_pFont32 pComic_19;
extern UB_pFont32 pTimes_18;



//--------------------------------------------------------------
// Globale Funktionen
//--------------------------------------------------------------
void UB_Font_DrawChar(uint16_t x, uint16_t y, uint8_t ascii, UB_Font *font, uint16_t vg, uint16_t bg);
void UB_Font_DrawString(uint16_t x, uint16_t y,char *ptr, UB_Font *font, uint16_t vg, uint16_t bg);
void UB_Font_DrawChar32(uint16_t x, uint16_t y, uint8_t ascii, UB_Font32 *font, uint16_t vg, uint16_t bg);
void UB_Font_DrawString32(uint16_t x, uint16_t y,char *ptr, UB_Font32 *font, uint16_t vg, uint16_t bg);
uint16_t UB_Font_DrawPChar(uint16_t x, uint16_t y, uint8_t ascii, UB_pFont *font, uint16_t vg, uint16_t bg);
void UB_Font_DrawPString(uint16_t x, uint16_t y,char *ptr, UB_pFont *font, uint16_t vg, uint16_t bg);
uint16_t UB_Font_DrawPChar32(uint16_t x, uint16_t y, uint8_t ascii, UB_pFont32 *font, uint16_t vg, uint16_t bg);
void UB_Font_DrawPString32(uint16_t x, uint16_t y,char *ptr, UB_pFont32 *font, uint16_t vg, uint16_t bg);

//--------------------------------------------------------------
#endif // __STM32F7_UB_FONT_H
