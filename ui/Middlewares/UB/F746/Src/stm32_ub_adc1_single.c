//--------------------------------------------------------------
// File     : stm32_ub_adc1_single.c
// Datum    : 27.02.2016
// Version  : 1.0
// Autor    : UB
// EMail    : mc-4u(@)t-online.de
// Web      : www.mikrocontroller-4u.de
// CPU      : STM32F746
// IDE      : OpenSTM32
// GCC      : 4.9 2015q2
// Module   : CubeHAL
// Funktion : AD-Wandler (ADC1 im Single-Conversion-Mode)
//--------------------------------------------------------------

//--------------------------------------------------------------
// Includes
//--------------------------------------------------------------
#include "stm32_ub_adc1_single.h"

//--------------------------------------------------------------
// interne Funktionen
//--------------------------------------------------------------
void P_ADC1s_InitIO(void);
void P_ADC1s_InitADC(void);
ADC_HandleTypeDef Adc1Handle;

//--------------------------------------------------------------
// Definition der benutzten ADC Pins
// Reihenfolge wie bei ADC1s_NAME_t
//--------------------------------------------------------------
ADC1s_t ADC1s[] = {
    // NAME   ,PORT , PIN      , Kanal       , Mittelwerte
    {ADC_PA01, GPIOB, GPIO_PIN_0, ADC_CHANNEL_18, MW_2561}, // ADC an PB0 = ADC1_IN18 Batteriespannung
    {ADC_PA4, GPIOA, GPIO_PIN_4, ADC_CHANNEL_4, MW_321}, // ADC an PA4 = ADC123_IN4
    {ADC_PA6, GPIOA, GPIO_PIN_6, ADC_CHANNEL_6, MW_321}, // ADC an PA6 = ADC123_IN6
};
static int ADC1s_ANZ = sizeof(ADC1s) / sizeof(ADC1s[0]); // Anzahl der Eintraege

//--------------------------------------------------------------
// init vom ADC im Single-Conversion-Mode
//--------------------------------------------------------------
void UB_ADC1_SINGLE_Init(void) {
  P_ADC1s_InitIO();
  P_ADC1s_InitADC();
}

//--------------------------------------------------------------
// starten einer Messung
// und auslesen des Messwertes
//--------------------------------------------------------------
uint16_t UB_ADC1_SINGLE_Read(ADC1s_NAME_t adc_name) {
  uint8_t retv;
  uint16_t messwert = 0;
  ADC_ChannelConfTypeDef sConfig;

  // Messkanal einrichten
  sConfig.Channel = ADC1s[adc_name].ADC_CH;
  sConfig.Rank = 1;
  sConfig.SamplingTime = ADC_SAMPLETIME_3CYCLES;
  sConfig.Offset = 0;
  // Messung starten
  if (HAL_ADC_ConfigChannel(&Adc1Handle, &sConfig) != HAL_OK)
    return 0;
  if (HAL_ADC_Start(&Adc1Handle) != HAL_OK)
    return 0;
  // warte bis Messung fertig ist
  retv = HAL_ADC_PollForConversion(&Adc1Handle, 10);
  if (retv != 0)
    return retv;
  if (HAL_ADC_GetState(&Adc1Handle) != HAL_ADC_STATE_EOC_REG)
    return 4;
  // Messwert auslesen
  messwert = HAL_ADC_GetValue(&Adc1Handle);

  return (messwert);
}

//--------------------------------------------------------------
// starten von n-Messungen
// und auslesen vom Mittelwert
//--------------------------------------------------------------
uint16_t UB_ADC1_SINGLE_Read_MW(ADC1s_NAME_t adc_name) {
  uint32_t mittelwert = 0;
  uint16_t messwert, n;
  uint16_t anz_mw = 1, anz_bit = 0;

  if (ADC1s[adc_name].ADC_MW == MW_NONE1) {
    anz_mw = 1;
    anz_bit = 0;
  } else if (ADC1s[adc_name].ADC_MW == MW_21) {
    anz_mw = 2;
    anz_bit = 1;
  } else if (ADC1s[adc_name].ADC_MW == MW_41) {
    anz_mw = 4;
    anz_bit = 2;
  } else if (ADC1s[adc_name].ADC_MW == MW_81) {
    anz_mw = 8;
    anz_bit = 3;
  } else if (ADC1s[adc_name].ADC_MW == MW_161) {
    anz_mw = 16;
    anz_bit = 4;
  } else if (ADC1s[adc_name].ADC_MW == MW_321) {
    anz_mw = 32;
    anz_bit = 5;
  } else if (ADC1s[adc_name].ADC_MW == MW_641) {
    anz_mw = 64;
    anz_bit = 6;
  } else if (ADC1s[adc_name].ADC_MW == MW_1281) {
    anz_mw = 128;
    anz_bit = 7;
  } else if (ADC1s[adc_name].ADC_MW == MW_2561) {
    anz_mw = 256;
    anz_bit = 8;
  }

  for (n = 0; n < anz_mw; n++) {
    messwert = UB_ADC1_SINGLE_Read(adc_name);
    mittelwert += messwert;
  }

  messwert = (mittelwert >> anz_bit);

  return (messwert);
}

//--------------------------------------------------------------
// interne Funktion
// Init aller IO-Pins
//--------------------------------------------------------------
void P_ADC1s_InitIO(void) {
  GPIO_InitTypeDef GPIO_InitStruct;
  ADC1s_NAME_t adc_name;

  for (adc_name = 0; adc_name < ADC1s_ANZ; adc_name++) {
    // Clock Enable
    UB_System_ClockEnable(ADC1s[adc_name].ADC_PORT);

    // Config des Pins als Analog-Eingang
    GPIO_InitStruct.Pin = ADC1s[adc_name].ADC_PIN;
    GPIO_InitStruct.Mode = GPIO_MODE_ANALOG;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_LOW;
    HAL_GPIO_Init(ADC1s[adc_name].ADC_PORT, &GPIO_InitStruct);
  }
}

//--------------------------------------------------------------
// interne Funktion
// Init von ADC
//--------------------------------------------------------------
void P_ADC1s_InitADC(void) {
  // Clock Enable
  __HAL_RCC_ADC1_CLK_ENABLE();

  // ADC-Config
  Adc1Handle.Instance = ADC1;
  Adc1Handle.Init.ClockPrescaler = ADC1s_VORTEILER;
  Adc1Handle.Init.Resolution = ADC_RESOLUTION_12B;
  Adc1Handle.Init.ScanConvMode = DISABLE;
  Adc1Handle.Init.ContinuousConvMode = DISABLE;
  Adc1Handle.Init.DiscontinuousConvMode = DISABLE;
  Adc1Handle.Init.NbrOfDiscConversion = 0;
  Adc1Handle.Init.ExternalTrigConvEdge = ADC_EXTERNALTRIGCONVEDGE_NONE;
  Adc1Handle.Init.ExternalTrigConv = ADC_EXTERNALTRIGCONV_T1_CC1;
  Adc1Handle.Init.DataAlign = ADC_DATAALIGN_RIGHT;
  Adc1Handle.Init.NbrOfConversion = 1;
  Adc1Handle.Init.DMAContinuousRequests = DISABLE;
  Adc1Handle.Init.EOCSelection = DISABLE;
  HAL_ADC_Init(&Adc1Handle);
}
