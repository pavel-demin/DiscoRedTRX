//--------------------------------------------------------------
// File     : ub_sgui_dropdown.c
// Datum    : 27.07.2015
// Version  : 1.0
// Autor    : UB
// EMail    : mc-4u(@)t-online.de
// Web      : www.mikrocontroller-4u.de
// CPU      : STM32F746
// IDE      : OpenSTM32
// GCC      : 4.9 2015q2
// Module   : SGUI_WINDOW, SGUI_STRINGLIST
// Funktion : DropDown-Felder koennen per Touch bedient werden
//            DropDown-Felder koennen CallBack-Funktion vom User aufrufen
//            DropDown-Felder zeigen auf eine String-Liste
//            die per ADD,Delete,Insert mit Strings gefüllt werden muss
//--------------------------------------------------------------


//--------------------------------------------------------------
// Includes
//--------------------------------------------------------------
#include "ub_sgui_dropdown.h"




//--------------------------------------------------------------
// interne Funktionen
//--------------------------------------------------------------
void P_SGUI_DropdownDraw(SDROPDOWN_t* ptr);


//--------------------------------------------------------------
// ein DropDown-Feld neu erzeugen
// x,y = Linke obere Ecke der Dropdown auf dem Display
// w = Breite (minimum 5 Pixel)
// h = hoehe  (minimum 5 Pixel)
//--------------------------------------------------------------
SDROPDOWN_t* SGUI_DropdownCreate(uint16_t x, uint16_t y, uint16_t w, uint16_t h)
{
  SDROPDOWN_t *ptr;
  size_t obj_size;
  uint16_t max_ypos=SGUI_SCREEN_MAXY;

  if(SGUI_WINDOW.aktiv_window==0) return NULL;
  if(SGUI_WINDOW.akt_win_ptr==NULL) return NULL;
  if(SGUI_DROPDOWN.akt_anz>=(MAX_DROPDOWN_ANZ-1)) return NULL;

  // parameter check
  if(SGUI_WINDOW.akt_win_ptr->typ==SWINDOW_CHILD) {
    // bei Child-Window, Offset fuer x und y
    x+=SGUI_WINDOW.akt_win_ptr->xpos;
    y+=SGUI_WINDOW.akt_win_ptr->ypos;
    max_ypos=SGUI_WINDOW.akt_win_ptr->ypos+SGUI_WINDOW.akt_win_ptr->height;
  }
  if((x>=SGUI_SCREEN_MAXX) || (y>=SGUI_SCREEN_MAXY)) return NULL;
  if((x+w)>SGUI_SCREEN_MAXX) w=(SGUI_SCREEN_MAXX-x);
  if((y+h)>SGUI_SCREEN_MAXY) h=(SGUI_SCREEN_MAXY-y);
  if((w<5) || (h<5)) return NULL;

  // speicher reservieren
  obj_size=sizeof(SDROPDOWN_t);
  ptr = (SDROPDOWN_t *) malloc(obj_size);
  if(ptr==NULL) return NULL;
  
  // init aller Werte
  ptr->window_nr=SGUI_WINDOW.aktiv_window;
  ptr->fkt_ptr=NULL;
  ptr->xpos=x;
  ptr->ypos=y;
  ptr->width=w;
  ptr->height=h;	
  ptr->frame_size=SDROPDOWN_DEF_FRAME_SIZE;
  ptr->col_text=SDROPDOWN_DEF_TEXT_COL;
  ptr->col_frame=SDROPDOWN_DEF_FRAME_COL;
  ptr->col_back=SDROPDOWN_DEF_BACK_COL;
  ptr->strlist=NULL;
  ptr->item_cnt=0;
  ptr->max_ypos=max_ypos;
  ptr->exp_height=h;
  ptr->item_aktiv=-1;
  ptr->item_first=-1;
  ptr->font=SDROPDOWN_DEF_FONT;
  ptr->status=0x00;

  if(SGUI_WINDOW.akt_win_ptr->typ==SWINDOW_CHILD) {
    ptr->status|=SDROPDOWN_BIT_CHILD; // child window
  }
  ptr->status|=SDROPDOWN_BIT_ARROW;   // arrow visible
  ptr->status|=SDROPDOWN_BIT_3DSTYLE; // 3d-style

  ptr->status|=SDROPDOWN_BIT_REDRAW;  // redraw at next loop
  SGUI_DROPDOWN.update=true;

  SGUI_DROPDOWN.ptr[SGUI_DROPDOWN.akt_anz]=ptr;
  SGUI_DROPDOWN.akt_anz++;
  
  // benötigter Speicherplatz
  SGUI_WINDOW.ram_used+=obj_size;    

  return ptr;
}

//--------------------------------------------------------------
// Style von einem Dropdown einstellen
// style : [STYLE_FLAT, STYLE_RAISED, STYLE_LOWERED]
//--------------------------------------------------------------
void SGUI_DropdownSetStyle(SDROPDOWN_t* ptr, STYLE_TYP_t style)
{
  if(ptr==NULL) return;
  
  if(style==STYLE_FLAT) {
    if((ptr->status&SDROPDOWN_BIT_3DSTYLE)!=0) {
      ptr->status&=~SDROPDOWN_BIT_3DSTYLE;
      ptr->status|=SDROPDOWN_BIT_REDRAW;  // redraw at next loop
      SGUI_DROPDOWN.update=true;      
    }
  }
  else {
    if((ptr->status&SDROPDOWN_BIT_3DSTYLE)==0) {
      ptr->status|=SDROPDOWN_BIT_3DSTYLE;
      ptr->status|=SDROPDOWN_BIT_REDRAW;  // redraw at next loop
      SGUI_DROPDOWN.update=true;      
    }    
  } 
}

//--------------------------------------------------------------
// PfeilButtons von einem Dropdown ein,ausschalten
//--------------------------------------------------------------
void SGUI_DropdownSetArrowVisible(SDROPDOWN_t* ptr, bool visible)
{
  if(ptr==NULL) return;

  if(visible==true) {
    if((ptr->status&SDROPDOWN_BIT_ARROW)==0) {
      ptr->status|=SDROPDOWN_BIT_ARROW;
      ptr->status|=SDROPDOWN_BIT_REDRAW;  // redraw at next loop
      SGUI_DROPDOWN.update=true;
    }
  }
  else {
    if((ptr->status&SDROPDOWN_BIT_ARROW)!=0) {
      ptr->status&=~SDROPDOWN_BIT_ARROW;
      ptr->status|=SDROPDOWN_BIT_REDRAW;  // redraw at next loop
      SGUI_DROPDOWN.update=true;
    }
  }
}

//--------------------------------------------------------------
// Rahmendicke von einer Dropdown einstellen
// px = dicke in pixel (0=ohne Rahmen)
//--------------------------------------------------------------
void SGUI_DropdownSetFrameSize(SDROPDOWN_t* ptr, uint8_t px)
{
  if(ptr==NULL) return;

  if(px!=ptr->frame_size) {
    ptr->frame_size=px;
    ptr->status|=SDROPDOWN_BIT_REDRAW;  // redraw at next loop
    SGUI_DROPDOWN.update=true;
  }
}

//--------------------------------------------------------------
// Farbe von einer Dropdown einstellen
// c1 = Text
// c2 = Rahmen
// c3 = Hintergrund
//--------------------------------------------------------------
void SGUI_DropdownSetColor(SDROPDOWN_t* ptr, uint16_t c1, uint16_t c2, uint16_t c3)
{
  if(ptr==NULL) return;

  if(c1!=ptr->col_text) {
    ptr->col_text=c1;
    ptr->status|=SDROPDOWN_BIT_REDRAW;  // redraw at next loop
    SGUI_DROPDOWN.update=true;
  }
  if(c2!=ptr->col_frame) {
    ptr->col_frame=c2;
    ptr->status|=SDROPDOWN_BIT_REDRAW;  // redraw at next loop
    SGUI_DROPDOWN.update=true;
  }
  if(c3!=ptr->col_back) {
    ptr->col_back=c3;
    ptr->status|=SDROPDOWN_BIT_REDRAW;  // redraw at next loop
    SGUI_DROPDOWN.update=true;
  }  
}

//--------------------------------------------------------------
// Font von einer Dropdown einstellen
//--------------------------------------------------------------
void SGUI_DropdownSetFont(SDROPDOWN_t* ptr, UB_Font* font)
{
  if(ptr==NULL) return;

  if(font!=ptr->font) {
    ptr->font=font;
    ptr->status|=SDROPDOWN_BIT_REDRAW;  // redraw at next loop
    SGUI_DROPDOWN.update=true;
  }
}

//--------------------------------------------------------------
// String am Ende einer Dropdown hinzufuegen
//--------------------------------------------------------------
void SGUI_DropdownAddItem(SDROPDOWN_t* ptr, char *txt)
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
    
  ptr->status|=SDROPDOWN_BIT_REDRAW;  // redraw at next loop
  SGUI_DROPDOWN.update=true;   

  // benötigter Speicherplatz
  SGUI_WINDOW.ram_used+=STRINGLIST_BYTES_PER_ITEM;  
}

//--------------------------------------------------------------
// String von einer Dropdown entfernen
// pos = nummer vom Eintrag [0...n]
//--------------------------------------------------------------
void SGUI_DropdownDeleteItem(SDROPDOWN_t* ptr, uint16_t pos)
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
  
  ptr->status|=SDROPDOWN_BIT_REDRAW;  // redraw at next loop
  SGUI_DROPDOWN.update=true; 

  // benötigter Speicherplatz
  SGUI_WINDOW.ram_used-=STRINGLIST_BYTES_PER_ITEM;   
}

//--------------------------------------------------------------
// String an einer Position einer Dropdown hinzufuegen
// pos = nummer vom Eintrag [0...n]
//--------------------------------------------------------------
void SGUI_DropdownInsertItem(SDROPDOWN_t* ptr, uint16_t pos, char *txt)
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
  
  ptr->status|=SDROPDOWN_BIT_REDRAW;  // redraw at next loop
  SGUI_DROPDOWN.update=true;  
  
  // benötigter Speicherplatz
  SGUI_WINDOW.ram_used+=STRINGLIST_BYTES_PER_ITEM;   
}

//--------------------------------------------------------------
// String in einer Dropdown ändern
// pos = nummer vom Eintrag [0...n]
//--------------------------------------------------------------
void SGUI_DropdownSetItem(SDROPDOWN_t* ptr, uint16_t pos, char *txt)
{
  if((ptr==NULL) || (txt==NULL)) return;
  if(ptr->item_cnt<=pos) return;
    
  // String-Inhalt ändern
  SGUI_StringListSet(ptr->strlist, pos, txt);
    
  ptr->status|=SDROPDOWN_BIT_REDRAW;  // redraw at next loop
  SGUI_DROPDOWN.update=true;     
}

//--------------------------------------------------------------
// String aus einer Dropdown auslesen
// pos = nummer vom Eintrag [0...n]
//--------------------------------------------------------------
char* SGUI_DropdownGetItem(SDROPDOWN_t* ptr, uint16_t pos)
{
  char *ret_ptr;
  
  if(ptr==NULL) return NULL;
  if(ptr->item_cnt<=pos) return NULL; 
  
  ret_ptr=ptr->strlist[pos];  
  
  return ret_ptr;
}

//--------------------------------------------------------------
// Anzahl der Einträge der DropDown auslesen
// ret_wert : [0...n]
//--------------------------------------------------------------
uint16_t SGUI_DropdownGetItemCnt(SDROPDOWN_t* ptr)
{
  if(ptr==NULL) return 0;

  return ptr->item_cnt;  
}

//--------------------------------------------------------------
// Item einer Dropdown aktivieren
// pos = nummer vom Eintrag [0...n]
//--------------------------------------------------------------
void SGUI_DropdownSetAktivItemNr(SDROPDOWN_t* ptr, uint16_t pos)
{
  if(ptr==NULL) return;
  
  if(pos>=ptr->item_cnt) return;    
  
  if(pos!=ptr->item_aktiv) {
    ptr->item_aktiv=pos;
    
    ptr->status|=SDROPDOWN_BIT_REDRAW;  // redraw at next loop
    SGUI_DROPDOWN.update=true;   
  }
}

//--------------------------------------------------------------
// ItemNr vom ausgewählten Eintrag auslesen
// ret_wert : [0...n]
//            -1 = kein Eintrag ausgewählt
//--------------------------------------------------------------
int16_t SGUI_DropdownGetAktivItemNr(SDROPDOWN_t* ptr)
{
  if(ptr==NULL) return -1;

  return ptr->item_aktiv;    
}

//--------------------------------------------------------------
// Dropdown expandieren oder kollabieren
//--------------------------------------------------------------
void SGUI_DropdownSetExpand(SDROPDOWN_t* ptr, bool expand)
{
  if(ptr==NULL) return;

  if(expand==true) {
    if((ptr->status&SDROPDOWN_BIT_EXPAND)==0) {
      ptr->status|=SDROPDOWN_BIT_EXPAND;
      ptr->status|=SDROPDOWN_BIT_REDRAW;  // redraw at next loop
      SGUI_DROPDOWN.update=true;
    }
  }
  else {
    if((ptr->status&SDROPDOWN_BIT_EXPAND)!=0) {
      ptr->status&=~SDROPDOWN_BIT_EXPAND;
      ptr->status|=SDROPDOWN_BIT_REDRAW;  // redraw at next loop
      SGUI_DROPDOWN.update=true;
      // komplettes window neu zeichnen
      SGUI_WINDOW.update_window=true;
      // check ob auch parent neu gezeichnet werden muss
      if((ptr->status&SDROPDOWN_BIT_SIZE)!=0) {
        ptr->status&=~SDROPDOWN_BIT_SIZE;
        SGUI_WINDOW.update_parent_mode=1;
      }
    }
  }
}

//--------------------------------------------------------------
// Handler einstellen
// Prototyp : "void name(uint16_t aktiv_nr)"
// "NULL" fuer keinen Handler
//--------------------------------------------------------------
void SGUI_DropdownSetHandler(SDROPDOWN_t* ptr, void *fkt_ptr)
{
  if(ptr==NULL) return;

  ptr->fkt_ptr=fkt_ptr;
}

//--------------------------------------------------------------
// interne Funktion fuer SGUI
// init der Objekte
//--------------------------------------------------------------
void SGUI_DropdownInit(void)
{
  uint32_t n;

  SGUI_DROPDOWN.update=false;
  SGUI_DROPDOWN.akt_anz=0;  
  for(n=0;n<MAX_DROPDOWN_ANZ;n++) {
    SGUI_DROPDOWN.ptr[n]=NULL;
  }
}

//--------------------------------------------------------------
// interne Funktion fuer SGUI
// check ob DropDown per Touch betätigt oder nicht
// (wird zyklisch aufgerufen)
//--------------------------------------------------------------
bool SGUI_DropdownTouch(bool pressed, uint16_t x, uint16_t y, bool blocked)
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
    for(n=0;n<SGUI_DROPDOWN.akt_anz;n++) {
      // nur beachten bei aktivem window
      if(SGUI_WINDOW.aktiv_window==SGUI_DROPDOWN.ptr[n]->window_nr) {
        // nur wenn dropdown mindestens zwei items
    	if(SGUI_DROPDOWN.ptr[n]->item_cnt>1) {
          // dropdown position bestimmen
          x1=SGUI_DROPDOWN.ptr[n]->xpos;
          y1=SGUI_DROPDOWN.ptr[n]->ypos;
          x2=SGUI_DROPDOWN.ptr[n]->xpos+SGUI_DROPDOWN.ptr[n]->width;
          if((SGUI_DROPDOWN.ptr[n]->status&SDROPDOWN_BIT_EXPAND)==0) {
            y2=SGUI_DROPDOWN.ptr[n]->ypos+SGUI_DROPDOWN.ptr[n]->height;
          }
          else {
            y2=SGUI_DROPDOWN.ptr[n]->ypos+SGUI_DROPDOWN.ptr[n]->exp_height;
          }

          // check ob kollision
          if((x>=x1) && (x<=x2)) {
            if((y>=y1) && (y<=y2)) {
              // dropdown gefunden
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
      if((SGUI_DROPDOWN.ptr[n]->status&SDROPDOWN_BIT_EXPAND)==0) {
        // expandieren
        SGUI_DROPDOWN.ptr[n]->status|=SDROPDOWN_BIT_EXPAND;
        // neu zeichnen
        P_SGUI_DropdownDraw(SGUI_DROPDOWN.ptr[n]);
      }
      else {
        // check ob arrow
        if(y<y1+SGUI_DROPDOWN.ptr[n]->height) {
          m=2;
          // kollabieren
          SGUI_DROPDOWN.ptr[n]->status&=~SDROPDOWN_BIT_EXPAND;
          // komplettes window neu zeichnen
          SGUI_WINDOW.update_window=true;
          // check ob auch parent neu gezeichnet werden muss
          if((SGUI_DROPDOWN.ptr[n]->status&SDROPDOWN_BIT_SIZE)!=0) {
            SGUI_DROPDOWN.ptr[n]->status&=~SDROPDOWN_BIT_SIZE;
            SGUI_WINDOW.update_parent_mode=1;
          }
        }
        else if((SGUI_DROPDOWN.ptr[n]->status&SDROPDOWN_BIT_SLIDER)!=0) {
          // check ob slider
          if(x>(x2-SGUI_DROPDOWN.ptr[n]->height)) {
            m=2;
            if(y>(y1+SGUI_DROPDOWN.ptr[n]->height+(SGUI_DROPDOWN.ptr[n]->exp_height-SGUI_DROPDOWN.ptr[n]->height)/2)) {
              // pfeil runter
              if(SGUI_DROPDOWN.ptr[n]->item_first<SGUI_DROPDOWN.ptr[n]->item_cnt-1) {
                SGUI_DROPDOWN.ptr[n]->item_first++;
                // neu zeichnen
                P_SGUI_DropdownDraw(SGUI_DROPDOWN.ptr[n]);
              }
            }
            else {
              // pfeil hoch
              if(SGUI_DROPDOWN.ptr[n]->item_first>0) {
                SGUI_DROPDOWN.ptr[n]->item_first--;
                // neu zeichnen
                P_SGUI_DropdownDraw(SGUI_DROPDOWN.ptr[n]);
              }
            }
          }
        }
        if(m==1) {
          // zeile herausfinden
          yd=y-y1-SGUI_DROPDOWN.ptr[n]->height;
          zeile=yd/SGUI_DROPDOWN.ptr[n]->font->height;
          if((SGUI_DROPDOWN.ptr[n]->status&SDROPDOWN_BIT_SLIDER)!=0) zeile+=SGUI_DROPDOWN.ptr[n]->item_first;
          // item auf aktiv schalten
          if(SGUI_DROPDOWN.ptr[n]->item_cnt>zeile) {
            if(SGUI_DROPDOWN.ptr[n]->item_aktiv!=zeile) {
              // zeile aktivieren
              SGUI_DROPDOWN.ptr[n]->item_aktiv=zeile;
              // neu zeichnen
              P_SGUI_DropdownDraw(SGUI_DROPDOWN.ptr[n]);
              // CallBack aufrufen
              if(SGUI_DROPDOWN.ptr[n]->fkt_ptr!=NULL) (*SGUI_DROPDOWN.ptr[n]->fkt_ptr)(zeile);
            }
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
void SGUI_DropdownUpdate(void)
{
  uint32_t n;
  uint8_t status=0;

  for(n=0;n<SGUI_DROPDOWN.akt_anz;n++) {
    // nur zeichnen, bei aktivem window
    if(SGUI_WINDOW.aktiv_window==SGUI_DROPDOWN.ptr[n]->window_nr) {
      // test ob dropdown neu gezeichnet werden muss
      status=(SGUI_DROPDOWN.ptr[n]->status&SDROPDOWN_BIT_REDRAW);
      if((status!=0) || (SGUI_WINDOW.update_all==true)) P_SGUI_DropdownDraw(SGUI_DROPDOWN.ptr[n]);
    }
  }
}

//--------------------------------------------------------------
// interne Funktion
// zeichnet eine Dropdown
//--------------------------------------------------------------
void P_SGUI_DropdownDraw(SDROPDOWN_t* ptr)
{
  uint16_t x,y,w,h,c1,c2,c3,d=0,n;
  uint16_t cnt,fy,fx,px_used;
  uint16_t btn1_xp,btn1_yp;  
  uint16_t btn2_xp,btn2_yp;
  uint16_t btn_size,sl_size;

  if(ptr==NULL) return;

  // Flag vom zeichnen zuruecksetzen
  ptr->status&=~SDROPDOWN_BIT_REDRAW;

  x=ptr->xpos;
  y=ptr->ypos;
  w=ptr->width;
  h=ptr->height;

  c1=ptr->col_back;
  c2=ptr->col_frame;
  c3=ptr->col_text;

  if((ptr->status&SDROPDOWN_BIT_3DSTYLE)==0) {
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
    // xpos ermitteln (left)
    fx=x+d;    
    
    n=ptr->item_aktiv;
    SGUI_ScreenDrawString(fx,fy,ptr->strlist[n],ptr->font,c3,c1);
  }   
    
  // pfeil-button groesse
  if(d>1) {
    btn_size=h-2*d+2;
    btn1_xp=x+w-d-btn_size+1;
    btn1_yp=y+d-1;
  }
  else {
    btn_size=h;
    btn1_xp=x+w-btn_size;
    btn1_yp=y;
  }
  if((ptr->status&SDROPDOWN_BIT_ARROW)!=0) {
    // check der abmessungen
    if(btn_size+2*d>=w) return;
    
    // pfeil-runter zeichnen 
    SGUI_ScreenDrawArrowBtn(btn1_xp,btn1_yp,btn_size,SARROW_DOWN,c2,c1);
    if((ptr->status&SDROPDOWN_BIT_3DSTYLE)!=0) {
      SGUI_ScreenDraw3DFrame(btn1_xp,btn1_yp,btn_size,btn_size,STYLE_RAISED);
    }
  }
  
  // event. liste anzeigen
  ptr->exp_height=h;
  if((ptr->status&SDROPDOWN_BIT_EXPAND)!=0) {
    if(cnt>0) {
      px_used=ptr->font->height*cnt;
      // check der abmessungen
      if(y+h+px_used+d>SGUI_SCREEN_MAXY) {
        // abmessung anpassen
        cnt=(SGUI_SCREEN_MAXY-y-h-d)/ptr->font->height;
        if(cnt==0) return;
        px_used=ptr->font->height*cnt;
        // slider merker setzen
        ptr->status|=SDROPDOWN_BIT_SLIDER;
        // start item berechnen
        if(ptr->item_first>=ptr->item_cnt) {
          ptr->item_first=0;
        }
        else if(ptr->item_first==-1) {
          ptr->item_first=0;
        }
      }
      else {
        // slider merker loeschen
        ptr->status&=~SDROPDOWN_BIT_SLIDER;
        ptr->item_first=0;
      }
     
      if((ptr->status&SDROPDOWN_BIT_3DSTYLE)==0) {
        // Rahmen zeichnen
        SGUI_ScreenDrawFullRect(x,y+h,w,px_used+d,c2);
        ptr->exp_height=h+px_used+d;
        // listbox zeichnen
        SGUI_ScreenDrawFullRect(x+d,y+h,w-2*d,px_used,c1);    
      }
      else {        
        SGUI_ScreenDraw3DFrame(x,y+h,w,px_used+d,STYLE_LOWERED);
        ptr->exp_height=h+px_used+d;  
        // listbox zeichnen   
        SGUI_ScreenDrawFullRect(x+d,y+h,w-2*d,px_used,c1); 
      }
      // strings anzeigen
      fy=y+h; 
      fx=x+d;
      if(ptr->item_first+cnt>ptr->item_cnt) cnt=ptr->item_cnt-ptr->item_first;
      for(n=ptr->item_first;n<ptr->item_first+cnt;n++) {
        if(n==ptr->item_aktiv) {
          SGUI_ScreenDrawString(fx,fy,ptr->strlist[n],ptr->font,c1,c3);
        }
        else {
          SGUI_ScreenDrawString(fx,fy,ptr->strlist[n],ptr->font,c3,c1); 
        }
        fy+=ptr->font->height;
      }
      // check ob size > child
      if((ptr->status&SDROPDOWN_BIT_CHILD)!=0) {
        if(y+h+px_used+d>ptr->max_ypos) {
          ptr->status|=SDROPDOWN_BIT_SIZE;  // merker setzen
        }
      }
      // event. slider zeichnen
      if((ptr->status&SDROPDOWN_BIT_SLIDER)!=0) {        
        if((ptr->status&SDROPDOWN_BIT_3DSTYLE)==0) {          
          // linie zeichnen
          SGUI_ScreenDrawLine(btn1_xp,y+h,btn1_xp,y+h+px_used,c2);
        }
        else {
          sl_size=btn_size+d;
          SGUI_ScreenDraw3DFrame(x+w-sl_size,y+sl_size,sl_size,px_used+d,STYLE_LOWERED);
        }
        if(px_used>2*btn_size) {
          // pfeil-hoch zeichnen
          btn1_yp=y+ptr->height-1;
          SGUI_ScreenDrawArrowBtn(btn1_xp,btn1_yp,btn_size,SARROW_UP,c2,c1);
          // pfeil-runter zeichnen
          btn2_xp=btn1_xp;
          btn2_yp=y+ptr->exp_height-d-btn_size+1;
          SGUI_ScreenDrawArrowBtn(btn2_xp,btn2_yp,btn_size,SARROW_DOWN,c2,c1);
          if((ptr->status&SDROPDOWN_BIT_3DSTYLE)!=0) { 
            SGUI_ScreenDraw3DFrame(btn1_xp,btn1_yp,btn_size,btn_size,STYLE_RAISED); 
            SGUI_ScreenDraw3DFrame(btn2_xp,btn2_yp,btn_size,btn_size,STYLE_RAISED);              
          }
        }
      }
    }    
  }
}
