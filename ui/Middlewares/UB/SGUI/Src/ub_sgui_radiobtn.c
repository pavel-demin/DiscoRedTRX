//--------------------------------------------------------------
// File     : ub_sgui_radiobtn.c
// Datum    : 27.07.2015
// Version  : 1.0
// Autor    : UB
// EMail    : mc-4u(@)t-online.de
// Web      : www.mikrocontroller-4u.de
// CPU      : STM32F746
// IDE      : OpenSTM32
// GCC      : 4.9 2015q2
// Module   : SGUI_WINDOW
// Funktion : RadioButtons koennen per Touch bedient werden
//            RadioButtons koennen CallBack-Funktion vom User aufrufen
//            Jeder RadioButton besitzt eine "group_nr"
//            Es kann immer nur ein Button der gleich gruppe aktiv sein
//--------------------------------------------------------------


//--------------------------------------------------------------
// Includes
//--------------------------------------------------------------
#include "ub_sgui_radiobtn.h"





//--------------------------------------------------------------
// interne Funktionen
//--------------------------------------------------------------
void P_SGUI_RadioButtonDraw(SRBTN_t* ptr);


//--------------------------------------------------------------
// einen RadioButton neu erzeugen
// x,y = Linke obere Ecke vom RadioButton auf dem Display
// s = Size (minimum 5 Pixel)
//--------------------------------------------------------------
SRBTN_t* SGUI_RadioButtonCreate(uint16_t x, uint16_t y, uint16_t s)
{
  SRBTN_t *ptr;
  size_t obj_size;

  if(SGUI_WINDOW.aktiv_window==0) return NULL;
  if(SGUI_WINDOW.akt_win_ptr==NULL) return NULL;
  if(SGUI_RBTN.akt_anz>=(MAX_RBTN_ANZ-1)) return NULL;

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
  obj_size=sizeof(SRBTN_t);
  ptr = (SRBTN_t *) malloc(obj_size);
  if(ptr==NULL) return NULL;  

  // init aller Werte
  ptr->window_nr=SGUI_WINDOW.aktiv_window;
  ptr->fkt_ptr=NULL;
  ptr->group_nr=0;
  ptr->style=STYLE_LOWERED;
  ptr->xpos=x;
  ptr->ypos=y;
  ptr->size=s;
  ptr->frame_size=SRBTN_DEF_FRAME_SIZE;
  ptr->col_frame=SRBTN_DEF_FRAME_COL;
  ptr->col_back=SRBTN_DEF_BACK_COL;  
  ptr->status=0x00;

  ptr->status|=SRBTN_BIT_REDRAW;  // redraw at next loop
  SGUI_RBTN.update=true;

  SGUI_RBTN.ptr[SGUI_RBTN.akt_anz]=ptr;
  SGUI_RBTN.akt_anz++;
  
  // benötigter Speicherplatz
  SGUI_WINDOW.ram_used+=obj_size;  

  return ptr;
}

//--------------------------------------------------------------
// Gruppe von einem RadioButton einstellen
// group_nr = nummer der Gruppe [0...n]
//--------------------------------------------------------------
void SGUI_RadioButtonSetGroup(SRBTN_t* ptr, uint16_t group_nr)
{
  if(ptr==NULL) return;
  
  ptr->group_nr=group_nr;
}

//--------------------------------------------------------------
// Style von einem RadioButton einstellen
// style : [STYLE_FLAT, STYLE_RAISED, STYLE_LOWERED]
//--------------------------------------------------------------
void SGUI_RadioButtonSetStyle(SRBTN_t* ptr, STYLE_TYP_t style)
{
  if(ptr==NULL) return;
  
  if(style!=ptr->style) {
    ptr->style=style;
    ptr->status|=SRBTN_BIT_REDRAW;  // redraw at next loop
    SGUI_RBTN.update=true;
  }  
} 

//--------------------------------------------------------------
// Rahmendicke von einem RadioButton einstellen
// px = dicke in pixel (0 = ohne Rahmen)
//--------------------------------------------------------------
void SGUI_RadioButtonSetFrameSize(SRBTN_t* ptr, uint8_t px)
{
  if(ptr==NULL) return;

  if(px!=ptr->frame_size) {
    ptr->frame_size=px;
    ptr->status|=SRBTN_BIT_REDRAW;  // redraw at next loop
    SGUI_RBTN.update=true;
  }
}

//--------------------------------------------------------------
// Farbe von einem RadioButton einstellen
// c1 = Rahmen
// c2 = Hintergrund
//--------------------------------------------------------------
void SGUI_RadioButtonSetColor(SRBTN_t* ptr, uint16_t c1, uint16_t c2)
{
  if(ptr==NULL) return;

  if(c1!=ptr->col_frame) {
    ptr->col_frame=c1;
    ptr->status|=SRBTN_BIT_REDRAW;  // redraw at next loop
    SGUI_RBTN.update=true;
  }
  if(c2!=ptr->col_back) {
    ptr->col_back=c2;
    ptr->status|=SRBTN_BIT_REDRAW;  // redraw at next loop
    SGUI_RBTN.update=true;
  }
}

//--------------------------------------------------------------
// RadioButton aktiv schalten
// die anderen RadioButtons der gleichen Gruppe werden
// inaktiv geschaltet
//--------------------------------------------------------------
void SGUI_RadioButtonSetAktiv(SRBTN_t* ptr)
{
  uint32_t n;
  
  if(ptr==NULL) return;
  
  if((ptr->status&SRBTN_BIT_AKTIV)==0) {
    // zuerst alle buttons der gleichen gruppe deaktivieren
    for(n=0;n<SGUI_RBTN.akt_anz;n++) {      
      if(SGUI_RBTN.ptr[n]->group_nr==ptr->group_nr) {                
        if((SGUI_RBTN.ptr[n]->status&SRBTN_BIT_AKTIV)!=0) {
          // auf inaktiv schalten
          SGUI_RBTN.ptr[n]->status&=~SRBTN_BIT_AKTIV;
          SGUI_RBTN.ptr[n]->status|=SRBTN_BIT_REDRAW;  // redraw at next loop
        }
      }
    }    
    
    // radio button aktivieren
    ptr->status|=SRBTN_BIT_AKTIV;
    ptr->status|=SRBTN_BIT_REDRAW;  // redraw at next loop
    SGUI_RBTN.update=true;
  }  
}

//--------------------------------------------------------------
// Handler einstellen
// Prototyp : "void name(bool aktiv)"
// "NULL" fuer keinen Handler
//--------------------------------------------------------------
void SGUI_RadioButtonSetHandler(SRBTN_t* ptr, void *fkt_ptr)
{
  if(ptr==NULL) return;

  ptr->fkt_ptr=fkt_ptr;
}

//--------------------------------------------------------------
// test ob der RadioButton aktiv ist
// ret_wert : true  => RadioButton ist aktiv
//            false => RadioButton ist nicht aktiv
//--------------------------------------------------------------
bool SGUI_RadioButtonIsAktiv(SRBTN_t* ptr)
{
  if(ptr==NULL) return false;

  if((ptr->status&SRBTN_BIT_AKTIV)==0) {
    return false;
  }

  return true;
}

//--------------------------------------------------------------
// interne Funktion fuer SGUI
// check ob RadioButton per Touch betätigt oder nicht
// (wird zyklisch aufgerufen)
//--------------------------------------------------------------
bool SGUI_RadioButtonTouch(bool pressed, uint16_t x, uint16_t y, bool blocked)
{
  bool ret_wert=false;
  uint32_t n,k;
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
    // alle sichtbaren RadioButtons auf kollision pruefen
    //-----------------------------------
    m=0;
    for(n=0;n<SGUI_RBTN.akt_anz;n++) {
      // nur beachten bei aktivem window
      if(SGUI_WINDOW.aktiv_window==SGUI_RBTN.ptr[n]->window_nr) {
        // radiobutton position
        x1=SGUI_RBTN.ptr[n]->xpos;
        y1=SGUI_RBTN.ptr[n]->ypos;
        x2=SGUI_RBTN.ptr[n]->xpos+SGUI_RBTN.ptr[n]->size;
        y2=SGUI_RBTN.ptr[n]->ypos+SGUI_RBTN.ptr[n]->size;

        // check ob kollision
        if((x>=x1) && (x<=x2)) {
          if((y>=y1) && (y<=y2)) {
            // radiobutton gefunden
            m=1;
            ret_wert=true;
            break;
          }
        }
      }
    }
    // check ob kollision gefunden
    if(m==1) {
      // check auf aktivierung
      if((SGUI_RBTN.ptr[n]->status&SRBTN_BIT_AKTIV)==0) {
        // alle buttons der gruppe auf inaktiv
        for(k=0;k<SGUI_RBTN.akt_anz;k++) {
          if(SGUI_RBTN.ptr[k]->group_nr==SGUI_RBTN.ptr[n]->group_nr) {
            if((SGUI_RBTN.ptr[k]->status&SRBTN_BIT_AKTIV)!=0) {
              // radiobutton auf inaktiv schalten
              SGUI_RBTN.ptr[k]->status&=~SRBTN_BIT_AKTIV;
              // neu zeichnen
              P_SGUI_RadioButtonDraw(SGUI_RBTN.ptr[k]);
            }
          }
        }
        // radiobutton auf aktiv schalten
        SGUI_RBTN.ptr[n]->status|=SRBTN_BIT_AKTIV;
        // neu zeichnen
        P_SGUI_RadioButtonDraw(SGUI_RBTN.ptr[n]);
        // CallBack aufrufen
        if(SGUI_RBTN.ptr[n]->fkt_ptr!=NULL) (*SGUI_RBTN.ptr[n]->fkt_ptr)(true);
      }
    }
  }

  old_status=pressed;

  return(ret_wert);
}

//--------------------------------------------------------------
// interne Funktion fuer SGUI
// init der Objekte
//--------------------------------------------------------------
void SGUI_RadioButtonInit(void)
{
  uint32_t n;

  SGUI_RBTN.update=false;
  SGUI_RBTN.akt_anz=0;  
  for(n=0;n<MAX_RBTN_ANZ;n++) {
    SGUI_RBTN.ptr[n]=NULL;
  }
}

//--------------------------------------------------------------
// interne Funktion fuer SGUI
// updated alle Objekte
// (wird aufgerufen, falls sich etwas verändert hat)
//--------------------------------------------------------------
void SGUI_RadioButtonUpdate(void)
{
  uint32_t n;
  uint8_t status=0;

  for(n=0;n<SGUI_RBTN.akt_anz;n++) {
    // nur zeichnen, bei aktivem window
    if(SGUI_WINDOW.aktiv_window==SGUI_RBTN.ptr[n]->window_nr) {
      // test ob radiobutton neu gezeichnet werden muss
      status=(SGUI_RBTN.ptr[n]->status&SRBTN_BIT_REDRAW);
      if((status!=0) || (SGUI_WINDOW.update_all==true)) P_SGUI_RadioButtonDraw(SGUI_RBTN.ptr[n]);
    }
  }
}

//--------------------------------------------------------------
// interne Funktion
// zeichnet einen RadioButton
//--------------------------------------------------------------
void P_SGUI_RadioButtonDraw(SRBTN_t* ptr)
{
  uint16_t x,y,s,c1,c2,d=0,r;
  float f;

  if(ptr==NULL) return;

  // Flag vom zeichnen zuruecksetzen
  ptr->status&=~SRBTN_BIT_REDRAW;

  x=ptr->xpos;
  y=ptr->ypos;
  s=ptr->size;
  c1=ptr->col_frame;
  c2=ptr->col_back;

  r=s/2; // Radius
  if(ptr->style==STYLE_FLAT) {
    d=ptr->frame_size;
    if(d>0) {
      // zuerst den rahmen zeichnen
      SGUI_ScreenDrawFullCircle(x+r,y+r,r,c1);
      // dann den hintergrund zeichnen
      // etwas kleiner als der rahmen
      if(d>r) d=r;
      SGUI_ScreenDrawFullCircle(x+r,y+r,r-d,c2);
    }
    else {
      // kein Rahmen
      SGUI_ScreenDrawFullCircle(x+r,y+r,r,c2);
    }
  }
  else {
    // 3d-style
    d=SGUI_3D_FRAMESIZE;
    SGUI_ScreenDrawFullCircle(x+r,y+r,r,c2);
    SGUI_ScreenDraw3DCircle(x+r,y+r,r,ptr->style);
  }
 
  if(((ptr->status)&SRBTN_BIT_AKTIV)!=0) {
    // jetzt den punkt zeichnen
    f=(float)(r-d)*0.8;
    d=(uint16_t)(f);
    SGUI_ScreenDrawFullCircle(x+r,y+r,d,c1);
  }    
}
