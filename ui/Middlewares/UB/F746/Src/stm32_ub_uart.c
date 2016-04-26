//--------------------------------------------------------------
// File     : stm32_ub_uart.c
// Datum    : 26.07.2015
// Version  : 1.1
// Autor    : UB
// EMail    : mc-4u(@)t-online.de
// Web      : www.mikrocontroller-4u.de
// CPU      : STM32F746
// IDE      : OpenSTM32
// GCC      : 4.9 2015q2
// Module   : CubeHAL
// Funktion : UART (RS232) In und OUT
//            Receive wird per Interrupt behandelt
//
// Hinweis  : moegliche Pinbelegungen
//            UART1 : TX:[PA9,PB6] RX:[PA10,PB7]
//            UART2 : TX:[PA2,PD5] RX:[PA3,PD6]
//            UART3 : TX:[PB10,PC10,PD8] RX:[PB11,PC11,PD9]
//            UART4 : TX:[PA0,PC10] RX:[PA1,PC11]
//            UART5 : TX:[PC12] RX:[PD2]
//            UART6 : TX:[PC6,PG14] RX:[PC7,PG9]
//            UART7 : TX:[PE8,PF7] RX:[PE7,PF6]
//            UART8 : TX:[PE1] RX:[PE0]
//
// Vorsicht : Als Endekennung beim Empfangen, muss der Sender
//            das Zeichen "0x0D" = Carriage-Return
//            an den String anhaengen !!
//
//  Im H-File einstellen welche Com-Ports benutzt werden sollen
//  und wie die Settings und GPIO-Belegung sein soll
//--------------------------------------------------------------


//--------------------------------------------------------------
// Includes
//--------------------------------------------------------------
#include "stm32_ub_uart.h"







//--------------------------------------------------------------
static void P_BSP_COM_Init(UART_NAME_t uart);
static void P_ISR_COM_Init(UART_NAME_t uart);



//--------------------------------------------------------------
static UART_HandleTypeDef UartHandle1; // Com1
static UART_HandleTypeDef UartHandle6; // Com6
static UART_HandleTypeDef UartHandle7; // Com7


//--------------------------------------------------------------
// RX-Buffer
//--------------------------------------------------------------
#if USE_UART1==1
  volatile uint8_t rx_buf1[RX_BUF_SIZE];
  volatile uint32_t rx_rd_ptr1;
  volatile uint32_t rx_wr_ptr1;
#endif
#if USE_UART6==1
  volatile uint8_t rx_buf6[RX_BUF_SIZE];
  volatile uint32_t rx_rd_ptr6;
  volatile uint32_t rx_wr_ptr6;
#endif
#if USE_UART7==1
  volatile uint8_t rx_buf7[RX_BUF_SIZE];
  volatile uint32_t rx_rd_ptr7;
  volatile uint32_t rx_wr_ptr7;
#endif


//--------------------------------------------------------------
// init aller UARTs
//--------------------------------------------------------------
void UB_Uart_Init(void)
{
  #if USE_UART1==1 
    P_BSP_COM_Init(COM1);

    // UART init
    UartHandle1.Instance = USART1;    
    UartHandle1.Init.BaudRate     = COM1_BAUD;
    UartHandle1.Init.WordLength   = UART_WORDLENGTH_8B;
    UartHandle1.Init.StopBits     = UART_STOPBITS_1;
    UartHandle1.Init.Parity       = UART_PARITY_NONE;
    UartHandle1.Init.HwFlowCtl    = UART_HWCONTROL_NONE;
    UartHandle1.Init.Mode         = UART_MODE_TX_RX; 
    UartHandle1.Init.OverSampling = UART_OVERSAMPLING_8;
    UartHandle1.Init.OneBitSampling = UART_ONEBIT_SAMPLING_DISABLED;
    UartHandle1.AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_NO_INIT;
    HAL_UART_Init(&UartHandle1); 
    
    P_ISR_COM_Init(COM1);

    rx_rd_ptr1 =0;
    rx_wr_ptr1 =0;

  #endif
  #if USE_UART6==1
    P_BSP_COM_Init(COM6);

    UartHandle6.Instance = USART6;
    UartHandle6.Init.BaudRate     = COM6_BAUD;
    UartHandle6.Init.WordLength   = UART_WORDLENGTH_8B;
    UartHandle6.Init.StopBits     = UART_STOPBITS_1;
    UartHandle6.Init.Parity       = UART_PARITY_NONE;
    UartHandle6.Init.HwFlowCtl    = UART_HWCONTROL_NONE;
    UartHandle6.Init.Mode         = UART_MODE_TX_RX;
    UartHandle6.Init.OverSampling = UART_OVERSAMPLING_8;
    UartHandle6.Init.OneBitSampling = UART_ONEBIT_SAMPLING_DISABLED;
    UartHandle6.AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_NO_INIT;
    HAL_UART_Init(&UartHandle6);

    P_ISR_COM_Init(COM6);
 
    rx_rd_ptr6 =0;
    rx_wr_ptr6 =0;
  #endif
  #if USE_UART7==1
    P_BSP_COM_Init(COM7);

    UartHandle7.Instance = UART7;
    UartHandle7.Init.BaudRate     = COM7_BAUD;
    UartHandle7.Init.WordLength   = UART_WORDLENGTH_8B;
    UartHandle7.Init.StopBits     = UART_STOPBITS_1;
    UartHandle7.Init.Parity       = UART_PARITY_NONE;
    UartHandle7.Init.HwFlowCtl    = UART_HWCONTROL_NONE;
    UartHandle7.Init.Mode         = UART_MODE_TX_RX;
    UartHandle7.Init.OverSampling = UART_OVERSAMPLING_8;
    UartHandle7.Init.OneBitSampling = UART_ONEBIT_SAMPLING_DISABLED;
    UartHandle7.AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_NO_INIT;
    HAL_UART_Init(&UartHandle7);

    P_ISR_COM_Init(COM7);

    rx_rd_ptr7 =0;
    rx_wr_ptr7 =0;
  #endif
}



//--------------------------------------------------------------
// ein Byte per UART senden
//--------------------------------------------------------------
void UB_Uart_SendByte(UART_NAME_t uart, uint8_t wert)
{
  if(uart==COM1) {
    HAL_UART_Transmit(&UartHandle1, &wert, 1, UART_TX_TIMEOUT);
  }
  if(uart==COM6) {
    HAL_UART_Transmit(&UartHandle6, &wert, 1, UART_TX_TIMEOUT);
  }
  if(uart==COM7) {
    HAL_UART_Transmit(&UartHandle7, &wert, 1, UART_TX_TIMEOUT);
  }
}



//--------------------------------------------------------------
// einen String per UART senden
//
// Endekennung vom String muss 0x00 sein !!
//
// end_cmd : NONE, LFCR, CRLF, LF, CR
//--------------------------------------------------------------
void UB_Uart_SendString(UART_NAME_t uart, char *ptr, UART_LASTBYTE_t end_cmd)
{
  uint16_t length = 0;

  // laenge ermitteln
  while (ptr[length] != '\0')
  {
    length++;
  } 

  // sende kompletten String
  if(uart==COM1) {
    HAL_UART_Transmit(&UartHandle1, (uint8_t*)(ptr), length, UART_TX_TIMEOUT);
  }
  if(uart==COM6) {
    HAL_UART_Transmit(&UartHandle6, (uint8_t*)(ptr), length, UART_TX_TIMEOUT);
  }
  if(uart==COM7) {
    HAL_UART_Transmit(&UartHandle7, (uint8_t*)(ptr), length, UART_TX_TIMEOUT);
  }

  // eventuell Endekennung senden
  if(end_cmd==LFCR) {
    UB_Uart_SendByte(uart,0x0A); // LineFeed senden
    UB_Uart_SendByte(uart,0x0D); // CariageReturn senden
  }
  else if(end_cmd==CRLF) {
    UB_Uart_SendByte(uart,0x0D); // CariageReturn senden
    UB_Uart_SendByte(uart,0x0A); // LineFeed senden
  }
  else if(end_cmd==LF) {
    UB_Uart_SendByte(uart,0x0A); // LineFeed senden
  }
  else if(end_cmd==CR) {
    UB_Uart_SendByte(uart,0x0D); // CariageReturn senden    
  }
}


//--------------------------------------------------------------
// einen String per UART empfangen
// (der Empfang wird per DMA abgehandelt)
// diese Funktion muss zyklisch gepollt werden
//
// Endekennung vom String muss "RX_END_CHR"=0x0D sein !!
//
// Return Wert :
//  -> wenn nichts empfangen = RX_EMPTY
//  -> wenn String empfangen = RX_READY -> String steht in *ptr
//--------------------------------------------------------------
UART_RXSTATUS_t UB_Uart_ReceiveString(UART_NAME_t uart, char *ptr)
{
  UART_RXSTATUS_t ret_wert=RX_EMPTY; 
  uint32_t akt_pos,anz,n,m;
  uint8_t wert;

  #if USE_UART1==1
    if(uart==COM1) {
      // Anzahl der Daten im Puffer ermitteln
      if(rx_wr_ptr1>=rx_rd_ptr1) {
        anz=rx_wr_ptr1-rx_rd_ptr1;
      }
      else {
        anz=(rx_wr_ptr1+RX_BUF_SIZE)-rx_rd_ptr1;
      }
      if(anz==0) return RX_EMPTY;

      akt_pos=rx_rd_ptr1;
      // alle daten bis endekennung auslesen
      m=0;
      for(n=0;n<anz;n++) {
        wert=rx_buf1[rx_rd_ptr1];
        rx_rd_ptr1++;
        if(rx_rd_ptr1>=RX_BUF_SIZE) rx_rd_ptr1=0;
        // nur Ascii-Zeichen uebergeben
        if((wert>=RX_FIRST_CHR) && (wert<=RX_LAST_CHR)) {
          ptr[m]=wert;
          m++;
        }
        else if(wert==RX_END_CHR) {
          // endekennung gefunden
          ptr[m]=0x00; // String Endekennung hinzufuegen
          return RX_READY;
        }
      }
      // falls keine Endekennung dabei war,
      // pointer wieder zuruecksetzen
      rx_rd_ptr1=akt_pos;
      return RX_EMPTY;
    }
  #endif
  #if USE_UART6==1
    if(uart==COM6) {
      // Anzahl der Daten im Puffer ermitteln
      if(rx_wr_ptr6>=rx_rd_ptr6) {
        anz=rx_wr_ptr6-rx_rd_ptr6;
      }
      else {
        anz=(rx_wr_ptr6+RX_BUF_SIZE)-rx_rd_ptr6;
      }
      if(anz==0) return RX_EMPTY;

      akt_pos=rx_rd_ptr6;
      // alle daten bis endekennung auslesen
      m=0;
      for(n=0;n<anz;n++) {
        wert=rx_buf6[rx_rd_ptr6];
        rx_rd_ptr6++;
        if(rx_rd_ptr6>=RX_BUF_SIZE) rx_rd_ptr6=0;
        // nur Ascii-Zeichen uebergeben
        if((wert>=RX_FIRST_CHR) && (wert<=RX_LAST_CHR)) {
          ptr[m]=wert;
          m++;
        }
        else if(wert==RX_END_CHR) {
          // endekennung gefunden
          ptr[m]=0x00; // String Endekennung hinzufuegen
          return RX_READY;
        }
      }
      // falls keine Endekennung dabei war,
      // pointer wieder zuruecksetzen
      rx_rd_ptr6=akt_pos;
      return RX_EMPTY;
    }
  #endif
  #if USE_UART7==1
    if(uart==COM7) {
      // Anzahl der Daten im Puffer ermitteln
      if(rx_wr_ptr7>=rx_rd_ptr7) {
        anz=rx_wr_ptr7-rx_rd_ptr7;
      }
      else {
        anz=(rx_wr_ptr7+RX_BUF_SIZE)-rx_rd_ptr7;
      }
      if(anz==0) return RX_EMPTY;

      akt_pos=rx_rd_ptr7;
      // alle daten bis endekennung auslesen
      m=0;
      for(n=0;n<anz;n++) {
        wert=rx_buf7[rx_rd_ptr7];
        rx_rd_ptr7++;
        if(rx_rd_ptr7>=RX_BUF_SIZE) rx_rd_ptr7=0;
        // nur Ascii-Zeichen uebergeben
        if((wert>=RX_FIRST_CHR) && (wert<=RX_LAST_CHR)) {
          ptr[m]=wert;
          m++;
        }
        else if(wert==RX_END_CHR) {
          // endekennung gefunden
          ptr[m]=0x00; // String Endekennung hinzufuegen
          return RX_READY;
        }
      }
      // falls keine Endekennung dabei war,
      // pointer wieder zuruecksetzen
      rx_rd_ptr7=akt_pos;
      return RX_EMPTY;
    }
  #endif

  return(ret_wert);
}

//--------------------------------------------------------------
// ein Daten Array per UART senden
// cnt = Anzahl der Daten die gesendet werden sollen
//--------------------------------------------------------------
void UB_Uart_SendArray(UART_NAME_t uart, uint8_t *data, uint16_t cnt)
{
  if(cnt==0) return;

  // sende alle Daten
  if(uart==COM1) {
    HAL_UART_Transmit(&UartHandle1, data, cnt, UART_TX_TIMEOUT);  
  }
  if(uart==COM6) {
    HAL_UART_Transmit(&UartHandle6, data, cnt, UART_TX_TIMEOUT);  
  }
  if(uart==COM7) {
    HAL_UART_Transmit(&UartHandle7, data, cnt, UART_TX_TIMEOUT);  
  }
}


//--------------------------------------------------------------
// ein Daten Array per UART empfangen
// Return Wert :
//   Anzahl der bis jetzt empfangenen Bytes [0...RX_BUF_SIZE]
//--------------------------------------------------------------
uint32_t UB_Uart_ReceiveArray(UART_NAME_t uart, uint8_t *data)
{
  uint16_t ret_wert=0;
  uint32_t anz,n;
  uint8_t wert;

  #if USE_UART1==1
    if(uart==COM1) {
      // Anzahl der Daten im Puffer ermitteln
      if(rx_wr_ptr1>=rx_rd_ptr1) {
        anz=rx_wr_ptr1-rx_rd_ptr1;
      }
      else {
        anz=(rx_wr_ptr1+RX_BUF_SIZE)-rx_rd_ptr1;
      }
      if(anz==0) return 0;

      // alle daten auslesen      
      for(n=0;n<anz;n++) {
        wert=rx_buf1[rx_rd_ptr1];
        rx_rd_ptr1++;
        if(rx_rd_ptr1>=RX_BUF_SIZE) rx_rd_ptr1=0;
        data[n]=wert;
      }
      return anz;
    }
  #endif
  #if USE_UART6==1
    if(uart==COM6) {
      // Anzahl der Daten im Puffer ermitteln
      if(rx_wr_ptr6>=rx_rd_ptr6) {
        anz=rx_wr_ptr6-rx_rd_ptr6;
      }
      else {
        anz=(rx_wr_ptr6+RX_BUF_SIZE)-rx_rd_ptr6;
      }
      if(anz==0) return 0;

      // alle daten auslesen      
      for(n=0;n<anz;n++) {
        wert=rx_buf6[rx_rd_ptr6];
        rx_rd_ptr6++;
        if(rx_rd_ptr6>=RX_BUF_SIZE) rx_rd_ptr6=0;
        data[n]=wert;
      }
      return anz;
    }
  #endif
  #if USE_UART7==1
    if(uart==COM7) {
      // Anzahl der Daten im Puffer ermitteln
      if(rx_wr_ptr7>=rx_rd_ptr7) {
        anz=rx_wr_ptr7-rx_rd_ptr7;
      }
      else {
        anz=(rx_wr_ptr7+RX_BUF_SIZE)-rx_rd_ptr7;
      }
      if(anz==0) return 0;

      // alle daten auslesen      
      for(n=0;n<anz;n++) {
        wert=rx_buf7[rx_rd_ptr7];
        rx_rd_ptr7++;
        if(rx_rd_ptr7>=RX_BUF_SIZE) rx_rd_ptr7=0;
        data[n]=wert;
      }
      return anz;
    }
  #endif

  return(ret_wert);
}


//--------------------------------------------------------------
// interne Funktion
// init vom Comport
//--------------------------------------------------------------
static void P_BSP_COM_Init(UART_NAME_t uart)
{
  GPIO_InitTypeDef gpio_init_structure;

  if(uart==COM1) {
    // GPIO Clock Enable
    UB_System_ClockEnable(COM1_TX_PORT);
    UB_System_ClockEnable(COM1_RX_PORT);

    // UART Clock Enable
    __HAL_RCC_USART1_CLK_ENABLE();

    // GPIO TX
    gpio_init_structure.Pin = COM1_TX_PIN;
    gpio_init_structure.Mode = GPIO_MODE_AF_PP;
    gpio_init_structure.Speed = GPIO_SPEED_FAST;
    gpio_init_structure.Pull = GPIO_PULLUP;
    gpio_init_structure.Alternate = GPIO_AF7_USART1;
    HAL_GPIO_Init(COM1_TX_PORT, &gpio_init_structure);

    // GPIO RX
    gpio_init_structure.Pin = COM1_RX_PIN;
    gpio_init_structure.Mode = GPIO_MODE_AF_PP;
    gpio_init_structure.Alternate = GPIO_AF7_USART1;
    HAL_GPIO_Init(COM1_RX_PORT, &gpio_init_structure);
  }
  if(uart==COM6) {
    // GPIO Clock Enable
    UB_System_ClockEnable(COM6_TX_PORT);
    UB_System_ClockEnable(COM6_RX_PORT);

    // UART Clock Enable
    __HAL_RCC_USART6_CLK_ENABLE();

    // GPIO TX
    gpio_init_structure.Pin = COM6_TX_PIN;
    gpio_init_structure.Mode = GPIO_MODE_AF_PP;
    gpio_init_structure.Speed = GPIO_SPEED_FAST;
    gpio_init_structure.Pull = GPIO_PULLUP;
    gpio_init_structure.Alternate = GPIO_AF8_USART6;
    HAL_GPIO_Init(COM6_TX_PORT, &gpio_init_structure);

    // GPIO RX
    gpio_init_structure.Pin = COM6_RX_PIN;
    gpio_init_structure.Mode = GPIO_MODE_AF_PP;
    gpio_init_structure.Alternate = GPIO_AF8_USART6;
    HAL_GPIO_Init(COM6_RX_PORT, &gpio_init_structure);
  }
  if(uart==COM7) {
    // GPIO Clock Enable
    UB_System_ClockEnable(COM7_TX_PORT);
    UB_System_ClockEnable(COM7_RX_PORT);

    // UART Clock Enable
    __HAL_RCC_UART7_CLK_ENABLE();

    // GPIO TX
    gpio_init_structure.Pin = COM7_TX_PIN;
    gpio_init_structure.Mode = GPIO_MODE_AF_PP;
    gpio_init_structure.Speed = GPIO_SPEED_FAST;
    gpio_init_structure.Pull = GPIO_PULLUP;
    gpio_init_structure.Alternate = GPIO_AF8_UART7;
    HAL_GPIO_Init(COM7_TX_PORT, &gpio_init_structure);

    // GPIO RX
    gpio_init_structure.Pin = COM7_RX_PIN;
    gpio_init_structure.Mode = GPIO_MODE_AF_PP;
    gpio_init_structure.Alternate = GPIO_AF8_UART7;
    HAL_GPIO_Init(COM7_RX_PORT, &gpio_init_structure);
  }
}

//--------------------------------------------------------------
// interne Funktion
// init vom RX-Interrupt
//--------------------------------------------------------------
static void P_ISR_COM_Init(UART_NAME_t uart)
{
  if(uart==COM1) {
    // RX-Interrupt enable
    __HAL_UART_ENABLE_IT(&UartHandle1, UART_IT_RXNE);

    HAL_NVIC_SetPriority(USART1_IRQn, 0, 0);
    HAL_NVIC_EnableIRQ(USART1_IRQn);
  }
  if(uart==COM6) {
    // RX-Interrupt enable
    __HAL_UART_ENABLE_IT(&UartHandle6, UART_IT_RXNE);

    HAL_NVIC_SetPriority(USART6_IRQn, 0, 0);
    HAL_NVIC_EnableIRQ(USART6_IRQn);
  } 
  if(uart==COM7) {
    // RX-Interrupt enable
    __HAL_UART_ENABLE_IT(&UartHandle7, UART_IT_RXNE);

    HAL_NVIC_SetPriority(UART7_IRQn, 0, 0);
    HAL_NVIC_EnableIRQ(UART7_IRQn);
  }
}

//--------------------------------------------------------------
// ISR COM1-RX
//--------------------------------------------------------------
#if USE_UART1==1
void USART1_IRQHandler(void)
{
  volatile uint8_t value;

  if(__HAL_UART_GET_IT(&UartHandle1,UART_IT_RXNE)!=RESET) {
    // wenn ein Byte im Empfangspuffer steht
    value=(uint8_t)(USART1->RDR & 0xFF);
    // byte speichern
    rx_buf1[rx_wr_ptr1]=value;
    rx_wr_ptr1++;
    if(rx_wr_ptr1>=RX_BUF_SIZE) rx_wr_ptr1=0;
  }
}
#endif

//--------------------------------------------------------------
// ISR COM6-RX
//--------------------------------------------------------------
#if USE_UART6==1
void USART6_IRQHandler(void)
{
  volatile uint8_t value;

  if(__HAL_UART_GET_IT(&UartHandle6,UART_IT_RXNE)!=RESET) {
    // wenn ein Byte im Empfangspuffer steht
    value=(uint8_t)(USART6->RDR & 0xFF);
    // byte speichern
    rx_buf6[rx_wr_ptr6]=value;
    rx_wr_ptr6++;
    if(rx_wr_ptr6>=RX_BUF_SIZE) rx_wr_ptr6=0;
  }
}
#endif

//--------------------------------------------------------------
// ISR COM7-RX
//--------------------------------------------------------------
#if USE_UART7==1
void UART7_IRQHandler(void)
{
  volatile uint8_t value;

  if(__HAL_UART_GET_IT(&UartHandle7,UART_IT_RXNE)!=RESET) {
    // wenn ein Byte im Empfangspuffer steht
    value=(uint8_t)(UART7->RDR & 0xFF);
    // byte speichern
    rx_buf7[rx_wr_ptr7]=value;
    rx_wr_ptr7++;
    if(rx_wr_ptr7>=RX_BUF_SIZE) rx_wr_ptr7=0;
  }
}
#endif

