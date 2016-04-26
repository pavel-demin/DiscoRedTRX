//--------------------------------------------------------------
// File     : stm32_ub_sgui.c
// Datum    : 27.07.2015
// Version  : 1.3
// Autor    : UB
// EMail    : mc-4u(@)t-online.de
// Web      : www.mikrocontroller-4u.de
// CPU      : STM32F746
// IDE      : OpenSTM32
// GCC      : 4.9 2015q2
// Module   : UB_LCD, UB_FONT, UB_TOUCH, UB_GRAPHIC2D
//            SGUI_SCREEN, SGUI_WINDOW, SGUI_TEXT
// Funktion : Simple-GUI
//
// Screen-Ausrichtung : Im File "ub_sgui_screen.h" einstellen
//                      (SGUI_SCREEN_ORIENTATION)
//
// Hinweis  : Copy all SGUI-LoLevel Files in a new folder : "\sgui"
//            and add the include Path : "${ProjDirPath}/sgui"
//
//--------------------------------------------------------------


//--------------------------------------------------------------
// Includes
//--------------------------------------------------------------
#include "stm32_ub_sgui.h"


//--------------------------------------------------------------
// interne Funktionen
//--------------------------------------------------------------
UB_SGUI_t UB_SGUI;
void P_SGUI_InitLCD(void);


//--------------------------------------------------------------
// init der SGUI
//--------------------------------------------------------------
void SGUI_Init(void)
{
  // init aller Variabeln
  UB_SGUI.pause_cnt=0;
  UB_SGUI.touch_flag=0;
  // init aller Variabeln
  SGUI.touch.pressed=false;
  SGUI.touch.xp=0;
  SGUI.touch.yp=0;

  // init vom LCD
  P_SGUI_InitLCD();
  #if SGUI_SCREEN_ORIENTATION!=1
    UB_LCD_SetMode(PORTRAIT);
  #endif
  // init vom Touch
  if(UB_Touch_Init()==ERROR) {
    UB_LCD_FillLayer(SGUI_ERROR_BACK_COL);
    while(1);
  }

  // init aller Objekte
  SGUI_WindowInit();
  SGUI_TextInit();

  #if SGUI_USE_PANEL==1
    SGUI_PanelInit();
  #endif

  #if SGUI_USE_BUTTON==1
    SGUI_ButtonInit();
  #endif

  #if SGUI_USE_LED==1
    SGUI_LedInit();
  #endif

  #if SGUI_USE_LABEL==1
    SGUI_LabelInit();
  #endif

  #if SGUI_USE_CHECKBOX==1
    SGUI_CheckboxInit();
  #endif 
    
  #if SGUI_USE_GAUGE==1
    SGUI_GaugeInit();
  #endif 

  #if SGUI_USE_RADIOBTN==1
    SGUI_RadioButtonInit();
  #endif    

  #if SGUI_USE_LISTBOX==1
    SGUI_ListboxInit();
  #endif

  #if SGUI_USE_SLIDER==1
    SGUI_SliderInit();
  #endif    
    
  #if SGUI_USE_DROPDOWN==1
    SGUI_DropdownInit();
  #endif     
    
  #if SGUI_USE_SELECTBTN==1
    SGUI_SelectButtonInit();
  #endif      

  #if SGUI_USE_INTEDIT==1
    SGUI_IntEditInit();
  #endif     

  #if SGUI_USE_FLOATEDIT==1
    SGUI_FloatEditInit();
  #endif

  #if SGUI_USE_PICTURE==1
    SGUI_PictureInit();
  #endif

  #if SGUI_USE_GRAPH==1
    SGUI_GraphInit();
  #endif
}



//--------------------------------------------------------------
// verwaltet alle Objekte
// muss zyklisch vom User aufgerufen werden
//--------------------------------------------------------------
void SGUI_Do(void)
{
  static uint16_t akt_win;
  bool touch_aktion=false;
  static uint32_t old_tic=0,touch_counter=0;
  uint32_t akt_tic,tic_time;

  //------------------------------------
  // TIC
  //------------------------------------
  akt_tic=HAL_GetTick();
  if(akt_tic>=old_tic) {
	  tic_time=akt_tic-old_tic;
	  old_tic=akt_tic;
  }
  else {
	  tic_time=0xFFFFFFFF-old_tic;
	  tic_time+=akt_tic+1;
	  old_tic=akt_tic;
  }
  // touch counter
  touch_counter+=tic_time;
  if(touch_counter>=SGUI_TOUCH_INTERVALL) {
    touch_counter=0;
    UB_SGUI.touch_flag=1; // flag setzen
  }

  //------------------------------------
  // check ob update vom Parent-Window
  //------------------------------------
  if(SGUI_WINDOW.update_parent_mode>0) {
    if(SGUI_WINDOW.update_parent_mode==1) {
      akt_win=SGUI_WindowGetActivNr();
      // parent anzeigen
      if(SGUI_WINDOW.old_window!=0) {
        SGUI_WindowShowPrev();
      }
      else {
        UB_LCD_FillLayer(SGUI_DEFAULT_BACK_COL);
      }
      SGUI_WINDOW.update_parent_mode=2;
    }
    else if(SGUI_WINDOW.update_parent_mode==2) {
      // child anzeigen
      SGUI_WindowShow(akt_win);
      SGUI_WINDOW.update_parent_mode=0;
    }
  }

  //------------------------------------
  // check ob update vom aktuellen Window
  //------------------------------------
  if(SGUI_WINDOW.update_window==true) {
    SGUI_WINDOW.update_window=false;
    SGUI_WindowUpdate();
    // merker setzen um alle objekte upzudaten
    SGUI_WINDOW.update_all=true;
  }

  //------------------------------------
  // check ob touch flag gesetzt
  //------------------------------------
  if(UB_SGUI.touch_flag==1) {
    UB_SGUI.touch_flag=0;
    touch_aktion=false;

    // Touch abfragen
    if(UB_Touch_Read()==SUCCESS) {
      if(Touch_Data.status==TOUCH_PRESSED) {
        SGUI.touch.pressed=true;
        #if SGUI_SCREEN_ORIENTATION!=0
          SGUI.touch.xp=Touch_Data.xp;
          SGUI.touch.yp=Touch_Data.yp;
        #else
          SGUI.touch.xp=Touch_Data.yp;
          SGUI.touch.yp=SGUI_SCREEN_MAXY-Touch_Data.xp-1;
        #endif
      }
      else {
        SGUI.touch.pressed=false;
        SGUI.touch.xp=0;
        SGUI.touch.yp=0;
      }      
    }

    #if SGUI_USE_BUTTON==1
      // check alle Buttons auf Touch position
      touch_aktion=SGUI_ButtonTouch(SGUI.touch.pressed, SGUI.touch.xp, SGUI.touch.yp, touch_aktion);
    #endif

    #if SGUI_USE_CHECKBOX==1
      // check alle Checkboxen auf Touch position
      touch_aktion=SGUI_CheckboxTouch(SGUI.touch.pressed, SGUI.touch.xp, SGUI.touch.yp, touch_aktion);
    #endif

    #if SGUI_USE_RADIOBTN==1
      // check alle RadioButtons auf Touch position
      touch_aktion=SGUI_RadioButtonTouch(SGUI.touch.pressed, SGUI.touch.xp, SGUI.touch.yp, touch_aktion);
    #endif

    #if SGUI_USE_LISTBOX==1
      // check alle Listboxen auf Touch position
      touch_aktion=SGUI_ListboxTouch(SGUI.touch.pressed, SGUI.touch.xp, SGUI.touch.yp, touch_aktion);
    #endif

    #if SGUI_USE_SLIDER==1
      // check alle Slider auf Touch position
      touch_aktion=SGUI_SliderTouch(SGUI.touch.pressed, SGUI.touch.xp, SGUI.touch.yp, touch_aktion);
    #endif

    #if SGUI_USE_DROPDOWN==1
      // check alle Dropdown auf Touch position
      touch_aktion=SGUI_DropdownTouch(SGUI.touch.pressed, SGUI.touch.xp, SGUI.touch.yp, touch_aktion);
    #endif

    #if SGUI_USE_SELECTBTN==1
      // check alle SelectButton auf Touch position
      touch_aktion=SGUI_SelectButtonTouch(SGUI.touch.pressed, SGUI.touch.xp, SGUI.touch.yp, touch_aktion);
    #endif

    #if SGUI_USE_INTEDIT==1
      // check alle IntEdit auf Touch position
      touch_aktion=SGUI_IntEditTouch(SGUI.touch.pressed, SGUI.touch.xp, SGUI.touch.yp, touch_aktion);
    #endif

    #if SGUI_USE_FLOATEDIT==1
      // check alle FloatEdit auf Touch position
      touch_aktion=SGUI_FloatEditTouch(SGUI.touch.pressed, SGUI.touch.xp, SGUI.touch.yp, touch_aktion);
    #endif

    #if SGUI_USE_PICTURE==1
      // check alle Picture auf Touch position
      touch_aktion=SGUI_PictureTouch(SGUI.touch.pressed, SGUI.touch.xp, SGUI.touch.yp, touch_aktion);
    #endif
  }

  //------------------------------------
  // check auf veraenderung der Objekte
  //------------------------------------

  // Panels zuerst zeichnen !! (damit sie im Hintergrund liegen)
  #if SGUI_USE_PANEL==1
  // check ob sich bei den PANELs etwas verändert hat
  if((SGUI_PANEL.update==true) || (SGUI_WINDOW.update_all==true)) {
    SGUI_PANEL.update=false;
    SGUI_PanelUpdate();
  }
  #endif

  // check ob sich bei den Texten etwas verändert hat
  if((SGUI_TEXT.update==true) || (SGUI_WINDOW.update_all==true)) {
    SGUI_TEXT.update=false;
    SGUI_TextUpdate();
  }

  #if SGUI_USE_BUTTON==1
    // check ob sich bei den Buttons etwas verändert hat
    if((SGUI_BUTTON.update==true) || (SGUI_WINDOW.update_all==true)) {
      SGUI_BUTTON.update=false;
      SGUI_ButtonUpdate();
    }
  #endif

  #if SGUI_USE_LED==1
    // check ob sich bei den LEDs etwas verändert hat
    if((SGUI_LED.update==true) || (SGUI_WINDOW.update_all==true)) {
      SGUI_LED.update=false;
      SGUI_LedUpdate();
    }
  #endif

  #if SGUI_USE_LABEL==1
  // check ob sich bei den LABELs etwas verändert hat
  if((SGUI_LABEL.update==true) || (SGUI_WINDOW.update_all==true)) {
    SGUI_LABEL.update=false;
    SGUI_LabelUpdate();
  }
  #endif

  #if SGUI_USE_CHECKBOX==1
  // check ob sich bei den CHECKBOXEN etwas verändert hat
  if((SGUI_CHECKBOX.update==true) || (SGUI_WINDOW.update_all==true)) {
    SGUI_CHECKBOX.update=false;
    SGUI_CheckboxUpdate();
  }
  #endif  
  
  #if SGUI_USE_GAUGE==1
  // check ob sich bei den GAUGE etwas verändert hat
  if((SGUI_GAUGE.update==true) || (SGUI_WINDOW.update_all==true)) {
    SGUI_GAUGE.update=false;
    SGUI_GaugeUpdate();
  }
  #endif  
  
  #if SGUI_USE_RADIOBTN==1
  // check ob sich bei den RadioButtons etwas verändert hat
  if((SGUI_RBTN.update==true) || (SGUI_WINDOW.update_all==true)) {
    SGUI_RBTN.update=false;
    SGUI_RadioButtonUpdate();
  }
  #endif  

  #if SGUI_USE_LISTBOX==1
  // check ob sich bei den Listboxen etwas verändert hat
  if((SGUI_LISTBOX.update==true) || (SGUI_WINDOW.update_all==true)) {
    SGUI_LISTBOX.update=false;
    SGUI_ListboxUpdate();
  }
  #endif

  #if SGUI_USE_SLIDER==1
  // check ob sich bei den Slider etwas verändert hat
  if((SGUI_SLIDER.update==true) || (SGUI_WINDOW.update_all==true)) {
    SGUI_SLIDER.update=false;
    SGUI_SliderUpdate();
  }
  #endif   
  
  #if SGUI_USE_DROPDOWN==1
  // check ob sich bei den Dropdown etwas verändert hat
  if((SGUI_DROPDOWN.update==true) || (SGUI_WINDOW.update_all==true)) {
    SGUI_DROPDOWN.update=false;
    SGUI_DropdownUpdate();
  }
  #endif  
  
  #if SGUI_USE_SELECTBTN==1
  // check ob sich bei den Dropdown etwas verändert hat
  if((SGUI_SELECTBTN.update==true) || (SGUI_WINDOW.update_all==true)) {
    SGUI_SELECTBTN.update=false;
    SGUI_SelectButtonUpdate();
  }
  #endif  

  #if SGUI_USE_INTEDIT==1
  // check ob sich bei den IntEdit etwas verändert hat
  if((SGUI_INTEDIT.update==true) || (SGUI_WINDOW.update_all==true)) {
    SGUI_INTEDIT.update=false;
    SGUI_IntEditUpdate();
  }
  #endif

  #if SGUI_USE_FLOATEDIT==1
    // check ob sich bei den FloatEdit etwas verändert hat
    if((SGUI_FLOATEDIT.update==true) || (SGUI_WINDOW.update_all==true)) {
      SGUI_FLOATEDIT.update=false;
      SGUI_FloatEditUpdate();
    }
  #endif   

  #if SGUI_USE_PICTURE==1
    // check ob sich bei den Picture etwas verändert hat
    if((SGUI_PICTURE.update==true) || (SGUI_WINDOW.update_all==true)) {
      SGUI_PICTURE.update=false;
      SGUI_PictureUpdate();
    }
  #endif

  #if SGUI_USE_GRAPH==1
    // check ob sich bei den Graphen etwas verändert hat
    if((SGUI_GRAPH.update==true) || (SGUI_WINDOW.update_all==true)) {
      SGUI_GRAPH.update=false;
      SGUI_GraphUpdate();
    }
  #endif


  //------------------------------------
  if(SGUI_WINDOW.update_all==true) {
    SGUI_WINDOW.update_all=false;
  }
}


//--------------------------------------------------------------
// zeichnet alle Objekte vom aktuelle Window nochmal neu
// kann vom User aufgerufen werden
//--------------------------------------------------------------
void SGUI_ReDraw(void)
{
  SGUI_WINDOW.update_window=true;
  SGUI_Do();
}

//--------------------------------------------------------------
// pause (in ms)
//--------------------------------------------------------------
void SGUI_Pause_ms(uint32_t delay)
{
  HAL_Delay(delay);
}

//--------------------------------------------------------------
// auslesen vom Speicherbedarf (in bytes)
//--------------------------------------------------------------
uint32_t SGUI_GetUsedRam(void)
{
  return SGUI_WINDOW.ram_used;
}

//--------------------------------------------------------------
// interne Funktion
// init vom LCD
//--------------------------------------------------------------
void P_SGUI_InitLCD(void)
{
  UB_LCD_Init();
  UB_LCD_LayerInit_Fullscreen();
  // auf Hintergrund schalten
  UB_LCD_SetLayer_1();
  // Hintergrund komplett mit einer Farbe füllen
  UB_LCD_FillLayer(SGUI_DEFAULT_BACK_COL);
  // auf Vordergrund schalten
  UB_LCD_SetLayer_2();
  // Vordergrund komplett mit einer Farbe füllen
  UB_LCD_FillLayer(SGUI_DEFAULT_BACK_COL);
}
