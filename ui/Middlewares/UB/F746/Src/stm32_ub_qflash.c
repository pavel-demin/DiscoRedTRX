//--------------------------------------------------------------
// File     : stm32_ub_qflash.c
// Datum    : 28.09.2015
// Version  : 1.0
// Autor    : UB
// EMail    : mc-4u(@)t-online.de
// Web      : www.mikrocontroller-4u.de
// CPU      : STM32F746
// IDE      : OpenSTM32
// GCC      : 4.9 2015q2
// Module   : CubeHAL
// Funktion : externes Quad-Flash
//            Chip = N25Q128A13EF840E (Micron)
//            Size = 128MBit (16MByte)
//
// Hinweis  : Das Flash benutzt den Quad-SPI-Bus
//            und die CPU-Pins :
//
//            PB2  = CLK
//            PB6  = CS
//            PD11 = IO_0
//            PD12 = IO_1
//            PD13 = IO_3
//            PE2  = IO_2
//--------------------------------------------------------------

//--------------------------------------------------------------
// Includes
//--------------------------------------------------------------
#include "stm32_ub_qflash.h"

//--------------------------------------------------------------
// interne Funktionen
//--------------------------------------------------------------
__weak void P_QSPI_MspInit(QSPI_HandleTypeDef *hqspi, void *Params);
static uint8_t P_QSPI_ResetMemory(QSPI_HandleTypeDef *hqspi);
static uint8_t P_QSPI_DummyCyclesCfg(QSPI_HandleTypeDef *hqspi);
static uint8_t P_QSPI_AutoPollingMemReady(QSPI_HandleTypeDef *hqspi,
                                          uint32_t Timeout);
static uint8_t P_QSPI_WriteEnable(QSPI_HandleTypeDef *hqspi);

//--------------------------------------------------------------
// Globale Variabeln
//--------------------------------------------------------------
QSPI_HandleTypeDef QSPIHandle;

//--------------------------------------------------------------
// Init vom QFlash
//
// return : QSPI_OK, wenn alles ok
//--------------------------------------------------------------
uint8_t UB_QFlash_Init(void) {
  QSPIHandle.Instance = QUADSPI;
  HAL_QSPI_DeInit(&QSPIHandle);

  P_QSPI_MspInit(&QSPIHandle, NULL);

  // init
  QSPIHandle.Init.ClockPrescaler = 1;
  QSPIHandle.Init.FifoThreshold = 4;
  QSPIHandle.Init.SampleShifting = QSPI_SAMPLE_SHIFTING_HALFCYCLE;
  QSPIHandle.Init.FlashSize = POSITION_VAL(N25Q128A_FLASH_SIZE) - 1;
  QSPIHandle.Init.ChipSelectHighTime = QSPI_CS_HIGH_TIME_2_CYCLE;
  QSPIHandle.Init.ClockMode = QSPI_CLOCK_MODE_0;
  QSPIHandle.Init.FlashID = QSPI_FLASH_ID_1;
  QSPIHandle.Init.DualFlash = QSPI_DUALFLASH_DISABLE;
  HAL_QSPI_Init(&QSPIHandle);

  // reset
  if (P_QSPI_ResetMemory(&QSPIHandle) != QSPI_OK)
    return QSPI_NOT_SUPPORTED;

  // config
  if (P_QSPI_DummyCyclesCfg(&QSPIHandle) != QSPI_OK)
    return QSPI_NOT_SUPPORTED;

  return QSPI_OK;
}

//--------------------------------------------------------------
// loeschen vom kompletten QFlash
// VORSICHT !! das loeschen dauert ca. 110 sekunden !!
//
// return : QSPI_OK, wenn alles ok
//--------------------------------------------------------------
uint8_t UB_QFlash_Erase_Complete(void) {
  QSPI_CommandTypeDef s_command;

  // Initialize the erase command
  s_command.InstructionMode = QSPI_INSTRUCTION_1_LINE;
  s_command.Instruction = BULK_ERASE_CMD;
  s_command.AddressMode = QSPI_ADDRESS_NONE;
  s_command.AlternateByteMode = QSPI_ALTERNATE_BYTES_NONE;
  s_command.DataMode = QSPI_DATA_NONE;
  s_command.DummyCycles = 0;
  s_command.DdrMode = QSPI_DDR_MODE_DISABLE;
  s_command.DdrHoldHalfCycle = QSPI_DDR_HHC_ANALOG_DELAY;
  s_command.SIOOMode = QSPI_SIOO_INST_EVERY_CMD;

  // Enable write operations
  if (P_QSPI_WriteEnable(&QSPIHandle) != QSPI_OK)
    return QSPI_ERROR;

  // Send the command
  if (HAL_QSPI_Command(&QSPIHandle, &s_command,
                       HAL_QPSI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
    return QSPI_ERROR;

  // Configure automatic polling mode to wait for end of erase
  if (P_QSPI_AutoPollingMemReady(&QSPIHandle, N25Q128A_BULK_ERASE_MAX_TIME) !=
      QSPI_OK)
    return QSPI_ERROR;

  return QSPI_OK;
}

//--------------------------------------------------------------
// loeschen von einem Subsector (4kByte) im QFlash
// subsector_nr : [0...4095]
//
// return : QSPI_OK, wenn alles ok
//--------------------------------------------------------------
uint8_t UB_QFlash_Erase_SubSector(uint32_t subsector_nr) {
  QSPI_CommandTypeDef s_command;

  if (subsector_nr >= N25Q128A_SUBSECTOR_CNT)
    return QSPI_ERROR;

  // Initialize the erase command
  s_command.InstructionMode = QSPI_INSTRUCTION_1_LINE;
  s_command.Instruction = SUBSECTOR_ERASE_CMD;
  s_command.AddressMode = QSPI_ADDRESS_1_LINE;
  s_command.AddressSize = QSPI_ADDRESS_24_BITS;
  s_command.Address = subsector_nr;
  s_command.AlternateByteMode = QSPI_ALTERNATE_BYTES_NONE;
  s_command.DataMode = QSPI_DATA_NONE;
  s_command.DummyCycles = 0;
  s_command.DdrMode = QSPI_DDR_MODE_DISABLE;
  s_command.DdrHoldHalfCycle = QSPI_DDR_HHC_ANALOG_DELAY;
  s_command.SIOOMode = QSPI_SIOO_INST_EVERY_CMD;

  // Enable write operations
  if (P_QSPI_WriteEnable(&QSPIHandle) != QSPI_OK)
    return QSPI_ERROR;

  // Send the command
  if (HAL_QSPI_Command(&QSPIHandle, &s_command,
                       HAL_QPSI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
    return QSPI_ERROR;

  // Configure automatic polling mode to wait for end of erase
  if (P_QSPI_AutoPollingMemReady(&QSPIHandle,
                                 N25Q128A_SUBSECTOR_ERASE_MAX_TIME) != QSPI_OK)
    return QSPI_ERROR;

  return QSPI_OK;
}

//--------------------------------------------------------------
// lesen von einem 8bit Datenblock vom QFlash
// start_adr : start adresse von der gelesen wird
// size      : anzahl der daten die gelesen werden
// data_buf  : pointer zu einem Puffer in dem die Daten landen
//
// return : QSPI_OK, wenn alles ok
//--------------------------------------------------------------
uint8_t UB_QFlash_Read_Block8b(uint32_t start_adr, uint32_t size,
                               uint8_t *data_buf) {
  QSPI_CommandTypeDef s_command;

  if (start_adr >= N25Q128A_FLASH_SIZE)
    return QSPI_ERROR;
  if ((start_adr + size) >= N25Q128A_FLASH_SIZE)
    return QSPI_ERROR;
  if (size == 0)
    return QSPI_ERROR;

  // Initialize the read command
  s_command.InstructionMode = QSPI_INSTRUCTION_1_LINE;
  s_command.Instruction = QUAD_INOUT_FAST_READ_CMD;
  s_command.AddressMode = QSPI_ADDRESS_4_LINES;
  s_command.AddressSize = QSPI_ADDRESS_24_BITS;
  s_command.Address = start_adr;
  s_command.AlternateByteMode = QSPI_ALTERNATE_BYTES_NONE;
  s_command.DataMode = QSPI_DATA_4_LINES;
  s_command.DummyCycles = N25Q128A_DUMMY_CYCLES_READ_QUAD;
  s_command.NbData = size;
  s_command.DdrMode = QSPI_DDR_MODE_DISABLE;
  s_command.DdrHoldHalfCycle = QSPI_DDR_HHC_ANALOG_DELAY;
  s_command.SIOOMode = QSPI_SIOO_INST_EVERY_CMD;

  // Configure the command
  if (HAL_QSPI_Command(&QSPIHandle, &s_command,
                       HAL_QPSI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
    return QSPI_ERROR;

  // Reception of the data
  if (HAL_QSPI_Receive(&QSPIHandle, data_buf, HAL_QPSI_TIMEOUT_DEFAULT_VALUE) !=
      HAL_OK)
    return QSPI_ERROR;

  return QSPI_OK;
}

//--------------------------------------------------------------
// schreiben von einem 8bit Datenblock ins QFlash
// start_adr : start adresse in die geschrieben wird
// size      : anzahl der daten die geschrieben werden
// data_buf  : pointer zu einem Puffer der Daten
//
// return : QSPI_OK, wenn alles ok
//--------------------------------------------------------------
uint8_t UB_QFlash_Write_Block8b(uint32_t start_adr, uint32_t size,
                                uint8_t *data_buf) {
  QSPI_CommandTypeDef s_command;
  uint32_t end_addr, current_size, current_addr;

  if (start_adr >= N25Q128A_FLASH_SIZE)
    return QSPI_ERROR;
  if ((start_adr + size) >= N25Q128A_FLASH_SIZE)
    return QSPI_ERROR;
  if (size == 0)
    return QSPI_ERROR;

  // calc size
  current_addr = 0;
  while (current_addr <= start_adr) {
    current_addr += N25Q128A_PAGE_SIZE;
  }
  current_size = current_addr - start_adr;

  // set variables
  if (current_size > size)
    current_size = size;
  current_addr = start_adr;
  end_addr = start_adr + size;

  // Initialize the program command
  s_command.InstructionMode = QSPI_INSTRUCTION_1_LINE;
  s_command.Instruction = EXT_QUAD_IN_FAST_PROG_CMD;
  s_command.AddressMode = QSPI_ADDRESS_4_LINES;
  s_command.AddressSize = QSPI_ADDRESS_24_BITS;
  s_command.AlternateByteMode = QSPI_ALTERNATE_BYTES_NONE;
  s_command.DataMode = QSPI_DATA_4_LINES;
  s_command.DummyCycles = 0;
  s_command.DdrMode = QSPI_DDR_MODE_DISABLE;
  s_command.DdrHoldHalfCycle = QSPI_DDR_HHC_ANALOG_DELAY;
  s_command.SIOOMode = QSPI_SIOO_INST_EVERY_CMD;

  // write all pages
  do {
    s_command.Address = current_addr;
    s_command.NbData = current_size;

    // Enable write operations
    if (P_QSPI_WriteEnable(&QSPIHandle) != QSPI_OK)
      return QSPI_ERROR;

    // Configure the command
    if (HAL_QSPI_Command(&QSPIHandle, &s_command,
                         HAL_QPSI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
      return QSPI_ERROR;

    // Transmission of the data
    if (HAL_QSPI_Transmit(&QSPIHandle, data_buf,
                          HAL_QPSI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
      return QSPI_ERROR;

    // Configure automatic polling mode to wait for end of program
    if (P_QSPI_AutoPollingMemReady(&QSPIHandle,
                                   HAL_QPSI_TIMEOUT_DEFAULT_VALUE) != QSPI_OK)
      return QSPI_ERROR;

    // Update the address and size variables for next page programming
    current_addr += current_size;
    data_buf += current_size;
    current_size = ((current_addr + N25Q128A_PAGE_SIZE) > end_addr)
                       ? (end_addr - current_addr)
                       : N25Q128A_PAGE_SIZE;
  } while (current_addr < end_addr);

  return QSPI_OK;
}

//--------------------------------------------------------------
// interne Funktion
//--------------------------------------------------------------
__weak void P_QSPI_MspInit(QSPI_HandleTypeDef *hqspi, void *Params) {
  GPIO_InitTypeDef gpio_init_structure;

  // qspi clock enable
  __HAL_RCC_QSPI_CLK_ENABLE();

  // qspi reset
  __HAL_RCC_QSPI_FORCE_RESET();
  __HAL_RCC_QSPI_RELEASE_RESET();

  // gpio clock enable
  UB_System_ClockEnable(QSPI_CS_GPIO_PORT);
  UB_System_ClockEnable(QSPI_CLK_GPIO_PORT);
  UB_System_ClockEnable(QSPI_D0_GPIO_PORT);
  UB_System_ClockEnable(QSPI_D1_GPIO_PORT);
  UB_System_ClockEnable(QSPI_D2_GPIO_PORT);
  UB_System_ClockEnable(QSPI_D3_GPIO_PORT);

  // QSPI CS GPIO pin configuration
  gpio_init_structure.Pin = QSPI_CS_PIN;
  gpio_init_structure.Mode = GPIO_MODE_AF_PP;
  gpio_init_structure.Pull = GPIO_PULLUP;
  gpio_init_structure.Speed = GPIO_SPEED_HIGH;
  gpio_init_structure.Alternate = GPIO_AF10_QUADSPI;
  HAL_GPIO_Init(QSPI_CS_GPIO_PORT, &gpio_init_structure);

  // QSPI CLK GPIO pin configuration
  gpio_init_structure.Pin = QSPI_CLK_PIN;
  gpio_init_structure.Pull = GPIO_NOPULL;
  gpio_init_structure.Alternate = GPIO_AF9_QUADSPI;
  HAL_GPIO_Init(QSPI_CLK_GPIO_PORT, &gpio_init_structure);

  // QSPI D0 GPIO pin configuration
  gpio_init_structure.Pin = QSPI_D0_PIN;
  gpio_init_structure.Alternate = GPIO_AF9_QUADSPI;
  HAL_GPIO_Init(QSPI_D0_GPIO_PORT, &gpio_init_structure);

  // QSPI D1 GPIO pin configuration
  gpio_init_structure.Pin = QSPI_D1_PIN;
  gpio_init_structure.Alternate = GPIO_AF9_QUADSPI;
  HAL_GPIO_Init(QSPI_D1_GPIO_PORT, &gpio_init_structure);

  // QSPI D2 GPIO pin configuration
  gpio_init_structure.Pin = QSPI_D2_PIN;
  gpio_init_structure.Alternate = GPIO_AF9_QUADSPI;
  HAL_GPIO_Init(QSPI_D2_GPIO_PORT, &gpio_init_structure);

  // QSPI D3 GPIO pin configuration
  gpio_init_structure.Pin = QSPI_D3_PIN;
  gpio_init_structure.Alternate = GPIO_AF9_QUADSPI;
  HAL_GPIO_Init(QSPI_D3_GPIO_PORT, &gpio_init_structure);

  // NVIC configuration for QSPI interrupt
  HAL_NVIC_SetPriority(QUADSPI_IRQn, 0x0F, 0);
  HAL_NVIC_EnableIRQ(QUADSPI_IRQn);
}

//--------------------------------------------------------------
// interne Funktion
//--------------------------------------------------------------
static uint8_t P_QSPI_ResetMemory(QSPI_HandleTypeDef *hqspi) {
  QSPI_CommandTypeDef s_command;

  // Initialize the reset enable command
  s_command.InstructionMode = QSPI_INSTRUCTION_1_LINE;
  s_command.Instruction = RESET_ENABLE_CMD;
  s_command.AddressMode = QSPI_ADDRESS_NONE;
  s_command.AlternateByteMode = QSPI_ALTERNATE_BYTES_NONE;
  s_command.DataMode = QSPI_DATA_NONE;
  s_command.DummyCycles = 0;
  s_command.DdrMode = QSPI_DDR_MODE_DISABLE;
  s_command.DdrHoldHalfCycle = QSPI_DDR_HHC_ANALOG_DELAY;
  s_command.SIOOMode = QSPI_SIOO_INST_EVERY_CMD;

  // Send the command
  if (HAL_QSPI_Command(hqspi, &s_command, HAL_QPSI_TIMEOUT_DEFAULT_VALUE) !=
      HAL_OK)
    return QSPI_ERROR;

  // Send the reset memory command
  s_command.Instruction = RESET_MEMORY_CMD;
  if (HAL_QSPI_Command(hqspi, &s_command, HAL_QPSI_TIMEOUT_DEFAULT_VALUE) !=
      HAL_OK)
    return QSPI_ERROR;

  // Configure automatic polling mode to wait the memory is ready
  if (P_QSPI_AutoPollingMemReady(hqspi, HAL_QPSI_TIMEOUT_DEFAULT_VALUE) !=
      QSPI_OK)
    return QSPI_ERROR;

  return QSPI_OK;
}

//--------------------------------------------------------------
// interne Funktion
//--------------------------------------------------------------
static uint8_t P_QSPI_DummyCyclesCfg(QSPI_HandleTypeDef *hqspi) {
  QSPI_CommandTypeDef s_command;
  uint8_t reg;

  // Initialize the read volatile configuration register command
  s_command.InstructionMode = QSPI_INSTRUCTION_1_LINE;
  s_command.Instruction = READ_VOL_CFG_REG_CMD;
  s_command.AddressMode = QSPI_ADDRESS_NONE;
  s_command.AlternateByteMode = QSPI_ALTERNATE_BYTES_NONE;
  s_command.DataMode = QSPI_DATA_1_LINE;
  s_command.DummyCycles = 0;
  s_command.NbData = 1;
  s_command.DdrMode = QSPI_DDR_MODE_DISABLE;
  s_command.DdrHoldHalfCycle = QSPI_DDR_HHC_ANALOG_DELAY;
  s_command.SIOOMode = QSPI_SIOO_INST_EVERY_CMD;

  // Configure the command
  if (HAL_QSPI_Command(hqspi, &s_command, HAL_QPSI_TIMEOUT_DEFAULT_VALUE) !=
      HAL_OK)
    return QSPI_ERROR;

  // Reception of the data
  if (HAL_QSPI_Receive(hqspi, &reg, HAL_QPSI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
    return QSPI_ERROR;

  // Enable write operations
  if (P_QSPI_WriteEnable(hqspi) != QSPI_OK)
    return QSPI_ERROR;

  // Update volatile configuration register (with new dummy cycles)
  s_command.Instruction = WRITE_VOL_CFG_REG_CMD;
  MODIFY_REG(
      reg, N25Q128A_VCR_NB_DUMMY,
      (N25Q128A_DUMMY_CYCLES_READ_QUAD << POSITION_VAL(N25Q128A_VCR_NB_DUMMY)));

  // Configure the write volatile configuration register command
  if (HAL_QSPI_Command(hqspi, &s_command, HAL_QPSI_TIMEOUT_DEFAULT_VALUE) !=
      HAL_OK)
    return QSPI_ERROR;

  // Transmission of the data
  if (HAL_QSPI_Transmit(hqspi, &reg, HAL_QPSI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
    return QSPI_ERROR;

  return QSPI_OK;
}

//--------------------------------------------------------------
// interne Funktion
//--------------------------------------------------------------
static uint8_t P_QSPI_AutoPollingMemReady(QSPI_HandleTypeDef *hqspi,
                                          uint32_t Timeout) {
  QSPI_CommandTypeDef s_command;
  QSPI_AutoPollingTypeDef s_config;

  // Configure automatic polling mode to wait for memory ready
  s_command.InstructionMode = QSPI_INSTRUCTION_1_LINE;
  s_command.Instruction = READ_STATUS_REG_CMD;
  s_command.AddressMode = QSPI_ADDRESS_NONE;
  s_command.AlternateByteMode = QSPI_ALTERNATE_BYTES_NONE;
  s_command.DataMode = QSPI_DATA_1_LINE;
  s_command.DummyCycles = 0;
  s_command.DdrMode = QSPI_DDR_MODE_DISABLE;
  s_command.DdrHoldHalfCycle = QSPI_DDR_HHC_ANALOG_DELAY;
  s_command.SIOOMode = QSPI_SIOO_INST_EVERY_CMD;

  s_config.Match = 0;
  s_config.Mask = N25Q128A_SR_WIP;
  s_config.MatchMode = QSPI_MATCH_MODE_AND;
  s_config.StatusBytesSize = 1;
  s_config.Interval = 0x10;
  s_config.AutomaticStop = QSPI_AUTOMATIC_STOP_ENABLE;

  if (HAL_QSPI_AutoPolling(hqspi, &s_command, &s_config, Timeout) != HAL_OK)
    return QSPI_ERROR;

  return QSPI_OK;
}

//--------------------------------------------------------------
// interne Funktion
//--------------------------------------------------------------
static uint8_t P_QSPI_WriteEnable(QSPI_HandleTypeDef *hqspi) {
  QSPI_CommandTypeDef s_command;
  QSPI_AutoPollingTypeDef s_config;

  // Enable write operations
  s_command.InstructionMode = QSPI_INSTRUCTION_1_LINE;
  s_command.Instruction = WRITE_ENABLE_CMD;
  s_command.AddressMode = QSPI_ADDRESS_NONE;
  s_command.AlternateByteMode = QSPI_ALTERNATE_BYTES_NONE;
  s_command.DataMode = QSPI_DATA_NONE;
  s_command.DummyCycles = 0;
  s_command.DdrMode = QSPI_DDR_MODE_DISABLE;
  s_command.DdrHoldHalfCycle = QSPI_DDR_HHC_ANALOG_DELAY;
  s_command.SIOOMode = QSPI_SIOO_INST_EVERY_CMD;

  if (HAL_QSPI_Command(hqspi, &s_command, HAL_QPSI_TIMEOUT_DEFAULT_VALUE) !=
      HAL_OK)
    return QSPI_ERROR;

  // Configure automatic polling mode to wait for write enabling
  s_config.Match = N25Q128A_SR_WREN;
  s_config.Mask = N25Q128A_SR_WREN;
  s_config.MatchMode = QSPI_MATCH_MODE_AND;
  s_config.StatusBytesSize = 1;
  s_config.Interval = 0x10;
  s_config.AutomaticStop = QSPI_AUTOMATIC_STOP_ENABLE;

  s_command.Instruction = READ_STATUS_REG_CMD;
  s_command.DataMode = QSPI_DATA_1_LINE;

  if (HAL_QSPI_AutoPolling(hqspi, &s_command, &s_config,
                           HAL_QPSI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
    return QSPI_ERROR;

  return QSPI_OK;
}
