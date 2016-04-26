//--------------------------------------------------------------
// File     : ub_sgui_text.c
// Datum    : 27.07.2015
// Version  : 1.0
// Autor    : UB
// EMail    : mc-4u(@)t-online.de
// Web      : www.mikrocontroller-4u.de
// CPU      : STM32F746
// IDE      : OpenSTM32
// GCC      : 4.9 2015q2
// Module   : SGUI_WINDOW
// Funktion : Mit Texten können Strings oder Zahlen (Int/Float)
//            angezeigt werden.
//            Geschrieben wird an aktueller CursorPos
//            und mit aktuellem Font und Farbe
//            Per "Print" werden die Texte direkt angezeigt
//            (ohne gespeichert zu werden)
//            Per "Create" werden Objekte erzeugt die
//            dauerhaft sind und auch geändert werden können.
//--------------------------------------------------------------


//--------------------------------------------------------------
// Includes
//--------------------------------------------------------------
#include "ub_sgui_text.h"



//--------------------------------------------------------------
// interne Funktionen
//--------------------------------------------------------------
void P_SGUI_TextDraw(STEXT_t* ptr);



//--------------------------------------------------------------
// setzt Cursor an eine Position
//--------------------------------------------------------------
void SGUI_TextSetCursor(uint16_t x, uint16_t y)
{
  if(SGUI_WINDOW.aktiv_window==0) return;
  if(SGUI_WINDOW.akt_win_ptr==NULL) return;

  // parameter check
  if(SGUI_WINDOW.akt_win_ptr->typ==SWINDOW_CHILD) {
    // bei Child-Window, Offset fuer x und y
    x+=SGUI_WINDOW.akt_win_ptr->xpos;
    y+=SGUI_WINDOW.akt_win_ptr->ypos;
  }
  if(x>=SGUI_SCREEN_MAXX) x=(SGUI_SCREEN_MAXX-1);
  if(y>=SGUI_SCREEN_MAXY) y=(SGUI_SCREEN_MAXY-1);

  SGUI_TEXT.akt.cursor_x=x;
  SGUI_TEXT.akt.cursor_y=y;
  SGUI_TEXT.akt.cursor_oldx=x; // fuer linefeed speichern
}

//--------------------------------------------------------------
// macht einen Linefeed vom Cursor
//--------------------------------------------------------------
void SGUI_TextCursorLinefeed(void)
{
  SGUI_TEXT.akt.cursor_x=SGUI_TEXT.akt.cursor_oldx;
  SGUI_TEXT.akt.cursor_y+=SGUI_TEXT.akt.font->height;
}

//--------------------------------------------------------------
// stellt default Font für Textausgabe ein
//--------------------------------------------------------------
void SGUI_TextSetDefFont(UB_Font *font)
{
  SGUI_TEXT.akt.font=font;
}

//--------------------------------------------------------------
// stellt default Farbe für Textausgabe ein
//--------------------------------------------------------------
void SGUI_TextSetDefColor(uint16_t c1, uint16_t c2)
{
  SGUI_TEXT.akt.col_text=c1;
  SGUI_TEXT.akt.col_back=c2;  
}

//--------------------------------------------------------------
// einen String anzeigen
// an aktueller x,y position (Cursor wird weitergeschaltet)
// mit aktueller Farbe und Schriftart
// Hinweis : der Text verschwindet nach einem wechsel vom Window
//--------------------------------------------------------------
void SGUI_TextPrintString(char *txt_ptr)
{
  uint16_t x,y,l,px;

  if(txt_ptr==NULL) return;  
  
  x=SGUI_TEXT.akt.cursor_x;
  y=SGUI_TEXT.akt.cursor_y;  
  // Text zeichnen
  SGUI_ScreenDrawString(x,y,txt_ptr,SGUI_TEXT.akt.font,SGUI_TEXT.akt.col_text,SGUI_TEXT.akt.col_back);
  
  // Cursor weiterschalten  
  l=strlen(txt_ptr);
  if(l>0) {
    px=l*SGUI_TEXT.akt.font->width;
    SGUI_TEXT.akt.cursor_x+=px;
    if(SGUI_TEXT.akt.cursor_x>=SGUI_SCREEN_MAXX) {
      SGUI_TEXT.akt.cursor_x=0; 
      px=SGUI_TEXT.akt.font->height;
      SGUI_TEXT.akt.cursor_y+=px;
    }         
  }
}

//--------------------------------------------------------------
// eine Integer Zahl anzeigen
// an aktueller x,y position (Cursor wird weitergeschaltet)
// mit aktueller Farbe und Schriftart
// Hinweis : der Text verschwindet nach einem wechsel vom Window
// digits : anzahl der stellen
// padding : true => mit fuehrenden Nullen
//--------------------------------------------------------------
void SGUI_TextPrintInt(int32_t value, uint8_t digits, bool padding)
{
  uint16_t x,y,l,px;
  
  x=SGUI_TEXT.akt.cursor_x;
  y=SGUI_TEXT.akt.cursor_y; 
  // Zahl in String wandeln
  SGUI_WindowFormatInt(value, digits, padding);
  // Text zeichnen
  SGUI_ScreenDrawString(x,y,SGUI_WINDOW.buf,SGUI_TEXT.akt.font,SGUI_TEXT.akt.col_text,SGUI_TEXT.akt.col_back);
  
  // Cursor weiterschalten  
  l=strlen(SGUI_WINDOW.buf);
  if(l>0) {
    px=l*SGUI_TEXT.akt.font->width;
    SGUI_TEXT.akt.cursor_x+=px;
    if(SGUI_TEXT.akt.cursor_x>=SGUI_SCREEN_MAXX) {
      SGUI_TEXT.akt.cursor_x=0; 
      px=SGUI_TEXT.akt.font->height;
      SGUI_TEXT.akt.cursor_y+=px;
    }         
  }  
}

//--------------------------------------------------------------
// eine Float Zahl anzeigen
// an aktueller x,y position (Cursor wird weitergeschaltet)
// mit aktueller Farbe und Schriftart
// Hinweis : der Text verschwindet nach einem wechsel vom Window
// digits : anzahl der stellen
// padding : true => mit fuehrenden Nullen
//--------------------------------------------------------------
void SGUI_TextPrintFloat(float value, uint8_t digits, bool padding)
{
  uint16_t x,y,l,px;

  x=SGUI_TEXT.akt.cursor_x;
  y=SGUI_TEXT.akt.cursor_y;
  // Zahl in String wandeln
  SGUI_WindowFormatFloat(value, digits, padding);
  // Text zeichnen
  SGUI_ScreenDrawString(x,y,SGUI_WINDOW.buf,SGUI_TEXT.akt.font,SGUI_TEXT.akt.col_text,SGUI_TEXT.akt.col_back);

  // Cursor weiterschalten
  l=strlen(SGUI_WINDOW.buf);
  if(l>0) {
    px=l*SGUI_TEXT.akt.font->width;
    SGUI_TEXT.akt.cursor_x+=px;
    if(SGUI_TEXT.akt.cursor_x>=SGUI_SCREEN_MAXX) {
      SGUI_TEXT.akt.cursor_x=0;
      px=SGUI_TEXT.akt.font->height;
      SGUI_TEXT.akt.cursor_y+=px;
    }
  }
}

//--------------------------------------------------------------
// einen String neu erzeugen
// an aktueller x,y position (Cursor wird weitergeschaltet)
// mit aktueller Farbe und Schriftart
//--------------------------------------------------------------
STEXT_t* SGUI_TextCreateString(char *txt_ptr)
{
  STEXT_t *ptr;
  size_t obj_size;
  uint16_t l,px;

  if(SGUI_WINDOW.aktiv_window==0) return NULL;
  if(SGUI_TEXT.akt_anz>=(MAX_TEXT_ANZ-1)) return NULL;

  // speicher reservieren
  obj_size=sizeof(STEXT_t);
  ptr = (STEXT_t *) malloc(obj_size);
  if(ptr==NULL) return NULL;
  
  // init aller Werte
  ptr->window_nr=SGUI_WINDOW.aktiv_window;
  ptr->typ=STEXT_ASCII;
  ptr->xpos=SGUI_TEXT.akt.cursor_x;
  ptr->ypos=SGUI_TEXT.akt.cursor_y;
  ptr->col_text=SGUI_TEXT.akt.col_text;
  ptr->col_back=SGUI_TEXT.akt.col_back;
  ptr->txt_ptr=txt_ptr;
  ptr->i32_value=0;
  ptr->f_value=0.0;
  ptr->digits=1; 
  ptr->font=SGUI_TEXT.akt.font;
  ptr->status=0x00;

  ptr->status|=STEXT_BIT_REDRAW;  // redraw at next loop
  SGUI_TEXT.update=true;

  SGUI_TEXT.ptr[SGUI_TEXT.akt_anz]=ptr;
  SGUI_TEXT.akt_anz++;  
  
  // Cursor weiterschalten
  if(ptr->txt_ptr!=NULL) {
    l=strlen(ptr->txt_ptr);
    if(l>0) {
      px=l*ptr->font->width;
      SGUI_TEXT.akt.cursor_x+=px;
      if(SGUI_TEXT.akt.cursor_x>=SGUI_SCREEN_MAXX) {
        SGUI_TEXT.akt.cursor_x=0; 
        px=ptr->font->height;
        SGUI_TEXT.akt.cursor_y+=px;
      }         
    }
  }
  
  // benötigter Speicherplatz
  SGUI_WINDOW.ram_used+=obj_size;  

  return ptr;  
}

//--------------------------------------------------------------
// eine Integer Zahl neu erzeugen
// an aktueller x,y position (Cursor wird weitergeschaltet)
// mit aktueller Farbe und Schriftart
// digits : anzahl der stellen
// padding : true => mit fuehrenden Nullen
//--------------------------------------------------------------
STEXT_t* SGUI_TextCreateInt(int32_t value, uint8_t digits, bool padding)
{
  STEXT_t *ptr;
  size_t obj_size;
  uint16_t l,px;

  if(SGUI_WINDOW.aktiv_window==0) return NULL;
  if(SGUI_TEXT.akt_anz>=(MAX_TEXT_ANZ-1)) return NULL;

  // speicher reservieren
  obj_size=sizeof(STEXT_t);
  ptr = (STEXT_t *) malloc(obj_size);
  if(ptr==NULL) return NULL;
  
  // init aller Werte
  ptr->window_nr=SGUI_WINDOW.aktiv_window;
  ptr->typ=STEXT_I32;
  ptr->xpos=SGUI_TEXT.akt.cursor_x;
  ptr->ypos=SGUI_TEXT.akt.cursor_y;
  ptr->col_text=SGUI_TEXT.akt.col_text;
  ptr->col_back=SGUI_TEXT.akt.col_back;
  ptr->txt_ptr=NULL;
  ptr->i32_value=value;
  ptr->f_value=0.0;
  ptr->digits=digits;
  ptr->font=SGUI_TEXT.akt.font;
  ptr->status=0x00;
  
  if(padding==true) ptr->status|=STEXT_BIT_PADDING;

  ptr->status|=STEXT_BIT_REDRAW;  // redraw at next loop
  SGUI_TEXT.update=true;

  SGUI_TEXT.ptr[SGUI_TEXT.akt_anz]=ptr;
  SGUI_TEXT.akt_anz++;  
  
  // Zahl in String wandeln
  SGUI_WindowFormatInt(value, digits, padding);

  // Cursor weiterschalten  
  l=strlen(SGUI_WINDOW.buf);
  if(l>0) {
    px=l*ptr->font->width;
    SGUI_TEXT.akt.cursor_x+=px;
    if(SGUI_TEXT.akt.cursor_x>=SGUI_SCREEN_MAXX) {
      SGUI_TEXT.akt.cursor_x=0; 
      px=ptr->font->height;
      SGUI_TEXT.akt.cursor_y+=px;
    }         
  }
  
  // benötigter Speicherplatz
  SGUI_WINDOW.ram_used+=obj_size;   

  return ptr;  
}

//--------------------------------------------------------------
// eine Float Zahl neu erzeugen
// an aktueller x,y position (Cursor wird weitergeschaltet)
// mit aktueller Farbe und Schriftart
// digits : anzahl der stellen
// padding : true => mit fuehrenden Nullen
//--------------------------------------------------------------
STEXT_t* SGUI_TextCreateFloat(float value, uint8_t digits, bool padding)
{
  STEXT_t *ptr;
  size_t obj_size;
  uint16_t l,px;

  if(SGUI_WINDOW.aktiv_window==0) return NULL;
  if(SGUI_TEXT.akt_anz>=(MAX_TEXT_ANZ-1)) return NULL;

  // speicher reservieren
  obj_size=sizeof(STEXT_t);
  ptr = (STEXT_t *) malloc(obj_size);
  if(ptr==NULL) return NULL;

  // init aller Werte
  ptr->window_nr=SGUI_WINDOW.aktiv_window;
  ptr->typ=STEXT_FLOAT;
  ptr->xpos=SGUI_TEXT.akt.cursor_x;
  ptr->ypos=SGUI_TEXT.akt.cursor_y;
  ptr->col_text=SGUI_TEXT.akt.col_text;
  ptr->col_back=SGUI_TEXT.akt.col_back;
  ptr->txt_ptr=NULL;
  ptr->i32_value=0;
  ptr->f_value=value;
  ptr->digits=digits;
  ptr->font=SGUI_TEXT.akt.font;
  ptr->status=0x00;

  if(padding==true) ptr->status|=STEXT_BIT_PADDING;

  ptr->status|=STEXT_BIT_REDRAW;  // redraw at next loop
  SGUI_TEXT.update=true;

  SGUI_TEXT.ptr[SGUI_TEXT.akt_anz]=ptr;
  SGUI_TEXT.akt_anz++;

  // Zahl in String wandeln
  SGUI_WindowFormatFloat(value, digits, padding);

  // Cursor weiterschalten
  l=strlen(SGUI_WINDOW.buf);
  if(l>0) {
    px=l*ptr->font->width;
    SGUI_TEXT.akt.cursor_x+=px;
    if(SGUI_TEXT.akt.cursor_x>=SGUI_SCREEN_MAXX) {
      SGUI_TEXT.akt.cursor_x=0;
      px=ptr->font->height;
      SGUI_TEXT.akt.cursor_y+=px;
    }
  }

  // benötigter Speicherplatz
  SGUI_WINDOW.ram_used+=obj_size;

  return ptr;
}

//--------------------------------------------------------------
// Farbe von einem Text einstellen
// c1 = Text
// c2 = Hintergrund
//--------------------------------------------------------------
void SGUI_TextSetColor(STEXT_t* ptr, uint16_t c1, uint16_t c2)
{
  if(ptr==NULL) return;

  if(c1!=ptr->col_text) {
    ptr->col_text=c1;
    ptr->status|=STEXT_BIT_REDRAW;  // redraw at next loop
    SGUI_TEXT.update=true;
  }
  if(c2!=ptr->col_back) {
    ptr->col_back=c2;
    ptr->status|=STEXT_BIT_REDRAW;  // redraw at next loop
    SGUI_TEXT.update=true;
  }
}

//--------------------------------------------------------------
// Font von einem Text einstellen
//--------------------------------------------------------------
void SGUI_TextSetFont(STEXT_t* ptr, UB_Font* font)
{
  if(ptr==NULL) return;

  if(font!=ptr->font) {
    ptr->font=font;
    ptr->status|=STEXT_BIT_REDRAW;  // redraw at next loop
    SGUI_TEXT.update=true;
  }
}

//--------------------------------------------------------------
// String von einem Text aendern
// txt_ptr = String
//--------------------------------------------------------------
void SGUI_TextSetString(STEXT_t* ptr, char *txt_ptr)
{
  if(ptr==NULL) return;

  if(ptr->typ!=STEXT_ASCII) return;  
  
  ptr->txt_ptr=txt_ptr;
  ptr->status|=STEXT_BIT_REDRAW;  // redraw at next loop
  SGUI_TEXT.update=true;   
}

//--------------------------------------------------------------
// Wert von einer Integer-Zahl aendern
// value = Wert
//--------------------------------------------------------------
void SGUI_TextSetInt(STEXT_t* ptr, int32_t value)
{
  if(ptr==NULL) return;

  if(ptr->typ!=STEXT_I32) return;

  if(value!=ptr->i32_value) {
    ptr->i32_value=value;
    ptr->status|=STEXT_BIT_REDRAW;  // redraw at next loop
    SGUI_TEXT.update=true;    
  }
}

//--------------------------------------------------------------
// Wert von einer Flat-Zahl aendern
// value = Wert
//--------------------------------------------------------------
void SGUI_TextSetFloat(STEXT_t* ptr, float value)
{
  if(ptr==NULL) return;

  if(ptr->typ!=STEXT_FLOAT) return;

  if(value!=ptr->f_value) {
    ptr->f_value=value;
    ptr->status|=STEXT_BIT_REDRAW;  // redraw at next loop
    SGUI_TEXT.update=true;
  }
}

//--------------------------------------------------------------
// interne Funktion fuer SGUI
// init der Objekte
//--------------------------------------------------------------
void SGUI_TextInit(void)
{
  uint32_t n;
  
  SGUI_TEXT.akt.cursor_oldx=0;
  SGUI_TEXT.akt.cursor_x=0;
  SGUI_TEXT.akt.cursor_y=0;
  SGUI_TEXT.akt.col_text=STEXT_DEF_TEXT_COL;
  SGUI_TEXT.akt.col_back=STEXT_DEF_BACK_COL;  
  SGUI_TEXT.akt.font=STEXT_DEF_FONT;

  SGUI_TEXT.update=false;
  SGUI_TEXT.akt_anz=0;    
  for(n=0;n<MAX_TEXT_ANZ;n++) {
    SGUI_TEXT.ptr[n]=NULL;
  }
}

//--------------------------------------------------------------
// interne Funktion fuer SGUI
// updated alle Objekte
// (wird aufgerufen, falls sich etwas verändert hat)
//--------------------------------------------------------------
void SGUI_TextUpdate(void)
{
  uint32_t n;
  uint8_t status=0;

  for(n=0;n<SGUI_TEXT.akt_anz;n++) {
    // nur zeichnen, bei aktivem window
    if(SGUI_WINDOW.aktiv_window==SGUI_TEXT.ptr[n]->window_nr) {
      // test ob Text neu gezeichnet werden muss
      status=(SGUI_TEXT.ptr[n]->status&STEXT_BIT_REDRAW);
      if((status!=0) || (SGUI_WINDOW.update_all==true)) P_SGUI_TextDraw(SGUI_TEXT.ptr[n]);
    }
  } 
}

//--------------------------------------------------------------
// interne Funktion
// zeichnet einen Text oder eine Zahl
//--------------------------------------------------------------
void P_SGUI_TextDraw(STEXT_t* ptr)
{
  uint16_t x,y;

  if(ptr==NULL) return;

  // Flag vom zeichnen zuruecksetzen
  ptr->status&=~STEXT_BIT_REDRAW;

  x=ptr->xpos;
  y=ptr->ypos;

  if(ptr->typ==STEXT_ASCII) {
    // Text zeichnen
    SGUI_ScreenDrawString(x,y,ptr->txt_ptr,ptr->font,ptr->col_text,ptr->col_back);
  }
  else if(ptr->typ==STEXT_I32) {
    // integer zahl in string wandeln
    if((ptr->status&STEXT_BIT_PADDING)==0) {
      SGUI_WindowFormatInt(ptr->i32_value, ptr->digits, false);
    }
    else {
      SGUI_WindowFormatInt(ptr->i32_value, ptr->digits, true);
    }

    // Text zeichnen
    SGUI_ScreenDrawString(x,y,SGUI_WINDOW.buf,ptr->font,ptr->col_text,ptr->col_back);
  }
  else if(ptr->typ==STEXT_FLOAT) {
    // float zahl in string wandeln
    if((ptr->status&STEXT_BIT_PADDING)==0) {
      SGUI_WindowFormatFloat(ptr->f_value, ptr->digits, false);
    }
    else {
      SGUI_WindowFormatFloat(ptr->f_value, ptr->digits, true);
    }

    // Text zeichnen
    SGUI_ScreenDrawString(x,y,SGUI_WINDOW.buf,ptr->font,ptr->col_text,ptr->col_back);
  }
}

