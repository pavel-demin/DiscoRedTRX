//--------------------------------------------------------------
// File     : ub_sgui_led.c
// Datum    : 27.07.2015
// Version  : 1.0
// Autor    : UB
// EMail    : mc-4u(@)t-online.de
// Web      : www.mikrocontroller-4u.de
// CPU      : STM32F746
// IDE      : OpenSTM32
// GCC      : 4.9 2015q2
// Module   : SGUI_WINDOW
// Funktion : LEDs dienen zum Signalisieren von zwei Zustaenden
//--------------------------------------------------------------



//--------------------------------------------------------------
// Includes
//--------------------------------------------------------------
#include "ub_sgui_led.h"




//--------------------------------------------------------------
// interne Funktionen
//--------------------------------------------------------------
void P_SGUI_LedDraw(SLED_t* ptr);


//--------------------------------------------------------------
// eine LED neu erzeugen
// x,y = Linke obere Ecke der LED auf dem Display
// s = Size (minimum 5 Pixel)
//--------------------------------------------------------------
SLED_t* SGUI_LedCreate(uint16_t x, uint16_t y, uint16_t s)
{
  SLED_t *ptr;
  size_t obj_size;

  if(SGUI_WINDOW.aktiv_window==0) return NULL;
  if(SGUI_WINDOW.akt_win_ptr==NULL) return NULL;
  if(SGUI_LED.akt_anz>=(MAX_LED_ANZ-1)) return NULL;

  // parameter check
  if(SGUI_WINDOW.akt_win_ptr->typ==SWINDOW_CHILD) {
    // bei Child-Window, Offset fuer x und y
    x+=SGUI_WINDOW.akt_win_ptr->xpos;
    y+=SGUI_WINDOW.akt_win_ptr->ypos;
  }
  if((x>=SGUI_SCREEN_MAXX) || (y>=SGUI_SCREEN_MAXY)) return NULL;
  if((x+s)>SGUI_SCREEN_MAXX) s=(SGUI_SCREEN_MAXX-x);
  if((y+s)>SGUI_SCREEN_MAXY) s=(SGUI_SCREEN_MAXY-y);
  if(s<5) return NULL;

  // speicher reservieren
  obj_size=sizeof(SLED_t);
  ptr = (SLED_t *) malloc(obj_size);
  if(ptr==NULL) return NULL;  

  // init aller Werte
  ptr->window_nr=SGUI_WINDOW.aktiv_window;
  ptr->typ=SLED_ROUND;
  ptr->style=STYLE_RAISED;
  ptr->xpos=x;
  ptr->ypos=y;
  ptr->size=s;
  ptr->frame_size=SLED_DEF_FRAME_SIZE;
  ptr->col_frame=SLED_DEF_FRAME_COL;
  ptr->col_off=SLED_DEF_COL_OFF;
  ptr->col_on=SLED_DEF_COL_ON;
  ptr->status=0x00;

  ptr->status|=SLED_BIT_REDRAW;  // redraw at next loop
  SGUI_LED.update=true;

  SGUI_LED.ptr[SGUI_LED.akt_anz]=ptr;
  SGUI_LED.akt_anz++;
  
  // benötigter Speicherplatz
  SGUI_WINDOW.ram_used+=obj_size;  

  return ptr;
}

//--------------------------------------------------------------
// Typ der LED einstellen
// typ : [SLED_ROUND, SLED_RECT]
//--------------------------------------------------------------
void SGUI_LedSetTyp(SLED_t* ptr, SLED_TYP_t typ)
{
  if(ptr==NULL) return;

  if(typ!=ptr->typ) {
    ptr->typ=typ;
    ptr->status|=SLED_BIT_REDRAW;  // redraw at next loop
    SGUI_LED.update=true;
  }   
}

//--------------------------------------------------------------
// Style von einer LED einstellen
// style : [STYLE_FLAT, STYLE_RAISED, STYLE_LOWERED]
//--------------------------------------------------------------
void SGUI_LedSetStyle(SLED_t* ptr, STYLE_TYP_t style)
{
  if(ptr==NULL) return;
  
  if(style!=ptr->style) {
    ptr->style=style;
    ptr->status|=SLED_BIT_REDRAW;  // redraw at next loop
    SGUI_LED.update=true;
  }  
}

//--------------------------------------------------------------
// LED akvivieren
//--------------------------------------------------------------
void SGUI_LedOn(SLED_t* ptr)
{
  if(ptr==NULL) return;

  if((ptr->status&SLED_BIT_AKTIV)==0) {
    ptr->status|=SLED_BIT_AKTIV;
    ptr->status|=SLED_BIT_REDRAW;  // redraw at next loop
    SGUI_LED.update=true;
  }
}

//--------------------------------------------------------------
// LED deaktivieren
//--------------------------------------------------------------
void SGUI_LedOff(SLED_t* ptr)
{
  if(ptr==NULL) return;

  if((ptr->status&SLED_BIT_AKTIV)!=0) {
    ptr->status&=~SLED_BIT_AKTIV;
    ptr->status|=SLED_BIT_REDRAW;  // redraw at next loop
    SGUI_LED.update=true;
  }
}

//--------------------------------------------------------------
// LED toggeln
//--------------------------------------------------------------
void SGUI_LedToggle(SLED_t* ptr)
{
  if(ptr==NULL) return;

  if((ptr->status&SLED_BIT_AKTIV)==0) {
    ptr->status|=SLED_BIT_AKTIV;
  }
  else {
    ptr->status&=~SLED_BIT_AKTIV;
  }
  ptr->status|=SLED_BIT_REDRAW;  // redraw at next loop
  SGUI_LED.update=true;
}

//--------------------------------------------------------------
// LED aktiv oder deaktiv schalten
//--------------------------------------------------------------
void SGUI_LedSetAktiv(SLED_t* ptr, bool aktiv)
{
  if(ptr==NULL) return;

  if(aktiv==true) {
    if((ptr->status&SLED_BIT_AKTIV)==0) {
      ptr->status|=SLED_BIT_AKTIV;
      ptr->status|=SLED_BIT_REDRAW;  // redraw at next loop
      SGUI_LED.update=true;
    }
  }
  else {
    if((ptr->status&SLED_BIT_AKTIV)!=0) {
      ptr->status&=~SLED_BIT_AKTIV;
      ptr->status|=SLED_BIT_REDRAW;  // redraw at next loop
      SGUI_LED.update=true;
    }
  }
}

//--------------------------------------------------------------
// Rahmendicke von einer LED einstellen
// px = dicke in pixel (0 = ohne Rahmen)
//--------------------------------------------------------------
void SGUI_LedSetFrameSize(SLED_t* ptr, uint8_t px)
{
  if(ptr==NULL) return;

  if(px!=ptr->frame_size) {
    ptr->frame_size=px;
    ptr->status|=SLED_BIT_REDRAW;  // redraw at next loop
    SGUI_LED.update=true;
  }
}

//--------------------------------------------------------------
// Farbe von einer LED einstellen
// c1 = Rahmen
// c2 = On
// c3 = Off
//--------------------------------------------------------------
void SGUI_LedSetColor(SLED_t* ptr, uint16_t c1, uint16_t c2, uint16_t c3)
{
  if(ptr==NULL) return;

  if(c1!=ptr->col_frame) {
    ptr->col_frame=c1;
    ptr->status|=SLED_BIT_REDRAW;  // redraw at next loop
    SGUI_LED.update=true;
  }
  if(c2!=ptr->col_on) {
    ptr->col_on=c2;
    ptr->status|=SLED_BIT_REDRAW;  // redraw at next loop
    SGUI_LED.update=true;
  }
  if(c3!=ptr->col_off) {
    ptr->col_off=c3;
    ptr->status|=SLED_BIT_REDRAW;  // redraw at next loop
    SGUI_LED.update=true;
  }
}

//--------------------------------------------------------------
// test ob eine LED aktiv ist
// ret_wert : true  => led ist aktiv
//            false => led ist nicht aktiv
//--------------------------------------------------------------
bool SGUI_LedIsAktiv(SLED_t* ptr)
{
  if(ptr==NULL) return false;

  if((ptr->status&SLED_BIT_AKTIV)==0) {
    return false;
  }

  return true;
}

//--------------------------------------------------------------
// interne Funktion fuer SGUI
// init der Objekte
//--------------------------------------------------------------
void SGUI_LedInit(void)
{
  uint32_t n;

  SGUI_LED.update=false;
  SGUI_LED.akt_anz=0;  
  for(n=0;n<MAX_LED_ANZ;n++) {
    SGUI_LED.ptr[n]=NULL;
  }
}

//--------------------------------------------------------------
// interne Funktion fuer SGUI
// updated alle Objekte
// (wird aufgerufen, falls sich etwas verändert hat)
//--------------------------------------------------------------
void SGUI_LedUpdate(void)
{
  uint32_t n;
  uint8_t status=0;

  for(n=0;n<SGUI_LED.akt_anz;n++) {
    // nur zeichnen, bei aktivem window
    if(SGUI_WINDOW.aktiv_window==SGUI_LED.ptr[n]->window_nr) {
      // test ob led neu gezeichnet werden muss
      status=(SGUI_LED.ptr[n]->status&SLED_BIT_REDRAW);
      if((status!=0) || (SGUI_WINDOW.update_all==true)) P_SGUI_LedDraw(SGUI_LED.ptr[n]);
    }
  }
}

//--------------------------------------------------------------
// interne Funktion
// zeichnet einen LED
//--------------------------------------------------------------
void P_SGUI_LedDraw(SLED_t* ptr)
{
  uint16_t x,y,s,c1,c2,d=0,r;

  if(ptr==NULL) return;

  // Flag vom zeichnen zuruecksetzen
  ptr->status&=~SLED_BIT_REDRAW;

  x=ptr->xpos;
  y=ptr->ypos;
  s=ptr->size;

  if(((ptr->status)&SLED_BIT_AKTIV)==0) {
    c1=ptr->col_off;
  }
  else {
    c1=ptr->col_on;
  }
  c2=ptr->col_frame;

  d=ptr->frame_size;
  if(ptr->typ==SLED_ROUND) {
    r=s/2; // Radius
    if(ptr->style==STYLE_FLAT) {
      if(d>0) {
        // zuerst den rahmen zeichnen
        SGUI_ScreenDrawFullCircle(x+r,y+r,r,c2);
        // dann die led zeichnen
        // etwas kleiner als der rahmen
        if(d>r) d=r;
        SGUI_ScreenDrawFullCircle(x+r,y+r,r-d,c1);
      }
      else {
        // kein Rahmen
        SGUI_ScreenDrawFullCircle(x+r,y+r,r,c1);
      }
    }
    else {
      // 3d-style
      SGUI_ScreenDrawFullCircle(x+r,y+r,r,c1);
      SGUI_ScreenDraw3DCircle(x+r,y+r,r,ptr->style);
    }   
  }
  else if(ptr->typ==SLED_RECT) {
    if(ptr->style==STYLE_FLAT) {
      if(d>0) {
        // zuerst den rahmen zeichnen
        SGUI_ScreenDrawFullRect(x,y,s,s,c2);
        // dann die led zeichnen
        // etwas kleiner als der rahmen
        if((d*2)>s) d=(s/2);
        SGUI_ScreenDrawFullRect(x+d,y+d,s-2*d,s-2*d,c1);
      }
      else {
        // ohne rahmen
        SGUI_ScreenDrawFullRect(x,y,s,s,c1);
      }     
    }
    else {
      // 3d-style
      SGUI_ScreenDrawFullRect(x,y,s,s,c1);
      SGUI_ScreenDraw3DFrame(x,y,s,s,ptr->style);
    }
  }
}
