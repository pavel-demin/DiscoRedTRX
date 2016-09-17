//--------------------------------------------------------------
// File     : stm32_ub_adc3_single.c
// Datum    : 27.02.2016
// Version  : 1.0
// Autor    : UB
// EMail    : mc-4u(@)t-online.de
// Web      : www.mikrocontroller-4u.de
// CPU      : STM32F746
// IDE      : OpenSTM32
// GCC      : 4.9 2015q2
// Module   : CubeHAL
// Funktion : AD-Wandler (ADC3 im Single-Conversion-Mode)
//--------------------------------------------------------------

//--------------------------------------------------------------
// Includes
//--------------------------------------------------------------
#include "stm32_ub_adc3_single.h"


//--------------------------------------------------------------
// interne Funktionen
//--------------------------------------------------------------
void P_ADC3s_InitIO(void);
void P_ADC3s_InitADC(void);
ADC_HandleTypeDef    Adc3Handle;


//--------------------------------------------------------------
// Definition der benutzten ADC Pins
// Reihenfolge wie bei ADC3s_NAME_t
//--------------------------------------------------------------
ADC3s_t ADC3s[] = {
  //NAME   ,PORT , PIN      , Kanal       , Mittelwerte
  {ADC_PA0 ,GPIOA,GPIO_PIN_0,ADC_CHANNEL_0,MW_32},   // ADC an PA0 = ADC123_IN0
  {ADC_PF6 ,GPIOF,GPIO_PIN_6,ADC_CHANNEL_4,MW_32},   // ADC an PF6 = ADC3_IN4
  {ADC_PF7 ,GPIOF,GPIO_PIN_7,ADC_CHANNEL_5,MW_32},   // ADC an PF7 = ADC3_IN5
};
static int ADC3s_ANZ = sizeof(ADC3s)/sizeof(ADC3s[0]);  // Anzahl der Eintraege



//--------------------------------------------------------------
// init vom ADC im Single-Conversion-Mode
//--------------------------------------------------------------
void UB_ADC3_SINGLE_Init(void)
{
  P_ADC3s_InitIO();
  P_ADC3s_InitADC();
}

//--------------------------------------------------------------
// starten einer Messung
// und auslesen des Messwertes
//--------------------------------------------------------------
uint16_t UB_ADC3_SINGLE_Read(ADC3s_NAME_t adc_name)
{
  uint16_t messwert=0;
  ADC_ChannelConfTypeDef sConfig;

  // Messkanal einrichten
  sConfig.Channel      = ADC3s[adc_name].ADC_CH;
  sConfig.Rank         = 1;
  sConfig.SamplingTime = ADC_SAMPLETIME_3CYCLES;
  sConfig.Offset       = 0;
  // Messung starten
  if (HAL_ADC_ConfigChannel(&Adc3Handle, &sConfig) != HAL_OK) return 0;
  if (HAL_ADC_Start(&Adc3Handle) != HAL_OK) return 0;
  // warte bis Messung fertig ist
  HAL_ADC_PollForConversion(&Adc3Handle, 10);
  if(HAL_IS_BIT_CLR(HAL_ADC_GetState(&Adc3Handle), HAL_ADC_STATE_REG_EOC)) return 0;
  // Messwert auslesen
  messwert=HAL_ADC_GetValue(&Adc3Handle);

  return(messwert);
}

//--------------------------------------------------------------
// starten von n-Messungen
// und auslesen vom Mittelwert
//--------------------------------------------------------------
uint16_t UB_ADC3_SINGLE_Read_MW(ADC3s_NAME_t adc_name)
{
  uint32_t mittelwert=0;
  uint16_t messwert,n;
  uint16_t anz_mw=1,anz_bit=0;

  if(ADC3s[adc_name].ADC_MW==MW_NONE) {
    anz_mw=1;anz_bit=0;
  }
  else if(ADC3s[adc_name].ADC_MW==MW_2) {
    anz_mw=2;anz_bit=1;
  }
  else if(ADC3s[adc_name].ADC_MW==MW_4) {
    anz_mw=4;anz_bit=2;
  }
  else if(ADC3s[adc_name].ADC_MW==MW_8) {
    anz_mw=8;anz_bit=3;
  }
  else if(ADC3s[adc_name].ADC_MW==MW_16) {
    anz_mw=16;anz_bit=4;
  }
  else if(ADC3s[adc_name].ADC_MW==MW_32) {
    anz_mw=32;anz_bit=5;
  }
  else if(ADC3s[adc_name].ADC_MW==MW_64) {
    anz_mw=64;anz_bit=6;
  }
  else if(ADC3s[adc_name].ADC_MW==MW_128) {
    anz_mw=128;anz_bit=7;
  }

  for(n=0;n<anz_mw;n++) {
    messwert=UB_ADC3_SINGLE_Read(adc_name);
    mittelwert+=messwert;
  }

  messwert=(mittelwert >> anz_bit);

  return(messwert);
}


//--------------------------------------------------------------
// interne Funktion
// Init aller IO-Pins
//--------------------------------------------------------------
void P_ADC3s_InitIO(void) {
  GPIO_InitTypeDef GPIO_InitStruct;
  ADC3s_NAME_t adc_name;

  for(adc_name=0;adc_name<ADC3s_ANZ;adc_name++) {
    // Clock Enable
    UB_System_ClockEnable(ADC3s[adc_name].ADC_PORT);

    // Config des Pins als Analog-Eingang
    GPIO_InitStruct.Pin = ADC3s[adc_name].ADC_PIN;
    GPIO_InitStruct.Mode = GPIO_MODE_ANALOG;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_LOW;
    HAL_GPIO_Init(ADC3s[adc_name].ADC_PORT, &GPIO_InitStruct);

  }
}

//--------------------------------------------------------------
// interne Funktion
// Init von ADC
//--------------------------------------------------------------
void P_ADC3s_InitADC(void)
{
  // Clock Enable
  __HAL_RCC_ADC3_CLK_ENABLE();

  // ADC-Config
  Adc3Handle.Instance          = ADC3;
  Adc3Handle.Init.ClockPrescaler        = ADC3s_VORTEILER;
  Adc3Handle.Init.Resolution            = ADC_RESOLUTION_12B;
  Adc3Handle.Init.ScanConvMode          = DISABLE;
  Adc3Handle.Init.ContinuousConvMode    = DISABLE;
  Adc3Handle.Init.DiscontinuousConvMode = DISABLE;
  Adc3Handle.Init.NbrOfDiscConversion   = 0;
  Adc3Handle.Init.ExternalTrigConvEdge  = ADC_EXTERNALTRIGCONVEDGE_NONE;
  Adc3Handle.Init.ExternalTrigConv      = ADC_EXTERNALTRIGCONV_T1_CC1;
  Adc3Handle.Init.DataAlign             = ADC_DATAALIGN_RIGHT;
  Adc3Handle.Init.NbrOfConversion       = 1;
  Adc3Handle.Init.DMAContinuousRequests = DISABLE;
  Adc3Handle.Init.EOCSelection          = DISABLE;
  HAL_ADC_Init(&Adc3Handle);
}

