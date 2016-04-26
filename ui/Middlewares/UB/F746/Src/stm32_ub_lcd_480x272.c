//--------------------------------------------------------------
// File     : stm32_ub_lcd_480x272.c
// Datum    : 12.07.2015
// Version  : 1.0
// Autor    : UB
// EMail    : mc-4u(@)t-online.de
// Web      : www.mikrocontroller-4u.de
// CPU      : STM32F746
// IDE      : OpenSTM32
// GCC      : 4.9 2015q2
// Module   : CubeHAL, STM32_UB_SDRAM
// Funktion : Grafik-LCD Funktionen (24bit RGB)
//            Der Zugriff erfolgt ueber den TFT-Controller
//            Auflösung : 480 x 272 Pixel
//            Farbmode = 16bit (5R6G5B = RGB565)
//            Display = RK043FN48H-CT672B
//          
// Hinweis  : Das Display benutzt die CPU-Pins :
//
//             PI15 = R0    PJ7  = G0    PE4  = B0
//             PJ0  = R1    PJ8  = G1    PJ13 = B1
//             PJ1  = R2    PJ9  = G2    PJ14 = B2
//             PJ2  = R3    PJ10 = G3    PJ15 = B3
//             PJ3  = R4    PJ11 = G4    PG12 = B4
//             PJ4  = R5    PK0  = G5    PK4  = B5
//             PJ5  = R6    PK1  = G6    PK5  = B6
//             PJ6  = R7    PK2  = G7    PK6  = B7
//             PI14 = CLK   PK7  = DE
//
//             PI10 = HSYNC       PI9  = VSYNC
//             PK3  = Backlight   PI12 = Display
//
// Das externe SDRAM wird als Grafik-RAM benutzt
// HSYNC+VSYNC im H-File aktivieren falls notwendig
//--------------------------------------------------------------


//--------------------------------------------------------------
// Includes
//--------------------------------------------------------------
#include "stm32_ub_lcd_480x272.h"
#include "stm32_ub_sdram.h"


//--------------------------------------------------------------
// interne Funktionen
//--------------------------------------------------------------
static void P_LCD_480x272_Init(void);
__weak void P_LCD_480x272_ClockConfig(LTDC_HandleTypeDef *hltdc, void *Params);
__weak void P_LCD_480x272_MspInit(LTDC_HandleTypeDef *hltdc, void *Params);



//--------------------------------------------------------------
// Globale Variabeln
//--------------------------------------------------------------
static uint16_t aktCursorX,aktCursorY;
static uint32_t aktCursorPos;
static LTDC_HandleTypeDef  hLtdcHandler;




//--------------------------------------------------------------
// Init vom LCD-Display
// Return_wert :
//  -> ERROR   , wenn Display nicht gefunden wurde
//  -> SUCCESS , wenn Display OK
//--------------------------------------------------------------
ErrorStatus UB_LCD_Init(void)
{ 
  ErrorStatus ret_wert=ERROR;

  // beim init auf Landscape-Mode
  LCD_DISPLAY_MODE=LANDSCAPE;
  // init vom SDRAM
  UB_SDRAM_Init();
  // init dvom LCD
  P_LCD_480x272_Init();

  ret_wert=SUCCESS;
  aktCursorX=0;
  aktCursorY=0;
  aktCursorPos=0;

  LCD_CurrentFrameBuffer=LCD_FRAME_BUFFER;
  LCD_CurrentLayer = 0;
  LCD_CurrentOrientation=0;

  return(ret_wert);
}


//--------------------------------------------------------------
// stellt beide Layer auf "Fullscreen-Mode"
//--------------------------------------------------------------
void UB_LCD_LayerInit_Fullscreen(void)
{
  LTDC_LayerCfgTypeDef  layer_cfg;

  // Layer 0
  layer_cfg.WindowX0 = 0;
  layer_cfg.WindowX1 = LCD_MAXX;
  layer_cfg.WindowY0 = 0;
  layer_cfg.WindowY1 = LCD_MAXY;
  layer_cfg.PixelFormat = LTDC_PIXEL_FORMAT_RGB565;
  layer_cfg.FBStartAdress = LCD_FRAME_BUFFER;
  layer_cfg.Alpha = 255;
  layer_cfg.Alpha0 = 0;
  layer_cfg.Backcolor.Blue = 0;
  layer_cfg.Backcolor.Green = 0;
  layer_cfg.Backcolor.Red = 0;
  layer_cfg.BlendingFactor1 = LTDC_BLENDING_FACTOR1_PAxCA;
  layer_cfg.BlendingFactor2 = LTDC_BLENDING_FACTOR2_PAxCA;
  layer_cfg.ImageWidth = LCD_MAXX;
  layer_cfg.ImageHeight = LCD_MAXY;
  HAL_LTDC_ConfigLayer(&hLtdcHandler, &layer_cfg, 0);

  // Layer 1
  layer_cfg.WindowX0 = 0;
  layer_cfg.WindowX1 = LCD_MAXX;
  layer_cfg.WindowY0 = 0;
  layer_cfg.WindowY1 = LCD_MAXY;
  layer_cfg.PixelFormat = LTDC_PIXEL_FORMAT_RGB565;
  layer_cfg.FBStartAdress = LCD_FRAME_BUFFER+LCD_FRAME_OFFSET;
  layer_cfg.Alpha = 255;
  layer_cfg.Alpha0 = 0;
  layer_cfg.Backcolor.Blue = 0;
  layer_cfg.Backcolor.Green = 0;
  layer_cfg.Backcolor.Red = 0;
  layer_cfg.BlendingFactor1 = LTDC_BLENDING_FACTOR1_PAxCA;
  layer_cfg.BlendingFactor2 = LTDC_BLENDING_FACTOR2_PAxCA;
  layer_cfg.ImageWidth = LCD_MAXX;
  layer_cfg.ImageHeight = LCD_MAXY;
  HAL_LTDC_ConfigLayer(&hLtdcHandler, &layer_cfg, 1);
}


//--------------------------------------------------------------
// Hintergrund-Layer aktivieren
//--------------------------------------------------------------
void UB_LCD_SetLayer_1(void)
{
  LCD_CurrentFrameBuffer = LCD_FRAME_BUFFER;
  LCD_CurrentLayer = 0;
}


//--------------------------------------------------------------
// Vordergrund-Layer aktivieren
//--------------------------------------------------------------
void UB_LCD_SetLayer_2(void)
{
  LCD_CurrentFrameBuffer = LCD_FRAME_BUFFER + LCD_FRAME_OFFSET;
  LCD_CurrentLayer = 1;
}


//--------------------------------------------------------------
// Füllt den aktuellen Layer komplett mit einer Farbe
//--------------------------------------------------------------
void UB_LCD_FillLayer(uint16_t color)
{
  uint32_t index = 0;

  // Bildschirm loeschen
  for (index = 0x00; index < LCD_FRAME_OFFSET; index+=2) {
    *(volatile uint16_t*)(LCD_CurrentFrameBuffer + index) = color;
  }
}


//--------------------------------------------------------------
// setzt Transparenz Wert vom aktuellen Layer
// wert : [0...255] 0=durchsichtig ... 255=solid
//--------------------------------------------------------------
void UB_LCD_SetTransparency(uint8_t wert)
{
  if (LCD_CurrentLayer == 0) {
    HAL_LTDC_SetAlpha(&hLtdcHandler, wert, 0);
  }
  else {
    HAL_LTDC_SetAlpha(&hLtdcHandler, wert, 1);
  }
}


//--------------------------------------------------------------
// setzt den Cursor auf x,y Position
//--------------------------------------------------------------
void UB_LCD_SetCursor2Draw(uint16_t xpos, uint16_t ypos)
{
  aktCursorX=xpos;
  aktCursorY=ypos;

  aktCursorPos=LCD_CurrentFrameBuffer+(2*((aktCursorY*LCD_MAXX)+aktCursorX));
}


//--------------------------------------------------------------
// zeichnet ein Pixel an aktueller XY-Position
// und incrementiert Cursor
//--------------------------------------------------------------
void UB_LCD_DrawPixel(uint16_t color)
{
  *(volatile uint16_t*)(aktCursorPos)=color;
  if(LCD_DISPLAY_MODE==LANDSCAPE) {
    aktCursorX++;
    if(aktCursorX>=LCD_MAXX) {
      aktCursorX=0;
      aktCursorY++;
      if(aktCursorY>=LCD_MAXY) aktCursorY=0;
    }
  }
  else {
    aktCursorY++;
    if(aktCursorY>=LCD_MAXY) {
      aktCursorY=0;
      aktCursorX++;
      if(aktCursorX>=LCD_MAXX) aktCursorX=0;
    }
  }
  aktCursorPos=LCD_CurrentFrameBuffer+(2*((aktCursorY*LCD_MAXX)+aktCursorX));
}


//--------------------------------------------------------------
// Screen-Mode einstellen
// muss direkt nach dem Init gemacht werden
//
// Mode : [PORTRAIT=default, LANDSCAPE]
//--------------------------------------------------------------
void UB_LCD_SetMode(LCD_MODE_t mode)
{
  if(mode==PORTRAIT) {
    LCD_DISPLAY_MODE=PORTRAIT;
  }
  else {
    LCD_DISPLAY_MODE=LANDSCAPE;
  }
}


//--------------------------------------------------------------
// Screen-Rotation einstellen auf 0 Grad
//--------------------------------------------------------------
void UB_LCD_Rotate_0(void)
{
  LCD_CurrentOrientation=0;
}


//--------------------------------------------------------------
// Screen-Rotation einstellen auf 180 Grad
//--------------------------------------------------------------
void UB_LCD_Rotate_180(void)
{
  LCD_CurrentOrientation=1;
}


//--------------------------------------------------------------
// kopiert kompletten Inhalt von Layer1 in Layer2
// (Hintergrund --> Vordergrund)
//--------------------------------------------------------------
void UB_LCD_Copy_Layer1_to_Layer2(void)
{
  uint32_t index;
  uint32_t quelle = LCD_FRAME_BUFFER;
  uint32_t ziel = LCD_FRAME_BUFFER + LCD_FRAME_OFFSET;

  for (index = 0 ; index < LCD_FRAME_OFFSET;index+=2) {
    *(volatile uint16_t*)(ziel + index) = *(volatile uint16_t*)(quelle + index);
  }
}


//--------------------------------------------------------------
// kopiert kompletten Inhalt von Layer2 in Layer1
// (Vordergrund --> Hintergrund)
//--------------------------------------------------------------
void UB_LCD_Copy_Layer2_to_Layer1(void)
{
  uint32_t index;
  uint32_t quelle = LCD_FRAME_BUFFER + LCD_FRAME_OFFSET;
  uint32_t ziel = LCD_FRAME_BUFFER;

  for (index = 0 ; index < LCD_FRAME_OFFSET;index+=2) {
    *(volatile uint16_t*)(ziel + index) = *(volatile uint16_t*)(quelle + index);
  }
}


//--------------------------------------------------------------
// wechselt den aktiven Layer zum zeichnen
// und zeigt den jeweils anderen Layer an
//--------------------------------------------------------------
void UB_LCD_Refresh(void)
{
  if(LCD_CurrentLayer==0) {
    UB_LCD_SetLayer_2();
    UB_LCD_SetTransparency(0);
  }
  else {
    UB_LCD_SetTransparency(255);
    UB_LCD_SetLayer_1();
  }
}


//--------------------------------------------------------------
// interne Funktion
//--------------------------------------------------------------
static void P_LCD_480x272_Init(void)
{
  /* Select the used LCD */

  /* The RK043FN48H LCD 480x272 is selected */
  /* Timing Configuration */
  hLtdcHandler.Init.HorizontalSync = (RK043FN48H_HSYNC - 1);
  hLtdcHandler.Init.VerticalSync = (RK043FN48H_VSYNC - 1);
  hLtdcHandler.Init.AccumulatedHBP = (RK043FN48H_HSYNC + RK043FN48H_HBP - 1);
  hLtdcHandler.Init.AccumulatedVBP = (RK043FN48H_VSYNC + RK043FN48H_VBP - 1);
  hLtdcHandler.Init.AccumulatedActiveH = (RK043FN48H_HEIGHT + RK043FN48H_VSYNC + RK043FN48H_VBP - 1);
  hLtdcHandler.Init.AccumulatedActiveW = (RK043FN48H_WIDTH + RK043FN48H_HSYNC + RK043FN48H_HBP - 1);
  hLtdcHandler.Init.TotalHeigh = (RK043FN48H_HEIGHT + RK043FN48H_VSYNC + RK043FN48H_VBP + RK043FN48H_VFP - 1);
  hLtdcHandler.Init.TotalWidth = (RK043FN48H_WIDTH + RK043FN48H_HSYNC + RK043FN48H_HBP + RK043FN48H_HFP - 1);

  /* LCD clock configuration */
  P_LCD_480x272_ClockConfig(&hLtdcHandler, NULL);

  /* Initialize the LCD pixel width and pixel height */
  hLtdcHandler.LayerCfg->ImageWidth  = RK043FN48H_WIDTH;
  hLtdcHandler.LayerCfg->ImageHeight = RK043FN48H_HEIGHT;

  /* Background value */
  hLtdcHandler.Init.Backcolor.Blue = 0;
  hLtdcHandler.Init.Backcolor.Green = 0;
  hLtdcHandler.Init.Backcolor.Red = 0;

  /* Polarity */
  hLtdcHandler.Init.HSPolarity = LTDC_HSPOLARITY_AL;
  hLtdcHandler.Init.VSPolarity = LTDC_VSPOLARITY_AL;
  hLtdcHandler.Init.DEPolarity = LTDC_DEPOLARITY_AL;
  hLtdcHandler.Init.PCPolarity = LTDC_PCPOLARITY_IPC;
  hLtdcHandler.Instance = LTDC;

  if(HAL_LTDC_GetState(&hLtdcHandler) == HAL_LTDC_STATE_RESET)
  {
    /* Initialize the LCD Msp: this __weak function can be rewritten by the application */
    P_LCD_480x272_MspInit(&hLtdcHandler, NULL);
  }
  HAL_LTDC_Init(&hLtdcHandler);

  /* Assert display enable LCD_DISP pin */
  HAL_GPIO_WritePin(LCD_DISP_GPIO_PORT, LCD_DISP_PIN, GPIO_PIN_SET);

  /* Assert backlight LCD_BL_CTRL pin */
  HAL_GPIO_WritePin(LCD_BL_CTRL_GPIO_PORT, LCD_BL_CTRL_PIN, GPIO_PIN_SET);
}

//--------------------------------------------------------------
// interne Funktion
//--------------------------------------------------------------
__weak void P_LCD_480x272_ClockConfig(LTDC_HandleTypeDef *hltdc, void *Params)
{
  static RCC_PeriphCLKInitTypeDef  periph_clk_init_struct;

  /* RK043FN48H LCD clock configuration */
  /* PLLSAI_VCO Input = HSE_VALUE/PLL_M = 1 Mhz */
  /* PLLSAI_VCO Output = PLLSAI_VCO Input * PLLSAIN = 192 Mhz */
  /* PLLLCDCLK = PLLSAI_VCO Output/PLLSAIR = 192/5 = 38.4 Mhz */
  /* LTDC clock frequency = PLLLCDCLK / LTDC_PLLSAI_DIVR_4 = 38.4/4 = 9.6Mhz */
  periph_clk_init_struct.PeriphClockSelection = RCC_PERIPHCLK_LTDC;
  periph_clk_init_struct.PLLSAI.PLLSAIN = 192;
  periph_clk_init_struct.PLLSAI.PLLSAIR = RK043FN48H_FREQUENCY_DIVIDER;
  periph_clk_init_struct.PLLSAIDivR = RCC_PLLSAIDIVR_4;
  HAL_RCCEx_PeriphCLKConfig(&periph_clk_init_struct);
}

//--------------------------------------------------------------
// interne Funktion
//--------------------------------------------------------------
__weak void P_LCD_480x272_MspInit(LTDC_HandleTypeDef *hltdc, void *Params)
{
  GPIO_InitTypeDef gpio_init_structure;

  /* Enable the LTDC and DMA2D clocks */
  __HAL_RCC_LTDC_CLK_ENABLE();
  __HAL_RCC_DMA2D_CLK_ENABLE();

  /* Enable GPIOs clock */
  __HAL_RCC_GPIOE_CLK_ENABLE();
  __HAL_RCC_GPIOG_CLK_ENABLE();
  __HAL_RCC_GPIOI_CLK_ENABLE();
  __HAL_RCC_GPIOJ_CLK_ENABLE();
  __HAL_RCC_GPIOK_CLK_ENABLE();
  LCD_DISP_GPIO_CLK_ENABLE();
  LCD_BL_CTRL_GPIO_CLK_ENABLE();

  /*** LTDC Pins configuration ***/
  /* GPIOE configuration */
  gpio_init_structure.Pin       = GPIO_PIN_4;
  gpio_init_structure.Mode      = GPIO_MODE_AF_PP;
  gpio_init_structure.Pull      = GPIO_NOPULL;
  gpio_init_structure.Speed     = GPIO_SPEED_FAST;
  gpio_init_structure.Alternate = GPIO_AF14_LTDC;
  HAL_GPIO_Init(GPIOE, &gpio_init_structure);

  /* GPIOG configuration */
  gpio_init_structure.Pin       = GPIO_PIN_12;
  gpio_init_structure.Mode      = GPIO_MODE_AF_PP;
  gpio_init_structure.Alternate = GPIO_AF9_LTDC;
  HAL_GPIO_Init(GPIOG, &gpio_init_structure);

  /* GPIOI LTDC alternate configuration */
#if USE_SYNC_GPIO==1
  gpio_init_structure.Pin       = GPIO_PIN_8 | GPIO_PIN_9 | GPIO_PIN_10 | \
                                  GPIO_PIN_13 | GPIO_PIN_14 | GPIO_PIN_15;
  gpio_init_structure.Mode      = GPIO_MODE_AF_PP;
  gpio_init_structure.Alternate = GPIO_AF14_LTDC;
  HAL_GPIO_Init(GPIOI, &gpio_init_structure);
#else
  gpio_init_structure.Pin       = GPIO_PIN_8 | \
                                  GPIO_PIN_13 | GPIO_PIN_14 | GPIO_PIN_15;
  gpio_init_structure.Mode      = GPIO_MODE_AF_PP;
  gpio_init_structure.Alternate = GPIO_AF14_LTDC;
  HAL_GPIO_Init(GPIOI, &gpio_init_structure);
#endif

  /* GPIOJ configuration */
  gpio_init_structure.Pin       = GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3 | \
                                  GPIO_PIN_4 | GPIO_PIN_5 | GPIO_PIN_6 | GPIO_PIN_7 | \
                                  GPIO_PIN_8 | GPIO_PIN_9 | GPIO_PIN_10 | GPIO_PIN_11 | \
                                  GPIO_PIN_13 | GPIO_PIN_14 | GPIO_PIN_15;
  gpio_init_structure.Mode      = GPIO_MODE_AF_PP;
  gpio_init_structure.Alternate = GPIO_AF14_LTDC;
  HAL_GPIO_Init(GPIOJ, &gpio_init_structure);

  /* GPIOK configuration */
  gpio_init_structure.Pin       = GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_4 | \
                                  GPIO_PIN_5 | GPIO_PIN_6 | GPIO_PIN_7;
  gpio_init_structure.Mode      = GPIO_MODE_AF_PP;
  gpio_init_structure.Alternate = GPIO_AF14_LTDC;
  HAL_GPIO_Init(GPIOK, &gpio_init_structure);

  /* LCD_DISP GPIO configuration */
  gpio_init_structure.Pin       = LCD_DISP_PIN;     /* LCD_DISP pin has to be manually controlled */
  gpio_init_structure.Mode      = GPIO_MODE_OUTPUT_PP;
  HAL_GPIO_Init(LCD_DISP_GPIO_PORT, &gpio_init_structure);

  /* LCD_BL_CTRL GPIO configuration */
  gpio_init_structure.Pin       = LCD_BL_CTRL_PIN;  /* LCD_BL_CTRL pin has to be manually controlled */
  gpio_init_structure.Mode      = GPIO_MODE_OUTPUT_PP;
  HAL_GPIO_Init(LCD_BL_CTRL_GPIO_PORT, &gpio_init_structure);
}

