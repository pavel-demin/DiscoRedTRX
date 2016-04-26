//--------------------------------------------------------------
// File     : stm32_ub_i2c3.h
//--------------------------------------------------------------

//--------------------------------------------------------------
#ifndef __STM32F7_UB_I2C3_H
#define __STM32F7_UB_I2C3_H

//--------------------------------------------------------------
// Includes
//--------------------------------------------------------------
#include "stm32_ub_system.h"



//--------------------------------------------------------------
// MultiByte defines
//--------------------------------------------------------------
#define    I2C3_MULTIBYTE_ANZ   10         // anzahl der Bytes
uint8_t    I2C3_DATA[I2C3_MULTIBYTE_ANZ];  // Array



//--------------------------------------------------------------
// APB1-Clock = 50MHz
//--------------------------------------------------------------
#define I2C3_TIMING      0x40912732   // 100kHz



//--------------------------------------------------------------
// Defines
//-------------------------------------------------------------- 
#define   I2C3_TIMEOUT     1000       // Timeout Zeit in ms



//--------------------------------------------------------------
// I2C-GPIO-Pins
// moegliche Pinbelegung :
//   SCL : [PA8, PH7]
//   SDA : [PC9, PH8]
//--------------------------------------------------------------
#define  I2C3_SCL_PORT    GPIOH
#define  I2C3_SCL_PIN     GPIO_PIN_7

#define  I2C3_SDA_PORT    GPIOH
#define  I2C3_SDA_PIN     GPIO_PIN_8


//--------------------------------------------------------------
// Globale Funktionen
//--------------------------------------------------------------
void UB_I2C3_Init(void);
int16_t UB_I2C3_ReadByte(uint8_t slave_adr, uint8_t adr);
int16_t UB_I2C3_WriteByte(uint8_t slave_adr, uint8_t adr, uint8_t wert);
int16_t UB_I2C3_ReadMultiByte(uint8_t slave_adr, uint8_t adr, uint8_t cnt);
int16_t UB_I2C3_WriteMultiByte(uint8_t slave_adr, uint8_t adr, uint8_t cnt);
int16_t UB_I2C3_WriteCMD(uint8_t slave_adr, uint8_t cmd);
int16_t UB_I2C3_ReadByte16(uint8_t slave_adr, uint16_t adr);
int16_t UB_I2C3_WriteByte16(uint8_t slave_adr, uint16_t adr, uint8_t wert);
void UB_I2C3_Delay(volatile uint32_t nCount);




//--------------------------------------------------------------
#endif // __STM32F7_UB_I2C3_H
