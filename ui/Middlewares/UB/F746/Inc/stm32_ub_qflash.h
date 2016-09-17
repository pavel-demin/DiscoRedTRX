//--------------------------------------------------------------
// File     : stm32_ub_qflash.h
//--------------------------------------------------------------

//--------------------------------------------------------------
#ifndef __STM32F7_UB_QFLASH_H
#define __STM32F7_UB_QFLASH_H

//--------------------------------------------------------------
// Includes
//--------------------------------------------------------------
#include "stm32_ub_system.h"

//--------------------------------------------------------------
// Pinbelegung
//--------------------------------------------------------------
#define QSPI_CS_PIN GPIO_PIN_6
#define QSPI_CS_GPIO_PORT GPIOB

#define QSPI_CLK_PIN GPIO_PIN_2
#define QSPI_CLK_GPIO_PORT GPIOB

#define QSPI_D0_PIN GPIO_PIN_11
#define QSPI_D0_GPIO_PORT GPIOD

#define QSPI_D1_PIN GPIO_PIN_12
#define QSPI_D1_GPIO_PORT GPIOD

#define QSPI_D2_PIN GPIO_PIN_2
#define QSPI_D2_GPIO_PORT GPIOE

#define QSPI_D3_PIN GPIO_PIN_13
#define QSPI_D3_GPIO_PORT GPIOD

//--------------------------------------------------------------
#define QSPI_OK ((uint8_t)0x00)
#define QSPI_ERROR ((uint8_t)0x01)
#define QSPI_BUSY ((uint8_t)0x02)
#define QSPI_NOT_SUPPORTED ((uint8_t)0x04)
#define QSPI_SUSPENDED ((uint8_t)0x08)

//--------------------------------------------------------------
// defines vom Quad-Flash
//--------------------------------------------------------------
#define N25Q128A_FLASH_SIZE 0x1000000  // 128 MBits => 16MBytes
#define N25Q128A_SECTOR_SIZE 0x10000   // 256 sectors of 64KBytes
#define N25Q128A_SUBSECTOR_SIZE 0x1000 // 4096 subsectors of 4kBytes
#define N25Q128A_PAGE_SIZE 0x100       // 65536 pages of 256 bytes
#define N25Q128A_SUBSECTOR_CNT 4096    // 4096 subsectors

#define N25Q128A_DUMMY_CYCLES_READ_QUAD 10

#define N25Q128A_BULK_ERASE_MAX_TIME 250000
#define N25Q128A_SECTOR_ERASE_MAX_TIME 3000
#define N25Q128A_SUBSECTOR_ERASE_MAX_TIME 800

//--------------------------------------------------------------
// register
//--------------------------------------------------------------
#define N25Q128A_SR_WIP ((uint8_t)0x01)
#define N25Q128A_VCR_NB_DUMMY ((uint8_t)0xF0)
#define N25Q128A_SR_WREN ((uint8_t)0x02)

//--------------------------------------------------------------
// commands
//--------------------------------------------------------------
#define RESET_ENABLE_CMD 0x66
#define RESET_MEMORY_CMD 0x99
#define READ_VOL_CFG_REG_CMD 0x85
#define WRITE_VOL_CFG_REG_CMD 0x81
#define READ_STATUS_REG_CMD 0x05
#define WRITE_STATUS_REG_CMD 0x01
#define WRITE_ENABLE_CMD 0x06
#define WRITE_DISABLE_CMD 0x04
#define SUBSECTOR_ERASE_CMD 0x20
#define SECTOR_ERASE_CMD 0xD8
#define BULK_ERASE_CMD 0xC7
#define QUAD_INOUT_FAST_READ_CMD 0xEB
#define EXT_QUAD_IN_FAST_PROG_CMD 0x12

//--------------------------------------------------------------
// Globale Funktionen
//--------------------------------------------------------------
uint8_t UB_QFlash_Init(void);
uint8_t UB_QFlash_Erase_Complete(void);
uint8_t UB_QFlash_Erase_SubSector(uint32_t subsector_nr);
uint8_t UB_QFlash_Read_Block8b(uint32_t start_adr, uint32_t size,
                               uint8_t *data_buf);
uint8_t UB_QFlash_Write_Block8b(uint32_t start_adr, uint32_t size,
                                uint8_t *data_buf);

//--------------------------------------------------------------
#endif // __STM32F7_UB_QFLASH_H
