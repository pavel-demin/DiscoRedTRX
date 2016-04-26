//--------------------------------------------------------------
// File     : stm32_ub_tim2.h
//--------------------------------------------------------------

//--------------------------------------------------------------
#ifndef __STM32F7_UB_TIM2_H
#define __STM32F7_UB_TIM2_H

//--------------------------------------------------------------
// Includes
//--------------------------------------------------------------
#include "stm32_ub_system.h"








//--------------------------------------------------------------
// Globale Funktionen
//--------------------------------------------------------------
void UB_TIMER2_Init(uint16_t prescaler, uint16_t periode);
void UB_TIMER2_Init_FRQ(uint32_t frq_hz);
void UB_TIMER2_Start(void);
void UB_TIMER2_Stop(void);
void UB_TIMER2_ISR_CallBack(void);  // Diese Funktion muss extern benutzt werden !!




//--------------------------------------------------------------
#endif // __STM32F7_UB_TIM2_H
