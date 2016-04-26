//--------------------------------------------------------------
// File     : ub_sgui_floatedit.c
// Datum    : 27.07.2015
// Version  : 1.1
// Autor    : UB
// EMail    : mc-4u(@)t-online.de
// Web      : www.mikrocontroller-4u.de
// CPU      : STM32F746
// IDE      : OpenSTM32
// GCC      : 4.9 2015q2
// Module   : SGUI_WINDOW
// Funktion : FloatEdits koennen per Touch bedient werden
//            FloatEdits koennen CallBack-Funktion vom User aufrufen
//            FloatEdits dienen zum anzeigen von Float-Zahlenwerten
//            und könnnen per Up/Down-Tasten zwischen
//            Min- und Maxwert geändert eingestellt werden.
//--------------------------------------------------------------


//--------------------------------------------------------------
// Includes
//--------------------------------------------------------------
#include "ub_sgui_floatedit.h"



//--------------------------------------------------------------
// interne Funktionen
//--------------------------------------------------------------
void P_SGUI_FloatEditDraw(SFLOATEDIT_t* ptr);


//--------------------------------------------------------------
// ein FloatEdit neu erzeugen
// x,y = Linke obere Ecke der FloatEdit auf dem Display
// w = Breite (minimum 5 Pixel)
// h = hoehe  (minimum 5 Pixel)
//--------------------------------------------------------------
SFLOATEDIT_t* SGUI_FloatEditCreate(uint16_t x, uint16_t y, uint16_t w, uint16_t h)
{
  SFLOATEDIT_t *ptr;
  size_t obj_size;  

  if(SGUI_WINDOW.aktiv_window==0) return NULL;
  if(SGUI_WINDOW.akt_win_ptr==NULL) return NULL;
  if(SGUI_FLOATEDIT.akt_anz>=(MAX_FLOATEDIT_ANZ-1)) return NULL;

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
  obj_size=sizeof(SFLOATEDIT_t);
  ptr = (SFLOATEDIT_t *) malloc(obj_size);
  if(ptr==NULL) return NULL;
  
  // init aller Werte
  ptr->window_nr=SGUI_WINDOW.aktiv_window;
  ptr->fkt_ptr=NULL;
  ptr->typ=SFLOATEDIT_H;
  ptr->xpos=x;
  ptr->ypos=y;
  ptr->width=w;
  ptr->height=h;	
  ptr->frame_size=SFLOATEDIT_DEF_FRAME_SIZE;
  ptr->col_text=SFLOATEDIT_DEF_TEXT_COL;
  ptr->col_frame=SFLOATEDIT_DEF_FRAME_COL;
  ptr->col_back=SFLOATEDIT_DEF_BACK_COL;   
  ptr->f_value=0.0;
  ptr->f_minvalue=0.0;
  ptr->f_maxvalue=100.0;
  ptr->step_value=10.0;
  ptr->digits=1;  
  ptr->font=SFLOATEDIT_DEF_FONT;
  ptr->align=SFLOATEDIT_ALIGN_CENTER;
  ptr->status=0x00;
  
  ptr->status|=SFLOATEDIT_BIT_ARROW;   // arrow visible
  ptr->status|=SFLOATEDIT_BIT_3DSTYLE; // 3d-style

  ptr->status|=SFLOATEDIT_BIT_REDRAW;  // redraw at next loop
  SGUI_FLOATEDIT.update=true;

  SGUI_FLOATEDIT.ptr[SGUI_FLOATEDIT.akt_anz]=ptr;
  SGUI_FLOATEDIT.akt_anz++;
  
  // benötigter Speicherplatz
  SGUI_WINDOW.ram_used+=obj_size;  

  return ptr;
}

//--------------------------------------------------------------
// Typ von einem FloatEdit einstellen
// typ : [SFLOATEDIT_H, SFLOATEDIT_H2, SFLOATEDIT_V]
//--------------------------------------------------------------
void SGUI_FloatEditSetTyp(SFLOATEDIT_t* ptr, SFLOATEDIT_TYP_t typ)
{
  if(ptr==NULL) return;

  if(typ!=ptr->typ) {
    ptr->typ=typ;
    ptr->status|=SFLOATEDIT_BIT_REDRAW;  // redraw at next loop
    SGUI_FLOATEDIT.update=true;
  } 
}

//--------------------------------------------------------------
// Style von einem FloatEdit einstellen
// style : [STYLE_FLAT, STYLE_RAISED, STYLE_LOWERED]
//--------------------------------------------------------------
void SGUI_FloatEditSetStyle(SFLOATEDIT_t* ptr, STYLE_TYP_t style)
{
  if(ptr==NULL) return;
  
  if(style==STYLE_FLAT) {
    if((ptr->status&SFLOATEDIT_BIT_3DSTYLE)!=0) {
      ptr->status&=~SFLOATEDIT_BIT_3DSTYLE;
      ptr->status|=SFLOATEDIT_BIT_REDRAW;  // redraw at next loop
      SGUI_FLOATEDIT.update=true;      
    }
  }
  else {
    if((ptr->status&SFLOATEDIT_BIT_3DSTYLE)==0) {
      ptr->status|=SFLOATEDIT_BIT_3DSTYLE;
      ptr->status|=SFLOATEDIT_BIT_REDRAW;  // redraw at next loop
      SGUI_FLOATEDIT.update=true;      
    }    
  } 
}

//--------------------------------------------------------------
// Formatierung von einem FloatEdit einstellen
// digits : anzahl der stellen
// padding : true => mit fuehrenden Nullen
//--------------------------------------------------------------
void SGUI_FloatEditSetNumFormat(SFLOATEDIT_t* ptr, uint8_t digits, bool padding)
{
  if(ptr==NULL) return;
  
  if(digits!=ptr->digits) {
    ptr->digits=digits;
    ptr->status|=SFLOATEDIT_BIT_REDRAW;  // redraw at next loop
    SGUI_FLOATEDIT.update=true;    
  }
  if(padding==true) {
    if((ptr->status&SFLOATEDIT_BIT_PADDING)==0) {
      ptr->status|=SFLOATEDIT_BIT_PADDING;
      ptr->status|=SFLOATEDIT_BIT_REDRAW;  // redraw at next loop
      SGUI_FLOATEDIT.update=true;        
    }
  }
  else {
    if((ptr->status&SFLOATEDIT_BIT_PADDING)!=0) {
      ptr->status&=~SFLOATEDIT_BIT_PADDING;
      ptr->status|=SFLOATEDIT_BIT_REDRAW;  // redraw at next loop
      SGUI_FLOATEDIT.update=true;        
    }    
  }
}

//--------------------------------------------------------------
// Alignment von einem FloatEdit einstellen
// align : [SFLOATEDIT_ALIGN_CENTER, SFLOATEDIT_ALIGN_LEFT, SFLOATEDIT_ALIGN_RIGHT]
//--------------------------------------------------------------
void SGUI_FloatEditSetAlignment(SFLOATEDIT_t* ptr, SFLOATEDIT_ALIGN_t align)
{
  if(ptr==NULL) return;
  
  if(align!=ptr->align) {
    ptr->align=align;
    ptr->status|=SFLOATEDIT_BIT_REDRAW;  // redraw at next loop
    SGUI_FLOATEDIT.update=true;    
  }  
}

//--------------------------------------------------------------
// PfeilButtons von einem FloatEdit ein,ausschalten
//--------------------------------------------------------------
void SGUI_FloatEditSetArrowVisible(SFLOATEDIT_t* ptr, bool visible)
{
  if(ptr==NULL) return;
  
  if(visible==true) {
    if((ptr->status&SFLOATEDIT_BIT_ARROW)==0) {
      ptr->status|=SFLOATEDIT_BIT_ARROW;
      ptr->status|=SFLOATEDIT_BIT_REDRAW;  // redraw at next loop
      SGUI_FLOATEDIT.update=true;      
    }
  }
  else {
    if((ptr->status&SFLOATEDIT_BIT_ARROW)!=0) {
      ptr->status&=~SFLOATEDIT_BIT_ARROW;
      ptr->status|=SFLOATEDIT_BIT_REDRAW;  // redraw at next loop
      SGUI_FLOATEDIT.update=true;      
    }    
  } 
}

//--------------------------------------------------------------
// Rahmendicke von einer FloatEdit einstellen
// px = dicke in pixel (0=ohne Rahmen)
//--------------------------------------------------------------
void SGUI_FloatEditSetFrameSize(SFLOATEDIT_t* ptr, uint8_t px)
{
  if(ptr==NULL) return;

  if(px!=ptr->frame_size) {
    ptr->frame_size=px;
    ptr->status|=SFLOATEDIT_BIT_REDRAW;  // redraw at next loop
    SGUI_FLOATEDIT.update=true;
  }
}

//--------------------------------------------------------------
// Farbe von einer FloatEdit einstellen
// c1 = Text
// c2 = Rahmen
// c3 = Hintergrund
//--------------------------------------------------------------
void SGUI_FloatEditSetColor(SFLOATEDIT_t* ptr, uint16_t c1, uint16_t c2, uint16_t c3)
{
  if(ptr==NULL) return;

  if(c1!=ptr->col_text) {
    ptr->col_text=c1;
    ptr->status|=SFLOATEDIT_BIT_REDRAW;  // redraw at next loop
    SGUI_FLOATEDIT.update=true;
  }
  if(c2!=ptr->col_frame) {
    ptr->col_frame=c2;
    ptr->status|=SFLOATEDIT_BIT_REDRAW;  // redraw at next loop
    SGUI_FLOATEDIT.update=true;
  }
  if(c3!=ptr->col_back) {
    ptr->col_back=c3;
    ptr->status|=SFLOATEDIT_BIT_REDRAW;  // redraw at next loop
    SGUI_FLOATEDIT.update=true;
  }  
}

//--------------------------------------------------------------
// Font von einer FloatEdit einstellen
//--------------------------------------------------------------
void SGUI_FloatEditSetFont(SFLOATEDIT_t* ptr, UB_Font* font)
{
  if(ptr==NULL) return;

  if(font!=ptr->font) {
    ptr->font=font;
    ptr->status|=SFLOATEDIT_BIT_REDRAW;  // redraw at next loop
    SGUI_FLOATEDIT.update=true;
  }
}

//--------------------------------------------------------------
// min und maxwert vom Floatedit einstellen
// max muss > als minwert sein
//--------------------------------------------------------------
void SGUI_FloatEditSetMinMax(SFLOATEDIT_t* ptr, float min, float max)
{
  if(ptr==NULL) return;  
  if(min>=max) return;
    
  if(min!=ptr->f_minvalue) {
    ptr->f_minvalue=min;
    ptr->status|=SFLOATEDIT_BIT_REDRAW;  // redraw at next loop
    SGUI_FLOATEDIT.update=true;    
  }
  if(max!=ptr->f_maxvalue) {
    ptr->f_maxvalue=max;
    ptr->status|=SFLOATEDIT_BIT_REDRAW;  // redraw at next loop
    SGUI_FLOATEDIT.update=true;    
  }  
  // istwert event. anpassen
  if(ptr->f_value<min) ptr->f_value=min;
  if(ptr->f_value>max) ptr->f_value=max;
}

//--------------------------------------------------------------
// schrittweite vom Floatedit einstellen
// step : [0.001..n]
//--------------------------------------------------------------
void SGUI_FloatEditSetStep(SFLOATEDIT_t* ptr, float step)
{
  if(ptr==NULL) return;
  if(step<0.001) step=0.001;

  if(step!=ptr->step_value) {
    ptr->step_value=step;
    ptr->status|=SFLOATEDIT_BIT_REDRAW;  // redraw at next loop
    SGUI_FLOATEDIT.update=true;
  }
}

//--------------------------------------------------------------
// value vom Floatedit einstellen
// value : zwischen min und maxwert
//--------------------------------------------------------------
void SGUI_FloatEditSetValue(SFLOATEDIT_t* ptr, float value)
{
  if(ptr==NULL) return;
  if(value<ptr->f_minvalue) value=ptr->f_minvalue;
  if(value>ptr->f_maxvalue) value=ptr->f_maxvalue;
  
  if(value!=ptr->f_value) {
    ptr->f_value=value;
    ptr->status|=SFLOATEDIT_BIT_REDRAW;  // redraw at next loop
    SGUI_FLOATEDIT.update=true;    
  }   
}

//--------------------------------------------------------------
// value vom Floatedit auslesen
//--------------------------------------------------------------
float SGUI_FloatEditGetValue(SFLOATEDIT_t* ptr)
{
  if(ptr==NULL) return 0;
  
  return ptr->f_value;
}

//--------------------------------------------------------------
// Handler einstellen
// Prototyp : "void name(float value)"
// "NULL" fuer keinen Handler
//--------------------------------------------------------------
void SGUI_FloatEditSetHandler(SFLOATEDIT_t* ptr, void *fkt_ptr)
{
  if(ptr==NULL) return;

  ptr->fkt_ptr=fkt_ptr;
}

//--------------------------------------------------------------
// interne Funktion fuer SGUI
// init der Objekte
//--------------------------------------------------------------
void SGUI_FloatEditInit(void)
{
  uint32_t n;

  SGUI_FLOATEDIT.update=false;
  SGUI_FLOATEDIT.akt_anz=0;  
  for(n=0;n<MAX_FLOATEDIT_ANZ;n++) {
    SGUI_FLOATEDIT.ptr[n]=NULL;
  }
}

//--------------------------------------------------------------
// interne Funktion fuer SGUI
// check ob FloatEdit per Touch betätigt oder nicht
// (wird zyklisch aufgerufen)
//--------------------------------------------------------------
bool SGUI_FloatEditTouch(bool pressed, uint16_t x, uint16_t y, bool blocked)
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
    for(n=0;n<SGUI_FLOATEDIT.akt_anz;n++) {
      // nur beachten bei aktivem window
      if(SGUI_WINDOW.aktiv_window==SGUI_FLOATEDIT.ptr[n]->window_nr) {
        // floatedit position bestimmen
        x1=SGUI_FLOATEDIT.ptr[n]->xpos;
        y1=SGUI_FLOATEDIT.ptr[n]->ypos;
        x2=SGUI_FLOATEDIT.ptr[n]->xpos+SGUI_FLOATEDIT.ptr[n]->width;
        y2=SGUI_FLOATEDIT.ptr[n]->ypos+SGUI_FLOATEDIT.ptr[n]->height;

        // check ob kollision
        if((x>=x1) && (x<=x2)) {
          if((y>=y1) && (y<=y2)) {
            // floatedit gefunden
            m=1;
            ret_wert=true;
            break;
          }
        }
      }
    }
    // check ob kollision gefunden
    if(m==1) {
      if((SGUI_FLOATEDIT.ptr[n]->typ==SFLOATEDIT_H) || (SGUI_FLOATEDIT.ptr[n]->typ==SFLOATEDIT_H2)) {
        if(x>(x1+SGUI_FLOATEDIT.ptr[n]->width/2)) {
          // pfeil hoch
          SGUI_FLOATEDIT.ptr[n]->f_value+=SGUI_FLOATEDIT.ptr[n]->step_value;
          if(SGUI_FLOATEDIT.ptr[n]->f_value>SGUI_FLOATEDIT.ptr[n]->f_maxvalue) {
            SGUI_FLOATEDIT.ptr[n]->f_value=SGUI_FLOATEDIT.ptr[n]->f_maxvalue;
          }
          // neu zeichnen
          P_SGUI_FloatEditDraw(SGUI_FLOATEDIT.ptr[n]);
          // CallBack aufrufen
          if(SGUI_FLOATEDIT.ptr[n]->fkt_ptr!=NULL) (*SGUI_FLOATEDIT.ptr[n]->fkt_ptr)(SGUI_FLOATEDIT.ptr[n]->f_value);           
        }
        else {
          // pfeil runter
          SGUI_FLOATEDIT.ptr[n]->f_value-=SGUI_FLOATEDIT.ptr[n]->step_value;
          if(SGUI_FLOATEDIT.ptr[n]->f_value<SGUI_FLOATEDIT.ptr[n]->f_minvalue) {
            SGUI_FLOATEDIT.ptr[n]->f_value=SGUI_FLOATEDIT.ptr[n]->f_minvalue;
          }
          // neu zeichnen
          P_SGUI_FloatEditDraw(SGUI_FLOATEDIT.ptr[n]);
          // CallBack aufrufen
          if(SGUI_FLOATEDIT.ptr[n]->fkt_ptr!=NULL) (*SGUI_FLOATEDIT.ptr[n]->fkt_ptr)(SGUI_FLOATEDIT.ptr[n]->f_value);
        }
      }
      else if(SGUI_FLOATEDIT.ptr[n]->typ==SFLOATEDIT_V) {
        if(y>(y1+SGUI_FLOATEDIT.ptr[n]->height/2)) {
          // pfeil runter
          SGUI_FLOATEDIT.ptr[n]->f_value-=SGUI_FLOATEDIT.ptr[n]->step_value;
          if(SGUI_FLOATEDIT.ptr[n]->f_value<SGUI_FLOATEDIT.ptr[n]->f_minvalue) {
            SGUI_FLOATEDIT.ptr[n]->f_value=SGUI_FLOATEDIT.ptr[n]->f_minvalue;
          }
          // neu zeichnen
          P_SGUI_FloatEditDraw(SGUI_FLOATEDIT.ptr[n]);
          // CallBack aufrufen
          if(SGUI_FLOATEDIT.ptr[n]->fkt_ptr!=NULL) (*SGUI_FLOATEDIT.ptr[n]->fkt_ptr)(SGUI_FLOATEDIT.ptr[n]->f_value);
        }
        else {
          // pfeil hoch
          SGUI_FLOATEDIT.ptr[n]->f_value+=SGUI_FLOATEDIT.ptr[n]->step_value;
          if(SGUI_FLOATEDIT.ptr[n]->f_value>SGUI_FLOATEDIT.ptr[n]->f_maxvalue) {
            SGUI_FLOATEDIT.ptr[n]->f_value=SGUI_FLOATEDIT.ptr[n]->f_maxvalue;
          }
          // neu zeichnen
          P_SGUI_FloatEditDraw(SGUI_FLOATEDIT.ptr[n]);
          // CallBack aufrufen
          if(SGUI_FLOATEDIT.ptr[n]->fkt_ptr!=NULL) (*SGUI_FLOATEDIT.ptr[n]->fkt_ptr)(SGUI_FLOATEDIT.ptr[n]->f_value);
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
void SGUI_FloatEditUpdate(void)
{
  uint32_t n;
  uint8_t status=0;

  for(n=0;n<SGUI_FLOATEDIT.akt_anz;n++) {
    // nur zeichnen, bei aktivem window
    if(SGUI_WINDOW.aktiv_window==SGUI_FLOATEDIT.ptr[n]->window_nr) {
      // test ob floatedit neu gezeichnet werden muss
      status=(SGUI_FLOATEDIT.ptr[n]->status&SFLOATEDIT_BIT_REDRAW);
      if((status!=0) || (SGUI_WINDOW.update_all==true)) P_SGUI_FloatEditDraw(SGUI_FLOATEDIT.ptr[n]);
    }
  }
}

//--------------------------------------------------------------
// interne Funktion
// zeichnet eine FloatEdit
//--------------------------------------------------------------
void P_SGUI_FloatEditDraw(SFLOATEDIT_t* ptr)
{
  uint16_t x,y,w,h,c1,c2,c3,d=0;
  uint16_t l,px_used,fx,fy;
  uint16_t btn1_xp,btn1_yp; 
  uint16_t btn2_xp,btn2_yp;
  uint16_t btn_size,xd;

  if(ptr==NULL) return;

  // Flag vom zeichnen zuruecksetzen
  ptr->status&=~SFLOATEDIT_BIT_REDRAW;

  x=ptr->xpos;
  y=ptr->ypos;
  w=ptr->width;
  h=ptr->height;

  c1=ptr->col_back;
  c2=ptr->col_frame;
  c3=ptr->col_text;

  if((ptr->status&SFLOATEDIT_BIT_3DSTYLE)==0) {
    d=ptr->frame_size;
    if(d>0) {
      // mit rahmen    
      if((d*2)>h) d=(h/2);     
      // zuerst den rahmen zeichnen 
      SGUI_ScreenDrawFullRect(x,y,w,h,c2);
      // feld zeichnen
      SGUI_ScreenDrawFullRect(x+d,y+d,w-2*d,h-2*d,c1);
    }
    else {
      // kein Rahmen 
      SGUI_ScreenDrawFullRect(x,y,w,h,c1);
    }    
  }
  else {
    // 3d-style
    d=SGUI_3D_FRAMESIZE;
    SGUI_ScreenDrawFullRect(x,y,w,h,c1);
    SGUI_ScreenDraw3DFrame(x,y,w,h,STYLE_LOWERED);   
  }
       
  xd=d;
  if((ptr->status&SFLOATEDIT_BIT_ARROW)!=0) {
    // pfeil-button groesse  
    if((ptr->typ==SFLOATEDIT_H) || (ptr->typ==SFLOATEDIT_H2)) {
      if(d>1) {  
        btn_size=h-2*d+2;        
        btn1_xp=x+d-1;
        btn1_yp=y+d-1;
        btn2_xp=x+w-d-btn_size+1;
        btn2_yp=y+d-1;
      }
      else {
        btn_size=h;        
        btn1_xp=x;
        btn1_yp=y;
        btn2_xp=x+w-btn_size;
        btn2_yp=y;
      }
      xd=btn_size+d;
      // check der abmessungen
      if(2*btn_size+2*d>=w) return;
      if(ptr->typ==SFLOATEDIT_H) {
        // pfeil-runter zeichnen
        SGUI_ScreenDrawArrowBtn(btn1_xp,btn1_yp,btn_size,SARROW_DOWN,c2,c1); 
        // pfeil-hoch zeichnen 
        SGUI_ScreenDrawArrowBtn(btn2_xp,btn2_yp,btn_size,SARROW_UP,c2,c1);
      }
      else {
        // pfeil-links zeichnen
        SGUI_ScreenDrawArrowBtn(btn1_xp,btn1_yp,btn_size,SARROW_LEFT,c2,c1); 
        // pfeil-rechts zeichnen 
        SGUI_ScreenDrawArrowBtn(btn2_xp,btn2_yp,btn_size,SARROW_RIGHT,c2,c1);        
      }
      if((ptr->status&SFLOATEDIT_BIT_3DSTYLE)!=0) {
        SGUI_ScreenDraw3DFrame(btn1_xp,btn1_yp,btn_size,btn_size,STYLE_RAISED); 
        SGUI_ScreenDraw3DFrame(btn2_xp,btn2_yp,btn_size,btn_size,STYLE_RAISED);
      }      
    }
    else if(ptr->typ==SFLOATEDIT_V) {
      if(d>1) {  
        btn_size=w-2*d+2;        
        btn1_xp=x+d-1;
        btn1_yp=y+d-1;
        btn2_xp=x+d-1;
        btn2_yp=y+h-d-btn_size+1;
      }
      else {
        btn_size=w;        
        btn1_xp=x;
        btn1_yp=y;
        btn2_xp=x;
        btn2_yp=y+h-btn_size;
      }
      // check der abmessungen
      if(2*btn_size+2*d>=h) return;
      // pfeil-hoch zeichnen
      SGUI_ScreenDrawArrowBtn(btn1_xp,btn1_yp,btn_size,SARROW_UP,c2,c1); 
      // pfeil-runter zeichnen 
      SGUI_ScreenDrawArrowBtn(btn2_xp,btn2_yp,btn_size,SARROW_DOWN,c2,c1);
      if((ptr->status&SFLOATEDIT_BIT_3DSTYLE)!=0) {
        SGUI_ScreenDraw3DFrame(btn1_xp,btn1_yp,btn_size,btn_size,STYLE_RAISED); 
        SGUI_ScreenDraw3DFrame(btn2_xp,btn2_yp,btn_size,btn_size,STYLE_RAISED);
      }      
    }
  }
 
  // zahl in string wandeln
  if((ptr->status&SFLOATEDIT_BIT_PADDING)==0) {
    SGUI_WindowFormatFloat(ptr->f_value, ptr->digits, false);
  }
  else {
    SGUI_WindowFormatFloat(ptr->f_value, ptr->digits, true);
  }
    
  // ypos ermitteln (center)
  px_used=ptr->font->height;
  fy=y;
  if(h>px_used) {
    fy=((h-px_used)/2)+y;
  }
  // xpos ermitteln  
  l=strlen(SGUI_WINDOW.buf);
  if(l==0) return; 
  px_used=l*ptr->font->width;
  fx=x;   
  if(w>px_used) {
    if(ptr->align==SFLOATEDIT_ALIGN_CENTER) fx=((w-px_used)/2)+x; 
    if(ptr->align==SFLOATEDIT_ALIGN_LEFT) fx=x+xd; 
    if(ptr->align==SFLOATEDIT_ALIGN_RIGHT) fx=x+(w-px_used)-xd; 
  }        
      
  SGUI_ScreenDrawString(fx,fy,SGUI_WINDOW.buf,ptr->font,c3,c1);  
}
