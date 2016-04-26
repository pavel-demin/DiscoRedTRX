//--------------------------------------------------------------
// File     : ub_sgui_label.c
// Datum    : 27.07.2015
// Version  : 1.0
// Autor    : UB
// EMail    : mc-4u(@)t-online.de
// Web      : www.mikrocontroller-4u.de
// CPU      : STM32F746
// IDE      : OpenSTM32
// GCC      : 4.9 2015q2
// Module   : SGUI_WINDOW
// Funktion : Labels dienen zur Beschriftung (Strings, Int, Float)
//            Anzeige entweder mit oder ohne Umgebendes Panel
//            Ausrichtung der Schrift (Center, Left, Right)
//--------------------------------------------------------------



//--------------------------------------------------------------
// Includes
//--------------------------------------------------------------
#include "ub_sgui_label.h"




//--------------------------------------------------------------
// interne Funktionen
//--------------------------------------------------------------
void P_SGUI_LabelDraw(SLABEL_t* ptr);



//--------------------------------------------------------------
// ein LABEL neu erzeugen
// x,y = Linke obere Ecke vom Label auf dem Display
// w = Breite (minimum 5 Pixel)
// h = hoehe  (minimum 5 Pixel)
//--------------------------------------------------------------
SLABEL_t* SGUI_LabelCreate(uint16_t x, uint16_t y, uint16_t w, uint16_t h)
{
  SLABEL_t *ptr;
  size_t obj_size;

  if(SGUI_WINDOW.aktiv_window==0) return NULL;
  if(SGUI_WINDOW.akt_win_ptr==NULL) return NULL;
  if(SGUI_LABEL.akt_anz>=(MAX_LABEL_ANZ-1)) return NULL;

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
  obj_size=sizeof(SLABEL_t);
  ptr = (SLABEL_t *) malloc(obj_size);
  if(ptr==NULL) return NULL;  

  // init aller Werte
  ptr->window_nr=SGUI_WINDOW.aktiv_window;
  ptr->typ=SLABEL_ASCII;
  ptr->style=STYLE_LOWERED;
  ptr->xpos=x;
  ptr->ypos=y;
  ptr->width=w;
  ptr->height=h;
  ptr->frame_size=SLABEL_DEF_FRAME_SIZE;
  ptr->col_frame=SLABEL_DEF_FRAME_COL;
  ptr->col_text=SLABEL_DEF_TEXT_COL;
  ptr->col_back=SLABEL_DEF_BACK_COL;
  ptr->txt_ptr=NULL;
  ptr->i32_value=0;
  ptr->f_value=0.0;
  ptr->digits=1;
  ptr->font=SLABEL_DEF_FONT;
  ptr->align=SLABEL_ALIGN_CENTER;
  ptr->status=0x00;
  
  ptr->status|=SLABEL_BIT_PANEL;   // mit Panel

  ptr->status|=SLABEL_BIT_REDRAW;  // redraw at next loop
  SGUI_LABEL.update=true;

  SGUI_LABEL.ptr[SGUI_LABEL.akt_anz]=ptr;
  SGUI_LABEL.akt_anz++;
  
  // benötigter Speicherplatz
  SGUI_WINDOW.ram_used+=obj_size;   

  return ptr;
}

//--------------------------------------------------------------
// Style von einem Label einstellen
// style : [STYLE_FLAT, STYLE_RAISED, STYLE_LOWERED]
//--------------------------------------------------------------
void SGUI_LabelSetStyle(SLABEL_t* ptr, STYLE_TYP_t style)
{
  if(ptr==NULL) return;
  
  if(style!=ptr->style) {
    ptr->style=style;
    ptr->status|=SLABEL_BIT_REDRAW;  // redraw at next loop
    SGUI_LABEL.update=true;
  }  
}

//--------------------------------------------------------------
// Panel von einem Label ein,ausschalten
//--------------------------------------------------------------
void SGUI_LabelSetPanelVisible(SLABEL_t* ptr, bool visible)
{
  if(ptr==NULL) return;

  if(visible==true) {
    if((ptr->status&SLABEL_BIT_PANEL)==0) {
      ptr->status|=SLABEL_BIT_PANEL;
      ptr->status|=SLABEL_BIT_REDRAW;  // redraw at next loop
      SGUI_LABEL.update=true;
    }
  }
  else {
    if((ptr->status&SLABEL_BIT_PANEL)!=0) {
      ptr->status&=~SLABEL_BIT_PANEL;
      ptr->status|=SLABEL_BIT_REDRAW;  // redraw at next loop
      SGUI_LABEL.update=true;
    }
  }  
}

//--------------------------------------------------------------
// Rahmendicke von einem Label einstellen
// px = dicke in pixel (0 = ohne Rahmen)
//--------------------------------------------------------------
void SGUI_LabelSetFrameSize(SLABEL_t* ptr, uint8_t px)
{
  if(ptr==NULL) return;

  if(px!=ptr->frame_size) {
    ptr->frame_size=px;
    ptr->status|=SLABEL_BIT_REDRAW;  // redraw at next loop
    SGUI_LABEL.update=true;
  }
}

//--------------------------------------------------------------
// Farbe von einem Label einstellen
// c1 = Text
// c2 = Rahmen
// c3 = Hintergrund
//--------------------------------------------------------------
void SGUI_LabelSetColor(SLABEL_t* ptr, uint16_t c1, uint16_t c2, uint16_t c3)
{
  if(ptr==NULL) return;

  if(c1!=ptr->col_text) {
    ptr->col_text=c1;
    ptr->status|=SLABEL_BIT_REDRAW;  // redraw at next loop
    SGUI_LABEL.update=true;
  }
  if(c2!=ptr->col_frame) {
    ptr->col_frame=c2;
    ptr->status|=SLABEL_BIT_REDRAW;  // redraw at next loop
    SGUI_LABEL.update=true;
  }
  if(c3!=ptr->col_back) {
    ptr->col_back=c3;
    ptr->status|=SLABEL_BIT_REDRAW;  // redraw at next loop
    SGUI_LABEL.update=true;
  }  
}

//--------------------------------------------------------------
// Text auf einem Label darstellen
//--------------------------------------------------------------
void SGUI_LabelSetText(SLABEL_t* ptr, char* txt)
{
  if(ptr==NULL) return;
  
  if(ptr->typ!=SLABEL_ASCII) {
    ptr->typ=SLABEL_ASCII;
    ptr->status|=SLABEL_BIT_REDRAW;  // redraw at next loop
    SGUI_LABEL.update=true;    
  }
  if(txt!=ptr->txt_ptr) {
    ptr->txt_ptr=txt;
    ptr->status|=SLABEL_BIT_REDRAW;  // redraw at next loop
    SGUI_LABEL.update=true;
  }
}

//--------------------------------------------------------------
// Integer Zahl auf einem Label darstellen
//--------------------------------------------------------------
void SGUI_LabelSetInt(SLABEL_t* ptr, int32_t value)
{
  if(ptr==NULL) return;
  
  if(ptr->typ!=SLABEL_I32) {
    ptr->typ=SLABEL_I32;
    ptr->status|=SLABEL_BIT_REDRAW;  // redraw at next loop
    SGUI_LABEL.update=true;    
  } 
  if(value!=ptr->i32_value) {
    ptr->i32_value=value;
    ptr->status|=SLABEL_BIT_REDRAW;  // redraw at next loop
    SGUI_LABEL.update=true;    
  }  
}

//--------------------------------------------------------------
// Float Zahl auf einem Label darstellen
//--------------------------------------------------------------
void SGUI_LabelSetFloat(SLABEL_t* ptr, float value)
{
  if(ptr==NULL) return;

  if(ptr->typ!=SLABEL_FLOAT) {
    ptr->typ=SLABEL_FLOAT;
    ptr->status|=SLABEL_BIT_REDRAW;  // redraw at next loop
    SGUI_LABEL.update=true;
  }
  if(value!=ptr->f_value) {
    ptr->f_value=value;
    ptr->status|=SLABEL_BIT_REDRAW;  // redraw at next loop
    SGUI_LABEL.update=true;
  }
}

//--------------------------------------------------------------
// Formatierung von einem Label einstellen
// digits : anzahl der stellen
// padding : true => mit fuehrenden Nullen
//--------------------------------------------------------------
void SGUI_LabelSetNumFormat(SLABEL_t* ptr, uint8_t digits, bool padding)
{
  if(ptr==NULL) return;
  
  if(digits!=ptr->digits) {
    ptr->digits=digits;
    ptr->status|=SLABEL_BIT_REDRAW;  // redraw at next loop
    SGUI_LABEL.update=true;    
  }
  if(padding==true) {
    if((ptr->status&SLABEL_BIT_PADDING)==0) {
      ptr->status|=SLABEL_BIT_PADDING;
      ptr->status|=SLABEL_BIT_REDRAW;  // redraw at next loop
      SGUI_LABEL.update=true;        
    }
  }
  else {
    if((ptr->status&SLABEL_BIT_PADDING)!=0) {
      ptr->status&=~SLABEL_BIT_PADDING;
      ptr->status|=SLABEL_BIT_REDRAW;  // redraw at next loop
      SGUI_LABEL.update=true;        
    }    
  }  
}

//--------------------------------------------------------------
// Font von einem Label einstellen
//--------------------------------------------------------------
void SGUI_LabelSetFont(SLABEL_t* ptr, UB_Font* font)
{
  if(ptr==NULL) return;

  if(font!=ptr->font) {
    ptr->font=font;
    ptr->status|=SLABEL_BIT_REDRAW;  // redraw at next loop
    SGUI_LABEL.update=true;
  }
}

//--------------------------------------------------------------
// Alignment von einem Label einstellen
// align : [SLABEL_ALIGN_CENTER, SLABEL_ALIGN_LEFT, SLABEL_ALIGN_RIGHT]
//--------------------------------------------------------------
void SGUI_LabelSetAlignment(SLABEL_t* ptr, SLABEL_ALIGN_t align)
{
  if(ptr==NULL) return;

  if(align!=ptr->align) {
    ptr->align=align;
    ptr->status|=SLABEL_BIT_REDRAW;  // redraw at next loop
    SGUI_LABEL.update=true;
  }
}

//--------------------------------------------------------------
// interne Funktion fuer SGUI
// init der Objekte
//--------------------------------------------------------------
void SGUI_LabelInit(void)
{
  uint32_t n;

  SGUI_LABEL.update=false;
  SGUI_LABEL.akt_anz=0;  
  for(n=0;n<MAX_LABEL_ANZ;n++) {
    SGUI_LABEL.ptr[n]=NULL;
  }
}

//--------------------------------------------------------------
// interne Funktion fuer SGUI
// updated alle Objekte
// (wird aufgerufen, falls sich etwas verändert hat)
//--------------------------------------------------------------
void SGUI_LabelUpdate(void)
{
  uint32_t n;
  uint8_t status=0;

  for(n=0;n<SGUI_LABEL.akt_anz;n++) {
    // nur zeichnen, bei aktivem window
    if(SGUI_WINDOW.aktiv_window==SGUI_LABEL.ptr[n]->window_nr) {
      // test ob label neu gezeichnet werden muss
      status=(SGUI_LABEL.ptr[n]->status&SLABEL_BIT_REDRAW);
      if((status!=0) || (SGUI_WINDOW.update_all==true)) P_SGUI_LabelDraw(SGUI_LABEL.ptr[n]);
    }
  }
}

//--------------------------------------------------------------
// interne Funktion
// zeichnet eine Label
//--------------------------------------------------------------
void P_SGUI_LabelDraw(SLABEL_t* ptr)
{
  uint16_t x,y,w,h,c1,c2,c3,d=0;
  uint16_t fx,fy,l,px_used;

  if(ptr==NULL) return;

  // Flag vom zeichnen zuruecksetzen
  ptr->status&=~SLABEL_BIT_REDRAW;

  x=ptr->xpos;
  y=ptr->ypos;
  w=ptr->width;
  h=ptr->height;

  c1=ptr->col_text;
  c2=ptr->col_back;
  c3=ptr->col_frame;

  if((ptr->status&SLABEL_BIT_PANEL)!=0) {
    // zuerst den hintergrund zeichnen
    if(ptr->style==STYLE_FLAT) {
      d=ptr->frame_size;
      if(d>0) {
        // zuerst den rahmen zeichnen
        SGUI_ScreenDrawFullRect(x,y,w,h,c3);
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
      // 3d-style
      d=SGUI_3D_FRAMESIZE;
      SGUI_ScreenDrawFullRect(x,y,w,h,c2);
      SGUI_ScreenDraw3DFrame(x,y,w,h,ptr->style);
    }
  }

  if(ptr->typ==SLABEL_ASCII) {
    // Text anzeigen  
    if(ptr->txt_ptr==NULL) return;
    l=strlen(ptr->txt_ptr);
    if(l==0) return;
  }
  else if(ptr->typ==SLABEL_I32) {
    // integer in string wandeln
    if((ptr->status&SLABEL_BIT_PADDING)==0) {
      SGUI_WindowFormatInt(ptr->i32_value, ptr->digits, false);
    }
    else {
      SGUI_WindowFormatInt(ptr->i32_value, ptr->digits, true);
    }   
    l=strlen(SGUI_WINDOW.buf);
    if(l==0) return;    
  }
  else if(ptr->typ==SLABEL_FLOAT) {
    // float in string wandeln
    if((ptr->status&SLABEL_BIT_PADDING)==0) {
      SGUI_WindowFormatFloat(ptr->f_value, ptr->digits, false);
    }
    else {
      SGUI_WindowFormatFloat(ptr->f_value, ptr->digits, true);
    }
    l=strlen(SGUI_WINDOW.buf);
    if(l==0) return;
  }

  // ypos ermitteln (center)
  px_used=ptr->font->height;
  fy=y;
  if(h>px_used) {    
    fy=((h-px_used)/2)+y;
  }

  // xpos ermitteln (center,left,right)
  px_used=l*ptr->font->width;
  fx=x;
  if(w>px_used) {
    if(ptr->align==SLABEL_ALIGN_CENTER) fx=((w-px_used)/2)+x;
    if(ptr->align==SLABEL_ALIGN_LEFT) fx=x+d;
    if(ptr->align==SLABEL_ALIGN_RIGHT) fx=x+(w-px_used)-d;
  }

  if(ptr->typ==SLABEL_ASCII) {
    SGUI_ScreenDrawString(fx,fy,ptr->txt_ptr,ptr->font,c1,c2);
  }
  else if((ptr->typ==SLABEL_I32) || (ptr->typ==SLABEL_FLOAT)) {
    SGUI_ScreenDrawString(fx,fy,SGUI_WINDOW.buf,ptr->font,c1,c2);
  }
}
