//--------------------------------------------------------------
// File     : ub_sgui_selectbtn.c
// Datum    : 27.07.2015
// Version  : 1.0
// Autor    : UB
// EMail    : mc-4u(@)t-online.de
// Web      : www.mikrocontroller-4u.de
// CPU      : STM32F746
// IDE      : OpenSTM32
// GCC      : 4.9 2015q2
// Module   : SGUI_WINDOW, SGUI_STRINGLIST
// Funktion : SelectButtons koennen per Touch bedient werden
//            SelectButtons koennen CallBack-Funktion vom User aufrufen
//            SelectButtons zeigen auf eine String-Liste
//            die per ADD,Delete,Insert mit Strings gefüllt werden muss 
//--------------------------------------------------------------


//--------------------------------------------------------------
// Includes
//--------------------------------------------------------------
#include "ub_sgui_selectbtn.h"




//--------------------------------------------------------------
// interne Funktionen
//--------------------------------------------------------------
void P_SGUI_SelectButtonDraw(SSELECTBTN_t* ptr);


//--------------------------------------------------------------
// ein SelectButton neu erzeugen
// x,y = Linke obere Ecke der SelectButton auf dem Display
// w = Breite (minimum 5 Pixel)
// h = hoehe  (minimum 5 Pixel)
//--------------------------------------------------------------
SSELECTBTN_t* SGUI_SelectButtonCreate(uint16_t x, uint16_t y, uint16_t w, uint16_t h)
{
  SSELECTBTN_t *ptr;
  size_t obj_size;  

  if(SGUI_WINDOW.aktiv_window==0) return NULL;
  if(SGUI_WINDOW.akt_win_ptr==NULL) return NULL;
  if(SGUI_SELECTBTN.akt_anz>=(MAX_SELECTBTN_ANZ-1)) return NULL;

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
  obj_size=sizeof(SSELECTBTN_t);
  ptr = (SSELECTBTN_t *) malloc(obj_size);
  if(ptr==NULL) return NULL;
  
  // init aller Werte
  ptr->window_nr=SGUI_WINDOW.aktiv_window;
  ptr->fkt_ptr=NULL;
  ptr->typ=SSELECTBTN_H;
  ptr->xpos=x;
  ptr->ypos=y;
  ptr->width=w;
  ptr->height=h;	
  ptr->frame_size=SSELECTBTN_DEF_FRAME_SIZE;
  ptr->col_text=SSELECTBTN_DEF_TEXT_COL;
  ptr->col_frame=SSELECTBTN_DEF_FRAME_COL;
  ptr->col_back=SSELECTBTN_DEF_BACK_COL;
  ptr->strlist=NULL;
  ptr->item_cnt=0;
  ptr->item_aktiv=-1;    
  ptr->font=SSELECTBTN_DEF_FONT;
  ptr->status=0x00;

  ptr->status|=SSELECTBTN_BIT_ARROW;   // Arrow visible
  ptr->status|=SSELECTBTN_BIT_3DSTYLE; // 3d-style

  ptr->status|=SSELECTBTN_BIT_REDRAW;  // redraw at next loop
  SGUI_SELECTBTN.update=true;

  SGUI_SELECTBTN.ptr[SGUI_SELECTBTN.akt_anz]=ptr;
  SGUI_SELECTBTN.akt_anz++;
  
  // benötigter Speicherplatz
  SGUI_WINDOW.ram_used+=obj_size;  

  return ptr;
}

//--------------------------------------------------------------
// Typ von einem SelectButton einstellen
// typ : [SSELECTBTN_H, SSELECTBTN_V]
//--------------------------------------------------------------
void SGUI_SelectButtonSetTyp(SSELECTBTN_t* ptr, SSELECTBTN_TYP_t typ)
{
  if(ptr==NULL) return;

  if(typ!=ptr->typ) {
    ptr->typ=typ;
    ptr->status|=SSELECTBTN_BIT_REDRAW;  // redraw at next loop
    SGUI_SELECTBTN.update=true;
  } 
}

//--------------------------------------------------------------
// Style von einem SelectButton einstellen
// style : [STYLE_FLAT, STYLE_RAISED, STYLE_LOWERED]
//--------------------------------------------------------------
void SGUI_SelectButtonSetStyle(SSELECTBTN_t* ptr, STYLE_TYP_t style)
{
  if(ptr==NULL) return;
  
  if(style==STYLE_FLAT) {
    if((ptr->status&SSELECTBTN_BIT_3DSTYLE)!=0) {
      ptr->status&=~SSELECTBTN_BIT_3DSTYLE;
      ptr->status|=SSELECTBTN_BIT_REDRAW;  // redraw at next loop
      SGUI_SELECTBTN.update=true;      
    }
  }
  else {
    if((ptr->status&SSELECTBTN_BIT_3DSTYLE)==0) {
      ptr->status|=SSELECTBTN_BIT_3DSTYLE;
      ptr->status|=SSELECTBTN_BIT_REDRAW;  // redraw at next loop
      SGUI_SELECTBTN.update=true;      
    }    
  } 
}

//--------------------------------------------------------------
// PfeilButtons von einem SelectButton ein,ausschalten
//--------------------------------------------------------------
void SGUI_SelectButtonSetArrowVisible(SSELECTBTN_t* ptr, bool visible)
{
  if(ptr==NULL) return;

  if(visible==true) {
    if((ptr->status&SSELECTBTN_BIT_ARROW)==0) {
      ptr->status|=SSELECTBTN_BIT_ARROW;
      ptr->status|=SSELECTBTN_BIT_REDRAW;  // redraw at next loop
      SGUI_SELECTBTN.update=true;
    }
  }
  else {
    if((ptr->status&SSELECTBTN_BIT_ARROW)!=0) {
      ptr->status&=~SSELECTBTN_BIT_ARROW;
      ptr->status|=SSELECTBTN_BIT_REDRAW;  // redraw at next loop
      SGUI_SELECTBTN.update=true;
    }
  }
}

//--------------------------------------------------------------
// Rahmendicke von einer SelectButton einstellen
// px = dicke in pixel (0=ohne Rahmen)
//--------------------------------------------------------------
void SGUI_SelectButtonSetFrameSize(SSELECTBTN_t* ptr, uint8_t px)
{
  if(ptr==NULL) return;

  if(px!=ptr->frame_size) {
    ptr->frame_size=px;
    ptr->status|=SSELECTBTN_BIT_REDRAW;  // redraw at next loop
    SGUI_SELECTBTN.update=true;
  }
}

//--------------------------------------------------------------
// Farbe von einer SelectButton einstellen
// c1 = Text
// c2 = Rahmen
// c3 = Hintergrund
//--------------------------------------------------------------
void SGUI_SelectButtonSetColor(SSELECTBTN_t* ptr, uint16_t c1, uint16_t c2, uint16_t c3)
{
  if(ptr==NULL) return;

  if(c1!=ptr->col_text) {
    ptr->col_text=c1;
    ptr->status|=SSELECTBTN_BIT_REDRAW;  // redraw at next loop
    SGUI_SELECTBTN.update=true;
  }
  if(c2!=ptr->col_frame) {
    ptr->col_frame=c2;
    ptr->status|=SSELECTBTN_BIT_REDRAW;  // redraw at next loop
    SGUI_SELECTBTN.update=true;
  }
  if(c3!=ptr->col_back) {
    ptr->col_back=c3;
    ptr->status|=SSELECTBTN_BIT_REDRAW;  // redraw at next loop
    SGUI_SELECTBTN.update=true;
  }  
}

//--------------------------------------------------------------
// Font von einer SelectButton einstellen
//--------------------------------------------------------------
void SGUI_SelectButtonSetFont(SSELECTBTN_t* ptr, UB_Font* font)
{
  if(ptr==NULL) return;

  if(font!=ptr->font) {
    ptr->font=font;
    ptr->status|=SSELECTBTN_BIT_REDRAW;  // redraw at next loop
    SGUI_SELECTBTN.update=true;
  }
}

//--------------------------------------------------------------
// String am Ende einer SelectButton hinzufuegen
//--------------------------------------------------------------
void SGUI_SelectButtonAddItem(SSELECTBTN_t* ptr, char *txt)
{
  StringList_t *strlist;  
  uint32_t cnt;
  
  if((ptr==NULL) || (txt==NULL)) return;
  if(ptr->item_cnt>=MAX_STRINGLIST_ANZ) return;
  
  // Stringliste aktuallisieren
  if(ptr->item_cnt==0) {
    // wenn noch keine Stringlist erzeugt wurde
    cnt=1;
    // stringliste erzeugen
    strlist=SGUI_StringListCreate(1);
    if(strlist==NULL) return; 
  }
  else {
    // Stringliste um einen Eintrag erweitern
    cnt=ptr->item_cnt;
    strlist=SGUI_StringListAdd(ptr->strlist, cnt);
    if(strlist==NULL) return; 
    cnt++;    
  }    
  
  // pointer und anzahl aktuallisieren
  ptr->strlist=strlist;
  ptr->item_cnt=cnt;  
  
  // String-Inhalt aendern
  SGUI_StringListSet(ptr->strlist, cnt-1, txt);
    
  ptr->status|=SSELECTBTN_BIT_REDRAW;  // redraw at next loop
  SGUI_SELECTBTN.update=true;    
  
  // benötigter Speicherplatz
  SGUI_WINDOW.ram_used+=STRINGLIST_BYTES_PER_ITEM;  
}

//--------------------------------------------------------------
// String von einer SelectButton entfernen
// pos = nummer vom Eintrag [0...n]
//--------------------------------------------------------------
void SGUI_SelectButtonDeleteItem(SSELECTBTN_t* ptr, uint16_t pos)
{
  StringList_t *strlist;
  uint32_t cnt;
  
  if(ptr==NULL) return;  
  if(ptr->item_cnt<=pos) return;
  if(ptr->item_cnt==0) return;
  
  cnt=ptr->item_cnt;
  // String löschen
  strlist=SGUI_StringListDelete(ptr->strlist, cnt, pos);
  
  // pointer und anzahl aktuallisieren
  cnt--;
  ptr->strlist=strlist;
  ptr->item_cnt=cnt;   
  if((cnt==0) || (pos==ptr->item_aktiv)){
    ptr->item_aktiv=-1;     
  }
  
  ptr->status|=SSELECTBTN_BIT_REDRAW;  // redraw at next loop
  SGUI_SELECTBTN.update=true; 

  // benötigter Speicherplatz
  SGUI_WINDOW.ram_used-=STRINGLIST_BYTES_PER_ITEM;  
}

//--------------------------------------------------------------
// String an einer Position einer SelectButton hinzufuegen
// pos = nummer vom Eintrag [0...n]
//--------------------------------------------------------------
void SGUI_SelectButtonInsertItem(SSELECTBTN_t* ptr, uint16_t pos, char *txt)
{
  StringList_t *strlist;
  uint32_t cnt;
  
  if((ptr==NULL) || (txt==NULL)) return; 
  if(ptr->item_cnt<=pos) return;
  if(ptr->item_cnt==0) return;  
  
  cnt=ptr->item_cnt;
  // String hinzufügen
  strlist=SGUI_StringListInsert(ptr->strlist, cnt, pos);
  
  // String-Inhalt aendern
  SGUI_StringListSet(strlist, pos, txt);   
    
  // pointer und anzahl aktuallisieren
  cnt++;
  ptr->strlist=strlist;
  ptr->item_cnt=cnt;    
  
  ptr->status|=SSELECTBTN_BIT_REDRAW;  // redraw at next loop
  SGUI_SELECTBTN.update=true; 
  
  // benötigter Speicherplatz
  SGUI_WINDOW.ram_used+=STRINGLIST_BYTES_PER_ITEM;  
}

//--------------------------------------------------------------
// String in einer SelectButton ändern
// pos = nummer vom Eintrag [0...n]
//--------------------------------------------------------------
void SGUI_SelectButtonSetItem(SSELECTBTN_t* ptr, uint16_t pos, char *txt)
{
  if((ptr==NULL) || (txt==NULL)) return;
  if(ptr->item_cnt<=pos) return;
    
  // String-Inhalt ändern
  SGUI_StringListSet(ptr->strlist, pos, txt);
    
  ptr->status|=SSELECTBTN_BIT_REDRAW;  // redraw at next loop
  SGUI_SELECTBTN.update=true;     
}

//--------------------------------------------------------------
// String aus einer SelectButton auslesen
// pos = nummer vom Eintrag [0...n]
//--------------------------------------------------------------
char* SGUI_SelectButtonGetItem(SSELECTBTN_t* ptr, uint16_t pos)
{
  char *ret_ptr;
  
  if(ptr==NULL) return NULL;
  if(ptr->item_cnt<=pos) return NULL; 
  
  ret_ptr=ptr->strlist[pos];  
  
  return ret_ptr;
}

//--------------------------------------------------------------
// Anzahl der Einträge vom SelectButton auslesen
// ret_wert : [0...n]
//--------------------------------------------------------------
uint16_t SGUI_SelectButtonGetItemCnt(SSELECTBTN_t* ptr)
{
  if(ptr==NULL) return 0;

  return ptr->item_cnt;  
}

//--------------------------------------------------------------
// Item einer SelectButton aktivieren
// pos = nummer vom Eintrag [0...n]
//--------------------------------------------------------------
void SGUI_SelectButtonSetAktivItemNr(SSELECTBTN_t* ptr, uint16_t pos)
{
  if(ptr==NULL) return;
  
  if(pos>=ptr->item_cnt) return;    
  
  if(pos!=ptr->item_aktiv) {
    ptr->item_aktiv=pos;
    
    ptr->status|=SSELECTBTN_BIT_REDRAW;  // redraw at next loop
    SGUI_SELECTBTN.update=true;   
  }
}

//--------------------------------------------------------------
// ItemNr vom ausgewählten Eintrag auslesen
// ret_wert : [0...n]
//            -1 = kein Eintrag ausgewählt
//--------------------------------------------------------------
int16_t SGUI_SelectButtonGetAktivItemNr(SSELECTBTN_t* ptr)
{
  if(ptr==NULL) return -1;

  return ptr->item_aktiv;    
}

//--------------------------------------------------------------
// Handler einstellen
// Prototyp : "void name(uint16_t aktiv_nr)"
// "NULL" fuer keinen Handler
//--------------------------------------------------------------
void SGUI_SelectButtonSetHandler(SSELECTBTN_t* ptr, void *fkt_ptr)
{
  if(ptr==NULL) return;

  ptr->fkt_ptr=fkt_ptr;
}

//--------------------------------------------------------------
// interne Funktion fuer SGUI
// init der Objekte
//--------------------------------------------------------------
void SGUI_SelectButtonInit(void)
{
  uint32_t n;

  SGUI_SELECTBTN.update=false;
  SGUI_SELECTBTN.akt_anz=0;  
  for(n=0;n<MAX_SELECTBTN_ANZ;n++) {
    SGUI_SELECTBTN.ptr[n]=NULL;
  }
}

//--------------------------------------------------------------
// interne Funktion fuer SGUI
// check ob Selectbutton per Touch betätigt oder nicht
// (wird zyklisch aufgerufen)
//--------------------------------------------------------------
bool SGUI_SelectButtonTouch(bool pressed, uint16_t x, uint16_t y, bool blocked)
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
    for(n=0;n<SGUI_SELECTBTN.akt_anz;n++) {
      // nur beachten bei aktivem window
      if(SGUI_WINDOW.aktiv_window==SGUI_SELECTBTN.ptr[n]->window_nr) {
        // nur wenn selectbutton mindestens zwei items
        if(SGUI_SELECTBTN.ptr[n]->item_cnt>1) {
          // selectbutton position bestimmen
          x1=SGUI_SELECTBTN.ptr[n]->xpos;
          y1=SGUI_SELECTBTN.ptr[n]->ypos;
          x2=SGUI_SELECTBTN.ptr[n]->xpos+SGUI_SELECTBTN.ptr[n]->width;
          y2=SGUI_SELECTBTN.ptr[n]->ypos+SGUI_SELECTBTN.ptr[n]->height;

          // check ob kollision
          if((x>=x1) && (x<=x2)) {
            if((y>=y1) && (y<=y2)) {
              // selectbutton gefunden
              m=1;
              ret_wert=true;
              break;
            }
          }
        }
      }
    }
    // check ob kollision gefunden
    if(m==1) {
      if(SGUI_SELECTBTN.ptr[n]->typ==SSELECTBTN_H) {
        if(x>(x1+SGUI_SELECTBTN.ptr[n]->width/2)) {
          // pfeil rechts
          if(SGUI_SELECTBTN.ptr[n]->item_aktiv<SGUI_SELECTBTN.ptr[n]->item_cnt-1) {
            SGUI_SELECTBTN.ptr[n]->item_aktiv++;
            // neu zeichnen
            P_SGUI_SelectButtonDraw(SGUI_SELECTBTN.ptr[n]);
            // CallBack aufrufen
            if(SGUI_SELECTBTN.ptr[n]->fkt_ptr!=NULL) (*SGUI_SELECTBTN.ptr[n]->fkt_ptr)(SGUI_SELECTBTN.ptr[n]->item_aktiv);
          }
        }
        else {
          // pfeil links
          if(SGUI_SELECTBTN.ptr[n]->item_aktiv>0) {
            SGUI_SELECTBTN.ptr[n]->item_aktiv--;
            // neu zeichnen
            P_SGUI_SelectButtonDraw(SGUI_SELECTBTN.ptr[n]);
            // CallBack aufrufen
            if(SGUI_SELECTBTN.ptr[n]->fkt_ptr!=NULL) (*SGUI_SELECTBTN.ptr[n]->fkt_ptr)(SGUI_SELECTBTN.ptr[n]->item_aktiv);
          }
        }
      }
      else if(SGUI_SELECTBTN.ptr[n]->typ==SSELECTBTN_V) {
        if(y>(y1+SGUI_SELECTBTN.ptr[n]->height/2)) {
          // pfeil runter
          if(SGUI_SELECTBTN.ptr[n]->item_aktiv<SGUI_SELECTBTN.ptr[n]->item_cnt-1) {
            SGUI_SELECTBTN.ptr[n]->item_aktiv++;
            // neu zeichnen
            P_SGUI_SelectButtonDraw(SGUI_SELECTBTN.ptr[n]);
            // CallBack aufrufen
            if(SGUI_SELECTBTN.ptr[n]->fkt_ptr!=NULL) (*SGUI_SELECTBTN.ptr[n]->fkt_ptr)(SGUI_SELECTBTN.ptr[n]->item_aktiv);
          }
        }
        else {
          // pfeil hoch
          if(SGUI_SELECTBTN.ptr[n]->item_aktiv>0) {
            SGUI_SELECTBTN.ptr[n]->item_aktiv--;
            // neu zeichnen
            P_SGUI_SelectButtonDraw(SGUI_SELECTBTN.ptr[n]);
            // CallBack aufrufen
            if(SGUI_SELECTBTN.ptr[n]->fkt_ptr!=NULL) (*SGUI_SELECTBTN.ptr[n]->fkt_ptr)(SGUI_SELECTBTN.ptr[n]->item_aktiv);
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
void SGUI_SelectButtonUpdate(void)
{
  uint32_t n;
  uint8_t status=0;

  for(n=0;n<SGUI_SELECTBTN.akt_anz;n++) {
    // nur zeichnen, bei aktivem window
    if(SGUI_WINDOW.aktiv_window==SGUI_SELECTBTN.ptr[n]->window_nr) {
      // test ob selectbutton neu gezeichnet werden muss
      status=(SGUI_SELECTBTN.ptr[n]->status&SSELECTBTN_BIT_REDRAW);
      if((status!=0) || (SGUI_WINDOW.update_all==true)) P_SGUI_SelectButtonDraw(SGUI_SELECTBTN.ptr[n]);
    }
  }
}

//--------------------------------------------------------------
// interne Funktion
// zeichnet eine SelectButton
//--------------------------------------------------------------
void P_SGUI_SelectButtonDraw(SSELECTBTN_t* ptr)
{
  uint16_t x,y,w,h,c1,c2,c3,d=0,n;
  uint16_t cnt,l,px_used,fx,fy;
  uint16_t btn1_xp,btn1_yp; 
  uint16_t btn2_xp,btn2_yp;
  uint16_t btn_size;

  if(ptr==NULL) return;

  // Flag vom zeichnen zuruecksetzen
  ptr->status&=~SSELECTBTN_BIT_REDRAW;

  x=ptr->xpos;
  y=ptr->ypos;
  w=ptr->width;
  h=ptr->height;

  c1=ptr->col_back;
  c2=ptr->col_frame;
  c3=ptr->col_text;

  if((ptr->status&SSELECTBTN_BIT_3DSTYLE)==0) {
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
       
  if((ptr->status&SSELECTBTN_BIT_ARROW)!=0) {
    // pfeil-button groesse  
    if(ptr->typ==SSELECTBTN_H) {
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
      // check der abmessungen
      if(2*btn_size+2*d>=w) return;
      // pfeil-links zeichnen
      SGUI_ScreenDrawArrowBtn(btn1_xp,btn1_yp,btn_size,SARROW_LEFT,c2,c1); 
      // pfeil-rechts zeichnen 
      SGUI_ScreenDrawArrowBtn(btn2_xp,btn2_yp,btn_size,SARROW_RIGHT,c2,c1);
      if((ptr->status&SSELECTBTN_BIT_3DSTYLE)!=0) {
        SGUI_ScreenDraw3DFrame(btn1_xp,btn1_yp,btn_size,btn_size,STYLE_RAISED); 
        SGUI_ScreenDraw3DFrame(btn2_xp,btn2_yp,btn_size,btn_size,STYLE_RAISED);
      }      
    }
    else if(ptr->typ==SSELECTBTN_V) {
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
      if((ptr->status&SSELECTBTN_BIT_3DSTYLE)!=0) {
        SGUI_ScreenDraw3DFrame(btn1_xp,btn1_yp,btn_size,btn_size,STYLE_RAISED); 
        SGUI_ScreenDraw3DFrame(btn2_xp,btn2_yp,btn_size,btn_size,STYLE_RAISED);
      }       
    }
  }
  
  // anzeige item berechnen
  cnt=ptr->item_cnt;
  if(cnt==0) {
    ptr->item_aktiv=-1;         
  }
  else if(ptr->item_aktiv==-1) {
    ptr->item_aktiv=0;      
  }    
  
  // aktiven string anzeigen   
  if(ptr->item_aktiv>=0) {   
    // ypos ermitteln (center)
    px_used=ptr->font->height;
    fy=y;
    if(h>px_used) {
      fy=((h-px_used)/2)+y;
    }
    // xpos ermitteln (center) 
    n=ptr->item_aktiv;
    l=strlen(ptr->strlist[n]);
    if(l==0) return; 
    px_used=l*ptr->font->width;
    fx=x;   
    if(w>px_used) {
      fx=((w-px_used)/2)+x; 
    }        
      
    SGUI_ScreenDrawString(fx,fy,ptr->strlist[n],ptr->font,c3,c1);
  }    
}
