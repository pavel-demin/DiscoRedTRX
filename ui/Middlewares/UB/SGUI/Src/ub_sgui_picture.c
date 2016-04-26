//--------------------------------------------------------------
// File     : ub_sgui_picture.c
// Datum    : 27.07.2015
// Version  : 1.1
// Autor    : UB
// EMail    : mc-4u(@)t-online.de
// Web      : www.mikrocontroller-4u.de
// CPU      : STM32F746
// IDE      : OpenSTM32
// GCC      : 4.9 2015q2
// Module   : SGUI_WINDOW
// Funktion : Anzeigen von Bildern aus dem Flash
//            Bilder koennen per Touch bedient werden
//            Bilder koennen CallBack-Funktion vom User aufrufen
//--------------------------------------------------------------


//--------------------------------------------------------------
// Includes
//--------------------------------------------------------------
#include "ub_sgui_picture.h"

//--------------------------------------------------------------
// interne Funktionen
//--------------------------------------------------------------
void P_SGUI_PictureDraw(SPICTURE_t* ptr);



//--------------------------------------------------------------
// ein Picture neu erzeugen
// x,y = Linke obere Ecke vom Picture auf dem Display
// w = Breite (minimum 5 Pixel)
// h = hoehe  (minimum 5 Pixel)
//--------------------------------------------------------------
SPICTURE_t* SGUI_PictureCreate(uint16_t x, uint16_t y, uint16_t w, uint16_t h)
{
  SPICTURE_t *ptr;
  size_t obj_size;

  if(SGUI_WINDOW.aktiv_window==0) return NULL;
  if(SGUI_WINDOW.akt_win_ptr==NULL) return NULL;
  if(SGUI_PICTURE.akt_anz>=(MAX_PICTURE_ANZ-1)) return NULL;

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
  obj_size=sizeof(SPICTURE_t);
  ptr = (SPICTURE_t *) malloc(obj_size);
  if(ptr==NULL) return NULL;

  // init aller Werte
  ptr->window_nr=SGUI_WINDOW.aktiv_window;
  ptr->fkt_ptr=NULL;
  ptr->mode=SPICTURE_NOBTN;
  ptr->xpos=x;
  ptr->ypos=y;
  ptr->width=w;
  ptr->height=h;
  ptr->img1=NULL;
  ptr->img2=NULL;
  ptr->status=0x00;

  ptr->status|=SPICTURE_BIT_REDRAW;  // redraw at next loop
  SGUI_PICTURE.update=true;

  SGUI_PICTURE.ptr[SGUI_PICTURE.akt_anz]=ptr;
  SGUI_PICTURE.akt_anz++;

  // benötigter Speicherplatz
  SGUI_WINDOW.ram_used+=obj_size;

  return ptr;
}


//--------------------------------------------------------------
// Mode von einem Picture einstellen
// mode : [SPICTURE_NOBTN, SPICTURE_PUSH, SPICTURE_PUSHPULL]
//--------------------------------------------------------------
void SGUI_PictureSetMode(SPICTURE_t* ptr, SPICTURE_MODE_t mode)
{
  if(ptr==NULL) return;

  if(mode!=ptr->mode) {
    ptr->mode=mode;
    ptr->status|=SPICTURE_BIT_REDRAW;  // redraw at next loop
    SGUI_PICTURE.update=true;
  }
}


//--------------------------------------------------------------
// Image-1 von einem Picture einstellen (Bild für inaktiv)
//--------------------------------------------------------------
void SGUI_PictureSetImage1(SPICTURE_t* ptr, UB_Image *img)
{
  if(ptr==NULL) return;

  if(img!=ptr->img1) {
    ptr->img1=img;
    ptr->status|=SPICTURE_BIT_REDRAW;  // redraw at next loop
    SGUI_PICTURE.update=true;
  }
}


//--------------------------------------------------------------
// Image-2 von einem Picture einstellen (Bild für aktiv)
//--------------------------------------------------------------
void SGUI_PictureSetImage2(SPICTURE_t* ptr, UB_Image *img)
{
  if(ptr==NULL) return;

  if(img!=ptr->img2) {
    ptr->img2=img;
    ptr->status|=SPICTURE_BIT_REDRAW;  // redraw at next loop
    SGUI_PICTURE.update=true;
  }
}


//--------------------------------------------------------------
// Picture aktiv oder inaktiv schalten
//--------------------------------------------------------------
void SGUI_PictureSetAktiv(SPICTURE_t* ptr, bool aktiv)
{
  if(ptr==NULL) return;

  if(aktiv==true) {
    if((ptr->status&SPICTURE_BIT_AKTIV)==0) {
      ptr->status|=SPICTURE_BIT_AKTIV;
      ptr->status|=SPICTURE_BIT_REDRAW;  // redraw at next loop
      SGUI_PICTURE.update=true;
    }
  }
  else {
    if((ptr->status&SPICTURE_BIT_AKTIV)!=0) {
      ptr->status&=~SPICTURE_BIT_AKTIV;
      ptr->status|=SPICTURE_BIT_REDRAW;  // redraw at next loop
      SGUI_PICTURE.update=true;
    }
  }
}


//--------------------------------------------------------------
// Handler einstellen
// Prototyp : "void name(bool aktiv)"
// "NULL" fuer keinen Handler
//--------------------------------------------------------------
void SGUI_PictureSetHandler(SPICTURE_t* ptr, void *fkt_ptr)
{
  if(ptr==NULL) return;

  ptr->fkt_ptr=fkt_ptr;
}


//--------------------------------------------------------------
// test ob ein Picture aktiv ist
// ret_wert : true  => picture ist aktiv
//            false => picture ist nicht aktiv
//--------------------------------------------------------------
bool SGUI_PictureIsAktiv(SPICTURE_t* ptr)
{
  if(ptr==NULL) return false;

  if((ptr->status&SPICTURE_BIT_AKTIV)==0) {
    return false;
  }

  return true;
}


//--------------------------------------------------------------
// interne Funktion fuer SGUI
// init der Objekte
//--------------------------------------------------------------
void SGUI_PictureInit(void)
{
  uint32_t n;

  SGUI_PICTURE.update=false;
  SGUI_PICTURE.akt_anz=0;
  for(n=0;n<MAX_PICTURE_ANZ;n++) {
    SGUI_PICTURE.ptr[n]=NULL;
  }
}


//--------------------------------------------------------------
// interne Funktion fuer SGUI
// check ob Pictures per Touch betätigt oder nicht
// (wird zyklisch aufgerufen)
//--------------------------------------------------------------
bool SGUI_PictureTouch(bool pressed, uint16_t x, uint16_t y, bool blocked)
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
    for(n=0;n<SGUI_PICTURE.akt_anz;n++) {
      // nur beachten bei aktivem window
      if(SGUI_WINDOW.aktiv_window==SGUI_PICTURE.ptr[n]->window_nr) {
        // typ = ?
        if(SGUI_PICTURE.ptr[n]->mode==SPICTURE_PUSH) {
          // check ob im Moment aktiv
          if((SGUI_PICTURE.ptr[n]->status&SPICTURE_BIT_AKTIV)!=0) {
            // picture auf inaktiv schalten
            SGUI_PICTURE.ptr[n]->status&=~SPICTURE_BIT_AKTIV;
            // neu zeichnen
            P_SGUI_PictureDraw(SGUI_PICTURE.ptr[n]);
            // CallBack aufrufen
            if(SGUI_PICTURE.ptr[n]->fkt_ptr!=NULL) (*SGUI_PICTURE.ptr[n]->fkt_ptr)(false);
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
    for(n=0;n<SGUI_PICTURE.akt_anz;n++) {
      // nur beachten bei aktivem window
      if((SGUI_WINDOW.aktiv_window==SGUI_PICTURE.ptr[n]->window_nr) && (SGUI_PICTURE.ptr[n]->mode!=SPICTURE_NOBTN)) {
        // picture position bestimmen
        x1=SGUI_PICTURE.ptr[n]->xpos;
        y1=SGUI_PICTURE.ptr[n]->ypos;
        x2=SGUI_PICTURE.ptr[n]->xpos+SGUI_PICTURE.ptr[n]->width;
        y2=SGUI_PICTURE.ptr[n]->ypos+SGUI_PICTURE.ptr[n]->height;

        // check ob kollision
        if((x>=x1) && (x<=x2)) {
          if((y>=y1) && (y<=y2)) {
            // picture gefunden
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
      if(SGUI_PICTURE.ptr[n]->mode==SPICTURE_PUSH) {
        // check auf veraenderung
        if((SGUI_PICTURE.ptr[n]->status&SPICTURE_BIT_AKTIV)==0) {
          // picture auf aktiv schalten
          SGUI_PICTURE.ptr[n]->status|=SPICTURE_BIT_AKTIV;
          // neu zeichnen
          P_SGUI_PictureDraw(SGUI_PICTURE.ptr[n]);
          // CallBack aufrufen
          if(SGUI_PICTURE.ptr[n]->fkt_ptr!=NULL) (*SGUI_PICTURE.ptr[n]->fkt_ptr)(true);
        }
      }
      else if(SGUI_PICTURE.ptr[n]->mode==SPICTURE_PUSHPULL) {
        // picture toggeln
        if((SGUI_PICTURE.ptr[n]->status&SPICTURE_BIT_AKTIV)==0) {
          // picture auf aktiv schalten
          SGUI_PICTURE.ptr[n]->status|=SPICTURE_BIT_AKTIV;
          // neu zeichnen
          P_SGUI_PictureDraw(SGUI_PICTURE.ptr[n]);
          // CallBack aufrufen
          //if(SGUI_PICTURE.ptr[n]->fkt_ptr!=NULL) (*SGUI_PICTURE.ptr[n]->fkt_ptr)(true);
        }
        else {
          // picture auf inaktiv schalten
          SGUI_PICTURE.ptr[n]->status&=~SPICTURE_BIT_AKTIV;
          // neu zeichnen
          P_SGUI_PictureDraw(SGUI_PICTURE.ptr[n]);
          // CallBack aufrufen
          if(SGUI_PICTURE.ptr[n]->fkt_ptr!=NULL) (*SGUI_PICTURE.ptr[n]->fkt_ptr)(false);
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
void SGUI_PictureUpdate(void)
{
  uint32_t n;
  uint8_t status=0;

  for(n=0;n<SGUI_PICTURE.akt_anz;n++) {
    // nur zeichnen, bei aktivem window
    if(SGUI_WINDOW.aktiv_window==SGUI_PICTURE.ptr[n]->window_nr) {
      // test ob picture neu gezeichnet werden muss
      status=(SGUI_PICTURE.ptr[n]->status&SPICTURE_BIT_REDRAW);
      if((status!=0) || (SGUI_WINDOW.update_all==true)) P_SGUI_PictureDraw(SGUI_PICTURE.ptr[n]);
    }
  }
}

//--------------------------------------------------------------
// interne Funktion
// zeichnet ein Picture
//--------------------------------------------------------------
void P_SGUI_PictureDraw(SPICTURE_t* ptr)
{
  uint16_t x,y,w,h;

  if(ptr==NULL) return;

  // Flag vom zeichnen zuruecksetzen
  ptr->status&=~SPICTURE_BIT_REDRAW;

  x=ptr->xpos;
  y=ptr->ypos;
  w=ptr->width;
  h=ptr->height;

  if(((ptr->status)&SPICTURE_BIT_AKTIV)==0) {
    if(ptr->img1==NULL) return;
    SGUI_ScreenDrawImage(ptr->img1,x,y,w,h);
  }
  else {
    if(ptr->img2==NULL) return;
    SGUI_ScreenDrawImage(ptr->img2,x,y,w,h);
  }
}
