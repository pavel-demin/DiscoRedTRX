//--------------------------------------------------------------
// File     : stm32_ub_lcd_480x272.h
//--------------------------------------------------------------

//--------------------------------------------------------------
#ifndef __STM32F7_UB_LCD_480x272_H
#define __STM32F7_UB_LCD_480x272_H


//--------------------------------------------------------------
// Includes
//--------------------------------------------------------------
#include "stm32_ub_system.h"



//--------------------------------------------------------------
// aktivieren von HSync/VSync Impulsen (an PI9, PI10)
// 0 = HSYNC, VSYNC wird nicht benutzt
// 1 = HSYNC, VSYNC wird benutzt
//--------------------------------------------------------------
#define   USE_SYNC_GPIO    1  // HSYNC, VSYNC GPIOs werden benutzt
//#define   USE_SYNC_GPIO    0  // HSYNC, VSYNC GPIOs werden nicht benutzt



//--------------------------------------------------------------
// Display Timing
//--------------------------------------------------------------
#define  RK043FN48H_HSYNC            ((uint16_t)41)   /* Horizontal synchronization */
#define  RK043FN48H_HBP              ((uint16_t)13)   /* Horizontal back porch      */
#define  RK043FN48H_HFP              ((uint16_t)32)   /* Horizontal front porch     */
#define  RK043FN48H_VSYNC            ((uint16_t)10)   /* Vertical synchronization   */
#define  RK043FN48H_VBP              ((uint16_t)2)    /* Vertical back porch        */
#define  RK043FN48H_VFP              ((uint16_t)2)    /* Vertical front porch       */
#define  RK043FN48H_FREQUENCY_DIVIDER    5            /* LCD Frequency divider      */


//--------------------------------------------------------------
// Display-Pin
//--------------------------------------------------------------
#define LCD_DISP_PIN                    GPIO_PIN_12
#define LCD_DISP_GPIO_PORT              GPIOI
#define LCD_DISP_GPIO_CLK_ENABLE()      __HAL_RCC_GPIOI_CLK_ENABLE()
#define LCD_DISP_GPIO_CLK_DISABLE()     __HAL_RCC_GPIOI_CLK_DISABLE()

//--------------------------------------------------------------
// Backlight-Pin
//--------------------------------------------------------------
#define LCD_BL_CTRL_PIN                  GPIO_PIN_3
#define LCD_BL_CTRL_GPIO_PORT            GPIOK
#define LCD_BL_CTRL_GPIO_CLK_ENABLE()    __HAL_RCC_GPIOK_CLK_ENABLE()
#define LCD_BL_CTRL_GPIO_CLK_DISABLE()   __HAL_RCC_GPIOK_CLK_DISABLE()




//--------------------------------------------------------------
// Standard Farbwerte
// 16bit Farbwert (R5G6B5)
// Red   (5bit) -> Bit15-Bit11
// Green (6bit) -> Bit10-Bit5
// Blue  (5bit) -> Bit4-Bit0
//--------------------------------------------------------------
#define  RGB_COL_BLACK          0x0000
#define  RGB_COL_BLUE           0x001F
#define  RGB_COL_GREEN          0x07E0
#define  RGB_COL_RED            0xF800
#define  RGB_COL_WHITE          0xFFFF

#define  RGB_COL_CYAN           0x07FF
#define  RGB_COL_MAGENTA        0xF81F
#define  RGB_COL_YELLOW         0xFFE0

#define  RGB_COL_GREY           0xF7DE


//--------------------------------------------------------------
// Display Mode
//--------------------------------------------------------------
typedef enum {
  PORTRAIT =0,
  LANDSCAPE
}LCD_MODE_t;
LCD_MODE_t  LCD_DISPLAY_MODE;


//--------------------------------------------------------------
// Globale Variabeln
//--------------------------------------------------------------
uint32_t LCD_CurrentFrameBuffer;   // aktuelle Adresse zum zeichnen
uint32_t LCD_CurrentLayer;         // 0=Hintergrund, 1=Vodergrund
uint32_t LCD_CurrentOrientation;   // 0=0Grad, 1=180Grad



//--------------------------------------------------------------
// Defines vom Display (im Landscape-Mode)
//--------------------------------------------------------------
#define  LCD_MAXX           ((uint16_t)480)      // Pixel in X-Richtung
#define  LCD_MAXY           ((uint16_t)272)      // Pixel in Y-Richtung
#define  LCD_PIXEL  LCD_MAXX*LCD_MAXY

#define  LCD_INIT_PAUSE   16600 // ca 1ms




//--------------------------------------------------------------
// Defines für das Display-RAM
//--------------------------------------------------------------
#define  LCD_FRAME_BUFFER     SDRAM_START_ADR         // Start vom SDRAM
#define  LCD_FRAME_OFFSET   ((uint32_t)(LCD_PIXEL*2)) // größe von einem Puffer (in Byte)

//--------------------------------------------------------------
// sonst
//--------------------------------------------------------------
#define  RK043FN48H_WIDTH    LCD_MAXX
#define  RK043FN48H_HEIGHT   LCD_MAXY


//--------------------------------------------------------------
// Globale Funktionen
//--------------------------------------------------------------
ErrorStatus UB_LCD_Init(void);
void UB_LCD_LayerInit_Fullscreen(void);
void UB_LCD_SetLayer_1(void);
void UB_LCD_SetLayer_2(void);
void UB_LCD_FillLayer(uint16_t color);
void UB_LCD_SetTransparency(uint8_t wert);
void UB_LCD_SetCursor2Draw(uint16_t xpos, uint16_t ypos);
void UB_LCD_DrawPixel(uint16_t color);
void UB_LCD_SetMode(LCD_MODE_t mode);
void UB_LCD_Rotate_0(void);
void UB_LCD_Rotate_180(void);
void UB_LCD_Copy_Layer1_to_Layer2(void);
void UB_LCD_Copy_Layer2_to_Layer1(void);
void UB_LCD_Refresh(void);



//--------------------------------------------------------------
#endif // __STM32F7_UB_LCD_480x272_H
