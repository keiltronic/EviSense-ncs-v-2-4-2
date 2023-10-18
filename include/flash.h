/**
 * @file flash.h
 * @author Thomas Keilbach | keiltronic GmbH
 * @date 27 Oct 2022
 * @brief This file contains functions headers to communicate with the pheripherals
 * @version 1.0.0
 */

#ifndef FLASH_H
#define FLASH_H

#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <zephyr/sys/printk.h>
#include <zephyr/devicetree.h>
#include <stdio.h>
#include "gpio.h"
#include "datalog_mem.h"
#include "parameter_mem.h"
#include "commands.h"
#include "spi.h"

#define FLASH_SUBSUBSECTOR_SIZE   4096UL // Byte
#define FLASH_SUBSECTOR_SIZE      32768UL // Byte
#define FLASH_SECTOR_SIZE         65536UL // Byte
#define FLASH_CHECKERBOARD_SIZE   128
#define DUMMY_BYTE                0x00

#define SAMPLE_LENGTH             32

#define SPI_MAX_BLOCK_LENGTH      1 // byte

#define FLASH1                    1
#define FLASH2                    2

/* Register definition */
#define FLASH_RDID                0x9E
#define FLASH_MANUFACTURER_ID_CMD 0x9F
#define FLASH_READ                0x03//0x13 
#define FLASH_READ_CMD            0x03
#define FLASH_READ4BYTE           0x13
#define FLASH_FAST_READ           0x0B
#define FLASH_FAST_READ4BYTE      0x0C
#define FLASH_RDSFDP              0x5A
#define FLASH_DOFR                0x3B
#define FLASH_DOFR4BYTE           0x3C
#define FLASH_DIOFR               0xBB
#define FLASH_DIOFR4BYTE          0xBC
#define FLASH_QOFR                0x6B 
#define FLASH_QOFR4BYTE           0x6C
#define FLASH_QIOFR               0xEB
#define FLASH_QIOFR4BYTE          0xEC
#define FLASH_ROTP                0x4B
#define FLASH_WREN                0x06
#define FLASH_WRDI                0x04
#define FLASH_PP                  0x02
#define flash_write_CMD           0x02
#define FLASH_DIFP                0xA2
#define FLASH_DIEFP               0xD2
#define FLASH_QIFP                0x32
#define FLASH_QIEFP               0x12
#define FLASH_POTP                0x42
#define FLASH_SSE_4KB             0x20   // 4kB Subs Sesctor Erase
#define FLASH_SSE_32KB            0x52   // 32kB Subs Sesctor Erase
#define FLASH_SE_64KB             0xD8   // 64kB Sector Erase
#define FLASH_DE                  0xC4   // Die Erase
#define FLASH_BE                  0xC7
#define FLASH_PER                 0x7A
#define FLASH_PES                 0x75
#define FLASH_RDSR1               0x05
#define FLASH_RDSR2               0x35
#define FLASH_RDSR3               0x15
#define FLASH_WRSR1               0x01
#define FLASH_WRSR2               0x31
#define FLASH_WRSR3               0x11
#define FLASH_RDLR                0xE8
#define FLASH_WRLR                0xE5
#define FLASH_RFSR                0x70
#define FLASH_CLFSR               0x50
#define FLASH_RDNVCR              0xB5
#define FLASH_WRNVCR              0xB1
#define FLASH_RDVCR               0x85
#define FLASH_WRVCR               0x81
#define FLASH_RDVECR              0x65
#define FLASH_WRVECR              0x61
#define FLASH_EN4BYTEADDR         0xB7
#define FLASH_EX4BYTEADDR         0xE9
#define FLASH_WREAR               0xC5
#define FLASH_RDEAR               0xC8
#define FLASH_RSTEN               0x66
#define FLASH_RST                 0x99
#define FLASH_WIP_BIT_MASK        0x01
#define FLASH_WEL_BIT_MASK        0x02
#define FLASH_BP0_BIT_MASK        0x03
#define FLASH_BP1_BIT_MASK        0x04
#define FLASH_BP2_BIT_MASK        0x05
#define FLASH_TB_BIT_MASK         0x06
#define FLASH_BP3_BIT_MASK        0x07
#define FLASH_SRWD_BIT_MASK       0x08
#define FLASH_JEDEC               0x9F

 extern void init_flash(uint8_t flash_no);
// extern uint8_t flash_access(const struct device *spi, struct spi_config *spi_cfg, uint8_t cmd, uint32_t addr, uint8_t *data,uint32_t len);
// extern uint8_t flash_access_register(const struct device *spi, struct spi_config *spi_cfg, uint8_t reg, uint8_t *data,uint32_t len);
// extern void flash_write(uint8_t flash_no, uint32_t addr, uint8_t *data,uint32_t len);
// extern void flash_read(uint8_t flash_no, uint32_t addr, uint8_t *data,uint32_t len);
// extern void flash_read_fast(uint8_t flash_no, uint32_t addr, uint8_t *data,uint32_t len);
// extern void flash_write_register(uint8_t flash_no, uint32_t reg, uint8_t *data,uint32_t len);
// extern void flash_read_register(uint8_t flash_no, uint32_t reg, uint8_t *data,uint32_t len);
// extern uint8_t flash_TestAndClearBlock(const uint8_t flash_no, const uint32_t addr, const uint32_t cmp_value);
// extern uint8_t acces_write_reg(const uint8_t flash_no, const struct device *spi, struct spi_config *spi_cfg, uint8_t reg, uint32_t addr);
// extern void flash_writeRegister(uint8_t flash_no, uint8_t reg, uint8_t data);
// extern void flash_WaitWhileBusy(uint8_t flash_no);
// extern void flash_writeEnable(uint8_t flash_no);
// extern void flash_EraseAll(const uint8_t flash_no);
// extern void flash_EraseSector_4kB(const uint8_t flash_no, const uint32_t addr);
// extern void flash_EraseSector_32kB(const uint8_t flash_no, const uint32_t addr); 
// extern void flash_EraseSector_64kB(const uint8_t flash_no, const uint32_t addr) ;
// extern void flash_ClearBlock_4kB(const uint8_t flash_no, const uint32_t memory, const uint32_t length);
// extern void flash_ClearBlock_64kB(const uint8_t flash_no, const uint32_t memory, const uint32_t length);
// extern void flash_ClearMemAll(uint8_t flash_no, uint32_t memory, uint32_t length);
 extern uint32_t flash_GetLastFrameNumber(const uint8_t flash_no, const uint32_t sub_sector_size, const uint32_t start_address, const uint32_t memory_length, const uint32_t frame_length);
// extern void flash_MemoryViewer(uint8_t flash_no, uint32_t start_address, uint32_t length);
// extern uint8_t flash_CommunicationTest(uint8_t flash_no);
// extern uint8_t flash_ValidateDataInMemory(uint8_t flash_no, uint32_t offset, uint32_t addr, uint16_t accumulation_length, uint8_t clear_value);

#endif