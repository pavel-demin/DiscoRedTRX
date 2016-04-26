//--------------------------------------------------------------
// File     : ub_sgui_listbox.c
// Datum    : 27.07.2015
// Version  : 1.0
// Autor    : UB
// EMail    : mc-4u(@)t-online.de
// Web      : www.mikrocontroller-4u.de
// CPU      : STM32F746
// IDE      : OpenSTM32
// GCC      : 4.9 2015q2
// Module   : SGUI_WINDOW, SGUI_STRINGLIST
// Funktion : Listboxen koennen per Touch bedient werden
//            Listboxen koennen CallBack-Funktion vom User aufrufen
//            Listboxen zeigen auf eine String-Liste
//            die per ADD,Delete,Insert mit Strings gefüllt werden muss
//--------------------------------------------------------------


//--------------------------------------------------------------
// Includes
//--------------------------------------------------------------
#include "ub_sgui_listbox.h"





//--------------------------------------------------------------
// interne Funktionen
//--------------------------------------------------------------
void P_SGUI_ListboxDraw(SLISTBOX_t* ptr);


//--------------------------------------------------------------
// eine Listbox neu erzeugen
// x,y = Linke obere Ecke der Listbox auf dem Display
// w = Breite (minimum 5 Pixel)
// h = hoehe  (minimum 5 Pixel)
//--------------------------------------------------------------
SLISTBOX_t* SGUI_ListboxCreate(uint16_t x, uint16_t y, uint16_t w, uint16_t h)
{
  SLISTBOX_t *ptr;
  size_t obj_size;  

  if(SGUI_WINDOW.aktiv_window==0) return NULL;
  if(SGUI_WINDOW.akt_win_ptr==NULL) return NULL;
  if(SGUI_LISTBOX.akt_anz>=(MAX_LISTBOX_ANZ-1)) return NULL;

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
  obj_size=sizeof(SLISTBOX_t);
  ptr = (SLISTBOX_t *) malloc(obj_size);
  if(ptr==NULL) return NULL;
  
  // init aller Werte
  ptr->window_nr=SGUI_WINDOW.aktiv_window;
  ptr->fkt_ptr=NULL;
  ptr->xpos=x;
  ptr->ypos=y;
  ptr->width=w;
  ptr->height=h;	
  ptr->frame_size=SLISTBOX_DEF_FRAME_SIZE;
  ptr->col_text=SLISTBOX_DEF_TEXT_COL;
  ptr->col_frame=SLISTBOX_DEF_FRAME_COL;
  ptr->col_back=SLISTBOX_DEF_BACK_COL;
  ptr->strlist=NULL;
  ptr->item_cnt=0;  
  ptr->item_aktiv=-1;
  ptr->slider_size=SLISTBOX_DEF_SLIDER_SIZE;
  ptr->item_first=-1;
  ptr->font=SLISTBOX_DEF_FONT;
  ptr->status=0x00;

  ptr->status|=SLISTBOX_BIT_SLIDER;  // slider visible
  ptr->status|=SLISTBOX_BIT_3DSTYLE; // 3d-style

  ptr->status|=SLISTBOX_BIT_REDRAW;  // redraw at next loop
  SGUI_LISTBOX.update=true;

  SGUI_LISTBOX.ptr[SGUI_LISTBOX.akt_anz]=ptr;
  SGUI_LISTBOX.akt_anz++;
  
  // benötigter Speicherplatz
  SGUI_WINDOW.ram_used+=obj_size;    

  return ptr;
}

//--------------------------------------------------------------
// Style von einer Listbox einstellen
// style : [STYLE_FLAT, STYLE_RAISED, STYLE_LOWERED]
//--------------------------------------------------------------
void SGUI_ListboxSetStyle(SLISTBOX_t* ptr, STYLE_TYP_t style)
{
  if(ptr==NULL) return;
  
  if(style==STYLE_FLAT) {
    if((ptr->status&SLISTBOX_BIT_3DSTYLE)!=0) {
      ptr->status&=~SLISTBOX_BIT_3DSTYLE;
      ptr->status|=SLISTBOX_BIT_REDRAW;  // redraw at next loop
      SGUI_LISTBOX.update=true;      
    }
  }
  else {
    if((ptr->status&SLISTBOX_BIT_3DSTYLE)==0) {
      ptr->status|=SLISTBOX_BIT_3DSTYLE;
      ptr->status|=SLISTBOX_BIT_REDRAW;  // redraw at next loop
      SGUI_LISTBOX.update=true;      
    }    
  } 
}

//--------------------------------------------------------------
// Rahmendicke von einer Listbox einstellen
// px = dicke in pixel (0 = ohne Rahmen)
//--------------------------------------------------------------
void SGUI_ListboxSetFrameSize(SLISTBOX_t* ptr, uint8_t px)
{
  if(ptr==NULL) return;

  if(px!=ptr->frame_size) {
    ptr->frame_size=px;
    ptr->status|=SLISTBOX_BIT_REDRAW;  // redraw at next loop
    SGUI_LISTBOX.update=true;
  }
}

//--------------------------------------------------------------
// Dicke vom Slider von einer Listbox einstellen
// px = dicke in pixel (minimum=5)
//--------------------------------------------------------------
void SGUI_ListboxSetSliderSize(SLISTBOX_t* ptr, uint8_t px)
{
  if(ptr==NULL) return;  

  if(px<5) return;
  
  if(px!=ptr->slider_size) {
    ptr->slider_size=px;
    ptr->status|=SLISTBOX_BIT_REDRAW;  // redraw at next loop
    SGUI_LISTBOX.update=true;
  }  
}

//--------------------------------------------------------------
// Slider von einer Listbox ein,ausschalten
//--------------------------------------------------------------
void SGUI_ListboxSetSliderVisible(SLISTBOX_t* ptr, bool visible)
{
  if(ptr==NULL) return;
  
  if(visible==true) {
    if((ptr->status&SLISTBOX_BIT_SLIDER)==0) {
      ptr->status|=SLISTBOX_BIT_SLIDER;
      ptr->status|=SLISTBOX_BIT_REDRAW;  // redraw at next loop
      SGUI_LISTBOX.update=true;
    }
  }
  else {
    if((ptr->status&SLISTBOX_BIT_SLIDER)!=0) {
      ptr->status&=~SLISTBOX_BIT_SLIDER;
      ptr->status|=SLISTBOX_BIT_REDRAW;  // redraw at next loop
      SGUI_LISTBOX.update=true;
    }
  }
}

//--------------------------------------------------------------
// Farbe von einer Listbox einstellen
// c1 = Text
// c2 = Rahmen
// c3 = Hintergrund
//--------------------------------------------------------------
void SGUI_ListboxSetColor(SLISTBOX_t* ptr, uint16_t c1, uint16_t c2, uint16_t c3)
{
  if(ptr==NULL) return;

  if(c1!=ptr->col_text) {
    ptr->col_text=c1;
    ptr->status|=SLISTBOX_BIT_REDRAW;  // redraw at next loop
    SGUI_LISTBOX.update=true;
  }
  if(c2!=ptr->col_frame) {
    ptr->col_frame=c2;
    ptr->status|=SLISTBOX_BIT_REDRAW;  // redraw at next loop
    SGUI_LISTBOX.update=true;
  }
  if(c3!=ptr->col_back) {
    ptr->col_back=c3;
    ptr->status|=SLISTBOX_BIT_REDRAW;  // redraw at next loop
    SGUI_LISTBOX.update=true;
  }  
}

//--------------------------------------------------------------
// Font von einer Listbox einstellen
//--------------------------------------------------------------
void SGUI_ListboxSetFont(SLISTBOX_t* ptr, UB_Font* font)
{
  if(ptr==NULL) return;

  if(font!=ptr->font) {
    ptr->font=font;
    ptr->status|=SLISTBOX_BIT_REDRAW;  // redraw at next loop
    SGUI_LISTBOX.update=true;
  }
}

//--------------------------------------------------------------
// String am Ende einer Listbox hinzufuegen
//--------------------------------------------------------------
void SGUI_ListboxAddItem(SLISTBOX_t* ptr, char *txt)
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
    
  ptr->status|=SLISTBOX_BIT_REDRAW;  // redraw at next loop
  SGUI_LISTBOX.update=true;   

  // benötigter Speicherplatz
  SGUI_WINDOW.ram_used+=STRINGLIST_BYTES_PER_ITEM;
}

//--------------------------------------------------------------
// String von einer Listbox entfernen
// pos = nummer vom Eintrag [0...n]
//--------------------------------------------------------------
void SGUI_ListboxDeleteItem(SLISTBOX_t* ptr, uint16_t pos)
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
  
  ptr->status|=SLISTBOX_BIT_REDRAW;  // redraw at next loop
  SGUI_LISTBOX.update=true;  
  
  // benötigter Speicherplatz
  SGUI_WINDOW.ram_used-=STRINGLIST_BYTES_PER_ITEM;  
}

//--------------------------------------------------------------
// String an einer Position einer Listbox hinzufuegen
// pos = nummer vom Eintrag [0...n]
//--------------------------------------------------------------
void SGUI_ListboxInsertItem(SLISTBOX_t* ptr, uint16_t pos, char *txt)
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
  
  ptr->status|=SLISTBOX_BIT_REDRAW;  // redraw at next loop
  SGUI_LISTBOX.update=true; 
  
  // benötigter Speicherplatz
  SGUI_WINDOW.ram_used+=STRINGLIST_BYTES_PER_ITEM;  
}

//--------------------------------------------------------------
// String in einer Listbox ändern
// pos = nummer vom Eintrag [0...n]
//--------------------------------------------------------------
void SGUI_ListboxSetItem(SLISTBOX_t* ptr, uint16_t pos, char *txt)
{
  if((ptr==NULL) || (txt==NULL)) return;
  if(ptr->item_cnt<=pos) return;
    
  // String-Inhalt ändern
  SGUI_StringListSet(ptr->strlist, pos, txt);
    
  ptr->status|=SLISTBOX_BIT_REDRAW;  // redraw at next loop
  SGUI_LISTBOX.update=true;     
}

//--------------------------------------------------------------
// String aus einer Listbox auslesen
// pos = nummer vom Eintrag [0...n]
//--------------------------------------------------------------
char* SGUI_ListboxGetItem(SLISTBOX_t* ptr, uint16_t pos)
{
  char *ret_ptr;
  
  if(ptr==NULL) return NULL;
  if(ptr->item_cnt<=pos) return NULL; 
  
  ret_ptr=ptr->strlist[pos];  
  
  return ret_ptr;
}

//--------------------------------------------------------------
// Anzahl der Einträge der Stringlist auslesen
// ret_wert : [0...n]
//--------------------------------------------------------------
uint16_t SGUI_ListboxGetItemCnt(SLISTBOX_t* ptr)
{
  if(ptr==NULL) return 0;

  return ptr->item_cnt;  
}

//--------------------------------------------------------------
// Item einer Listbox aktivieren
// pos = nummer vom Eintrag [0...n]
//       -1 = keinen Eintrag aktivieren
//--------------------------------------------------------------
void SGUI_ListboxSetAktivItemNr(SLISTBOX_t* ptr, int16_t pos)
{
  if(ptr==NULL) return;
  
  if(pos>=ptr->item_cnt) return;
  if(pos<-1) pos=-1;
  
  if(pos!=ptr->item_aktiv) {
    ptr->item_aktiv=pos;
    
    ptr->status|=SLISTBOX_BIT_REDRAW;  // redraw at next loop
    SGUI_LISTBOX.update=true;   
  }
}

//--------------------------------------------------------------
// ItemNr vom ausgewählten Eintrag auslesen
// ret_wert : [0...n]
//            -1 = kein Eintrag ausgewählt
//--------------------------------------------------------------
int16_t SGUI_ListboxGetAktivItemNr(SLISTBOX_t* ptr)
{
  if(ptr==NULL) return -1;

  return ptr->item_aktiv;    
}

//--------------------------------------------------------------
// ersters sichtbare Item einer Listbox einstellen
// pos = nummer vom Eintrag [0...n]
//--------------------------------------------------------------
void SGUI_ListboxSetFirstItemNr(SLISTBOX_t* ptr, uint16_t pos)
{
  if(ptr==NULL) return;  
  if(pos>=ptr->item_cnt) return;
  
  if(pos!=ptr->item_first) {
    ptr->item_first=pos;
    
    ptr->status|=SLISTBOX_BIT_REDRAW;  // redraw at next loop
    SGUI_LISTBOX.update=true;    
  }  
}

//--------------------------------------------------------------
// Handler einstellen
// Prototyp : "void name(uint16_t aktiv_nr)"
// "NULL" fuer keinen Handler
//--------------------------------------------------------------
void SGUI_ListboxSetHandler(SLISTBOX_t* ptr, void *fkt_ptr)
{
  if(ptr==NULL) return;

  ptr->fkt_ptr=fkt_ptr;
}

//--------------------------------------------------------------
// interne Funktion fuer SGUI
// init der Objekte
//--------------------------------------------------------------
void SGUI_ListboxInit(void)
{
  uint32_t n;

  SGUI_LISTBOX.update=false;
  SGUI_LISTBOX.akt_anz=0;  
  for(n=0;n<MAX_LISTBOX_ANZ;n++) {
    SGUI_LISTBOX.ptr[n]=NULL;
  }
}

//--------------------------------------------------------------
// interne Funktion fuer SGUI
// check ob Listbox per Touch betätigt oder nicht
// (wird zyklisch aufgerufen)
//--------------------------------------------------------------
bool SGUI_ListboxTouch(bool pressed, uint16_t x, uint16_t y, bool blocked)
{
  bool ret_wert=false;
  uint32_t n,yd;
  uint16_t x1,x2,y1,y2,zeile;
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
    for(n=0;n<SGUI_LISTBOX.akt_anz;n++) {
      // nur beachten bei aktivem window
      if(SGUI_WINDOW.aktiv_window==SGUI_LISTBOX.ptr[n]->window_nr) {
        // nur wenn listbox nicht leer
        if(SGUI_LISTBOX.ptr[n]->item_cnt>0) {
          // listbox position bestimmen
          x1=SGUI_LISTBOX.ptr[n]->xpos;
          y1=SGUI_LISTBOX.ptr[n]->ypos;
          x2=SGUI_LISTBOX.ptr[n]->xpos+SGUI_LISTBOX.ptr[n]->width;
          y2=SGUI_LISTBOX.ptr[n]->ypos+SGUI_LISTBOX.ptr[n]->height;

          // check ob kollision
          if((x>=x1) && (x<=x2)) {
            if((y>=y1) && (y<=y2)) {
              // listbox gefunden
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
      // check ob slider
      if((SGUI_LISTBOX.ptr[n]->status&SLISTBOX_BIT_SLIDER)!=0) {
        if(x>(x2-SGUI_LISTBOX.ptr[n]->slider_size)) {
          m=2;
          if(y>(y1+SGUI_LISTBOX.ptr[n]->height/2)) {
            // pfeil runter
            if(SGUI_LISTBOX.ptr[n]->item_first<SGUI_LISTBOX.ptr[n]->item_cnt-1) {
              SGUI_LISTBOX.ptr[n]->item_first++;
              // neu zeichnen
              P_SGUI_ListboxDraw(SGUI_LISTBOX.ptr[n]);
            }
          }
          else {
            // pfeil hoch
            if(SGUI_LISTBOX.ptr[n]->item_first>0) {
              SGUI_LISTBOX.ptr[n]->item_first--;
              // neu zeichnen
              P_SGUI_ListboxDraw(SGUI_LISTBOX.ptr[n]);
            }
          }
        }
      }
      if(m==1) {
        // zeile herausfinden
        if((SGUI_LISTBOX.ptr[n]->status&SLISTBOX_BIT_3DSTYLE)==0) {
          yd=y-y1-SGUI_LISTBOX.ptr[n]->frame_size;
        }
        else {
          yd=y-y1-2;
        }
        zeile=yd/SGUI_LISTBOX.ptr[n]->font->height;
        zeile+=SGUI_LISTBOX.ptr[n]->item_first;
        // item auf aktiv schalten
        if(SGUI_LISTBOX.ptr[n]->item_cnt>zeile) {
          if(SGUI_LISTBOX.ptr[n]->item_aktiv!=zeile) {
            // zeile aktivieren
            SGUI_LISTBOX.ptr[n]->item_aktiv=zeile;
            // neu zeichnen
            P_SGUI_ListboxDraw(SGUI_LISTBOX.ptr[n]);
            // CallBack aufrufen
            if(SGUI_LISTBOX.ptr[n]->fkt_ptr!=NULL) (*SGUI_LISTBOX.ptr[n]->fkt_ptr)(zeile);
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
void SGUI_ListboxUpdate(void)
{
  uint32_t n;
  uint8_t status=0;

  for(n=0;n<SGUI_LISTBOX.akt_anz;n++) {
    // nur zeichnen, bei aktivem window
    if(SGUI_WINDOW.aktiv_window==SGUI_LISTBOX.ptr[n]->window_nr) {
      // test ob listbox neu gezeichnet werden muss
      status=(SGUI_LISTBOX.ptr[n]->status&SLISTBOX_BIT_REDRAW);
      if((status!=0) || (SGUI_WINDOW.update_all==true)) P_SGUI_ListboxDraw(SGUI_LISTBOX.ptr[n]);
    }
  }
}

//--------------------------------------------------------------
// interne Funktion
// zeichnet eine Listbox
//--------------------------------------------------------------
void P_SGUI_ListboxDraw(SLISTBOX_t* ptr)
{
  uint16_t x,y,w,h,c1,c2,c3,d=0,n;
  uint16_t fy,fh,cnt;
  uint16_t btn1_xp,btn1_yp;  
  uint16_t btn2_xp,btn2_yp;
  uint16_t btn_size,sl_size;    

  if(ptr==NULL) return;

  // Flag vom zeichnen zuruecksetzen
  ptr->status&=~SLISTBOX_BIT_REDRAW;

  x=ptr->xpos;
  y=ptr->ypos;
  w=ptr->width;
  h=ptr->height;

  c1=ptr->col_back;
  c2=ptr->col_frame;
  c3=ptr->col_text;

  if((ptr->status&SLISTBOX_BIT_3DSTYLE)==0) {
    d=ptr->frame_size;
    if(d>0) {
      // zuerst den rahmen zeichnen 
      SGUI_ScreenDrawFullRect(x,y,w,h,c2);
      // listbox zeichnen
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
  
  // start item berechnen
  if(ptr->item_cnt==0) {
    ptr->item_first=-1;       
  }
  else if(ptr->item_first>=ptr->item_cnt) {
    ptr->item_first=0;
  }
  else if(ptr->item_first==-1) {
    ptr->item_first=0;
  }

  // Liste anzeigen
  if(ptr->item_cnt>0) {
    // strings anzeigen
    fh=ptr->font->height;
    fy=y+d;
    cnt=0;
    for(n=ptr->item_first;n<ptr->item_cnt;n++) {
      if(n==ptr->item_aktiv) {
        SGUI_ScreenDrawString(x+d,fy,ptr->strlist[n],ptr->font,c1,c3);
      }
      else {
        SGUI_ScreenDrawString(x+d,fy,ptr->strlist[n],ptr->font,c3,c1);
      }
      fy+=fh;
      cnt++;
      if(fy+fh>y+h-d) break;
    }     
  }
   
  if((ptr->status&SLISTBOX_BIT_SLIDER)!=0) {
    sl_size=ptr->slider_size;
    if((ptr->status&SLISTBOX_BIT_3DSTYLE)!=0) {
      // slider zeichnen
      SGUI_ScreenDraw3DFrame(x+w-sl_size,y,sl_size,h,STYLE_LOWERED);
      btn_size=sl_size-d;
    }
    else {
      // linie zeichnen
      SGUI_ScreenDrawLine(x+w-sl_size-d+1,y,x+w-sl_size-d+1,y+h-1,c2);
      btn_size=sl_size;
    }    
    
    // pfeil-button groesse
    if(d>1) {        
      btn1_xp=x+w-d-btn_size+1;
      btn1_yp=y+d-1;
      btn2_xp=x+w-d-btn_size+1;
      btn2_yp=y+h-d-btn_size+1;      
    }
    else {      
      btn1_xp=x+w-btn_size;
      btn1_yp=y;
      btn2_xp=x+w-btn_size;
      btn2_yp=y+h-btn_size;      
    }   

    // check der abmessungen
    if(btn_size+2*d>=w) return;
    if(2*btn_size+2*d>=h) return;  
           
    // pfeil-hoch zeichnen          
    SGUI_ScreenDrawArrowBtn(btn1_xp,btn1_yp,btn_size,SARROW_UP,c2,c1);  
    // pfeil-runter zeichnen          
    SGUI_ScreenDrawArrowBtn(btn2_xp,btn2_yp,btn_size,SARROW_DOWN,c2,c1);
    if((ptr->status&SLISTBOX_BIT_3DSTYLE)!=0) {
      SGUI_ScreenDraw3DFrame(btn1_xp,btn1_yp,btn_size,btn_size,STYLE_RAISED); 
      SGUI_ScreenDraw3DFrame(btn2_xp,btn2_yp,btn_size,btn_size,STYLE_RAISED);
    }
  }     
}
