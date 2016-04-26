//--------------------------------------------------------------
// File     : ub_sgui_slider.c
// Datum    : 27.07.2015
// Version  : 1.0
// Autor    : UB
// EMail    : mc-4u(@)t-online.de
// Web      : www.mikrocontroller-4u.de
// CPU      : STM32F746
// IDE      : OpenSTM32
// GCC      : 4.9 2015q2
// Module   : SGUI_WINDOW
// Funktion : Eine Slider kann per Touch bedient werden
//            Slider koennen CallBack-Funktion vom User aufrufen
//--------------------------------------------------------------


//--------------------------------------------------------------
// Includes
//--------------------------------------------------------------
#include "ub_sgui_slider.h"





//--------------------------------------------------------------
// interne Funktionen
//--------------------------------------------------------------
void P_SGUI_SliderDraw(SSLIDER_t* ptr);



//--------------------------------------------------------------
// einen Slider neu erzeugen
// x,y = Linke obere Ecke vom Slider auf dem Display
// w = Breite (minimum 5 Pixel)
// h = hoehe  (minimum 5 Pixel)
//--------------------------------------------------------------
SSLIDER_t* SGUI_SliderCreate(uint16_t x, uint16_t y, uint16_t w, uint16_t h)
{
  SSLIDER_t *ptr;
  size_t obj_size;

  if(SGUI_WINDOW.aktiv_window==0) return NULL;
  if(SGUI_WINDOW.akt_win_ptr==NULL) return NULL;
  if(SGUI_SLIDER.akt_anz>=(MAX_SLIDER_ANZ-1)) return NULL;

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
  obj_size=sizeof(SSLIDER_t);
  ptr = (SSLIDER_t *) malloc(obj_size);
  if(ptr==NULL) return NULL;  

  // init aller Werte
  ptr->window_nr=SGUI_WINDOW.aktiv_window;  
  ptr->fkt_ptr=NULL;
  ptr->typ=SSLIDER_H;  
  ptr->xpos=x;
  ptr->ypos=y;
  ptr->width=w;
  ptr->height=h;
  ptr->frame_size=SSLIDER_DEF_FRAME_SIZE;
  ptr->col_frame=SSLIDER_DEF_FRAME_COL;
  ptr->col_back=SSLIDER_DEF_BACK_COL;
  ptr->min_value=0;
  ptr->max_value=100;
  ptr->akt_value=0;
  ptr->step_value=10;
  ptr->status=0x00;

  ptr->status|=SSLIDER_BIT_ARROW;   // arrow visible
  ptr->status|=SSLIDER_BIT_3DSTYLE; // 3d-style

  ptr->status|=SSLIDER_BIT_REDRAW;  // redraw at next loop
  SGUI_SLIDER.update=true;

  SGUI_SLIDER.ptr[SGUI_SLIDER.akt_anz]=ptr;
  SGUI_SLIDER.akt_anz++;
  
  // benötigter Speicherplatz
  SGUI_WINDOW.ram_used+=obj_size;   

  return ptr;
}

//--------------------------------------------------------------
// Typ von einem Slider einstellen
// typ : [SSLIDER_H, SSLIDER_V]
//--------------------------------------------------------------
void SGUI_SliderSetTyp(SSLIDER_t* ptr, SSLIDER_TYP_t typ)
{
  if(ptr==NULL) return;

  if(typ!=ptr->typ) {
    ptr->typ=typ;
    ptr->status|=SSLIDER_BIT_REDRAW;  // redraw at next loop
    SGUI_SLIDER.update=true;
  }
}

//--------------------------------------------------------------
// Style von einem Slider einstellen
// style : [STYLE_FLAT, STYLE_RAISED, STYLE_LOWERED]
//--------------------------------------------------------------
void SGUI_SliderSetStyle(SSLIDER_t* ptr, STYLE_TYP_t style)
{
  if(ptr==NULL) return;
  
  if(style==STYLE_FLAT) {
    if((ptr->status&SSLIDER_BIT_3DSTYLE)!=0) {
      ptr->status&=~SSLIDER_BIT_3DSTYLE;
      ptr->status|=SSLIDER_BIT_REDRAW;  // redraw at next loop
      SGUI_SLIDER.update=true;      
    }
  }
  else {
    if((ptr->status&SSLIDER_BIT_3DSTYLE)==0) {
      ptr->status|=SSLIDER_BIT_3DSTYLE;
      ptr->status|=SSLIDER_BIT_REDRAW;  // redraw at next loop
      SGUI_SLIDER.update=true;      
    }    
  } 
}

//--------------------------------------------------------------
// PfeilButtons von einem Slider ein,ausschalten
//--------------------------------------------------------------
void SGUI_SliderSetArrowVisible(SSLIDER_t* ptr, bool visible)
{
  if(ptr==NULL) return;

  if(visible==true) {
    if((ptr->status&SSLIDER_BIT_ARROW)==0) {
      ptr->status|=SSLIDER_BIT_ARROW;
      ptr->status|=SSLIDER_BIT_REDRAW;  // redraw at next loop
      SGUI_SLIDER.update=true;
    }
  }
  else {
    if((ptr->status&SSLIDER_BIT_ARROW)!=0) {
      ptr->status&=~SSLIDER_BIT_ARROW;
      ptr->status|=SSLIDER_BIT_REDRAW;  // redraw at next loop
      SGUI_SLIDER.update=true;
    }
  }
}

//--------------------------------------------------------------
// Rahmendicke von einem Slider einstellen
// px = dicke in pixel (0 = ohne Rahmen)
//--------------------------------------------------------------
void SGUI_SliderSetFrameSize(SSLIDER_t* ptr, uint8_t px)
{
  if(ptr==NULL) return;

  if(px!=ptr->frame_size) {
    ptr->frame_size=px;
    ptr->status|=SSLIDER_BIT_REDRAW;  // redraw at next loop
    SGUI_SLIDER.update=true;
  }
}

//--------------------------------------------------------------
// Farbe von einem Slider einstellen
// c1 = Rahmen
// c2 = Hintergrund
//--------------------------------------------------------------
void SGUI_SliderSetColor(SSLIDER_t* ptr, uint16_t c1, uint16_t c2)
{
  if(ptr==NULL) return;

  if(c1!=ptr->col_frame) {
    ptr->col_frame=c1;
    ptr->status|=SSLIDER_BIT_REDRAW;  // redraw at next loop
    SGUI_SLIDER.update=true;
  }
  if(c2!=ptr->col_back) {
    ptr->col_back=c2;
    ptr->status|=SSLIDER_BIT_REDRAW;  // redraw at next loop
    SGUI_SLIDER.update=true;
  }
}

//--------------------------------------------------------------
// min und maxwert vom Slider einstellen
// max muss > als minwert sein
//--------------------------------------------------------------
void SGUI_SliderSetMinMax(SSLIDER_t* ptr, int32_t min, int32_t max)
{
  if(ptr==NULL) return;  
  if(min>=max) return;
    
  if(ptr->min_value!=min) {
    ptr->min_value=min;
    ptr->status|=SSLIDER_BIT_REDRAW;  // redraw at next loop
    SGUI_SLIDER.update=true;
  }
  if(ptr->max_value!=max) {
    ptr->max_value=max;
    ptr->status|=SSLIDER_BIT_REDRAW;  // redraw at next loop
    SGUI_SLIDER.update=true;
  }  
  // istwert event. anpassen
  if(ptr->akt_value<min) ptr->akt_value=min;
  if(ptr->akt_value>max) ptr->akt_value=max;

}

//--------------------------------------------------------------
// schrittweite vom Slider einstellen
// step : [1..n]
//--------------------------------------------------------------
void SGUI_SliderSetStep(SSLIDER_t* ptr, uint16_t step)
{
  if(ptr==NULL) return;
  if(step<1) step=1;

  if(ptr->step_value!=step) {
    ptr->step_value=step;
    ptr->status|=SSLIDER_BIT_REDRAW;  // redraw at next loop
    SGUI_SLIDER.update=true;
  }
}

//--------------------------------------------------------------
// value vom Slider einstellen
// value : zwischen min und maxwert
//--------------------------------------------------------------
void SGUI_SliderSetValue(SSLIDER_t* ptr, int32_t value)
{
  if(ptr==NULL) return;
  if(value<ptr->min_value) value=ptr->min_value;
  if(value>ptr->max_value) value=ptr->max_value;
  
  if(ptr->akt_value!=value) {
    ptr->akt_value=value;
    ptr->status|=SSLIDER_BIT_REDRAW;  // redraw at next loop
    SGUI_SLIDER.update=true;
  }   
}

//--------------------------------------------------------------
// value vom Slider auslesen
//--------------------------------------------------------------
int32_t SGUI_SliderGetValue(SSLIDER_t* ptr)
{
  if(ptr==NULL) return 0;
  
  return ptr->akt_value;
}

//--------------------------------------------------------------
// Handler einstellen
// Prototyp : "void name(int32_t value)"
// "NULL" fuer keinen Handler
//--------------------------------------------------------------
void SGUI_SliderSetHandler(SSLIDER_t* ptr, void *fkt_ptr)
{
  if(ptr==NULL) return;

  ptr->fkt_ptr=fkt_ptr;
}

//--------------------------------------------------------------
// interne Funktion fuer SGUI
// init der Objekte
//--------------------------------------------------------------
void SGUI_SliderInit(void)
{
  uint32_t n;

  SGUI_SLIDER.update=false;
  SGUI_SLIDER.akt_anz=0;
  for(n=0;n<MAX_SLIDER_ANZ;n++) {
    SGUI_SLIDER.ptr[n]=NULL;
  }
}

//--------------------------------------------------------------
// interne Funktion fuer SGUI
// check ob Slider per Touch betätigt oder nicht
// (wird zyklisch aufgerufen)
//--------------------------------------------------------------
bool SGUI_SliderTouch(bool pressed, uint16_t x, uint16_t y, bool blocked)
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
    for(n=0;n<SGUI_SLIDER.akt_anz;n++) {
      // nur beachten bei aktivem window
      if(SGUI_WINDOW.aktiv_window==SGUI_SLIDER.ptr[n]->window_nr) {
        // slider position bestimmen
        x1=SGUI_SLIDER.ptr[n]->xpos;
        y1=SGUI_SLIDER.ptr[n]->ypos;
        x2=SGUI_SLIDER.ptr[n]->xpos+SGUI_SLIDER.ptr[n]->width;
        y2=SGUI_SLIDER.ptr[n]->ypos+SGUI_SLIDER.ptr[n]->height;

        // check ob kollision
        if((x>=x1) && (x<=x2)) {
          if((y>=y1) && (y<=y2)) {
            // slider gefunden
            m=1;
            ret_wert=true;
            break;
          }
        }
      }
    }
    // check ob kollision gefunden
    if(m==1) {
      if(SGUI_SLIDER.ptr[n]->typ==SSLIDER_H) {
        if(x>(x1+SGUI_SLIDER.ptr[n]->width/2)) {
          // pfeil rechts
          if(SGUI_SLIDER.ptr[n]->akt_value<SGUI_SLIDER.ptr[n]->max_value) {
            SGUI_SLIDER.ptr[n]->akt_value+=SGUI_SLIDER.ptr[n]->step_value;
            if(SGUI_SLIDER.ptr[n]->akt_value>SGUI_SLIDER.ptr[n]->max_value) {
              SGUI_SLIDER.ptr[n]->akt_value=SGUI_SLIDER.ptr[n]->max_value;
            }
            // neu zeichnen
            P_SGUI_SliderDraw(SGUI_SLIDER.ptr[n]);
            // CallBack aufrufen
            if(SGUI_SLIDER.ptr[n]->fkt_ptr!=NULL) (*SGUI_SLIDER.ptr[n]->fkt_ptr)(SGUI_SLIDER.ptr[n]->akt_value);
          }
        }
        else {
          // pfeil links
          if(SGUI_SLIDER.ptr[n]->akt_value>SGUI_SLIDER.ptr[n]->min_value) {
            SGUI_SLIDER.ptr[n]->akt_value-=SGUI_SLIDER.ptr[n]->step_value;
            if(SGUI_SLIDER.ptr[n]->akt_value<SGUI_SLIDER.ptr[n]->min_value) {
              SGUI_SLIDER.ptr[n]->akt_value=SGUI_SLIDER.ptr[n]->min_value;
            }
            // neu zeichnen
            P_SGUI_SliderDraw(SGUI_SLIDER.ptr[n]);
            // CallBack aufrufen
            if(SGUI_SLIDER.ptr[n]->fkt_ptr!=NULL) (*SGUI_SLIDER.ptr[n]->fkt_ptr)(SGUI_SLIDER.ptr[n]->akt_value);
          }
        }
      }
      else if(SGUI_SLIDER.ptr[n]->typ==SSLIDER_V) {
        if(y>(y1+SGUI_SLIDER.ptr[n]->height/2)) {
          // pfeil runter
          if(SGUI_SLIDER.ptr[n]->akt_value<SGUI_SLIDER.ptr[n]->max_value) {
            SGUI_SLIDER.ptr[n]->akt_value+=SGUI_SLIDER.ptr[n]->step_value;
            if(SGUI_SLIDER.ptr[n]->akt_value>SGUI_SLIDER.ptr[n]->max_value) {
              SGUI_SLIDER.ptr[n]->akt_value=SGUI_SLIDER.ptr[n]->max_value;
            }
            // neu zeichnen
            P_SGUI_SliderDraw(SGUI_SLIDER.ptr[n]);
            // CallBack aufrufen
            if(SGUI_SLIDER.ptr[n]->fkt_ptr!=NULL) (*SGUI_SLIDER.ptr[n]->fkt_ptr)(SGUI_SLIDER.ptr[n]->akt_value);
          }
        }
        else {
          // pfeil hoch
          if(SGUI_SLIDER.ptr[n]->akt_value>SGUI_SLIDER.ptr[n]->min_value) {
            SGUI_SLIDER.ptr[n]->akt_value-=SGUI_SLIDER.ptr[n]->step_value;
            if(SGUI_SLIDER.ptr[n]->akt_value<SGUI_SLIDER.ptr[n]->min_value) {
              SGUI_SLIDER.ptr[n]->akt_value=SGUI_SLIDER.ptr[n]->min_value;
            }
            // neu zeichnen
            P_SGUI_SliderDraw(SGUI_SLIDER.ptr[n]);
            // CallBack aufrufen
            if(SGUI_SLIDER.ptr[n]->fkt_ptr!=NULL) (*SGUI_SLIDER.ptr[n]->fkt_ptr)(SGUI_SLIDER.ptr[n]->akt_value);
          }
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
void SGUI_SliderUpdate(void)
{
  uint32_t n;
  uint8_t status=0;

  for(n=0;n<SGUI_SLIDER.akt_anz;n++) {
    // nur zeichnen, bei aktivem window
    if(SGUI_WINDOW.aktiv_window==SGUI_SLIDER.ptr[n]->window_nr) {
      // test ob Slider neu gezeichnet werden muss
      status=(SGUI_SLIDER.ptr[n]->status&SSLIDER_BIT_REDRAW);
      if((status!=0) || (SGUI_WINDOW.update_all==true)) P_SGUI_SliderDraw(SGUI_SLIDER.ptr[n]);
    }
  }
}

//--------------------------------------------------------------
// interne Funktion
// zeichnet einen Slider
//--------------------------------------------------------------
void P_SGUI_SliderDraw(SSLIDER_t* ptr)
{
  uint16_t x,y,w,h,c1,c2,d=0,d2,px;
  int32_t d1;
  float f;
  uint16_t btn1_xp,btn1_yp;  
  uint16_t btn2_xp,btn2_yp;
  uint16_t btn_size;  

  if(ptr==NULL) return;

  // Flag vom zeichnen zuruecksetzen
  ptr->status&=~SSLIDER_BIT_REDRAW;

  x=ptr->xpos;
  y=ptr->ypos;
  w=ptr->width;
  h=ptr->height; 
  c1=ptr->col_frame;
  c2=ptr->col_back;

  if((ptr->status&SSLIDER_BIT_3DSTYLE)==0) {
    d=ptr->frame_size;
    if(d>0) {
      // mit rahmen
      if((d*2)>w) d=(w/2);
      if((d*2)>h) d=(h/2);       
      
      // zuerst den rahmen zeichnen
      SGUI_ScreenDrawFullRect(x,y,w,h,c1);      
      // dann den balken zeichnen
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
    SGUI_ScreenDraw3DFrame(x,y,w,h,STYLE_LOWERED);    
  }

  
  if(ptr->typ==SSLIDER_H) {    
    // pfeil-button groesse    
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
    if((ptr->status&SSLIDER_BIT_ARROW)!=0) {
      // check der abmessungen
      if(3*btn_size+2*d>=w) return;
      // Pfeil-links zeichnen
      SGUI_ScreenDrawArrowBtn(btn1_xp,btn1_yp,btn_size,SARROW_LEFT,c1,c2);
      // Pfeil-rechts zeichnen
      SGUI_ScreenDrawArrowBtn(btn2_xp,btn2_yp,btn_size,SARROW_RIGHT,c1,c2);
      if((ptr->status&SSLIDER_BIT_3DSTYLE)!=0) {
        SGUI_ScreenDraw3DFrame(btn1_xp,btn1_yp,btn_size,btn_size,STYLE_RAISED); 
        SGUI_ScreenDraw3DFrame(btn2_xp,btn2_yp,btn_size,btn_size,STYLE_RAISED);
      }      
      // position vom marker ausrechnen      
      d1=ptr->max_value-ptr->min_value;
      d2=(w-2*d-3*btn_size);
      f=((float)(d2)/(float)(d1))*((float)(ptr->akt_value) - (float)(ptr->min_value));
      if(f<0) f=0;    
      px=(uint16_t)(f);    
      SGUI_ScreenDrawFullRect(x+d+btn_size+px,btn1_yp,btn_size,btn_size,c1);
      if((ptr->status&SSLIDER_BIT_3DSTYLE)!=0) {
        SGUI_ScreenDraw3DFrame(x+d+btn_size+px,btn1_yp,btn_size,btn_size,STYLE_RAISED);  
      }
    }
    else {
      // check der abmessungen
      if(btn_size+2*d>=w) return;      
      // position vom marker ausrechnen
      d1=ptr->max_value-ptr->min_value;
      d2=(w-2*d-btn_size);      
      f=((float)(d2)/(float)(d1))*((float)(ptr->akt_value) - (float)(ptr->min_value));
      if(f<0) f=0;    
      px=(uint16_t)(f);    
      SGUI_ScreenDrawFullRect(x+d+px,btn1_yp,btn_size,btn_size,c1);
      if((ptr->status&SSLIDER_BIT_3DSTYLE)!=0) {
        SGUI_ScreenDraw3DFrame(x+d+px,btn1_yp,btn_size,btn_size,STYLE_RAISED);  
      }      
    }
  }
  else if(ptr->typ==SSLIDER_V) {
    // pfeil-button groesse
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
    if((ptr->status&SSLIDER_BIT_ARROW)!=0) {
      // check der abmessungen
      if(3*btn_size+2*d>=h) return;
      // Pfeil-hoch zeichnen
      SGUI_ScreenDrawArrowBtn(btn1_xp,btn1_yp,btn_size,SARROW_UP,c1,c2); 
      // Pfeil-runter zeichnen
      SGUI_ScreenDrawArrowBtn(btn2_xp,btn2_yp,btn_size,SARROW_DOWN,c1,c2);
      if((ptr->status&SSLIDER_BIT_3DSTYLE)!=0) {
        SGUI_ScreenDraw3DFrame(btn1_xp,btn1_yp,btn_size,btn_size,STYLE_RAISED); 
        SGUI_ScreenDraw3DFrame(btn2_xp,btn2_yp,btn_size,btn_size,STYLE_RAISED);
      }       
      // position vom marker ausrechnen
      d1=ptr->max_value-ptr->min_value;
      d2=(h-2*d-3*btn_size);
      f=((float)(d2)/(float)(d1))*((float)(ptr->akt_value) - (float)(ptr->min_value));
      if(f<0) f=0;    
      px=(uint16_t)(f);
      SGUI_ScreenDrawFullRect(btn1_xp,y+d+btn_size+px,btn_size,btn_size,c1);    
      if((ptr->status&SSLIDER_BIT_3DSTYLE)!=0) {
        SGUI_ScreenDraw3DFrame(btn1_xp,y+d+btn_size+px,btn_size,btn_size,STYLE_RAISED);  
      }      
    }
    else {
      // check der abmessungen
      if(btn_size+2*d>=h) return;      
      // position vom marker ausrechnen
      d1=ptr->max_value-ptr->min_value;
      d2=(h-2*d-btn_size);
      f=((float)(d2)/(float)(d1))*((float)(ptr->akt_value) - (float)(ptr->min_value));
      if(f<0) f=0;    
      px=(uint16_t)(f);
      SGUI_ScreenDrawFullRect(btn1_xp,y+d+px,btn_size,btn_size,c1); 
      if((ptr->status&SSLIDER_BIT_3DSTYLE)!=0) {
        SGUI_ScreenDraw3DFrame(btn1_xp,y+d+px,btn_size,btn_size,STYLE_RAISED);  
      }      
    }
  }
}
