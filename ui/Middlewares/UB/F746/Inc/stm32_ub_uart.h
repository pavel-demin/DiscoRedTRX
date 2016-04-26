//--------------------------------------------------------------
// File     : stm32_ub_uart.h
//--------------------------------------------------------------

//--------------------------------------------------------------
#ifndef __STM32F7_UB_UART_H
#define __STM32F7_UB_UART_H


//--------------------------------------------------------------
// Includes
//--------------------------------------------------------------
#include "stm32_ub_system.h"



//--------------------------------------------------------------
// hier die benutzten UARTS freigeben
//   1 = enable
//   0 = disable
//--------------------------------------------------------------
#define   USE_UART1       1   // COM1 (VCP bei STM32F746-Disco)
#define   USE_UART6       0   // COM6
#define   USE_UART7       0   // COM7



//--------------------------------------------------------------
// Liste aller UARTs
// (keine Nummer doppelt und von 0 beginnend)
//--------------------------------------------------------------
typedef enum
{
  COM1 = 0,   // COM1 (TX=PA9, RX=PB7)
  COM6 = 1,   // COM6 (TX=PC6, RX=PC7)
  COM7 = 2    // COM7 (TX=PF7, RX=PF6)
}UART_NAME_t;


//--------------------------------------------------------------
// UART Settings (Com-1)
//--------------------------------------------------------------
#define    COM1_BAUD           115200
#define    COM1_TX_PORT         GPIOA
#define    COM1_TX_PIN          GPIO_PIN_9 
#define    COM1_RX_PORT         GPIOB
#define    COM1_RX_PIN          GPIO_PIN_7 
 

//--------------------------------------------------------------
// UART Settings (Com-6)
//--------------------------------------------------------------
#define    COM6_BAUD           115200
#define    COM6_TX_PORT         GPIOC
#define    COM6_TX_PIN          GPIO_PIN_6 
#define    COM6_RX_PORT         GPIOC
#define    COM6_RX_PIN          GPIO_PIN_7 
 

//--------------------------------------------------------------
// UART Settings (Com-7)
//--------------------------------------------------------------
#define    COM7_BAUD           115200
#define    COM7_TX_PORT         GPIOF
#define    COM7_TX_PIN          GPIO_PIN_7 
#define    COM7_RX_PORT         GPIOF
#define    COM7_RX_PIN          GPIO_PIN_6 
 



//--------------------------------------------------------------
#define    UART_TX_TIMEOUT         100




//--------------------------------------------------------------
// Endekennung beim Senden
//--------------------------------------------------------------
typedef enum {
  NONE = 0,  // keine Endekennung
  LFCR,      // LineFeed + CarriageReturn (0x0A,0x0D)
  CRLF,      // CarriageReturn + LineFeed (0x0D,0x0A)
  LF,        // nur LineFeed (0x0A)
  CR         // nur CarriageReturn (0x0D)
}UART_LASTBYTE_t;


//--------------------------------------------------------------
// Status beim Empfangen
//--------------------------------------------------------------
typedef enum {
  RX_EMPTY = 0,  // nichts empfangen
  RX_READY,      // es steht was im Empfangspuffer
  RX_FULL        // RX-Puffer ist voll
}UART_RXSTATUS_t;




//--------------------------------------------------------------
// Defines fuer das Empfangen
//--------------------------------------------------------------
#define  RX_BUF_SIZE   100   // Groesse vom RX-Puffer in Bytes
#define  RX_FIRST_CHR  0x20  // erstes erlaubte Zeichen (Ascii-Wert)
#define  RX_LAST_CHR   0x7E  // letztes erlaubt Zeichen (Ascii-Wert)
#define  RX_END_CHR    0x0D  // Endekennung (Ascii-Wert)




//--------------------------------------------------------------
// Globale Funktionen
//--------------------------------------------------------------
void UB_Uart_Init(void);
void UB_Uart_SendByte(UART_NAME_t uart, uint8_t wert);
void UB_Uart_SendString(UART_NAME_t uart, char *ptr, UART_LASTBYTE_t end_cmd);
UART_RXSTATUS_t UB_Uart_ReceiveString(UART_NAME_t uart, char *ptr);
void UB_Uart_SendArray(UART_NAME_t uart, uint8_t *data, uint16_t cnt);
uint32_t UB_Uart_ReceiveArray(UART_NAME_t uart, uint8_t *data);



//--------------------------------------------------------------
#endif // __STM32F7_UB_UART_H
