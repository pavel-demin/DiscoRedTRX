//--------------------------------------------------------------
// File     : ub_sgui_checkbox.c
// Datum    : 27.07.2015
// Version  : 1.0
// Autor    : UB
// EMail    : mc-4u(@)t-online.de
// Web      : www.mikrocontroller-4u.de
// CPU      : STM32F746
// IDE      : OpenSTM32
// GCC      : 4.9 2015q2
// Module   : SGUI_WINDOW
// Funktion : Checkboxen koennen per Touch bedient werden
//            Checkboxen koennen CallBack-Funktion vom User aufrufen
//--------------------------------------------------------------


//--------------------------------------------------------------
// Includes
//--------------------------------------------------------------
#include "ub_sgui_checkbox.h"



//--------------------------------------------------------------
// interne Funktionen
//--------------------------------------------------------------
void P_SGUI_CheckboxDraw(SCHECKBOX_t* ptr);


//--------------------------------------------------------------
// eine CHECKBOX neu erzeugen
// x,y = Linke obere Ecke der Checkbox auf dem Display
// s = Size (minimum 5 Pixel)
//--------------------------------------------------------------
SCHECKBOX_t* SGUI_CheckboxCreate(uint16_t x, uint16_t y, uint16_t s)
{
  SCHECKBOX_t *ptr;
  size_t obj_size;

  if(SGUI_WINDOW.aktiv_window==0) return NULL;
  if(SGUI_WINDOW.akt_win_ptr==NULL) return NULL;
  if(SGUI_CHECKBOX.akt_anz>=(MAX_CHECKBOX_ANZ-1)) return NULL;

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
  obj_size=sizeof(SCHECKBOX_t);
  ptr = (SCHECKBOX_t *) malloc(obj_size);
  if(ptr==NULL) return NULL;  

  // init aller Werte
  ptr->window_nr=SGUI_WINDOW.aktiv_window;
  ptr->fkt_ptr=NULL;
  ptr->style=STYLE_LOWERED;
  ptr->xpos=x;
  ptr->ypos=y;
  ptr->size=s;
  ptr->frame_size=SCHECKBOX_DEF_FRAME_SIZE;
  ptr->col_frame=SCHECKBOX_DEF_FRAME_COL;
  ptr->col_back=SCHECKBOX_DEF_BACK_COL;  
  ptr->status=0x00;

  ptr->status|=SCHECKBOX_BIT_REDRAW;  // redraw at next loop
  SGUI_CHECKBOX.update=true;

  SGUI_CHECKBOX.ptr[SGUI_CHECKBOX.akt_anz]=ptr;
  SGUI_CHECKBOX.akt_anz++;
  
  // benötigter Speicherplatz
  SGUI_WINDOW.ram_used+=obj_size;   

  return ptr;
}

//--------------------------------------------------------------
// Style von einer CHECKBOX einstellen
// style : [STYLE_FLAT, STYLE_RAISED, STYLE_LOWERED]
//--------------------------------------------------------------
void SGUI_CheckboxSetStyle(SCHECKBOX_t* ptr, STYLE_TYP_t style)
{
  if(ptr==NULL) return;
  
  if(style!=ptr->style) {
    ptr->style=style;
    ptr->status|=SCHECKBOX_BIT_REDRAW;  // redraw at next loop
    SGUI_CHECKBOX.update=true;
  }  
}

//--------------------------------------------------------------
// Rahmendicke von einer CHECKBOX einstellen
// px = dicke in pixel (0=ohne Rahmen)
//--------------------------------------------------------------
void SGUI_CheckboxSetFrameSize(SCHECKBOX_t* ptr, uint8_t px)
{
  if(ptr==NULL) return;

  if(px!=ptr->frame_size) {
    ptr->frame_size=px;
    ptr->status|=SCHECKBOX_BIT_REDRAW;  // redraw at next loop
    SGUI_CHECKBOX.update=true;
  }
}

//--------------------------------------------------------------
// Farbe von einer CHECKBOX einstellen
// c1 = Rahmen
// c2 = Hintergrund
//--------------------------------------------------------------
void SGUI_CheckboxSetColor(SCHECKBOX_t* ptr, uint16_t c1, uint16_t c2)
{
  if(ptr==NULL) return;

  if(c1!=ptr->col_frame) {
    ptr->col_frame=c1;
    ptr->status|=SCHECKBOX_BIT_REDRAW;  // redraw at next loop
    SGUI_CHECKBOX.update=true;
  }
  if(c2!=ptr->col_back) {
    ptr->col_back=c2;
    ptr->status|=SCHECKBOX_BIT_REDRAW;  // redraw at next loop
    SGUI_CHECKBOX.update=true;
  }
}

//--------------------------------------------------------------
// CHECKBOX aktiv oder deaktiv schalten
//--------------------------------------------------------------
void SGUI_CheckboxSetAktiv(SCHECKBOX_t* ptr, bool aktiv)
{
  if(ptr==NULL) return;

  if(aktiv==true) {
    if((ptr->status&SCHECKBOX_BIT_AKTIV)==0) {
      ptr->status|=SCHECKBOX_BIT_AKTIV;
      ptr->status|=SCHECKBOX_BIT_REDRAW;  // redraw at next loop
      SGUI_CHECKBOX.update=true;
    }
  }
  else {
    if((ptr->status&SCHECKBOX_BIT_AKTIV)!=0) {
      ptr->status&=~SCHECKBOX_BIT_AKTIV;
      ptr->status|=SCHECKBOX_BIT_REDRAW;  // redraw at next loop
      SGUI_CHECKBOX.update=true;
    }
  }  
}

//--------------------------------------------------------------
// Handler einstellen
// Prototyp : "void name(bool aktiv)"
// "NULL" fuer keinen Handler
//--------------------------------------------------------------
void SGUI_CheckboxSetHandler(SCHECKBOX_t* ptr, void *fkt_ptr)
{
  if(ptr==NULL) return;

  ptr->fkt_ptr=fkt_ptr;
}

//--------------------------------------------------------------
// test ob eine Checkbox aktiv ist
// ret_wert : true  => Checkbox ist aktiv
//            false => Checkbox ist nicht aktiv
//--------------------------------------------------------------
bool SGUI_CheckboxIsAktiv(SCHECKBOX_t* ptr)
{
  if(ptr==NULL) return false;

  if((ptr->status&SCHECKBOX_BIT_AKTIV)==0) {
    return false;
  }

  return true;
}

//--------------------------------------------------------------
// interne Funktion fuer SGUI
// check ob Checkbox per Touch betätigt oder nicht
// (wird zyklisch aufgerufen)
//--------------------------------------------------------------
bool SGUI_CheckboxTouch(bool pressed, uint16_t x, uint16_t y, bool blocked)
{
  bool ret_wert=false;
  uint32_t n;
  uint16_t x1,x2,y1,y2;
  uint8_t m=0;
  static bool old_status=false;

  if(blocked==true) {
    old_status=true;
    return(true);
  }

  if(pressed==false) {
    //-----------------------------------
    // touch ist nicht betaetigt
    //-----------------------------------
    // hier nichts zu tun
    //-----------------------------------
  }
  else if(old_status==false) {
    //-----------------------------------
    // touch ist betaetigt worden
    //-----------------------------------
    m=0;
    for(n=0;n<SGUI_CHECKBOX.akt_anz;n++) {
      // nur beachten bei aktivem window
      if(SGUI_WINDOW.aktiv_window==SGUI_CHECKBOX.ptr[n]->window_nr) {
        // checkbox position
        x1=SGUI_CHECKBOX.ptr[n]->xpos;
        y1=SGUI_CHECKBOX.ptr[n]->ypos;
        x2=SGUI_CHECKBOX.ptr[n]->xpos+SGUI_CHECKBOX.ptr[n]->size;
        y2=SGUI_CHECKBOX.ptr[n]->ypos+SGUI_CHECKBOX.ptr[n]->size;

        // check ob kollision
        if((x>=x1) && (x<=x2)) {
          if((y>=y1) && (y<=y2)) {
            // checkbox gefunden
            m=1;
            ret_wert=true;
            break;
          }
        }
      }
    }
    // check ob kollision gefunden
    if(m==1) {
      // checkbox toggeln
      if((SGUI_CHECKBOX.ptr[n]->status&SCHECKBOX_BIT_AKTIV)==0) {
        // checkbox auf aktiv schalten
        SGUI_CHECKBOX.ptr[n]->status|=SCHECKBOX_BIT_AKTIV;
        // neu zeichnen
        P_SGUI_CheckboxDraw(SGUI_CHECKBOX.ptr[n]);
        // CallBack aufrufen
        if(SGUI_CHECKBOX.ptr[n]->fkt_ptr!=NULL) (*SGUI_CHECKBOX.ptr[n]->fkt_ptr)(true);
      }
      else {
        // checkbox auf inaktiv schalten
        SGUI_CHECKBOX.ptr[n]->status&=~SCHECKBOX_BIT_AKTIV;
        // neu zeichnen
        P_SGUI_CheckboxDraw(SGUI_CHECKBOX.ptr[n]);
        // CallBack aufrufen
        if(SGUI_CHECKBOX.ptr[n]->fkt_ptr!=NULL) (*SGUI_CHECKBOX.ptr[n]->fkt_ptr)(false);
      }
    }
  }

  old_status=pressed;

  return(ret_wert);
}

//--------------------------------------------------------------
// interne Funktion fuer SGUI
// init aller Objekte
//--------------------------------------------------------------
void SGUI_CheckboxInit(void)
{
  uint32_t n;

  SGUI_CHECKBOX.update=false;
  SGUI_CHECKBOX.akt_anz=0;  
  for(n=0;n<MAX_CHECKBOX_ANZ;n++) {
    SGUI_CHECKBOX.ptr[n]=NULL;
  }
}

//--------------------------------------------------------------
// interne Funktion fuer SGUI
// updated alle Objekte
// (wird aufgerufen, falls sich etwas verändert hat)
//--------------------------------------------------------------
void SGUI_CheckboxUpdate(void)
{
  uint32_t n;
  uint8_t status=0;

  for(n=0;n<SGUI_CHECKBOX.akt_anz;n++) {
    // nur zeichnen, bei aktivem window
    if(SGUI_WINDOW.aktiv_window==SGUI_CHECKBOX.ptr[n]->window_nr) {
      // test ob checkbox neu gezeichnet werden muss
      status=(SGUI_CHECKBOX.ptr[n]->status&SCHECKBOX_BIT_REDRAW);
      if((status!=0) || (SGUI_WINDOW.update_all==true)) P_SGUI_CheckboxDraw(SGUI_CHECKBOX.ptr[n]);
    }
  }
}

//--------------------------------------------------------------
// interne Funktion
// zeichnet eine Checkbox
//--------------------------------------------------------------
void P_SGUI_CheckboxDraw(SCHECKBOX_t* ptr)
{
  uint16_t x,y,s,c1,c2,d=0;

  if(ptr==NULL) return;
  
  x=ptr->xpos;
  y=ptr->ypos;
  s=ptr->size;
  c1=ptr->col_frame;
  c2=ptr->col_back;
  
  if(ptr->style==STYLE_FLAT) {
    d=ptr->frame_size;
    if(d>0) {
      // zuerst den rahmen zeichnen
      SGUI_ScreenDrawFullRect(x,y,s,s,c1);
      // dann die checkbox zeichnen
      // etwas kleiner als der rahmen
      if((d*2)>s) d=(s/2);    
      SGUI_ScreenDrawFullRect(x+d,y+d,s-2*d,s-2*d,c2);
    }
    else {
      // kein Rahmen
      SGUI_ScreenDrawFullRect(x,y,s,s,c2);
    }
  }
  else {
    // 3d-style
    SGUI_ScreenDrawFullRect(x,y,s,s,c2);
    SGUI_ScreenDraw3DFrame(x,y,s,s,ptr->style);
  }
  
  if(((ptr->status)&SCHECKBOX_BIT_AKTIV)!=0) {
    // Kreuz "x" als checked markierung zeichnen
    if(ptr->style==STYLE_FLAT) {
      SGUI_ScreenDrawLine(x+1,y,x+s-1,y+s-2,c1);
      SGUI_ScreenDrawLine(x,y,x+s-1,y+s-1,c1);
      SGUI_ScreenDrawLine(x,y+1,x+s-2,y+s-1,c1);

      SGUI_ScreenDrawLine(x+s-2,y,x,y+s-2,c1);
      SGUI_ScreenDrawLine(x+s-1,y,x,y+s-1,c1);
      SGUI_ScreenDrawLine(x+s-1,y+1,x+1,y+s-1,c1);
    }
    else {
      SGUI_ScreenDrawLine(x+3,y+2,x+s-3,y+s-4,c1);
      SGUI_ScreenDrawLine(x+2,y+2,x+s-3,y+s-3,c1);
      SGUI_ScreenDrawLine(x+2,y+3,x+s-4,y+s-3,c1);

      SGUI_ScreenDrawLine(x+s-4,y+2,x+2,y+s-4,c1);
      SGUI_ScreenDrawLine(x+s-3,y+2,x+2,y+s-3,c1);
      SGUI_ScreenDrawLine(x+s-3,y+3,x+3,y+s-3,c1);      
    }
  }
}
