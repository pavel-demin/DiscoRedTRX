//--------------------------------------------------------------
// File     : ub_sgui_panel.c
// Datum    : 27.07.2015
// Version  : 1.0
// Autor    : UB
// EMail    : mc-4u(@)t-online.de
// Web      : www.mikrocontroller-4u.de
// CPU      : STM32F746
// IDE      : OpenSTM32
// GCC      : 4.9 2015q2
// Module   : SGUI_WINDOW
// Funktion : Panels dienen nur zur optischen Gestaltung
//            und bestehen aus Flaechen die zuerst gezeichnet
//            werden und damit im Hintergrund liegen
//--------------------------------------------------------------



//--------------------------------------------------------------
// Includes
//--------------------------------------------------------------
#include "ub_sgui_panel.h"




//--------------------------------------------------------------
// interne Funktionen
//--------------------------------------------------------------
void P_SGUI_PanelDraw(SPANEL_t* ptr);




//--------------------------------------------------------------
// ein PANEL neu erzeugen
// x,y = Linke obere Ecke vom Panel auf dem Display
// w = Breite (minimum 5 Pixel)
// h = hoehe  (minimum 5 Pixel)
//--------------------------------------------------------------
SPANEL_t* SGUI_PanelCreate(uint16_t x, uint16_t y, uint16_t w, uint16_t h)
{
  SPANEL_t *ptr;
  size_t obj_size;

  if(SGUI_WINDOW.aktiv_window==0) return NULL;
  if(SGUI_WINDOW.akt_win_ptr==NULL) return NULL;
  if(SGUI_PANEL.akt_anz>=(MAX_PANEL_ANZ-1)) return NULL;

  // parameter check
  if(SGUI_WINDOW.akt_win_ptr->typ==SWINDOW_CHILD) {
    // bei Child-Window, Offset fuer x und y
    x+=SGUI_WINDOW.akt_win_ptr->xpos;
    y+=SGUI_WINDOW.akt_win_ptr->ypos;
  }
  if((x>=SGUI_SCREEN_MAXX) || (y>=SGUI_SCREEN_MAXY)) return NULL;
  if((x+w)>SGUI_SCREEN_MAXX) w=(SGUI_SCREEN_MAXX-x);
  if((y+h)>SGUI_SCREEN_MAXY) h=(SGUI_SCREEN_MAXY-y);
  if((w<5) || (h<5)) return NULL;

  // speicher reservieren
  obj_size=sizeof(SPANEL_t);
  ptr = (SPANEL_t *) malloc(obj_size);
  if(ptr==NULL) return NULL;
  
  // init aller Werte
  ptr->window_nr=SGUI_WINDOW.aktiv_window;
  ptr->style=STYLE_LOWERED;
  ptr->xpos=x;
  ptr->ypos=y;
  ptr->width=w;
  ptr->height=h;
  ptr->frame_size=SPANEL_DEF_FRAME_SIZE;
  ptr->col_frame=SPANEL_DEF_FRAME_COL;
  ptr->col_back=SPANEL_DEF_BACK_COL;
  ptr->status=0x00;

  ptr->status|=SPANEL_BIT_REDRAW;  // redraw at next loop
  SGUI_PANEL.update=true;

  SGUI_PANEL.ptr[SGUI_PANEL.akt_anz]=ptr;
  SGUI_PANEL.akt_anz++;
  
  // benötigter Speicherplatz
  SGUI_WINDOW.ram_used+=obj_size;  

  return ptr;
}

//--------------------------------------------------------------
// Style von einem Panel einstellen
// style : [STYLE_FLAT, STYLE_RAISED, STYLE_LOWERED]
//--------------------------------------------------------------
void SGUI_PanelSetStyle(SPANEL_t* ptr, STYLE_TYP_t style)
{
  if(ptr==NULL) return;
  
  if(style!=ptr->style) {
    ptr->style=style;
    ptr->status|=SPANEL_BIT_REDRAW;  // redraw at next loop
    SGUI_PANEL.update=true;
  }  
}

//--------------------------------------------------------------
// Rahmendicke von einem Panel einstellen
// px = dicke in pixel (0 = ohne Rahmen)
//--------------------------------------------------------------
void SGUI_PanelSetFrameSize(SPANEL_t* ptr, uint8_t px)
{
  if(ptr==NULL) return;

  if(px!=ptr->frame_size) {
    ptr->frame_size=px;
    ptr->status|=SPANEL_BIT_REDRAW;  // redraw at next loop
    SGUI_PANEL.update=true;
  }
}

//--------------------------------------------------------------
// Farbe von einem Panel einstellen
// c1 = Rahmen
// c2 = Hintergrund
//--------------------------------------------------------------
void SGUI_PanelSetColor(SPANEL_t* ptr, uint16_t c1, uint16_t c2)
{
  if(ptr==NULL) return;

  if(c1!=ptr->col_frame) {
    ptr->col_frame=c1;
    ptr->status|=SPANEL_BIT_REDRAW;  // redraw at next loop
    SGUI_PANEL.update=true;
  }  
  if(c2!=ptr->col_back) {
    ptr->col_back=c2;
    ptr->status|=SPANEL_BIT_REDRAW;  // redraw at next loop
    SGUI_PANEL.update=true;
  }
}

//--------------------------------------------------------------
// interne Funktion fuer SGUI
// init der Objekte
//--------------------------------------------------------------
void SGUI_PanelInit(void)
{
  uint32_t n;

  SGUI_PANEL.update=false;
  SGUI_PANEL.akt_anz=0;  
  for(n=0;n<MAX_PANEL_ANZ;n++) {
    SGUI_PANEL.ptr[n]=NULL;
  }
}

//--------------------------------------------------------------
// interne Funktion fuer SGUI
// updated alle Objekte
// (wird aufgerufen, falls sich etwas verändert hat)
//--------------------------------------------------------------
void SGUI_PanelUpdate(void)
{
  uint32_t n;
  uint8_t status=0;

  for(n=0;n<SGUI_PANEL.akt_anz;n++) {
    // nur zeichnen, bei aktivem window
    if(SGUI_WINDOW.aktiv_window==SGUI_PANEL.ptr[n]->window_nr) {
      // test ob panel neu gezeichnet werden muss
      status=(SGUI_PANEL.ptr[n]->status&SPANEL_BIT_REDRAW);
      if((status!=0) || (SGUI_WINDOW.update_all==true)) P_SGUI_PanelDraw(SGUI_PANEL.ptr[n]);
    }
  }
}

//--------------------------------------------------------------
// interne Funktion
// zeichnet ein Panel
//--------------------------------------------------------------
void P_SGUI_PanelDraw(SPANEL_t* ptr)
{
  uint16_t x,y,w,h,c1,c2,d=0;

  if(ptr==NULL) return;

  // Flag vom zeichnen zuruecksetzen
  ptr->status&=~SPANEL_BIT_REDRAW;

  x=ptr->xpos;
  y=ptr->ypos;
  w=ptr->width;
  h=ptr->height;
  
  c1=ptr->col_frame;
  c2=ptr->col_back;

  // panel zeichnen
  if(ptr->style==STYLE_FLAT) {
    d=ptr->frame_size;
    if(d>0) {
      // zuerst den rahmen zeichnen
      SGUI_ScreenDrawFullRect(x,y,w,h,c1);
      // dann das panel zeichnen
      // etwas kleiner als der rahmen
      if((d*2)>w) d=(w/2);
      if((d*2)>h) d=(h/2);
      SGUI_ScreenDrawFullRect(x+d,y+d,w-2*d,h-2*d,c2);
    }
    else {
      // kein Rahmen
      SGUI_ScreenDrawFullRect(x,y,w,h,c2);
    }
  }
  else {
    // 3d-sytle
    SGUI_ScreenDrawFullRect(x,y,w,h,c2);
    SGUI_ScreenDraw3DFrame(x,y,w,h,ptr->style);
  }
}
