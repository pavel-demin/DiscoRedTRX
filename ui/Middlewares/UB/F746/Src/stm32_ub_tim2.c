//--------------------------------------------------------------
// File     : stm32_ub_tim2.c
// Datum    : 17.01.2016
// Version  : 1.0
// Autor    : UB
// EMail    : mc-4u(@)t-online.de
// Web      : www.mikrocontroller-4u.de
// CPU      : STM32F746
// IDE      : OpenSTM32
// GCC      : 4.9 2015q2
// Module   : CubeHAL
// Funktion : Timer-Funktionen per Timer2
//            (mit Callback-Funktion für externe ISR)
//
// Hinweis  : beim Timerevent wird die Funktion :
//            "UB_TIMER2_ISR_CallBack()" aufgerufen
//            diese Funktion muss vom User programmiert werden
//--------------------------------------------------------------


//--------------------------------------------------------------
// Includes
//--------------------------------------------------------------
#include "stm32_ub_tim2.h"



//--------------------------------------------------------------
// interne Funktionen
//--------------------------------------------------------------
void P_TIM2_TIMER(uint16_t prescaler, uint16_t periode);
void P_TIM2_NVIC(void);
//--------------------------------------------------------------
TIM_HandleTypeDef TIM2_Handle;
uint32_t tim2_enable_flag=0;



//--------------------------------------------------------------
// Init und Stop vom Timer mit Vorteiler und Counterwert
// prescaler : [0...65535]
// periode   : [0...65535]
//
// F_TIM = 100 MHz / (prescaler+1) / (periode+1)
//--------------------------------------------------------------
void UB_TIMER2_Init(uint16_t prescaler, uint16_t periode)
{
  // Timer flag löschen
  tim2_enable_flag=0;
  // Timer einstellen
  P_TIM2_TIMER(prescaler, periode);
  P_TIM2_NVIC();
}

//--------------------------------------------------------------
// Init und Stop vom Timer mit einem FRQ-Wert (in Hz)
// frq_hz : [1...50000000]
//
// Hinweis : die tatsächliche Frq weicht wegen Rundungsfehlern
//           etwas vom Sollwert ab (Bitte nachrechnen falls Wichtig)
//--------------------------------------------------------------
void UB_TIMER2_Init_FRQ(uint32_t frq_hz)
{
  uint32_t clk_frq;
  uint16_t prescaler, periode;
  uint32_t u_temp;
  float teiler,f_temp;

  // Clock-Frequenzen (PCLK1) auslesen
  clk_frq = HAL_RCC_GetPCLK1Freq();

  // check der werte
  if(frq_hz==0) frq_hz=1;
  if(frq_hz>clk_frq) frq_hz=clk_frq;

  // berechne teiler
  teiler=(float)(clk_frq<<1)/(float)(frq_hz);

  // berechne prescaler
  u_temp=(uint32_t)(teiler);
  prescaler=(u_temp>>16);

  // berechne periode
  f_temp=(float)(teiler)/(float)(prescaler+1);
  periode=(uint16_t)(f_temp-1);

  // werte einstellen
  UB_TIMER2_Init(prescaler, periode);
}

//--------------------------------------------------------------
// Timer starten
//--------------------------------------------------------------
void UB_TIMER2_Start(void)
{
  // Timer enable
  HAL_TIM_Base_Start_IT(&TIM2_Handle);
  // Timer flag setzen
  tim2_enable_flag=1;
}

//--------------------------------------------------------------
// Timer anhalten
//--------------------------------------------------------------
void UB_TIMER2_Stop(void)
{
  // Timer flag löschen
  tim2_enable_flag=0;
  // Timer disable
  HAL_TIM_Base_Stop_IT(&TIM2_Handle);
}

//--------------------------------------------------------------
// interne Funktion
// init vom Timer
//--------------------------------------------------------------
void P_TIM2_TIMER(uint16_t prescaler, uint16_t periode)
{
  // Clock enable
  __HAL_RCC_TIM2_CLK_ENABLE();

  TIM2_Handle.Instance = TIM2;
  TIM2_Handle.Init.Period            = periode;
  TIM2_Handle.Init.Prescaler         = prescaler;
  TIM2_Handle.Init.ClockDivision     = TIM_CLOCKDIVISION_DIV1;
  TIM2_Handle.Init.CounterMode       = TIM_COUNTERMODE_UP;
  TIM2_Handle.Init.RepetitionCounter = 0;
  HAL_TIM_Base_Init(&TIM2_Handle);

}

//--------------------------------------------------------------
// interne Funktion
// init vom Interrupt
//--------------------------------------------------------------
void P_TIM2_NVIC(void)
{
  // NVIC konfig
  HAL_NVIC_SetPriority(TIM2_IRQn, 3, 0);
  HAL_NVIC_EnableIRQ(TIM2_IRQn);
}

//--------------------------------------------------------------
// Timer-Interrupt
// wird beim erreichen vom Counterwert aufgerufen
// die Callback funktion muss extern benutzt werden
//--------------------------------------------------------------
void TIM2_IRQHandler(void)
{
  HAL_TIM_IRQHandler(&TIM2_Handle);

  // Callback Funktion aufrufen
  // Diese Funktion muss extern vom User benutzt werden !!
  if(tim2_enable_flag!=0) {
    // nur wenn Timer aktiviert ist
    UB_TIMER2_ISR_CallBack();
  }
}
