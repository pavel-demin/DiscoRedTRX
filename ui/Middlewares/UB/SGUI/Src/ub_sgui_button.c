//--------------------------------------------------------------
// File     : ub_sgui_button.c
// Datum    : 27.07.2015
// Version  : 1.0
// Autor    : UB
// EMail    : mc-4u(@)t-online.de
// Web      : www.mikrocontroller-4u.de
// CPU      : STM32F746
// IDE      : OpenSTM32
// GCC      : 4.9 2015q2
// Module   : SGUI_WINDOW
// Funktion : Buttons koennen per Touch bedient werden
//            Buttons koennen CallBack-Funktion vom User aufrufen
//--------------------------------------------------------------



//--------------------------------------------------------------
// Includes
//--------------------------------------------------------------
#include "ub_sgui_button.h"





//--------------------------------------------------------------
// interne Funktionen
//--------------------------------------------------------------
void P_SGUI_ButtonDraw(SBUTTON_t* ptr);



//--------------------------------------------------------------
// einen Button neu erzeugen
// x,y = Linke obere Ecke vom Button auf dem Display
// w = Breite (minimum 5 Pixel)
// h = hoehe  (minimum 5 Pixel)
//--------------------------------------------------------------
SBUTTON_t* SGUI_ButtonCreate(uint16_t x, uint16_t y, uint16_t w, uint16_t h)
{
  SBUTTON_t *ptr;
  size_t obj_size;

  if(SGUI_WINDOW.aktiv_window==0) return NULL;
  if(SGUI_WINDOW.akt_win_ptr==NULL) return NULL;
  if(SGUI_BUTTON.akt_anz>=(MAX_BTN_ANZ-1)) return NULL;

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
  obj_size=sizeof(SBUTTON_t);
  ptr = (SBUTTON_t *) malloc(obj_size);
  if(ptr==NULL) return NULL;  

  // init aller Werte
  ptr->window_nr=SGUI_WINDOW.aktiv_window;
  ptr->fkt_ptr=NULL;
  ptr->mode=SBUTTON_PUSH;
  ptr->xpos=x;
  ptr->ypos=y;
  ptr->width=w;
  ptr->height=h;
  ptr->frame_size=SBUTTON_DEF_FRAME_SIZE;
  ptr->col_frame=SBUTTON_DEF_FRAME_COL;
  ptr->col_back=SBUTTON_DEF_BACK_COL;
  ptr->txt_ptr=NULL;
  ptr->font=SBUTTON_DEF_FONT;
  ptr->status=0x00;
  
  ptr->status|=SBTN_BIT_3DSTYLE; // 3d-Style

  ptr->status|=SBTN_BIT_REDRAW;  // redraw at next loop
  SGUI_BUTTON.update=true;

  SGUI_BUTTON.ptr[SGUI_BUTTON.akt_anz]=ptr;
  SGUI_BUTTON.akt_anz++;
  
  // benötigter Speicherplatz
  SGUI_WINDOW.ram_used+=obj_size;   

  return ptr;
}

//--------------------------------------------------------------
// Mode von einem Button einstellen
// mode : [SBUTTON_PUSH, SBUTTON_PUSHPULL]
//--------------------------------------------------------------
void SGUI_ButtonSetMode(SBUTTON_t* ptr, SBUTTON_MODE_t mode)
{
  if(ptr==NULL) return;

  if(mode!=ptr->mode) {
    ptr->mode=mode;
    ptr->status|=SBTN_BIT_REDRAW;  // redraw at next loop
    SGUI_BUTTON.update=true;
  }
}

//--------------------------------------------------------------
// Style von einem Button einstellen
// style : [STYLE_FLAT, STYLE_RAISED, STYLE_LOWERED]
//--------------------------------------------------------------
void SGUI_ButtonSetStyle(SBUTTON_t* ptr, STYLE_TYP_t style)
{
  if(ptr==NULL) return;

  if(style==STYLE_FLAT) {
    if((ptr->status&SBTN_BIT_3DSTYLE)!=0) {
      ptr->status&=~SBTN_BIT_3DSTYLE;
      ptr->status|=SBTN_BIT_REDRAW;  // redraw at next loop
      SGUI_BUTTON.update=true;      
    }
  }
  else {
    if((ptr->status&SBTN_BIT_3DSTYLE)==0) {
      ptr->status|=SBTN_BIT_3DSTYLE;
      ptr->status|=SBTN_BIT_REDRAW;  // redraw at next loop
      SGUI_BUTTON.update=true;      
    }    
  }
}

//--------------------------------------------------------------
// Rahmendicke von einem Button einstellen
// px = dicke in pixel (0=ohne Rahmen)
//--------------------------------------------------------------
void SGUI_ButtonSetFrameSize(SBUTTON_t* ptr, uint8_t px)
{
  if(ptr==NULL) return;

  if(px!=ptr->frame_size) {
    ptr->frame_size=px;
    ptr->status|=SBTN_BIT_REDRAW;  // redraw at next loop
    SGUI_BUTTON.update=true;
  }
}

//--------------------------------------------------------------
// Farbe von einem Button einstellen
// c1 = Rahmen
// c2 = Hintergrund
//--------------------------------------------------------------
void SGUI_ButtonSetColor(SBUTTON_t* ptr, uint16_t c1, uint16_t c2)
{
  if(ptr==NULL) return;

  if(c1!=ptr->col_frame) {
    ptr->col_frame=c1;
    ptr->status|=SBTN_BIT_REDRAW;  // redraw at next loop
    SGUI_BUTTON.update=true;
  }
  if(c2!=ptr->col_back) {
    ptr->col_back=c2;
    ptr->status|=SBTN_BIT_REDRAW;  // redraw at next loop
    SGUI_BUTTON.update=true;
  }
}

//--------------------------------------------------------------
// Tetxt von einem Button einstellen
//--------------------------------------------------------------
void SGUI_ButtonSetText(SBUTTON_t* ptr, char* txt)
{
  if(ptr==NULL) return;

  if(txt!=ptr->txt_ptr) {
    ptr->txt_ptr=txt;
    ptr->status|=SBTN_BIT_REDRAW;  // redraw at next loop
    SGUI_BUTTON.update=true;
  }
}

//--------------------------------------------------------------
// Font von einem Button einstellen
//--------------------------------------------------------------
void SGUI_ButtonSetFont(SBUTTON_t* ptr, UB_Font* font)
{
  if(ptr==NULL) return;

  if(font!=ptr->font) {
    ptr->font=font;
    ptr->status|=SBTN_BIT_REDRAW;  // redraw at next loop
    SGUI_BUTTON.update=true;
  }
}

//--------------------------------------------------------------
// Button aktiv oder inaktiv schalten
//--------------------------------------------------------------
void SGUI_ButtonSetAktiv(SBUTTON_t* ptr, bool aktiv)
{
  if(ptr==NULL) return;

  if(aktiv==true) {
    if((ptr->status&SBTN_BIT_AKTIV)==0) {
      ptr->status|=SBTN_BIT_AKTIV;
      ptr->status|=SBTN_BIT_REDRAW;  // redraw at next loop
      SGUI_BUTTON.update=true;
    }
  }
  else {
    if((ptr->status&SBTN_BIT_AKTIV)!=0) {
      ptr->status&=~SBTN_BIT_AKTIV;
      ptr->status|=SBTN_BIT_REDRAW;  // redraw at next loop
      SGUI_BUTTON.update=true;
    }
  }  
}

//--------------------------------------------------------------
// Handler einstellen
// Prototyp : "void name(bool aktiv)"
// "NULL" fuer keinen Handler
//--------------------------------------------------------------
void SGUI_ButtonSetHandler(SBUTTON_t* ptr, void *fkt_ptr)
{
  if(ptr==NULL) return;

  ptr->fkt_ptr=fkt_ptr;
}

//--------------------------------------------------------------
// test ob ein Button aktiv ist
// ret_wert : true  => button ist aktiv
//            false => button ist nicht aktiv
//--------------------------------------------------------------
bool SGUI_ButtonIsAktiv(SBUTTON_t* ptr)
{
  if(ptr==NULL) return false;

  if((ptr->status&SBTN_BIT_AKTIV)==0) {
    return false;
  }

  return true;
}

//--------------------------------------------------------------
// interne Funktion fuer SGUI
// init der Objekte
//--------------------------------------------------------------
void SGUI_ButtonInit(void)
{
  uint32_t n;

  SGUI_BUTTON.update=false;
  SGUI_BUTTON.akt_anz=0;  
  for(n=0;n<MAX_BTN_ANZ;n++) {
    SGUI_BUTTON.ptr[n]=NULL;
  }
}

//--------------------------------------------------------------
// interne Funktion fuer SGUI
// check ob Buttons per Touch betätigt oder nicht
// (wird zyklisch aufgerufen)
//--------------------------------------------------------------
bool SGUI_ButtonTouch(bool pressed, uint16_t x, uint16_t y, bool blocked)
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
    for(n=0;n<SGUI_BUTTON.akt_anz;n++) {
      // nur beachten bei aktivem window
      if(SGUI_WINDOW.aktiv_window==SGUI_BUTTON.ptr[n]->window_nr) {
        // typ = ?
        if(SGUI_BUTTON.ptr[n]->mode==SBUTTON_PUSH) {
          // check ob im Moment aktiv
          if((SGUI_BUTTON.ptr[n]->status&SBTN_BIT_AKTIV)!=0) {
            // button auf inaktiv schalten
            SGUI_BUTTON.ptr[n]->status&=~SBTN_BIT_AKTIV;
            // neu zeichnen
            P_SGUI_ButtonDraw(SGUI_BUTTON.ptr[n]);
            // CallBack aufrufen
            if(SGUI_BUTTON.ptr[n]->fkt_ptr!=NULL) (*SGUI_BUTTON.ptr[n]->fkt_ptr)(false);
          }
        }
      }
    }
  }
  else if(old_status==false) {
    //-----------------------------------
    // touch ist betaetigt worden
    //-----------------------------------    
    m=0;
    for(n=0;n<SGUI_BUTTON.akt_anz;n++) {
      // nur beachten bei aktivem window
      if(SGUI_WINDOW.aktiv_window==SGUI_BUTTON.ptr[n]->window_nr) {   
        // button position bestimmen
        x1=SGUI_BUTTON.ptr[n]->xpos;
        y1=SGUI_BUTTON.ptr[n]->ypos;
        x2=SGUI_BUTTON.ptr[n]->xpos+SGUI_BUTTON.ptr[n]->width;
        y2=SGUI_BUTTON.ptr[n]->ypos+SGUI_BUTTON.ptr[n]->height;

        // check ob kollision
        if((x>=x1) && (x<=x2)) {
          if((y>=y1) && (y<=y2)) {
            // button gefunden
            m=1;
            ret_wert=true;
            break;
          }
        }
      }
    }
    // check ob kollision gefunden
    if(m==1) {
      // typ = ?
      if(SGUI_BUTTON.ptr[n]->mode==SBUTTON_PUSH) {
        // check auf veraenderung
        if((SGUI_BUTTON.ptr[n]->status&SBTN_BIT_AKTIV)==0) {
          // button auf aktiv schalten
          SGUI_BUTTON.ptr[n]->status|=SBTN_BIT_AKTIV;
          // neu zeichnen
          P_SGUI_ButtonDraw(SGUI_BUTTON.ptr[n]);
          // CallBack aufrufen
          if(SGUI_BUTTON.ptr[n]->fkt_ptr!=NULL) (*SGUI_BUTTON.ptr[n]->fkt_ptr)(true);
        }
      }
      else if(SGUI_BUTTON.ptr[n]->mode==SBUTTON_PUSHPULL) {
        // button toggeln
        if((SGUI_BUTTON.ptr[n]->status&SBTN_BIT_AKTIV)==0) {
          // button auf aktiv schalten
          SGUI_BUTTON.ptr[n]->status|=SBTN_BIT_AKTIV;
          // neu zeichnen
          P_SGUI_ButtonDraw(SGUI_BUTTON.ptr[n]);
          // CallBack aufrufen
          if(SGUI_BUTTON.ptr[n]->fkt_ptr!=NULL) (*SGUI_BUTTON.ptr[n]->fkt_ptr)(true);
        }
        else {
          // button auf inaktiv schalten
          SGUI_BUTTON.ptr[n]->status&=~SBTN_BIT_AKTIV;
          // neu zeichnen
          P_SGUI_ButtonDraw(SGUI_BUTTON.ptr[n]);
          // CallBack aufrufen
          if(SGUI_BUTTON.ptr[n]->fkt_ptr!=NULL) (*SGUI_BUTTON.ptr[n]->fkt_ptr)(false);
        }
      }
    }
  }

  old_status=pressed;

  return(ret_wert);
}

//--------------------------------------------------------------
// interne Funktion fuer SGUI
// updated alle Objekte
// (wird aufgerufen, falls sich etwas verändert hat)
//--------------------------------------------------------------
void SGUI_ButtonUpdate(void)
{
  uint32_t n;
  uint8_t status=0;

  for(n=0;n<SGUI_BUTTON.akt_anz;n++) {
    // nur zeichnen, bei aktivem window
    if(SGUI_WINDOW.aktiv_window==SGUI_BUTTON.ptr[n]->window_nr) {
      // test ob button neu gezeichnet werden muss
      status=(SGUI_BUTTON.ptr[n]->status&SBTN_BIT_REDRAW);
      if((status!=0) || (SGUI_WINDOW.update_all==true)) P_SGUI_ButtonDraw(SGUI_BUTTON.ptr[n]);
    }
  }
}

//--------------------------------------------------------------
// interne Funktion
// zeichnet einen Button
//--------------------------------------------------------------
void P_SGUI_ButtonDraw(SBUTTON_t* ptr)
{
  uint16_t x,y,w,h,c1,c2,d=0;
  uint16_t px_used,fx,fy,l;

  if(ptr==NULL) return;

  // Flag vom zeichnen zuruecksetzen
  ptr->status&=~SBTN_BIT_REDRAW;

  x=ptr->xpos;
  y=ptr->ypos;
  w=ptr->width;
  h=ptr->height;

  if(((ptr->status)&SBTN_BIT_AKTIV)==0) {
    c1=ptr->col_frame;
    c2=ptr->col_back;
  }
  else {
    c1=ptr->col_back;
    c2=ptr->col_frame;
  }

  if((ptr->status&SBTN_BIT_3DSTYLE)==0) {
    d=ptr->frame_size;
    if(d>0) {
      // zuerst den rahmen zeichnen
      SGUI_ScreenDrawFullRect(x,y,w,h,c1);
      // dann den button zeichnen
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
    // 3d-style
    SGUI_ScreenDrawFullRect(x,y,w,h,c2);
    if(((ptr->status)&SBTN_BIT_AKTIV)==0) {
      SGUI_ScreenDraw3DFrame(x,y,w,h,STYLE_RAISED);
    }
    else {
      SGUI_ScreenDraw3DFrame(x,y,w,h,STYLE_LOWERED);
    }
  }  

  // schrift anzeigen
  if(ptr->txt_ptr==NULL) return;
  l=strlen(ptr->txt_ptr);
  if(l==0) return;

  // ypos ermitteln (center)
  px_used=ptr->font->height;  
  fy=y;
  if(h>px_used) {
    fy=((h-px_used)/2)+y;
  }

  // xpos ermitteln (center)
  px_used=l*ptr->font->width;
  fx=x;
  if(w>px_used) {
    fx=((w-px_used)/2)+x;
  }

  SGUI_ScreenDrawString(fx,fy,ptr->txt_ptr,ptr->font,c1,c2);
}

