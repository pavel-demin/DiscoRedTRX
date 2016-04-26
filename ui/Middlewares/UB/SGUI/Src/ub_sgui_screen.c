//--------------------------------------------------------------
// File     : ub_sgui_screen.c
// Datum    : 27.07.2015
// Version  : 1.1
// Autor    : UB
// EMail    : mc-4u(@)t-online.de
// Web      : www.mikrocontroller-4u.de
// CPU      : STM32F746
// IDE      : OpenSTM32
// GCC      : 4.9 2015q2
// Module   : UB_GRAPHIC2D, UB_FONT
// Funktion : LCD-Screen-Funktionen fuer das SGUI-Projekt
//--------------------------------------------------------------


//--------------------------------------------------------------
// Includes
//--------------------------------------------------------------
#include "ub_sgui_screen.h"



//--------------------------------------------------------------
// interne Funktionen
//--------------------------------------------------------------
void P_SGUI_Screen3DArc(uint16_t xp, uint16_t yp, uint16_t r, uint16_t c, uint8_t m);


//--------------------------------------------------------------
// Hinweis : diese Funktion event. an eigenes Display anpassen !!
//
// Zeichnet ein Pixel mit einer Farbe
// an xp,yp  [x=0...SGUI_SCREEN_MAXX, y=0...SGUI_SCREEN_MAXY]
// Farbe = c
//--------------------------------------------------------------
void SGUI_ScreenDrawPixel(uint16_t xp, uint16_t yp, uint16_t c)
{
  // check auf limit
  if(xp>=SGUI_SCREEN_MAXX) xp=SGUI_SCREEN_MAXX-1;
  if(yp>=SGUI_SCREEN_MAXY) yp=SGUI_SCREEN_MAXY-1;

  #if SGUI_SCREEN_ORIENTATION!=0
    UB_Graphic2D_DrawPixelNormal(xp,yp,c);
  #else
    uint16_t lcd_x, lcd_y;

    lcd_x=SGUI_SCREEN_MAXY-yp;
    lcd_y=xp;
    UB_Graphic2D_DrawPixelNormal(lcd_x,lcd_y,c);
  #endif
}

//--------------------------------------------------------------
// Hinweis : diese Funktion event. an eigenes Display anpassen !!
//
// Zeichnet eine Linie mit einer Farbe
// von x1,y1 nach x2,y2  [x=0...SGUI_SCREEN_MAXX, y=0...SGUI_SCREEN_MAXY]
// Farbe = c
//--------------------------------------------------------------
void SGUI_ScreenDrawLine(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint16_t c)
{
  // check auf limit
  if(x1>=SGUI_SCREEN_MAXX) x1=SGUI_SCREEN_MAXX-1;
  if(y1>=SGUI_SCREEN_MAXY) y1=SGUI_SCREEN_MAXY-1;
  if(x2>=SGUI_SCREEN_MAXX) x2=SGUI_SCREEN_MAXX-1;
  if(y2>=SGUI_SCREEN_MAXY) y2=SGUI_SCREEN_MAXY-1;

  #if SGUI_SCREEN_ORIENTATION!=0
    UB_Graphic2D_DrawLineNormal(x1,y1,x2,y2,c);
  #else
    uint16_t lcd_x1, lcd_y1;
    uint16_t lcd_x2, lcd_y2; 

    lcd_x1=SGUI_SCREEN_MAXY-y1-1;
    lcd_y1=x1;
    lcd_x2=SGUI_SCREEN_MAXY-y2-1;
    lcd_y2=x2;
    UB_Graphic2D_DrawLineNormal(lcd_x1,lcd_y1,lcd_x2,lcd_y2,c);
  #endif
}

//--------------------------------------------------------------
// Hinweis : diese Funktion event. an eigenes Display anpassen !!
//
// Zeichnet ein gefuelltes Rechteck mit einer Farbe
// Ecke   = xp,yp [x=0...SGUI_SCREEN_MAXX, y=0...SGUI_SCREEN_MAXY]
// Breite = w
// Hoehe  = h
// Farbe  = c
//--------------------------------------------------------------
void SGUI_ScreenDrawFullRect(uint16_t xp, uint16_t yp, uint16_t w, uint16_t h, uint16_t c)
{
  // check auf limit
  if(xp>=SGUI_SCREEN_MAXX) xp=SGUI_SCREEN_MAXX-1;
  if(yp>=SGUI_SCREEN_MAXY) yp=SGUI_SCREEN_MAXY-1;
  if((w==0) || (h==0)) return;
  if((xp+w)>SGUI_SCREEN_MAXX) w=SGUI_SCREEN_MAXX-xp;
  if((yp+h)>SGUI_SCREEN_MAXY) h=SGUI_SCREEN_MAXY-yp;
  

  #if SGUI_SCREEN_ORIENTATION!=0
    UB_Graphic2D_DrawFullRectDMA(xp,yp,w,h,c);
  #else
    uint16_t lcd_x, lcd_y;

    lcd_x=SGUI_SCREEN_MAXY-yp-h;
    lcd_y=xp;
    UB_Graphic2D_DrawFullRectDMA(lcd_x,lcd_y,w,h,c);
  #endif
}

//--------------------------------------------------------------
// Hinweis : diese Funktion event. an eigenes Display anpassen !!
//
// Zeichnet einen gefuellten Kreis mit einer Farbe
// Mittelpunkt   = xp,yp [x=0...SGUI_SCREEN_MAXX, y=0...SGUI_SCREEN_MAXY]
// Radius = r
//--------------------------------------------------------------
void SGUI_ScreenDrawFullCircle(uint16_t xp, uint16_t yp, uint16_t r, uint16_t c)
{
  // check auf limit
  if(xp>=SGUI_SCREEN_MAXX) xp=SGUI_SCREEN_MAXX-1;
  if(yp>=SGUI_SCREEN_MAXY) yp=SGUI_SCREEN_MAXY-1;
  if(r==0) return;
  if((xp+r)>SGUI_SCREEN_MAXX) r=SGUI_SCREEN_MAXX-xp;
  if((yp+r)>SGUI_SCREEN_MAXY) r=SGUI_SCREEN_MAXY-yp;

  #if SGUI_SCREEN_ORIENTATION!=0
    UB_Graphic2D_DrawFullCircleDMA(xp,yp,r,c);
  #else
    uint16_t lcd_x, lcd_y;

    lcd_x=SGUI_SCREEN_MAXY-yp;
    lcd_y=xp;
    UB_Graphic2D_DrawFullCircleDMA(lcd_x,lcd_y,r,c);
  #endif
}

//--------------------------------------------------------------
// Hinweis : diese Funktion event. an eigenes Display anpassen !!
//
// Zeichnet eine gerade Linie mit einer Farbe
// Startpunkt   = xp,yp [x=0...SGUI_SCREEN_MAXX, y=0...SGUI_SCREEN_MAXY]
// Länge = l
// Richtung = d
// Farbe = c
//--------------------------------------------------------------
void SGUI_ScreenStraight(uint16_t xp, uint16_t yp, uint16_t l, LCD_DIR_t d, uint16_t c)
{
  // check auf limit
  if(xp>=SGUI_SCREEN_MAXX) xp=SGUI_SCREEN_MAXX-1;
  if(yp>=SGUI_SCREEN_MAXY) yp=SGUI_SCREEN_MAXY-1;
  if(l==0) return;
  if(d==LCD_DIR_HORIZONTAL) {
    if((xp+l)>SGUI_SCREEN_MAXX) l=SGUI_SCREEN_MAXX-xp;
  }
  else {
    if((yp+l)>SGUI_SCREEN_MAXY) l=SGUI_SCREEN_MAXY-yp;
  }

  #if SGUI_SCREEN_ORIENTATION!=0
    UB_Graphic2D_DrawStraightDMA(xp,yp,l,d,c);
  #else
    uint16_t lcd_x, lcd_y;

    if(d==LCD_DIR_HORIZONTAL) {
      lcd_x=SGUI_SCREEN_MAXY-yp-1;
    }
    else {
      lcd_x=SGUI_SCREEN_MAXY-yp-l;
    }
    lcd_y=xp;
    UB_Graphic2D_DrawStraightDMA(lcd_x,lcd_y,l,d,c);
  #endif
}

//--------------------------------------------------------------
// Hinweis : diese Funktion event. an eigenes Display anpassen !!
//
// Zeichnet einen String eines monospaced Fonts an x,y Position
// mit Vorder- und Hintergrundfarbe (Font = max 16 Pixel breite)
// -> Font muss mit &-Operator uebergeben werden
// Ecke   = xp,yp [x=0...SGUI_SCREEN_MAXX, y=0...SGUI_SCREEN_MAXY]
//--------------------------------------------------------------
void SGUI_ScreenDrawString(uint16_t xp, uint16_t yp,char *ptr, UB_Font *font, uint16_t vg, uint16_t bg)
{
  // check auf limit
  if(xp>=SGUI_SCREEN_MAXX) xp=SGUI_SCREEN_MAXX-1;
  if(yp>=SGUI_SCREEN_MAXY) yp=SGUI_SCREEN_MAXY-1;
  if(ptr==NULL) return;
   
  #if SGUI_SCREEN_ORIENTATION!=0
    UB_Font_DrawString(xp,yp,ptr,font,vg,bg);
  #else
    uint16_t lcd_x, lcd_y;

    lcd_x=SGUI_SCREEN_MAXY-yp-font->height-1;
    lcd_y=xp;  
    UB_Font_DrawString(lcd_x,lcd_y,ptr,font,vg,bg);
  #endif
}

//--------------------------------------------------------------
// interne Funktion fuer SGUI
// Zeichnet einen Pfeil-Button
// x,y = position
// s = groesse
// m = richtung vom Pfeil
// c1 = rahmenfarbe
// c2 = hintergrundfarbe
//--------------------------------------------------------------
void SGUI_ScreenDrawArrowBtn(uint16_t x, uint16_t y, uint16_t s, SSCREEN_ARROW_t m, uint16_t c1, uint16_t c2)
{
  // check auf limit
  if(x>=SGUI_SCREEN_MAXX) x=SGUI_SCREEN_MAXX-1;
  if(y>=SGUI_SCREEN_MAXY) y=SGUI_SCREEN_MAXY-1;
  if(s==0) return;
  if((x+s)>SGUI_SCREEN_MAXX) s=SGUI_SCREEN_MAXX-x;
  if((y+s)>SGUI_SCREEN_MAXY) s=SGUI_SCREEN_MAXY-y;

  // zuerst den Rahmen zeichnen
  SGUI_ScreenDrawFullRect(x,y,s,s,c1);
  // dann den hintergrund zeichnen
  SGUI_ScreenDrawFullRect(x+1,y+1,s-2,s-2,c2);
  
  // Pfeil zeichnen
  if(m==SARROW_UP) {
    SGUI_ScreenDrawLine(x+4,y+s-4,x+s-4,y+s-4,c1);
    SGUI_ScreenDrawLine(x+4,y+s-4,x+s/2,y+4,c1);
    SGUI_ScreenDrawLine(x+s-4,y+s-4,x+s/2,y+4,c1);
  }
  else if(m==SARROW_DOWN) {
    SGUI_ScreenDrawLine(x+4,y+4,x+s-4,y+4,c1);
    SGUI_ScreenDrawLine(x+4,y+4,x+s/2,y+s-4,c1);
    SGUI_ScreenDrawLine(x+s-4,y+4,x+s/2,y+s-4,c1);
  }
  else if(m==SARROW_LEFT) {
    SGUI_ScreenDrawLine(x+s-4,y+4,x+s-4,y+s-4,c1);
    SGUI_ScreenDrawLine(x+s-4,y+4,x+4,y+s/2,c1);
    SGUI_ScreenDrawLine(x+s-4,y+s-4,x+4,y+s/2,c1);
   }
  else if(m==SARROW_RIGHT) {
    SGUI_ScreenDrawLine(x+4,y+4,x+4,y+s-4,c1);
    SGUI_ScreenDrawLine(x+4,y+4,x+s-4,y+s/2,c1);
    SGUI_ScreenDrawLine(x+4,y+s-4,x+s-4,y+s/2,c1);
  }
}

//--------------------------------------------------------------
// interne Funktion fuer SGUI
// zeichnet einen 3D-Rahmen
// x,y = position
// w,h = breite, hoehe
// style : [STYLE_RAISED, STYLE_LOWERED]
//--------------------------------------------------------------
void SGUI_ScreenDraw3DFrame(uint16_t x, uint16_t y, uint16_t w, uint16_t h, STYLE_TYP_t style)
{
  // check auf limit
  if(x>=SGUI_SCREEN_MAXX) x=SGUI_SCREEN_MAXX-1;
  if(y>=SGUI_SCREEN_MAXY) y=SGUI_SCREEN_MAXY-1;
  if((w==0) || (h==0)) return;
  if((x+w)>SGUI_SCREEN_MAXX) w=SGUI_SCREEN_MAXX-x;
  if((y+h)>SGUI_SCREEN_MAXY) h=SGUI_SCREEN_MAXY-y;

  if(style==STYLE_FLAT) {
    return; 
  }
  else if(style==STYLE_RAISED) {
    // col1 (oben+links)
    SGUI_ScreenStraight(x,y,w,LCD_DIR_HORIZONTAL,SGUI_3DCOL_1);
    SGUI_ScreenStraight(x,y,h,LCD_DIR_VERTICAL,SGUI_3DCOL_1);
    // col2 (oben+links)
    SGUI_ScreenStraight(x+1,y+1,w-2,LCD_DIR_HORIZONTAL,SGUI_3DCOL_2);
    SGUI_ScreenStraight(x+1,y+1,h-2,LCD_DIR_VERTICAL,SGUI_3DCOL_2);
    // col3 (unten+rechts)
    SGUI_ScreenStraight(x+1,y+h-2,w-2,LCD_DIR_HORIZONTAL,SGUI_3DCOL_3);
    SGUI_ScreenStraight(x+w-2,y+1,h-2,LCD_DIR_VERTICAL,SGUI_3DCOL_3);
    // col4 (unten+rechts)
    SGUI_ScreenStraight(x,y+h-1,w,LCD_DIR_HORIZONTAL,SGUI_3DCOL_4);
    SGUI_ScreenStraight(x+w-1,y,h,LCD_DIR_VERTICAL,SGUI_3DCOL_4);
  }
  else if(style==STYLE_LOWERED) {
    // col4 (oben+links)
    SGUI_ScreenStraight(x,y,w,LCD_DIR_HORIZONTAL,SGUI_3DCOL_4);
    SGUI_ScreenStraight(x,y,h,LCD_DIR_VERTICAL,SGUI_3DCOL_4);
    // col3 (oben+links)
    SGUI_ScreenStraight(x+1,y+1,w-2,LCD_DIR_HORIZONTAL,SGUI_3DCOL_3);
    SGUI_ScreenStraight(x+1,y+1,h-2,LCD_DIR_VERTICAL,SGUI_3DCOL_3);
    // col2 (unten+rechts)
    SGUI_ScreenStraight(x+1,y+h-2,w-2,LCD_DIR_HORIZONTAL,SGUI_3DCOL_2);
    SGUI_ScreenStraight(x+w-2,y+1,h-2,LCD_DIR_VERTICAL,SGUI_3DCOL_2);
    // col1 (unten+rechts)
    SGUI_ScreenStraight(x,y+h-1,w,LCD_DIR_HORIZONTAL,SGUI_3DCOL_1);
    SGUI_ScreenStraight(x+w-1,y,h,LCD_DIR_VERTICAL,SGUI_3DCOL_1);
  }
}

//--------------------------------------------------------------
// interne Funktion fuer SGUI
// zeichnet einen 3D-Kreis
// x,y = mittelpunkt
// r = Radius
// style : [STYLE_RAISED, STYLE_LOWERED]
//--------------------------------------------------------------
void SGUI_ScreenDraw3DCircle(uint16_t x, uint16_t y, uint16_t r, STYLE_TYP_t style)
{
  // check auf limit
  if(x>=SGUI_SCREEN_MAXX) x=SGUI_SCREEN_MAXX-1;
  if(y>=SGUI_SCREEN_MAXY) y=SGUI_SCREEN_MAXY-1;
  if(r==0) return;
  if((x+r)>SGUI_SCREEN_MAXX) r=SGUI_SCREEN_MAXX-x;
  if((y+r)>SGUI_SCREEN_MAXY) r=SGUI_SCREEN_MAXY-y;

  if(style==STYLE_FLAT) {
    return; 
  }
  else if(style==STYLE_RAISED) {
    // col1 (oben+links)
    P_SGUI_Screen3DArc(x,y,r,SGUI_3DCOL_1,0);
    // col2 (oben+links)   
    P_SGUI_Screen3DArc(x,y,r-1,SGUI_3DCOL_2,0);
    // col3 (unten+rechts)
    P_SGUI_Screen3DArc(x,y,r-1,SGUI_3DCOL_3,1);
    // col4 (unten+rechts)
    P_SGUI_Screen3DArc(x,y,r,SGUI_3DCOL_4,1);
  }
  else if(style==STYLE_LOWERED) {
    // col4 (oben+links)
    P_SGUI_Screen3DArc(x,y,r,SGUI_3DCOL_4,0);
    // col3 (oben+links)   
    P_SGUI_Screen3DArc(x,y,r-1,SGUI_3DCOL_3,0);
    // col2 (unten+rechts)
    P_SGUI_Screen3DArc(x,y,r-1,SGUI_3DCOL_2,1);
    // col1 (unten+rechts)
    P_SGUI_Screen3DArc(x,y,r,SGUI_3DCOL_1,1);    
  }
}

//--------------------------------------------------------------
// interne Funktion fuer SGUI
// zeichnet einen Image aus dem Flash
// x,y = position
// w,h = breite, hoehe
//--------------------------------------------------------------
void SGUI_ScreenDrawImage(UB_Image *img, uint16_t x, uint16_t y, uint16_t w, uint16_t h)
{
  #if SGUI_SCREEN_ORIENTATION!=0
  DMA2D_Koord koord;
  #else
  uint16_t xp,yp,lcd_x,lcd_y;
  uint32_t  Yaddress = 0;
  uint16_t c;
  #endif

  // check auf limit
  if(x>=SGUI_SCREEN_MAXX) x=SGUI_SCREEN_MAXX-1;
  if(y>=SGUI_SCREEN_MAXY) y=SGUI_SCREEN_MAXY-1;
  if((w==0) || (h==0)) return;
  if((x+w)>SGUI_SCREEN_MAXX) w=SGUI_SCREEN_MAXX-x;
  if((y+h)>SGUI_SCREEN_MAXY) h=SGUI_SCREEN_MAXY-y;

  if(w>img->width) w=img->width;
  if(h>img->height) h=img->height;

  #if SGUI_SCREEN_ORIENTATION!=0
    koord.dest_xp=x;
    koord.dest_yp=y;
    koord.source_xp=0;
    koord.source_yp=0;
    koord.source_w=w;
    koord.source_h=h;

    UB_Graphic2D_CopyImgDMA(img,koord);
  #else
    lcd_x=SGUI_SCREEN_MAXY-y;
    lcd_y=x;
    for(xp=0;xp<h;xp++) {
      UB_LCD_SetCursor2Draw(lcd_x-xp,lcd_y);
      for(yp=0;yp<w;yp++) {
        Yaddress = (xp*img->width)+yp;
        c=img->table[Yaddress];
        UB_LCD_DrawPixel(c);
      }
    }
  #endif
}


//--------------------------------------------------------------
// interne Funktion
// zeichnet einen 180Grad Halbkreis
// m : 0 = links oben , 1=rechts unten
//--------------------------------------------------------------
void P_SGUI_Screen3DArc(uint16_t xp, uint16_t yp, uint16_t r, uint16_t c, uint8_t m)
{
  int16_t f=1-r, ddF_x=0, ddF_y=-2*r, x=0, y=r;
  
  // check auf Limit
  if(xp>=SGUI_SCREEN_MAXX) xp=SGUI_SCREEN_MAXX-1;
  if(yp>=SGUI_SCREEN_MAXY) yp=SGUI_SCREEN_MAXY-1;
  if(r==0) return;
    
  if(m==0) {
    // links oben    
    SGUI_ScreenDrawPixel(xp, yp - r, c);
    SGUI_ScreenDrawPixel(xp - r, yp, c);
    
    while(x < y) {
      if(f >= 0) {
        y--;
        ddF_y += 2;
        f += ddF_y;
      }
      x++;
      ddF_x += 2;
      f += ddF_x + 1;
      
      SGUI_ScreenDrawPixel(xp - y, yp + x, c);
      SGUI_ScreenDrawPixel(xp + x, yp - y, c);
      SGUI_ScreenDrawPixel(xp - x, yp - y, c);
      SGUI_ScreenDrawPixel(xp - y, yp - x, c);
    }   
  }
  else {
    // rechts unten
    SGUI_ScreenDrawPixel(xp, yp + r, c);
    SGUI_ScreenDrawPixel(xp + r, yp, c);
    
    while(x < y) {
      if(f >= 0) {
        y--;
        ddF_y += 2;
        f += ddF_y;
      }
      x++;
      ddF_x += 2;
      f += ddF_x + 1;

      SGUI_ScreenDrawPixel(xp + x, yp + y, c);
      SGUI_ScreenDrawPixel(xp + y, yp + x, c);
      SGUI_ScreenDrawPixel(xp - x, yp + y, c);
      SGUI_ScreenDrawPixel(xp + y, yp - x, c);
    }    
  }
}

