//--------------------------------------------------------------
// File     : ub_sgui_gauge.c
// Datum    : 27.07.2015
// Version  : 1.0
// Autor    : UB
// EMail    : mc-4u(@)t-online.de
// Web      : www.mikrocontroller-4u.de
// CPU      : STM32F746
// IDE      : OpenSTM32
// GCC      : 4.9 2015q2
// Module   : SGUI_WINDOW
// Funktion : Eine Gauge stellt einen Zahlenwert
//            in Balkenform dar (Fortschrittsbalken)
//            Der Istwert muss zwischen dem Min und Maxwert liegen
//--------------------------------------------------------------


//--------------------------------------------------------------
// Includes
//--------------------------------------------------------------
#include "ub_sgui_gauge.h"





//--------------------------------------------------------------
// interne Funktionen
//--------------------------------------------------------------
void P_SGUI_GaugeDraw(SGAUGE_t* ptr);



//--------------------------------------------------------------
// einen Gauge neu erzeugen
// x,y = Linke obere Ecke vom Gauge auf dem Display
// w = Breite (minimum 5 Pixel)
// h = hoehe  (minimum 5 Pixel)
//--------------------------------------------------------------
SGAUGE_t* SGUI_GaugeCreate(uint16_t x, uint16_t y, uint16_t w, uint16_t h)
{
  SGAUGE_t *ptr;
  size_t obj_size;

  if(SGUI_WINDOW.aktiv_window==0) return NULL;
  if(SGUI_WINDOW.akt_win_ptr==NULL) return NULL;
  if(SGUI_GAUGE.akt_anz>=(MAX_GAUGE_ANZ-1)) return NULL;

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
  obj_size=sizeof(SGAUGE_t);
  ptr = (SGAUGE_t *) malloc(obj_size);
  if(ptr==NULL) return NULL;  

  // init aller Werte
  ptr->window_nr=SGUI_WINDOW.aktiv_window;  
  ptr->typ=SGAUGE_RL;
  ptr->style=STYLE_LOWERED;
  ptr->xpos=x;
  ptr->ypos=y;
  ptr->width=w;
  ptr->height=h;
  ptr->frame_size=SGAUGE_DEF_FRAME_SIZE;
  ptr->col_frame=SGAUGE_DEF_FRAME_COL;
  ptr->col_back=SGAUGE_DEF_BACK_COL;
  ptr->min_value=0;
  ptr->max_value=100;
  ptr->akt_value=0;
  ptr->status=0x00;

  ptr->status|=SGAUGE_BIT_REDRAW;  // redraw at next loop
  SGUI_GAUGE.update=true;

  SGUI_GAUGE.ptr[SGUI_GAUGE.akt_anz]=ptr;
  SGUI_GAUGE.akt_anz++;
  
  // benötigter Speicherplatz
  SGUI_WINDOW.ram_used+=obj_size;  

  return ptr;
}

//--------------------------------------------------------------
// Typ von einem Gauge einstellen
// tpy : [SGAUGE_RL, SGAUGE_LR, SGAUGE_TB, SGAUGE_BT]
//--------------------------------------------------------------
void SGUI_GaugeSetTyp(SGAUGE_t* ptr, SGAUGE_TYP_t typ)
{
  if(ptr==NULL) return;

  if(typ!=ptr->typ) {
    ptr->typ=typ;
    ptr->status|=SGAUGE_BIT_REDRAW;  // redraw at next loop
    SGUI_GAUGE.update=true;
  }
}

//--------------------------------------------------------------
// Style von einem Gauge einstellen
// style : [STYLE_FLAT, STYLE_RAISED, STYLE_LOWERED]
//--------------------------------------------------------------
void SGUI_GaugeSetStyle(SGAUGE_t* ptr, STYLE_TYP_t style)
{
  if(ptr==NULL) return;
  
  if(style!=ptr->style) {
    ptr->style=style;
    ptr->status|=SGAUGE_BIT_REDRAW;  // redraw at next loop
    SGUI_GAUGE.update=true;
  }  
}

//--------------------------------------------------------------
// Rahmendicke von einem Gauge einstellen
// px = dicke in pixel (0=ohne Rahmen)
//--------------------------------------------------------------
void SGUI_GaugeSetFrameSize(SGAUGE_t* ptr, uint8_t px)
{
  if(ptr==NULL) return;

  if(px!=ptr->frame_size) {
    ptr->frame_size=px;
    ptr->status|=SGAUGE_BIT_REDRAW;  // redraw at next loop
    SGUI_GAUGE.update=true;
  }
}

//--------------------------------------------------------------
// Farbe von einem Gauge einstellen
// c1 = Rahmen
// c2 = Hintergrund
//--------------------------------------------------------------
void SGUI_GaugeSetColor(SGAUGE_t* ptr, uint16_t c1, uint16_t c2)
{
  if(ptr==NULL) return;

  if(c1!=ptr->col_frame) {
    ptr->col_frame=c1;
    ptr->status|=SGAUGE_BIT_REDRAW;  // redraw at next loop
    SGUI_GAUGE.update=true;
  }
  if(c2!=ptr->col_back) {
    ptr->col_back=c2;
    ptr->status|=SGAUGE_BIT_REDRAW;  // redraw at next loop
    SGUI_GAUGE.update=true;
  }
}

//--------------------------------------------------------------
// min und maxwert der Gauge einstellen
// max muss > als minwert sein
//--------------------------------------------------------------
void SGUI_GaugeSetMinMax(SGAUGE_t* ptr, int32_t min, int32_t max)
{
  if(ptr==NULL) return;  
  if(min>=max) return;
    
  if(ptr->min_value!=min) {
    ptr->min_value=min;
    ptr->status|=SGAUGE_BIT_REDRAW;  // redraw at next loop
    SGUI_GAUGE.update=true;    
  }
  if(ptr->max_value!=max) {
    ptr->max_value=max;
    ptr->status|=SGAUGE_BIT_REDRAW;  // redraw at next loop
    SGUI_GAUGE.update=true;    
  }
  // istwert event. anpassen
  if(ptr->akt_value<min) ptr->akt_value=min;
  if(ptr->akt_value>max) ptr->akt_value=max;
}

//--------------------------------------------------------------
// value der Gauge einstellen
// value : zwischen min und maxwert
//--------------------------------------------------------------
void SGUI_GaugeSetValue(SGAUGE_t* ptr, int32_t value)
{
  if(ptr==NULL) return;
  if(value<ptr->min_value) value=ptr->min_value;
  if(value>ptr->max_value) value=ptr->max_value;
  
  if(ptr->akt_value!=value) {
    ptr->akt_value=value;
    ptr->status|=SGAUGE_BIT_REDRAW;  // redraw at next loop
    SGUI_GAUGE.update=true;    
  }   
}

//--------------------------------------------------------------
// value der Gauge auslesen
//--------------------------------------------------------------
int32_t SGUI_GaugeGetValue(SGAUGE_t* ptr)
{
  if(ptr==NULL) return 0;
  
  return ptr->akt_value;
}

//--------------------------------------------------------------
// interne Funktion fuer SGUI
// init der Objekte
//--------------------------------------------------------------
void SGUI_GaugeInit(void)
{
  uint32_t n;

  SGUI_GAUGE.update=false;
  SGUI_GAUGE.akt_anz=0;  
  for(n=0;n<MAX_GAUGE_ANZ;n++) {
    SGUI_GAUGE.ptr[n]=NULL;
  }
}

//--------------------------------------------------------------
// interne Funktion fuer SGUI
// updated alle Objekte
// (wird aufgerufen, falls sich etwas verändert hat)
//--------------------------------------------------------------
void SGUI_GaugeUpdate(void)
{
  uint32_t n;
  uint8_t status=0;

  for(n=0;n<SGUI_GAUGE.akt_anz;n++) {
    // nur zeichnen, bei aktivem window
    if(SGUI_WINDOW.aktiv_window==SGUI_GAUGE.ptr[n]->window_nr) {
      // test ob gauge neu gezeichnet werden muss
      status=(SGUI_GAUGE.ptr[n]->status&SGAUGE_BIT_REDRAW);
      if((status!=0) || (SGUI_WINDOW.update_all==true)) P_SGUI_GaugeDraw(SGUI_GAUGE.ptr[n]);
    }
  }
}

//--------------------------------------------------------------
// interne Funktion
// zeichnet einen Gauge
//--------------------------------------------------------------
void P_SGUI_GaugeDraw(SGAUGE_t* ptr)
{
  uint16_t x,y,w,h,c1,c2,d=0,d2,px;
  int32_t d1;
  float f;

  if(ptr==NULL) return;

  // Flag vom zeichnen zuruecksetzen
  ptr->status&=~SGAUGE_BIT_REDRAW;

  x=ptr->xpos;
  y=ptr->ypos;
  w=ptr->width;
  h=ptr->height; 
  c1=ptr->col_frame;
  c2=ptr->col_back;

  if(ptr->style==STYLE_FLAT) {
    d=ptr->frame_size;
    if(d>0) {
      // mit rahmen
      if((d*2)>w) d=(w/2);
      if((d*2)>h) d=(h/2);       
    }
    // rahmen zeichnen
    SGUI_ScreenDrawFullRect(x,y,w,h,c1);    
  }
  else {
    // 3d-style
    d=SGUI_3D_FRAMESIZE;
    SGUI_ScreenDrawFullRect(x,y,w,h,c1);
    SGUI_ScreenDraw3DFrame(x,y,w,h,ptr->style);       
  }
  
  // balken zeichnen
  if(ptr->typ==SGAUGE_RL) { 
    // laenge vom balken ausrechnen
    d1=ptr->max_value-ptr->min_value;
    d2=(w-2*d);
    f=((float)(d2)/(float)(d1))*((float)(ptr->akt_value) - (float)(ptr->min_value));
    if(f<0) f=0;
    px=(uint16_t)(f);    
    SGUI_ScreenDrawFullRect(x+d+px,y+d,(d2-px),h-2*d,c2);
  }
  else if(ptr->typ==SGAUGE_LR) {   
    d1=ptr->max_value-ptr->min_value;
    d2=(w-2*d);
    f=((float)(d2)/(float)(d1))*((float)(ptr->akt_value) - (float)(ptr->min_value));
    if(f<0) f=0;
    px=(uint16_t)(f);     
    SGUI_ScreenDrawFullRect(x+d,y+d,(d2-px),h-2*d,c2);
  }
  else if(ptr->typ==SGAUGE_TB) {
    d1=ptr->max_value-ptr->min_value;
    d2=(h-2*d);
    f=((float)(d2)/(float)(d1))*((float)(ptr->akt_value) - (float)(ptr->min_value));
    if(f<0) f=0;
    px=(uint16_t)(f); 
    SGUI_ScreenDrawFullRect(x+d,y+d+px,w-2*d,(d2-px),c2);
  }
  else if(ptr->typ==SGAUGE_BT) {
    d1=ptr->max_value-ptr->min_value;
    d2=(h-2*d);
    f=((float)(d2)/(float)(d1))*((float)(ptr->akt_value) - (float)(ptr->min_value));
    if(f<0) f=0;
    px=(uint16_t)(f); 
    SGUI_ScreenDrawFullRect(x+d,y+d,w-2*d,(d2-px),c2);
  }  
}
