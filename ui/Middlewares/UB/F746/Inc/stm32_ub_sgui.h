//--------------------------------------------------------------
// File     : stm32_ub_sgui.h
//--------------------------------------------------------------

//--------------------------------------------------------------
#ifndef __STM32F4_UB_SGUI_H
#define __STM32F4_UB_SGUI_H


//--------------------------------------------------------------
// Includes
//--------------------------------------------------------------
#include "stm32_ub_system.h"
#include "stm32_ub_lcd_480x272.h"
#include "stm32_ub_font.h"
#include "stm32_ub_touch_480x272.h"
#include <stdbool.h>
#include "ub_sgui_window.h"
#include "ub_sgui_text.h"






//--------------------------------------------------------------
// defines
//--------------------------------------------------------------
#define   SGUI_DEFAULT_BACK_COL     RGB_COL_WHITE
#define   SGUI_ERROR_BACK_COL       RGB_COL_RED
#define   SGUI_TOUCH_INTERVALL      50 // 50ms




//--------------------------------------------------------------
// hier kann eingestellt werden, welche Objekte benutzt
// werden sollen oder nicht
//  0=disable, 1=enable
//--------------------------------------------------------------
#define   SGUI_USE_PANEL       1
#define   SGUI_USE_BUTTON      1
#define   SGUI_USE_LED         1
#define   SGUI_USE_LABEL       1
#define   SGUI_USE_CHECKBOX    1
#define   SGUI_USE_GAUGE       1
#define   SGUI_USE_RADIOBTN    1
#define   SGUI_USE_LISTBOX     1
#define   SGUI_USE_SLIDER      1
#define   SGUI_USE_DROPDOWN    1
#define   SGUI_USE_SELECTBTN   1
#define   SGUI_USE_INTEDIT     1
#define   SGUI_USE_FLOATEDIT   1
#define   SGUI_USE_PICTURE     1
#define   SGUI_USE_GRAPH       1





#if SGUI_USE_PANEL==1
  #include "ub_sgui_panel.h"
#endif

#if SGUI_USE_BUTTON==1
  #include "ub_sgui_button.h"
#endif

#if SGUI_USE_LED==1
  #include "ub_sgui_led.h"
#endif

#if SGUI_USE_LABEL==1
  #include "ub_sgui_label.h"
#endif

#if SGUI_USE_CHECKBOX==1
  #include "ub_sgui_checkbox.h"
#endif

#if SGUI_USE_GAUGE==1
  #include "ub_sgui_gauge.h"
#endif

#if SGUI_USE_RADIOBTN==1
  #include "ub_sgui_radiobtn.h"
#endif

#if SGUI_USE_LISTBOX==1
  #include "ub_sgui_listbox.h"
#endif

#if SGUI_USE_SLIDER==1
  #include "ub_sgui_slider.h"
#endif

#if SGUI_USE_DROPDOWN==1
  #include "ub_sgui_dropdown.h"
#endif

#if SGUI_USE_SELECTBTN==1
  #include "ub_sgui_selectbtn.h"
#endif

#if SGUI_USE_INTEDIT==1
  #include "ub_sgui_intedit.h"
#endif

#if SGUI_USE_FLOATEDIT==1
  #include "ub_sgui_floatedit.h"
#endif

#if SGUI_USE_PICTURE==1
  #include "ub_sgui_picture.h"
#endif

#if SGUI_USE_GRAPH==1
  #include "ub_sgui_graph.h"
#endif



//--------------------------------------------------------------
// Struktur vom Touch
//--------------------------------------------------------------
typedef struct {
  bool pressed;   // true => touch ist betaetigt
  uint16_t xp;    // xpos (wenn touch betaetigt)
  uint16_t yp;    // ypos (wenn touch betaetigt)
}SGUI_TOUCH_t;


//--------------------------------------------------------------
// Struktur der SGUI
// (kann vom User benutzt werden)
//--------------------------------------------------------------
typedef struct {
  SGUI_TOUCH_t touch;  // daten vom touch
}SGUI_t;
SGUI_t SGUI;


//--------------------------------------------------------------
// interne Struktur
//--------------------------------------------------------------
typedef struct {
  uint32_t pause_cnt;  // counter fuer pausen
  uint8_t touch_flag;  // flag fuer touch update
}UB_SGUI_t;




//--------------------------------------------------------------
// Globale Funktionen
//--------------------------------------------------------------
void SGUI_Init(void);
void SGUI_Do(void);
void SGUI_ReDraw(void);
void SGUI_Pause_ms(uint32_t delay);
uint32_t SGUI_GetUsedRam(void); 


//--------------------------------------------------------------
#endif // __STM32F4_UB_SGUI_H
