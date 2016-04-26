//--------------------------------------------------------------
// File     : ub_sgui_window.c
// Datum    : 27.07.2015
// Version  : 1.0
// Autor    : UB
// EMail    : mc-4u(@)t-online.de
// Web      : www.mikrocontroller-4u.de
// CPU      : STM32F746
// IDE      : OpenSTM32
// GCC      : 4.9 2015q2
// Module   : SGUI_SCREEN, SGUI_TEXT
// Funktion : Ein "Window" dient als Hintergrund aller anderen
//            Objekte. Alle Buttons, LEDs usw werden mit einem
//            Window verknuepft. Objekte werden nur angezeigt,
//            wenn das entsprechende Window aktiv ist.
//            Main-Windows fuellen das komplette Display
//            Child-Windows besitzen einen Rahmen und
//            sind kleiner als das Display
//--------------------------------------------------------------



//--------------------------------------------------------------
// Includes
//--------------------------------------------------------------
#include "ub_sgui_window.h"




//--------------------------------------------------------------
// interne Funktionen
//--------------------------------------------------------------
void P_SGUI_WindowDraw(SWINDOW_t* ptr);
SWINDOW_t* P_SGUI_WindowGetPtr(uint16_t window_nr);




//--------------------------------------------------------------
// ein Main-Window neu erzeugen
// window_nr : = nummer vom Window [1...n]
//--------------------------------------------------------------
SWINDOW_t* SGUI_WindowCreateMain(uint16_t window_nr)
{
  SWINDOW_t *ptr;
  size_t obj_size;

  // parameter check
  if(window_nr<1) return NULL;

  if(SGUI_WINDOW.akt_anz>=(MAX_WINDOW_ANZ-1)) return NULL;

  // speicher reservieren
  obj_size=sizeof(SWINDOW_t);
  ptr = (SWINDOW_t *) malloc(obj_size);
  if(ptr==NULL) return NULL;
 
  // init aller Werte 
  ptr->window_nr=window_nr;
  ptr->typ=SWINDOW_MAIN;
  ptr->style=STYLE_FLAT;
  ptr->xpos=0;
  ptr->ypos=0;
  ptr->width=SGUI_SCREEN_MAXX;
  ptr->height=SGUI_SCREEN_MAXY;
  ptr->frame_size=0;
  ptr->col_frame=SWINDOW_DEF_FRAME_COL;
  ptr->col_back=SWINDOW_DEF_BACK_COL;
  ptr->status=0x00;

  SGUI_WINDOW.update_window=true;
  SGUI_WINDOW.update_all=true;
  SGUI_WINDOW.update_parent_mode=0;

  SGUI_WINDOW.ptr[SGUI_WINDOW.akt_anz]=ptr;
  SGUI_WINDOW.akt_anz++;

  // aktuelle Window-Nr updaten
  SGUI_WINDOW.aktiv_window=window_nr;
  SGUI_WINDOW.akt_win_ptr=ptr;
  
  // Cursor updaten
  SGUI_TEXT.akt.cursor_x=0;
  SGUI_TEXT.akt.cursor_y=0;
  SGUI_TEXT.akt.cursor_oldx=0;

  // benötigter Speicherplatz
  SGUI_WINDOW.ram_used+=obj_size;

  return ptr;
}

//--------------------------------------------------------------
// ein Child-Window neu erzeugen
// window_nr : = nummer vom Window [1...n]
// x,y = Linke obere Ecke vom Window auf dem Display
// w = Breite (minimum 5 Pixel)
// h = hoehe  (minimum 5 Pixel)
//--------------------------------------------------------------
SWINDOW_t* SGUI_WindowCreateChild(uint16_t window_nr, uint16_t x, uint16_t y, uint16_t w, uint16_t h)
{
  SWINDOW_t *ptr;
  size_t obj_size;

  // parameter check
  if(window_nr<1) return NULL;
  if((w<5) || (h<5)) return NULL;
  if((x>=SGUI_SCREEN_MAXX) || (y>=SGUI_SCREEN_MAXY)) return NULL;
  if((x+w)>SGUI_SCREEN_MAXX) w=(SGUI_SCREEN_MAXX-x);
  if((y+h)>SGUI_SCREEN_MAXY) h=(SGUI_SCREEN_MAXY-y);

  if(SGUI_WINDOW.akt_anz>=(MAX_WINDOW_ANZ-1)) return NULL;

  // speicher reservieren
  obj_size=sizeof(SWINDOW_t);
  ptr = (SWINDOW_t *) malloc(obj_size);
  if(ptr==NULL) return 0;

  // init aller Werte
  ptr->window_nr=window_nr;
  ptr->typ=SWINDOW_CHILD; 
  ptr->style=STYLE_FLAT;
  ptr->xpos=x;
  ptr->ypos=y;
  ptr->width=w;
  ptr->height=h;
  ptr->frame_size=SWINDOW_DEF_FRAME_SIZE;
  ptr->col_frame=SWINDOW_DEF_FRAME_COL;
  ptr->col_back=SWINDOW_DEF_BACK_COL;
  ptr->status=0x00;

  SGUI_WINDOW.update_window=true;
  SGUI_WINDOW.update_all=true;
  SGUI_WINDOW.update_parent_mode=0;

  SGUI_WINDOW.ptr[SGUI_WINDOW.akt_anz]=ptr;
  SGUI_WINDOW.akt_anz++;

  // aktuelle Window-Nr updaten
  SGUI_WINDOW.aktiv_window=window_nr;
  SGUI_WINDOW.akt_win_ptr=ptr;
  
  // Cursor updaten
  SGUI_TEXT.akt.cursor_x=x;
  SGUI_TEXT.akt.cursor_y=y;
  SGUI_TEXT.akt.cursor_oldx=x;

  // benötigter Speicherplatz
  SGUI_WINDOW.ram_used+=obj_size;  

  return ptr;
}

//--------------------------------------------------------------
// Style von einem Window einstellen (nur Child-Window)
// style : [STYLE_FLAT, STYLE_RAISED, STYLE_LOWERED]
//--------------------------------------------------------------
void SGUI_WindowSetStyle(SWINDOW_t* ptr, STYLE_TYP_t style)
{
  if(ptr==NULL) return;
  if(ptr->typ!=SWINDOW_CHILD) return;

  if(style!=ptr->style) {
    ptr->style=style;
    SGUI_WINDOW.update_window=true; 
  }
}

//--------------------------------------------------------------
// Rahmendicke von einem Window einstellen (nur Child-Window)
// px = dicke in pixel (0 = ohne Rahmen)
//--------------------------------------------------------------
void SGUI_WindowSetFrameSize(SWINDOW_t* ptr, uint8_t px)
{
  if(ptr==NULL) return;
  if(ptr->typ!=SWINDOW_CHILD) return;

  if(px!=ptr->frame_size) {
    ptr->frame_size=px;
    SGUI_WINDOW.update_window=true;
  }
}

//--------------------------------------------------------------
// Farbe von einem Window einstellen
// c1 = Rahmen (nur Child-Window)
// c2 = Hintergrund
//--------------------------------------------------------------
void SGUI_WindowSetColor(SWINDOW_t* ptr, uint16_t c1, uint16_t c2)
{
  if(ptr==NULL) return;

  if(ptr->typ==SWINDOW_CHILD) {
    if(c1!=ptr->col_frame) {
      ptr->col_frame=c1;
      SGUI_WINDOW.update_window=true;
    }
  }
  if(c2!=ptr->col_back) {
    ptr->col_back=c2;
    SGUI_WINDOW.update_window=true;
  }
}

//--------------------------------------------------------------
// ein Window anzeigen
// window_nr : = nummer vom Window [1...n]
//               0 = lcd loeschen
//--------------------------------------------------------------
void SGUI_WindowShow(uint16_t window_nr)
{
  if(window_nr>0) {
    if(window_nr!=SGUI_WINDOW.aktiv_window) {
      SGUI_WINDOW.old_window=SGUI_WINDOW.aktiv_window;
      SGUI_WINDOW.aktiv_window=window_nr;
      SGUI_WINDOW.akt_win_ptr=P_SGUI_WindowGetPtr(SGUI_WINDOW.aktiv_window);
      SGUI_WINDOW.update_window=true;
    }
  }
  else if(SGUI_WINDOW.aktiv_window!=0) {
    // bei Window-Nr 0 wird das komplette Display mit
    // der default Hintergrundfarbe gelöscht
    SGUI_WINDOW.old_window=SGUI_WINDOW.aktiv_window;
    SGUI_ScreenDrawFullRect(0,0,SGUI_SCREEN_MAXX,SGUI_SCREEN_MAXY,SWINDOW_DEF_BACK_COL);
    SGUI_WINDOW.aktiv_window=0;
    SGUI_WINDOW.akt_win_ptr=NULL;
  }
}

//--------------------------------------------------------------
// das zuletzt aktive Window anzeigen
//--------------------------------------------------------------
void SGUI_WindowShowPrev(void)
{
  if(SGUI_WINDOW.old_window!=SGUI_WINDOW.aktiv_window) {
    SGUI_WINDOW.aktiv_window=SGUI_WINDOW.old_window;
    SGUI_WINDOW.akt_win_ptr=P_SGUI_WindowGetPtr(SGUI_WINDOW.aktiv_window);
    SGUI_WINDOW.update_window=true;
  }
}

//--------------------------------------------------------------
// auslesen der aktiven Window-Nr
//--------------------------------------------------------------
uint16_t SGUI_WindowGetActivNr(void)
{
  return SGUI_WINDOW.aktiv_window;
}

//--------------------------------------------------------------
// interne Funktion fuer SGUI
// init der Objekte
//--------------------------------------------------------------
void SGUI_WindowInit(void)
{
  uint32_t n;
  
  SGUI_WINDOW.aktiv_window=0;
  SGUI_WINDOW.old_window=0;
  SGUI_WINDOW.akt_win_ptr=NULL;
  SGUI_WINDOW.update_window=false;
  SGUI_WINDOW.update_all=false;
  SGUI_WINDOW.ram_used=0;
  SGUI_WINDOW.akt_anz=0;
  for(n=0;n<MAX_WINDOW_ANZ;n++) {
    SGUI_WINDOW.ptr[n]=NULL;
  }  
}

//--------------------------------------------------------------
// interne Funktion fuer SGUI
// updat vom Window
// (wird aufgerufen, falls sich etwas verändert hat)
//--------------------------------------------------------------
void SGUI_WindowUpdate(void)
{
  uint32_t n;  

  if(SGUI_WINDOW.aktiv_window==0) return;   

  for(n=0;n<SGUI_WINDOW.akt_anz;n++) {
    if(SGUI_WINDOW.aktiv_window==SGUI_WINDOW.ptr[n]->window_nr) {     
      P_SGUI_WindowDraw(SGUI_WINDOW.ptr[n]);
    }
  }
}

//--------------------------------------------------------------
// interne Funktion fuer SGUI
// formatiert eine Integer-Zahl
//--------------------------------------------------------------
void SGUI_WindowFormatInt(int32_t value, uint8_t digits, bool padding)
{
  if(padding==false) {
    // ohne fuehrende null
    if(digits==1) sprintf(SGUI_WINDOW.buf,"%1d",(int)(value));
    else if(digits==2) sprintf(SGUI_WINDOW.buf,"%2d",(int)(value));
    else if(digits==3) sprintf(SGUI_WINDOW.buf,"%3d",(int)(value));
    else if(digits==4) sprintf(SGUI_WINDOW.buf,"%4d",(int)(value));
    else if(digits==5) sprintf(SGUI_WINDOW.buf,"%5d",(int)(value));
    else if(digits==6) sprintf(SGUI_WINDOW.buf,"%6d",(int)(value));
    else if(digits==7) sprintf(SGUI_WINDOW.buf,"%7d",(int)(value));
    else if(digits==8) sprintf(SGUI_WINDOW.buf,"%8d",(int)(value));
    else if(digits==9) sprintf(SGUI_WINDOW.buf,"%9d",(int)(value));
    else if(digits==10) sprintf(SGUI_WINDOW.buf,"%10d",(int)(value));
    else if(digits==11) sprintf(SGUI_WINDOW.buf,"%11d",(int)(value));
    else if(digits==12) sprintf(SGUI_WINDOW.buf,"%12d",(int)(value));
    else sprintf(SGUI_WINDOW.buf,"%d",(int)(value));
  }
  else {
    // mit fuehrender null
    if(digits==1) sprintf(SGUI_WINDOW.buf,"%01d",(int)(value));
    else if(digits==2) sprintf(SGUI_WINDOW.buf,"%02d",(int)(value));
    else if(digits==3) sprintf(SGUI_WINDOW.buf,"%03d",(int)(value));
    else if(digits==4) sprintf(SGUI_WINDOW.buf,"%04d",(int)(value));
    else if(digits==5) sprintf(SGUI_WINDOW.buf,"%05d",(int)(value));
    else if(digits==6) sprintf(SGUI_WINDOW.buf,"%06d",(int)(value));
    else if(digits==7) sprintf(SGUI_WINDOW.buf,"%07d",(int)(value));
    else if(digits==8) sprintf(SGUI_WINDOW.buf,"%08d",(int)(value));
    else if(digits==9) sprintf(SGUI_WINDOW.buf,"%09d",(int)(value));
    else if(digits==10) sprintf(SGUI_WINDOW.buf,"%010d",(int)(value));
    else if(digits==11) sprintf(SGUI_WINDOW.buf,"%011d",(int)(value));
    else if(digits==12) sprintf(SGUI_WINDOW.buf,"%012d",(int)(value));
    else sprintf(SGUI_WINDOW.buf,"%0d",(int)(value));
  }
}

//--------------------------------------------------------------
// interne Funktion fuer SGUI
// formatiert eine Float-Zahl
//--------------------------------------------------------------
void SGUI_WindowFormatFloat(float value, uint8_t digits, bool padding)
{
  int32_t vorkomma;
  uint32_t nachkomma;
  float rest;

  // float umrechnen
  vorkomma=(int32_t)(value);
  if(value>=0) {
    rest = value-(float)(vorkomma);
  }
  else {
    rest = (float)(vorkomma)-value;
  }
  // Faktor 1000 = 3 Nachkommastellen
  nachkomma = (uint32_t)(rest*(float)(1000)+0.5);

  if(padding==false) {
    // ohne fuehrende null (mit 3 Nachkommastellen)
    if(digits==1) sprintf(SGUI_WINDOW.buf,"%1d.%03d",(int)(vorkomma),(int)(nachkomma));
    else if(digits==2) sprintf(SGUI_WINDOW.buf,"%2d.%03d",(int)(vorkomma),(int)(nachkomma));
    else if(digits==3) sprintf(SGUI_WINDOW.buf,"%3d.%03d",(int)(vorkomma),(int)(nachkomma));
    else if(digits==4) sprintf(SGUI_WINDOW.buf,"%4d.%03d",(int)(vorkomma),(int)(nachkomma));
    else if(digits==5) sprintf(SGUI_WINDOW.buf,"%5d.%03d",(int)(vorkomma),(int)(nachkomma));
    else if(digits==6) sprintf(SGUI_WINDOW.buf,"%6d.%03d",(int)(vorkomma),(int)(nachkomma));
    else if(digits==7) sprintf(SGUI_WINDOW.buf,"%7d.%03d",(int)(vorkomma),(int)(nachkomma));
    else if(digits==8) sprintf(SGUI_WINDOW.buf,"%8d.%03d",(int)(vorkomma),(int)(nachkomma));
    else if(digits==9) sprintf(SGUI_WINDOW.buf,"%9d.%03d",(int)(vorkomma),(int)(nachkomma));
    else if(digits==10) sprintf(SGUI_WINDOW.buf,"%10d.%03d",(int)(vorkomma),(int)(nachkomma));
    else if(digits==11) sprintf(SGUI_WINDOW.buf,"%11d.%03d",(int)(vorkomma),(int)(nachkomma));
    else if(digits==12) sprintf(SGUI_WINDOW.buf,"%12d.%03d",(int)(vorkomma),(int)(nachkomma));
    else sprintf(SGUI_WINDOW.buf,"%d.%03d",(int)(vorkomma),(int)(nachkomma));
  }
  else {
    // mit fuehrender null (mit 3 Nachkommastellen)
    if(digits==1) sprintf(SGUI_WINDOW.buf,"%01d.%03d",(int)(vorkomma),(int)(nachkomma));
    else if(digits==2) sprintf(SGUI_WINDOW.buf,"%02d.%03d",(int)(vorkomma),(int)(nachkomma));
    else if(digits==3) sprintf(SGUI_WINDOW.buf,"%03d.%03d",(int)(vorkomma),(int)(nachkomma));
    else if(digits==4) sprintf(SGUI_WINDOW.buf,"%04d.%03d",(int)(vorkomma),(int)(nachkomma));
    else if(digits==5) sprintf(SGUI_WINDOW.buf,"%05d.%03d",(int)(vorkomma),(int)(nachkomma));
    else if(digits==6) sprintf(SGUI_WINDOW.buf,"%06d.%03d",(int)(vorkomma),(int)(nachkomma));
    else if(digits==7) sprintf(SGUI_WINDOW.buf,"%07d.%03d",(int)(vorkomma),(int)(nachkomma));
    else if(digits==8) sprintf(SGUI_WINDOW.buf,"%08d.%03d",(int)(vorkomma),(int)(nachkomma));
    else if(digits==9) sprintf(SGUI_WINDOW.buf,"%09d.%03d",(int)(vorkomma),(int)(nachkomma));
    else if(digits==10) sprintf(SGUI_WINDOW.buf,"%010d.%03d",(int)(vorkomma),(int)(nachkomma));
    else if(digits==11) sprintf(SGUI_WINDOW.buf,"%011d.%03d",(int)(vorkomma),(int)(nachkomma));
    else if(digits==12) sprintf(SGUI_WINDOW.buf,"%012d.%03d",(int)(vorkomma),(int)(nachkomma));
    else sprintf(SGUI_WINDOW.buf,"%0d.%03d",(int)(vorkomma),(int)(nachkomma));
  }
}

//--------------------------------------------------------------
// interne Funktion
// zeichnet eine Window
//--------------------------------------------------------------
void P_SGUI_WindowDraw(SWINDOW_t* ptr)
{
  uint16_t x,y,w,h,c1,c2,d;

  if(ptr==NULL) return;

  x=ptr->xpos;
  y=ptr->ypos;
  w=ptr->width;
  h=ptr->height;
  c1=ptr->col_frame;
  c2=ptr->col_back;

  if(ptr->typ==SWINDOW_CHILD) {
    if(ptr->style==STYLE_FLAT) {
      // bei child zuerst den Rahmen zeichnen
      d=ptr->frame_size;
      if(d>0) {
        SGUI_ScreenDrawFullRect(x,y,w,h,c1);
        // dann das fenster zeichnen
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
      SGUI_ScreenDraw3DFrame(x,y,w,h,ptr->style);
    }
  }
  else {
    // main-window ohne rahmen
    SGUI_ScreenDrawFullRect(x,y,w,h,c2);
  }
}

//--------------------------------------------------------------
// interne Funktion
// sucht Pointer vom Window mit window_nr
//--------------------------------------------------------------
SWINDOW_t* P_SGUI_WindowGetPtr(uint16_t window_nr)
{
  SWINDOW_t *ptr;
  uint32_t n;

  ptr=NULL;

  if(window_nr>0) {
    for(n=0;n<SGUI_WINDOW.akt_anz;n++) {
      if(SGUI_WINDOW.ptr[n]->window_nr==window_nr) {
        ptr=SGUI_WINDOW.ptr[n];
      }
    }
  }

  return ptr;
}


