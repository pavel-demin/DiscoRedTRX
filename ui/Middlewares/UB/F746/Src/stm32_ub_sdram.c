//--------------------------------------------------------------
// File     : stm32_ub_sdram.c
// Datum    : 12.07.2015
// Version  : 1.0
// Autor    : UB
// EMail    : mc-4u(@)t-online.de
// Web      : www.mikrocontroller-4u.de
// CPU      : STM32F746
// IDE      : OpenSTM32
// GCC      : 4.9 2015q2
// Module   : CubeHAL
// Funktion : Externes SDRAM [8MByte]
//            Typ = MT48LC4M32B2B5-7
//            Der Zugriff erfolgt ueber den FMC-Controller
//
// Hinweis  : Das SDRAM benutzt die CPU-Pins :
//             PC0  = SDCKE0 (CKE)      PF0  = A0
//             PH3  = SDNE0  (/CS)      PF1  = A1
//             PH5  = SDNWE  (/WE)      PF2  = A2
//             PD0  = D2                PF3  = A3
//             PD1  = D3                PF4  = A4
//             PD8  = D13               PF5  = A5
//             PD9  = D14               PF11 = SDNRAS (/RAS)
//             PD10 = D15               PF12 = A6
//             PD14 = D0                PF13 = A7
//             PD15 = D1                PF14 = A8
//             PE0  = NBL0   (LDQM)     PF15 = A9
//             PE1  = NBL1   (UDQM)     PG0  = A10
//             PE7  = D4                PG1  = A11
//             PE8  = D5                PG4  = BA0    (BANK A0)
//             PE9  = D6                PG5  = BA1    (BANK A1)
//             PE10 = D7                PG8  = SDCLK  (CLK)
//             PE11 = D8                PG15 = SDNCAS (/CAS)
//             PE12 = D9
//             PE13 = D10
//             PE14 = D11
//             PE15 = D12
//         
//--------------------------------------------------------------


//--------------------------------------------------------------
// Includes
//--------------------------------------------------------------
#include "stm32_ub_sdram.h"


//--------------------------------------------------------------
// interne Funktionen
//--------------------------------------------------------------
ErrorStatus P_SDRAM_Init(void);
void P_SDRAM_Initialization_sequence(uint32_t RefreshCount);
__weak void P_SDRAM_MspInit(SDRAM_HandleTypeDef  *hsdram, void *Params);


//--------------------------------------------------------------
// globale Variabeln
//--------------------------------------------------------------
static SDRAM_HandleTypeDef sdramHandle;
static FMC_SDRAM_TimingTypeDef Timing;
static FMC_SDRAM_CommandTypeDef Command;



//--------------------------------------------------------------
// Init vom externen SDRAM
// Return_wert :
//  -> ERROR   , wenn SDRAM nicht gefunden wurde
//  -> SUCCESS , wenn SDRAM gefunden wurde
//--------------------------------------------------------------
ErrorStatus UB_SDRAM_Init(void)
{
  ErrorStatus ret_wert=ERROR;
  uint16_t oldwert,istwert;
  static uint8_t init_ok=0;

  // initialisierung darf nur einmal gemacht werden
  if(init_ok!=0) { 
    if(init_ok==1) {
      return(SUCCESS);
    }
    else {
      return(ERROR);
    }
  }

  // SDRAM initialisieren
  ret_wert=P_SDRAM_Init();
  if(ret_wert!=SUCCESS) return(ERROR);

  //-----------------------------------------
  // check ob SDRAM vorhanden ist
  // schreib-/lese-Test auf Adr 0x00
  //-----------------------------------------
  oldwert=UB_SDRAM_Read16b(0x00);
  UB_SDRAM_Write16b(0x00, 0x5A3C);
  istwert=UB_SDRAM_Read16b(0x00);
  UB_SDRAM_Write16b(0x00, oldwert);
  if(istwert==0x5A3C) ret_wert=SUCCESS; // RAM vorhanden

  // init Mode speichern
  if(ret_wert==SUCCESS) {
    init_ok=1;
  }
  else {
    init_ok=2;
  }

  return(ret_wert);
}


//--------------------------------------------------------------
// schreibt einen 8bit Wert ins externe SDRAM
// in eine Adresse
// wert : [0 bis 255]
// adr  : [0 bis SDRAM_MAX_ADR]
// Adressen muessen 1 Byte abstand zueinander haben
//--------------------------------------------------------------
void UB_SDRAM_Write8b(uint32_t adr, uint8_t wert)
{
  *(uint8_t*) (SDRAM_START_ADR + adr) = wert;
}


//--------------------------------------------------------------
// lieﬂt einen 8bit Wert aus dem externen SDRAM
// von einer Adresse
// ret_wert : [0 bis 255]
// adr  : [0 bis SDRAM_MAX_ADR]
// Adressen muessen 1 Byte abstand zueinander haben
//-------------------------------------------------------------- 
uint8_t UB_SDRAM_Read8b(uint32_t adr)
{
  uint8_t ret_wert=0;

  ret_wert = *(__IO uint8_t*)(SDRAM_START_ADR + adr);

  return(ret_wert);
}


//--------------------------------------------------------------
// schreibt einen 16bit Wert ins externe SDRAM
// in eine Adresse
// wert : [0 bis 65535]
// adr  : [0 bis SDRAM_MAX_ADR]
// Adressen muessen 2 Bytes abstand zueinander haben
//--------------------------------------------------------------
void UB_SDRAM_Write16b(uint32_t adr, uint16_t wert)
{
  *(uint16_t*) (SDRAM_START_ADR + adr) = wert;
}


//--------------------------------------------------------------
// lieﬂt einen 16bit Wert aus dem externen SDRAM
// von einer Adresse
// ret_wert : [0 bis 65535]
// adr  : [0 bis SDRAM_MAX_ADR]
// Adressen muessen 2 Bytes abstand zueinander haben
//-------------------------------------------------------------- 
uint16_t UB_SDRAM_Read16b(uint32_t adr)
{
  uint16_t ret_wert=0;

  ret_wert = *(__IO uint16_t*)(SDRAM_START_ADR + adr);

  return(ret_wert);
}


//--------------------------------------------------------------
// schreibt einen 32bit Wert ins externe SDRAM
// in eine Adresse
// wert : [0 bis 0xFFFFFFFF]
// adr  : [0 bis SDRAM_MAX_ADR]
// Adressen muessen 4 Bytes abstand zueinander haben
//--------------------------------------------------------------
void UB_SDRAM_Write32b(uint32_t adr, uint32_t wert)
{
  *(uint32_t*) (SDRAM_START_ADR + adr) = wert;
}


//--------------------------------------------------------------
// lieﬂt einen 32bit Wert aus dem externen SDRAM
// von einer Adresse
// ret_wert : [0 bis 0xFFFFFFFF]
// adr  : [0 bis SDRAM_MAX_ADR]
// Adressen muessen 4 Bytes abstand zueinander haben
//-------------------------------------------------------------- 
uint32_t UB_SDRAM_Read32b(uint32_t adr)
{
  uint32_t ret_wert=0;

  ret_wert = *(__IO uint32_t*)(SDRAM_START_ADR + adr);

  return(ret_wert);
}


//--------------------------------------------------------------
// schreibt einen Block von 32bit Werten ins externe SDRAM
// ab einer Adresse
// ptrBuffer : Pointer auf den Start vom Block der geschrieben wird
// startAdr  : Start Adresse vom Ram ab der geschrieben wird
// lenBuffer : anzahl der Daten die geschrieben werden sollen
//--------------------------------------------------------------
ErrorStatus UB_SDRAM_WriteBuffer32b(uint32_t* ptrBuffer, uint32_t startAdr, uint32_t lenBuffer)
{
  if(HAL_SDRAM_Write_32b(&sdramHandle, (uint32_t *)(startAdr+SDRAM_START_ADR), ptrBuffer, lenBuffer) != HAL_OK)
  {
    return ERROR;
  }
  else
  {
    return SUCCESS;
  }
}


//--------------------------------------------------------------
// lieﬂt einen Block von 32bit Werten aus dem externen SDRAM
// ab einer Adresse
// ptrBuffer : Pointer auf den Start vom Block der gelesen wird
// startAdr  : Start Adresse vom Ram ab der gelesen wird
// lenBuffer : anzahl der Daten die gelesen werden sollen
//--------------------------------------------------------------
ErrorStatus UB_SDRAM_ReadBuffer32b(uint32_t* ptrBuffer, uint32_t startAdr, uint32_t lenBuffer)
{
  if(HAL_SDRAM_Read_32b(&sdramHandle, (uint32_t *)(startAdr+SDRAM_START_ADR), ptrBuffer, lenBuffer) != HAL_OK)
  {
    return ERROR;
  }
  else
  {
    return SUCCESS;
  }
}


//--------------------------------------------------------------
// schreibt einen Block von 32bit Werten ins externe SDRAM
// ab einer Adresse (per DMA)
// ptrBuffer : Pointer auf den Start vom Block der geschrieben wird
// startAdr  : Start Adresse vom Ram ab der geschrieben wird
// lenBuffer : anzahl der Daten die geschrieben werden sollen
//--------------------------------------------------------------
ErrorStatus UB_SDRAM_WriteBuffer32bDMA(uint32_t* ptrBuffer, uint32_t startAdr, uint32_t lenBuffer)
{
  if(HAL_SDRAM_Write_DMA(&sdramHandle, (uint32_t *)(startAdr+SDRAM_START_ADR), ptrBuffer, lenBuffer) != HAL_OK)
  {
    return ERROR;
  }
  else
  {
    return SUCCESS;
  }
}




//--------------------------------------------------------------
// interne Funktion
// Init vom SDRAM
//--------------------------------------------------------------
ErrorStatus P_SDRAM_Init(void)
{
  static ErrorStatus sdramstatus = ERROR;

  /* SDRAM device configuration */
  sdramHandle.Instance = FMC_SDRAM_DEVICE;

  /* Timing configuration for 100Mhz as SD clock frequency (System clock is up to 200Mhz) */
  Timing.LoadToActiveDelay    = 2;
  Timing.ExitSelfRefreshDelay = 7;
  Timing.SelfRefreshTime      = 4;
  Timing.RowCycleDelay        = 7;
  Timing.WriteRecoveryTime    = 2;
  Timing.RPDelay              = 2;
  Timing.RCDDelay             = 2;

  sdramHandle.Init.SDBank             = FMC_SDRAM_BANK1;
  sdramHandle.Init.ColumnBitsNumber   = FMC_SDRAM_COLUMN_BITS_NUM_8;
  sdramHandle.Init.RowBitsNumber      = FMC_SDRAM_ROW_BITS_NUM_12;
  sdramHandle.Init.MemoryDataWidth    = SDRAM_MEMORY_WIDTH;
  sdramHandle.Init.InternalBankNumber = FMC_SDRAM_INTERN_BANKS_NUM_4;
  sdramHandle.Init.CASLatency         = FMC_SDRAM_CAS_LATENCY_2;
  sdramHandle.Init.WriteProtection    = FMC_SDRAM_WRITE_PROTECTION_DISABLE;
  sdramHandle.Init.SDClockPeriod      = SDCLOCK_PERIOD;
  sdramHandle.Init.ReadBurst          = FMC_SDRAM_RBURST_ENABLE;
  sdramHandle.Init.ReadPipeDelay      = FMC_SDRAM_RPIPE_DELAY_0;

  /* SDRAM controller initialization */
  P_SDRAM_MspInit(&sdramHandle, NULL); /* __weak function can be rewritten by the application */

  if(HAL_SDRAM_Init(&sdramHandle, &Timing) == HAL_OK) sdramstatus = SUCCESS;

  /* SDRAM initialization sequence */
  P_SDRAM_Initialization_sequence(REFRESH_COUNT);

  return sdramstatus;
}

//--------------------------------------------------------------
// interne Funktion
//--------------------------------------------------------------
void P_SDRAM_Initialization_sequence(uint32_t RefreshCount)
{
  __IO uint32_t tmpmrd = 0;

  /* Step 1: Configure a clock configuration enable command */
  Command.CommandMode            = FMC_SDRAM_CMD_CLK_ENABLE;
  Command.CommandTarget          = FMC_SDRAM_CMD_TARGET_BANK1;
  Command.AutoRefreshNumber      = 1;
  Command.ModeRegisterDefinition = 0;

  /* Send the command */
  HAL_SDRAM_SendCommand(&sdramHandle, &Command, SDRAM_TIMEOUT);

  /* Step 2: Insert 100 us minimum delay */
  /* Inserted delay is equal to 1 ms due to systick time base unit (ms) */
  HAL_Delay(1);

  /* Step 3: Configure a PALL (precharge all) command */
  Command.CommandMode            = FMC_SDRAM_CMD_PALL;
  Command.CommandTarget          = FMC_SDRAM_CMD_TARGET_BANK1;
  Command.AutoRefreshNumber      = 1;
  Command.ModeRegisterDefinition = 0;

  /* Send the command */
  HAL_SDRAM_SendCommand(&sdramHandle, &Command, SDRAM_TIMEOUT);

  /* Step 4: Configure an Auto Refresh command */
  Command.CommandMode            = FMC_SDRAM_CMD_AUTOREFRESH_MODE;
  Command.CommandTarget          = FMC_SDRAM_CMD_TARGET_BANK1;
  Command.AutoRefreshNumber      = 8;
  Command.ModeRegisterDefinition = 0;

  /* Send the command */
  HAL_SDRAM_SendCommand(&sdramHandle, &Command, SDRAM_TIMEOUT);

  /* Step 5: Program the external memory mode register */
  tmpmrd = (uint32_t)SDRAM_MODEREG_BURST_LENGTH_1          |\
	                     SDRAM_MODEREG_BURST_TYPE_SEQUENTIAL   |\
	                     SDRAM_MODEREG_CAS_LATENCY_2           |\
	                     SDRAM_MODEREG_OPERATING_MODE_STANDARD |\
	                     SDRAM_MODEREG_WRITEBURST_MODE_SINGLE;

  Command.CommandMode            = FMC_SDRAM_CMD_LOAD_MODE;
  Command.CommandTarget          = FMC_SDRAM_CMD_TARGET_BANK1;
  Command.AutoRefreshNumber      = 1;
  Command.ModeRegisterDefinition = tmpmrd;

  /* Send the command */
  HAL_SDRAM_SendCommand(&sdramHandle, &Command, SDRAM_TIMEOUT);

  /* Step 6: Set the refresh rate counter */
  /* Set the device refresh rate */
  HAL_SDRAM_ProgramRefreshRate(&sdramHandle, RefreshCount);
}

//--------------------------------------------------------------
// interne Funktion
//--------------------------------------------------------------
__weak void P_SDRAM_MspInit(SDRAM_HandleTypeDef  *hsdram, void *Params)
{
  GPIO_InitTypeDef gpio_init_structure;

  /* Enable FMC clock */
  __HAL_RCC_FMC_CLK_ENABLE();

  /* Enable GPIOs clock */
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOD_CLK_ENABLE();
  __HAL_RCC_GPIOE_CLK_ENABLE();
  __HAL_RCC_GPIOF_CLK_ENABLE();
  __HAL_RCC_GPIOG_CLK_ENABLE();
  __HAL_RCC_GPIOH_CLK_ENABLE();

  /* Common GPIO configuration */
  gpio_init_structure.Mode      = GPIO_MODE_AF_PP;
  gpio_init_structure.Pull      = GPIO_PULLUP;
  gpio_init_structure.Speed     = GPIO_SPEED_FAST;
  gpio_init_structure.Alternate = GPIO_AF12_FMC;

  /* GPIOC configuration */
  gpio_init_structure.Pin   = GPIO_PIN_3;
  HAL_GPIO_Init(GPIOC, &gpio_init_structure);

  /* GPIOD configuration */
  gpio_init_structure.Pin   = GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_3 | GPIO_PIN_8 | GPIO_PIN_9 |
                              GPIO_PIN_10 | GPIO_PIN_14 | GPIO_PIN_15;
  HAL_GPIO_Init(GPIOD, &gpio_init_structure);

  /* GPIOE configuration */
  gpio_init_structure.Pin   = GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_7| GPIO_PIN_8 | GPIO_PIN_9 |\
                              GPIO_PIN_10 | GPIO_PIN_11 | GPIO_PIN_12 | GPIO_PIN_13 | GPIO_PIN_14 |\
                              GPIO_PIN_15;
  HAL_GPIO_Init(GPIOE, &gpio_init_structure);

  /* GPIOF configuration */
  gpio_init_structure.Pin   = GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_2| GPIO_PIN_3 | GPIO_PIN_4 |\
                              GPIO_PIN_5 | GPIO_PIN_11 | GPIO_PIN_12 | GPIO_PIN_13 | GPIO_PIN_14 |\
                              GPIO_PIN_15;
  HAL_GPIO_Init(GPIOF, &gpio_init_structure);

  /* GPIOG configuration */
  gpio_init_structure.Pin   = GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_4| GPIO_PIN_5 | GPIO_PIN_8 |\
                              GPIO_PIN_15;
  HAL_GPIO_Init(GPIOG, &gpio_init_structure);

  /* GPIOH configuration */
  gpio_init_structure.Pin   = GPIO_PIN_3 | GPIO_PIN_5;
  HAL_GPIO_Init(GPIOH, &gpio_init_structure);
}





