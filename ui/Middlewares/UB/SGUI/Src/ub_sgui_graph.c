//--------------------------------------------------------------
// File     : ub_sgui_graph.c
// Datum    : 27.07.2015
// Version  : 1.0
// Autor    : UB
// EMail    : mc-4u(@)t-online.de
// Web      : www.mikrocontroller-4u.de
// CPU      : STM32F746
// IDE      : OpenSTM32
// GCC      : 4.9 2015q2
// Module   : SGUI_WINDOW
// Funktion : Graphs dienen zum anzeigen von Werten
//            als Kurve bzw. Linie
//--------------------------------------------------------------



//--------------------------------------------------------------
// Includes
//--------------------------------------------------------------
#include "ub_sgui_graph.h"




//--------------------------------------------------------------
// interne Funktionen
//--------------------------------------------------------------
void P_SGUI_GraphDraw(SGRAPH_t* ptr);
DataArray_t* P_SGUI_GraphCreateDataArray(uint8_t ch, uint16_t cnt);



//--------------------------------------------------------------
// ein GRAPH neu erzeugen
// x,y = Linke obere Ecke vom Graph auf dem Display
// w = Breite (minimum 5 Pixel)
// h = hoehe  (minimum 5 Pixel)
//--------------------------------------------------------------
SGRAPH_t* SGUI_GraphCreate(uint16_t x, uint16_t y, uint16_t w, uint16_t h)
{
  SGRAPH_t *ptr;
  size_t obj_size;
  uint16_t n;

  if(SGUI_WINDOW.aktiv_window==0) return NULL;
  if(SGUI_WINDOW.akt_win_ptr==NULL) return NULL;
  if(SGUI_GRAPH.akt_anz>=(MAX_GRAPH_ANZ-1)) return NULL;

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
  obj_size=sizeof(SGRAPH_t);
  ptr = (SGRAPH_t *) malloc(obj_size);
  if(ptr==NULL) return NULL;
  
  // init aller Werte
  ptr->window_nr=SGUI_WINDOW.aktiv_window;
  ptr->style=STYLE_LOWERED;
  ptr->xpos=x;
  ptr->ypos=y;
  ptr->width=w;
  ptr->height=h;
  ptr->frame_size=SGRAPH_DEF_FRAME_SIZE;
  ptr->col_frame=SGRAPH_DEF_FRAME_COL;
  ptr->col_back=SGRAPH_DEF_BACK_COL;
  
  ptr->ch_cnt=0;
  ptr->item_cnt=0;  
  ptr->data=NULL;
  for(n=0;n<MAX_GRAPHCH_ANZ;n++) {    
    ptr->ch[n].visible=true;
    ptr->ch[n].col=SGRAPH_DEF_CH_COL;
  }
  
  ptr->status=0x00;

  ptr->status|=SGRAPH_BIT_REDRAW;  // redraw at next loop
  SGUI_GRAPH.update=true;

  SGUI_GRAPH.ptr[SGUI_GRAPH.akt_anz]=ptr;
  SGUI_GRAPH.akt_anz++;
  
  // benötigter Speicherplatz
  SGUI_WINDOW.ram_used+=obj_size;  

  return ptr;
}

//--------------------------------------------------------------
// Style von einem Graph einstellen
// style : [STYLE_FLAT, STYLE_RAISED, STYLE_LOWERED]
//--------------------------------------------------------------
void SGUI_GraphSetStyle(SGRAPH_t* ptr, STYLE_TYP_t style)
{
  if(ptr==NULL) return;
  
  if(style!=ptr->style) {
    ptr->style=style;
    ptr->status|=SGRAPH_BIT_REDRAW;  // redraw at next loop
    SGUI_GRAPH.update=true;
  }  
}

//--------------------------------------------------------------
// Rahmendicke von einem Graph einstellen
// px = dicke in pixel (0 = ohne Rahmen)
//--------------------------------------------------------------
void SGUI_GraphSetFrameSize(SGRAPH_t* ptr, uint8_t px)
{
  if(ptr==NULL) return;

  if(px!=ptr->frame_size) {
    ptr->frame_size=px;
    ptr->status|=SGRAPH_BIT_REDRAW;  // redraw at next loop
    SGUI_GRAPH.update=true;
  }
}

//--------------------------------------------------------------
// Farbe von einem Graph einstellen
// c1 = Rahmen
// c2 = Hintergrund
//--------------------------------------------------------------
void SGUI_GraphSetColor(SGRAPH_t* ptr, uint16_t c1, uint16_t c2)
{
  if(ptr==NULL) return;

  if(c1!=ptr->col_frame) {
    ptr->col_frame=c1;
    ptr->status|=SGRAPH_BIT_REDRAW;  // redraw at next loop
    SGUI_GRAPH.update=true;
  }  
  if(c2!=ptr->col_back) {
    ptr->col_back=c2;
    ptr->status|=SGRAPH_BIT_REDRAW;  // redraw at next loop
    SGUI_GRAPH.update=true;
  }
}


//--------------------------------------------------------------
// DatenArray vom Graph erstellen
// ch = Anzahl der Kanäle [1...MAX_GRAPHCH_ANZ]
// cnt = Anzahl der Daten pro Kanal [1...MAX_GRAPHDATA_ANZ]
//--------------------------------------------------------------
void SGUI_GraphCreateDataArray(SGRAPH_t* ptr, uint8_t ch, uint16_t cnt)
{
  DataArray_t *data;
  
  if(ptr==NULL) return;
  if((ch==0) || (ch>MAX_GRAPHCH_ANZ)) return;
  if((cnt==0) || (cnt>MAX_GRAPHDATA_ANZ)) return;
  
  if((ptr->ch_cnt==0) && (ptr->item_cnt==0)) {
    // DatenArray kann nur einmal erstellt werden
    data=P_SGUI_GraphCreateDataArray(ch,cnt);
    if(data==NULL) return;
    ptr->ch_cnt=ch;
    ptr->item_cnt=cnt;
    ptr->data=data;
    
    // benötigter Speicherplatz
    SGUI_WINDOW.ram_used+=(ch*4)+(ch*cnt);    
  }
}

//--------------------------------------------------------------
// Wert von einem Eintrag eines Kanals einstellen
// ch = Kanal [0...ch_cnt-1]
// pos = Position [0...item_cnt-1]
//--------------------------------------------------------------
void SGUI_GraphSetCHValue(SGRAPH_t* ptr, uint8_t ch, uint16_t pos, uint8_t value)
{
  if(ptr==NULL) return;
  if(ch>=ptr->ch_cnt) return;  
  if(pos>=ptr->item_cnt) return;
    
  if(value!=ptr->data[ch][pos]) {
    ptr->data[ch][pos]=value;
    ptr->status|=SGRAPH_BIT_REDRAW;  // redraw at next loop
    SGUI_GRAPH.update=true;     
  }
}

//--------------------------------------------------------------
// Wert von einem Eintrag eines Kanals auslesen
// ch = Kanal [0...ch_cnt-1]
// pos = Position [0...item_cnt-1]
//--------------------------------------------------------------
uint8_t SGUI_GraphGetCHValue(SGRAPH_t* ptr, uint8_t ch, uint16_t pos)
{
  if(ptr==NULL) return 0;
  if(ch>=ptr->ch_cnt) return 0;  
  if(pos>=ptr->item_cnt) return 0;
  
  return ptr->data[ch][pos];
}

//--------------------------------------------------------------
// Kanal von einer Graph ein,ausschalten
// ch = Kanal [0...ch_cnt-1]
//--------------------------------------------------------------
void SGUI_GraphSetCHVisible(SGRAPH_t* ptr, uint8_t ch, bool visible)
{
  if(ptr==NULL) return; 
  if(ch>=ptr->ch_cnt) return;
  
  if(visible!=ptr->ch[ch].visible) {
    ptr->ch[ch].visible=visible;
    ptr->status|=SGRAPH_BIT_REDRAW;  // redraw at next loop
    SGUI_GRAPH.update=true;      
  }
}

//--------------------------------------------------------------
// Farbe von einem Kanal einstellen
// ch = Kanal [0...ch_cnt-1]
// c = Farbe
//--------------------------------------------------------------
void SGUI_GraphSetCHColor(SGRAPH_t* ptr, uint8_t ch, uint16_t c)
{
  if(ptr==NULL) return; 
  if(ch>=ptr->ch_cnt) return;
  
  if(c!=ptr->ch[ch].col) {
    ptr->ch[ch].col=c;
    ptr->status|=SGRAPH_BIT_REDRAW;  // redraw at next loop
    SGUI_GRAPH.update=true;      
  }
}

//--------------------------------------------------------------
// interne Funktion fuer SGUI
// init der Objekte
//--------------------------------------------------------------
void SGUI_GraphInit(void)
{
  uint32_t n;

  SGUI_GRAPH.update=false;
  SGUI_GRAPH.akt_anz=0;  
  for(n=0;n<MAX_GRAPH_ANZ;n++) {
    SGUI_GRAPH.ptr[n]=NULL;
  }
}

//--------------------------------------------------------------
// interne Funktion fuer SGUI
// updated alle Objekte
// (wird aufgerufen, falls sich etwas verändert hat)
//--------------------------------------------------------------
void SGUI_GraphUpdate(void)
{
  uint32_t n;
  uint8_t status=0;

  for(n=0;n<SGUI_GRAPH.akt_anz;n++) {
    // nur zeichnen, bei aktivem window
    if(SGUI_WINDOW.aktiv_window==SGUI_GRAPH.ptr[n]->window_nr) {
      // test ob graph neu gezeichnet werden muss
      status=(SGUI_GRAPH.ptr[n]->status&SGRAPH_BIT_REDRAW);
      if((status!=0) || (SGUI_WINDOW.update_all==true)) P_SGUI_GraphDraw(SGUI_GRAPH.ptr[n]);
    }
  }
}


//--------------------------------------------------------------
// interne Funktion
// ein leeres zwei Dimensionales DatanArray neu erzeugen
// ch = Anzahl der Kanäle (uint8_t)
// cnt = Anzahl der Einträge (uint8_t)
// Speicherbedarf (bytes) = (ch*4) + (ch*cnt)
//--------------------------------------------------------------
DataArray_t* P_SGUI_GraphCreateDataArray(uint8_t ch, uint16_t cnt)
{
  DataArray_t* ptr;
  uint16_t n,m;
  
  if(cnt==0) return NULL;
  if(ch==0) return NULL;
  if(ch>=MAX_GRAPHCH_ANZ) return NULL;
  if(cnt>=MAX_GRAPHDATA_ANZ) return NULL;
  
  // speicher reservieren 
  // zuerst 'ch' Pointer auf uint8_t reservieren
  ptr = (uint8_t **) malloc(ch * sizeof(uint8_t *));  
  if(ptr==NULL) return NULL;
  // dann 'ch' datenarrays reservieren
  for (n = 0; n < ch; n += 1) {
    ptr[n] = (uint8_t *) malloc(cnt * sizeof(uint8_t));    
    if(ptr[n]==NULL) return NULL;
  }
  
  // init aller Werte
  for(n=0;n<ch;n++) {
    for(m=0;m<cnt;m++) {
      ptr[n][m]=0x00; 
    }
  }  
   
  return ptr;
}


//--------------------------------------------------------------
// interne Funktion
// zeichnet ein Graph
//--------------------------------------------------------------
void P_SGUI_GraphDraw(SGRAPH_t* ptr)
{
  uint16_t x,y,w,h,c1,c2,d=0;
  uint16_t n,m,v,xs,ys,s,t;
  uint16_t xp1,yp1,xp2,yp2;

  if(ptr==NULL) return;

  // Flag vom zeichnen zuruecksetzen
  ptr->status&=~SGRAPH_BIT_REDRAW;

  x=ptr->xpos;
  y=ptr->ypos;
  w=ptr->width;
  h=ptr->height;
  
  c1=ptr->col_frame;
  c2=ptr->col_back;

  // hintergrund zeichnen
  if(ptr->style==STYLE_FLAT) {
    d=ptr->frame_size;
    if(d>0) {
      // zuerst den rahmen zeichnen
      SGUI_ScreenDrawFullRect(x,y,w,h,c1);
      // dann das graph zeichnen
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
    // 3d-sytle
    d=2;
    SGUI_ScreenDrawFullRect(x,y,w,h,c2);
    SGUI_ScreenDraw3DFrame(x,y,w,h,ptr->style);
  }
  
  // alle kurven zeichnen
  for(n=0;n<ptr->ch_cnt;n++) {
    // nur wenn kurve sichtbar und mindestens 1 messwert
    if((ptr->ch[n].visible==true) && (ptr->item_cnt>0)) {
      // startpunkte (x,y)
      xs=x+d;
      ys=y+h-d-1;    
      // schrittweite in x richtung
      s=(w-(2*d))/(ptr->item_cnt);
      // max hoehe in y richtung
      t=(h-(2*d));
      // punkt-0 zeichnen     
      xp1=xs;
      v=ptr->data[n][0];
      if(v>t) v=t;
      yp1=ys-v;
      SGUI_ScreenDrawPixel(xp1,yp1,ptr->ch[n].col);      
      if(m==1) return;
      // punkte-1...n zeichnen (mit linien verbinden)     
      for(m=1;m<ptr->item_cnt;m++) {
        xp2=xp1+s;
        v=ptr->data[n][m];
        if(v>t) v=t;        
        yp2=ys-v;      
        SGUI_ScreenDrawLine(xp1,yp1,xp2,yp2,ptr->ch[n].col);
        xp1=xp2;
        yp1=yp2;
      }
    }
  }
}
