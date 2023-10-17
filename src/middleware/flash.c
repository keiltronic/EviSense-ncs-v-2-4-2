/**
 * @file flash.c
 * @author Thomas Keilbach | keiltronic GmbH
 * @date 05 Oct 2023
 * @brief This file contains functions to communicate with the pheripherals
 * @version 2.0.0
 */

/*!
 * @defgroup Peripherals
 * @brief This file contains functions to communicate with the pheripherals
 * @{*/

/* NOR flash used: Micron MT25QL256ABA8E12-0AAT TR  ---- 32MB (256Mb) 64KB sectors, 4KB and 32KB sub-sectors*/
#include "flash.h"

#define FLASH_DEVICE "MT25QL256"
#define FLASH_NAME "JEDEC SPI-NOR"
#define FLASH_TEST_REGION_OFFSET 0xff000
#define FLASH_SECTOR_SIZE 4096

void init_flash(uint8_t flash_no)
{
  /* dummy read*/
  uint8_t data = 0x00;
  flash_read(flash_no, 0x00, &data, 1);
}

uint8_t flash_access(const struct device *spi, struct spi_config *spi_cfg, uint8_t cmd, uint32_t addr, uint8_t *data, uint32_t len)
{
  uint8_t access[4] = {0, 0, 0, 0};

  struct spi_buf bufs[] = {
      {
          .buf = access,
      },
      {.buf = data,
       .len = len}};

  struct spi_buf_set tx = {
      .buffers = bufs};

  access[0] = cmd;

  if (cmd == FLASH_WRITE_CMD || cmd == FLASH_READ_CMD)
  {
    access[1] = (addr >> 16) & 0xFF;
    access[2] = (addr >> 8) & 0xFF;
    access[3] = addr & 0xFF;

    bufs[0].len = sizeof(access);
    tx.count = 2;

    if (cmd == FLASH_READ_CMD)
    {
      struct spi_buf_set rx = {
          .buffers = bufs,
          .count = 2};

      return spi_transceive(spi, spi_cfg, &tx, &rx);
    }
  }
  else
  {
    tx.count = 1;

    if (addr == 0UL && data == NULL && len == 0)
    {
      bufs[0].len = 1;
    }
  }

  return spi_write(spi, spi_cfg, &tx);
}

uint8_t flash_access_register(const struct device *spi, struct spi_config *spi_cfg, uint8_t reg, uint8_t *data, uint32_t len)
{
  uint8_t access[1];
  struct spi_buf bufs[] = {
      {
          .buf = access,
      },
      {.buf = data,
       .len = len}};

  struct spi_buf_set tx = {
      .buffers = bufs};

  struct spi_buf_set rx = {
      .buffers = bufs,
      .count = 2};

  access[0] = reg;

  tx.count = 1;
  bufs[0].len = 1;

  return spi_transceive(spi, spi_cfg, &tx, &rx);
}

void flash_WaitWhileBusy(uint8_t flash_no)
{
  uint8_t rslt = 1;

  // while (rslt & FLASH_WIP_BIT_MASK)
  // {
  //   flash_read_register(flash_no, FLASH_RDSR1, &rslt, 1);
  // }
}

void flash_WriteEnable_fast(uint8_t flash_no)
{
  // flash_WaitWhileBusy(flash_no);
  if (flash_no == FLASH1)
  {
    //flash_access(spi_dev, &spi_cfg1, FLASH_WREN, 0, NULL, 0);
  }
  else if (flash_no == FLASH2)
  {
    //flash_access(spi_dev, &spi_cfg2, FLASH_WREN, 0, NULL, 0);
  }
}

void flash_WriteEnable(uint8_t flash_no)
{
  flash_WaitWhileBusy(flash_no);
  if (flash_no == FLASH1)
  {
    //flash_access(spi_dev, &spi_cfg1, FLASH_WREN, 0, NULL, 0);
  }
  else if (flash_no == FLASH2)
  {
    //flash_access(spi_dev, &spi_cfg2, FLASH_WREN, 0, NULL, 0);
  }
}

void flash_read_register(uint8_t flash_no, uint32_t reg, uint8_t *data, uint32_t len)
{
  if (flash_no == FLASH1)
  {
    //flash_access_register(spi_dev, &spi_cfg1, reg, data, len);
  }
  else if (flash_no == FLASH2)
  {
    //flash_access_register(spi_dev, &spi_cfg2, reg, data, len);
  }
}

uint8_t acces_write_reg(const uint8_t flash_no, const struct device *spi, struct spi_config *spi_cfg, uint8_t reg, uint32_t addr)
{
  int16_t res = 0;

  flash_WriteEnable(flash_no);
  flash_WaitWhileBusy(flash_no);

  uint8_t access[4];
  struct spi_buf bufs[] = {
      {
          .buf = access,
      },
      {.buf = NULL,
       .len = 0}};

  struct spi_buf_set tx = {
      .buffers = bufs};

  struct spi_buf_set rx = {
      .buffers = bufs,
      .count = 2};

  access[0] = reg;
  access[1] = (addr >> 16) & 0xFF;
  access[2] = (addr >> 8) & 0xFF;
  access[3] = addr & 0xFF;

  tx.count = 1;
  bufs[0].len = sizeof(access);

  res = spi_transceive(spi, spi_cfg, &tx, &rx);

  return res;
}

void flash_EraseAll(const uint8_t flash_no)
{
  flash_WriteEnable(flash_no);
  flash_WaitWhileBusy(flash_no);

  if (flash_no == FLASH1)
  {
    acces_write_reg(flash_no, spi_dev, &spi_cfg1, FLASH_BE, 0);
  }
  else if (flash_no == FLASH2)
  {
    acces_write_reg(flash_no, spi_dev, &spi_cfg2, FLASH_BE, 0);
  }

  System.datalogFrameNumber = 0UL;
  setaddress_flag = false;
}

void flash_EraseSector_4kB(const uint8_t flash_no, const uint32_t addr)
{
  flash_WriteEnable(flash_no);
  flash_WaitWhileBusy(flash_no);

  if (addr < DATALOG_MEM)
  {
    rtc_print_debug_timestamp();
    shell_fprintf(shell_backend_uart_get_ptr(), SHELL_VT100_COLOR_DEFAULT, "####### WARNING: Erased 4kB sector, addr: %d ########################\n", addr);
  }

  if (flash_no == FLASH1)
  {
    acces_write_reg(flash_no, spi_dev, &spi_cfg1, FLASH_SSE_4KB, addr);
  }
  else if (flash_no == FLASH2)
  {
    acces_write_reg(flash_no, spi_dev, &spi_cfg2, FLASH_SSE_4KB, addr);
  }

  if (Parameter.debug == true || Parameter.flash_verbose == true)
  {
    rtc_print_debug_timestamp();
    shell_fprintf(shell_backend_uart_get_ptr(), SHELL_VT100_COLOR_DEFAULT, "Erased 4kB sector, addr: 0x%X\n", addr);
  }
}

void flash_EraseSector_32kB(const uint8_t flash_no, const uint32_t addr)
{
  flash_WriteEnable(flash_no);
  flash_WaitWhileBusy(flash_no);

  if (addr < DATALOG_MEM)
  {
    rtc_print_debug_timestamp();
    shell_fprintf(shell_backend_uart_get_ptr(), SHELL_VT100_COLOR_DEFAULT, "####### WARNING: Erased 32kB sector, addr: %d ########################\n", addr);
  }

  if (flash_no == FLASH1)
  {
    acces_write_reg(flash_no, spi_dev, &spi_cfg1, FLASH_SSE_32KB, addr);
  }
  else if (flash_no == FLASH2)
  {
    acces_write_reg(flash_no, spi_dev, &spi_cfg2, FLASH_SSE_32KB, addr);
  }

  if (Parameter.debug == true || Parameter.flash_verbose == true)
  {
    rtc_print_debug_timestamp();
    shell_fprintf(shell_backend_uart_get_ptr(), SHELL_VT100_COLOR_DEFAULT, "Erased 32kB sector, addr: %d\n", addr);
  }
}

void flash_EraseSector_64kB(const uint8_t flash_no, const uint32_t addr)
{
  flash_WriteEnable(flash_no);
  flash_WaitWhileBusy(flash_no);

  if (addr < DATALOG_MEM)
  {
    rtc_print_debug_timestamp();
    shell_fprintf(shell_backend_uart_get_ptr(), SHELL_VT100_COLOR_DEFAULT, "####### WARNING: Erased 64kB sector, addr: %d ########################\n", addr);
  }

  if (flash_no == FLASH1)
  {
    acces_write_reg(flash_no, spi_dev, &spi_cfg1, FLASH_SE_64KB, addr);
  }
  else if (flash_no == FLASH2)
  {
    acces_write_reg(flash_no, spi_dev, &spi_cfg2, FLASH_SE_64KB, addr);
  }

  if (Parameter.debug == true || Parameter.flash_verbose == true)
  {
    rtc_print_debug_timestamp();
    shell_fprintf(shell_backend_uart_get_ptr(), SHELL_VT100_COLOR_DEFAULT, "Erased 64kB sector, addr: %d\n", addr);
  }
}

/* Step through the sector and read one frame at the beginning of
 * the sector. If each frame components is not 0xFF, erase the sector
 */
void flash_ClearMem(const uint8_t flash_no, const uint32_t sub_sector_size, const uint32_t memory, const uint32_t length)
{
  uint32_t current_address = 0UL;
  uint32_t data = 0UL;

  for (current_address = 0UL; current_address <= (memory + length); current_address += sub_sector_size)
  {

    flash_read(flash_no, memory + current_address, &data, sizeof(data));

    if (Parameter.debug == true || Parameter.flash_verbose == true)
    {
      rtc_print_debug_timestamp();
      shell_fprintf(shell_backend_uart_get_ptr(), SHELL_VT100_COLOR_DEFAULT, "Clear addr: 0x%X, data: %ld\n", memory + current_address, data);
    }

    if (data != 0xFFFFFFFF)
    {
      flash_EraseSector_64kB(flash_no, memory + current_address);
    }
    else
    {
      if (Parameter.debug == true || Parameter.flash_verbose == true)
      {
        shell_fprintf(shell_backend_uart_get_ptr(), SHELL_VT100_COLOR_DEFAULT, "done\n");
      }
      return;
    }
  }
}

/* Step through the sector and read one frame at the beginning of
 * the sector. If each frame components is not 0xFF, erase the sector
 */
void flash_ClearBlock_64kB(const uint8_t flash_no, const uint32_t memory, const uint32_t length)
{
  uint32_t current_address = 0UL;
  uint32_t data = 0UL;

  for (current_address = 0UL; current_address <= (memory + length); current_address += 65536UL)
  {
    flash_read(flash_no, memory + current_address, &data, sizeof(data));

    if (Parameter.debug == true || Parameter.flash_verbose == true)
    {
      rtc_print_debug_timestamp();
      shell_fprintf(shell_backend_uart_get_ptr(), SHELL_VT100_COLOR_DEFAULT, "Clear addr: 0x%X, data: %ld\n", memory + current_address, data);
    }

    if (data != 0xFFFFFFFF)
    {
      flash_EraseSector_64kB(flash_no, memory + current_address);
    }
    else
    {
      if (Parameter.debug == true || Parameter.flash_verbose == true)
      {
        shell_fprintf(shell_backend_uart_get_ptr(), SHELL_VT100_COLOR_DEFAULT, "done\n");
      }
      return;
    }
  }
}

/* Step through the sector and read one frame at the beginning of
 * the sector. If each frame components is not 0xFF, erase the sector
 */
void flash_ClearBlock_4kB(const uint8_t flash_no, const uint32_t memory, const uint32_t length)
{
  uint32_t current_address = 0UL;
  uint32_t data = 0UL;

  for (current_address = 0UL; current_address <= (memory + length); current_address += 4096UL)
  {
    flash_read(flash_no, memory + current_address, &data, sizeof(data));

    if (Parameter.debug == true || Parameter.flash_verbose == true)
    {
      rtc_print_debug_timestamp();
      shell_fprintf(shell_backend_uart_get_ptr(), SHELL_VT100_COLOR_DEFAULT, "Clear addr: 0x%X, data: %ld\n", memory + current_address, data);
    }

    if (data != 0xFFFFFFFF)
    {
      flash_EraseSector_4kB(flash_no, memory + current_address);
    }
    else
    {
      if (Parameter.debug == true || Parameter.flash_verbose == true)
      {
        shell_fprintf(shell_backend_uart_get_ptr(), SHELL_VT100_COLOR_DEFAULT, "done\n");
      }
      return;
    }
  }
}

/* Step through the sector and read one frame at the beginning of
 * the sector. If each frame components is not 0xFF, erase the sector
 */
void flash_ClearMemAll(uint8_t flash_no, uint32_t memory, uint32_t length)
{
  uint32_t current_address = 0UL;

#define SAMPLE_LENGTH 16

  uint8_t flash_read_buffer[SAMPLE_LENGTH];
  float average = 0.0;
  float percentage = 0.0;
  float normalization = (100.0 / ((float)memory + (float)length));

  // rtc_print_debug_timestamp();
  // shell_fprintf(shell_backend_uart_get_ptr(), SHELL_VT100_COLOR_DEFAULT, "Formatting started, CS pin no. %d, start address: 0x%X, length: %d...\r\n", cs_pin, memory, length);

  for (current_address = 0UL; current_address <= (memory + length); current_address += FLASH_SECTOR_SIZE)
  {
    wdt_reset();

    /* Read a couple of byte*/
    flash_read(flash_no, memory + current_address, flash_read_buffer, SAMPLE_LENGTH);

    /* Caclulage the average of the bytes read */
    for (uint8_t i = 0; i < SAMPLE_LENGTH; i++)
    {
      average += flash_read_buffer[i];
    }
    average /= SAMPLE_LENGTH;

    /* Check if byte are set to 0xFF (255) to skip sektor erase if it is already set to 0xFF*/
    if (average != 255.0)
    {
      flash_EraseSector_64kB(flash_no, memory + current_address);
    }
    average = 0.0;

    percentage = normalization * (float)(current_address + FLASH_SECTOR_SIZE);
    shell_fprintf(shell_backend_uart_get_ptr(), SHELL_VT100_COLOR_DEFAULT, "\r%3.02f%%", percentage);
  }

  shell_fprintf(shell_backend_uart_get_ptr(), SHELL_VT100_COLOR_DEFAULT, "\r");
}

void flash_write(uint8_t flash_no, uint32_t addr, uint8_t *data, uint32_t len)
{
  uint32_t length = 0; // store original length
  uint32_t offset = 0;
  int32_t rest = (int32_t)len;

  do
  {
    /* Split message in 256 byte tuples. Driver can only handle SPI message length up to 256 bytes */
    if (rest > SPI_MAX_BLOCK_LENGTH)
    {
      length = SPI_MAX_BLOCK_LENGTH;
    }
    else
    {
      length = rest;
    }

    rest -= SPI_MAX_BLOCK_LENGTH;

    if (rest < 0)
    {
      rest = 0;
    }

    /* Start SPI write procedere */
    flash_WriteEnable(flash_no);
    flash_WaitWhileBusy(flash_no);

    if (Parameter.debug == true || Parameter.flash_verbose == true)
    {
      rtc_print_debug_timestamp();
      shell_fprintf(shell_backend_uart_get_ptr(), SHELL_VT100_COLOR_RED, "Write flash addr: 0x%X, data addr: 0x%X, length: 0x%X (%d bytes), offset: 0x%X, rest: 0x%X (%d bytes)\n", (addr + offset), (data + offset), length, length, offset, rest, rest);
    }

    if (flash_no == FLASH1)
    {
      //flash_access(spi_dev, &spi_cfg1, FLASH_WRITE_CMD, (addr + (offset * sizeof(uint8_t))), (data + (offset * sizeof(uint8_t))), length);
    }
    else if (flash_no == FLASH2)
    {
      //flash_access(spi_dev, &spi_cfg2, FLASH_WRITE_CMD, (addr + (offset * sizeof(uint8_t))), (data + (offset * sizeof(uint8_t))), length);
    }

    // //! # <-
    // uint8_t read_out = 0;
    // shell_fprintf(shell_backend_uart_get_ptr(), SHELL_VT100_COLOR_YELLOW, "%02X ", *(data + (offset * sizeof(uint8_t))));
    // //! # ->

    // offset += SPI_MAX_BLOCK_LENGTH;

    // //! # <-
    // k_usleep(500);
    // uint8_t read_out = 0;
    // flash_read(cs_pin, (addr + (offset * sizeof(uint8_t))), &read_out, length);
    // shell_fprintf(shell_backend_uart_get_ptr(), SHELL_VT100_COLOR_BLUE, "0x%X: %02X, %02X", (addr + (offset * sizeof(uint8_t))),  *(data + (offset * sizeof(uint8_t))), read_out);

    // if (read_out != *(data + (offset * sizeof(uint8_t))))
    // {
    //   shell_fprintf(shell_backend_uart_get_ptr(), SHELL_VT100_COLOR_RED, " ERROR", read_out);
    // }

    // shell_fprintf(shell_backend_uart_get_ptr(), SHELL_VT100_COLOR_BLUE, "\n");
    // //! # ->
    offset += SPI_MAX_BLOCK_LENGTH;
  } while (rest > 0);
}

void flash_write_register(uint8_t flash_no, uint32_t reg, uint8_t *data, uint32_t len)
{
  flash_WriteEnable(flash_no);
  flash_WaitWhileBusy(flash_no);

  if (flash_no == FLASH1)
  {
    //flash_access_register(spi_dev, &spi_cfg1, reg, data, len);
  }
  else if (flash_no == FLASH2)
  {
    //flash_access_register(spi_dev, &spi_cfg2, reg, data, len);
  }

  if (Parameter.debug == true || Parameter.flash_verbose == true)
  {
    rtc_print_debug_timestamp();
    shell_fprintf(shell_backend_uart_get_ptr(), SHELL_VT100_COLOR_DEFAULT, "Writing %d bytes to reg: %ld with value %d\n", len, reg, *((uint8_t *)data));
  }
}

/* This read does not check the busy flag in the device memory. It is optimized for speed, but uncertain in the results */
void flash_read_fast(uint8_t flash_no, uint32_t addr, uint8_t *data, uint32_t len)
{
  // flash_read(cs_pin, addr, data, len);

  uint32_t length = 0; // store original length
  uint32_t offset = 0;
  int32_t rest = len;

  do
  {
    /* Split message in 256 byte tuples. Driver can only handle SPI message length up to 256 bytes */
    if (rest > SPI_MAX_BLOCK_LENGTH)
    {
      length = SPI_MAX_BLOCK_LENGTH;
    }
    else
    {
      length = rest;
    }

    rest -= SPI_MAX_BLOCK_LENGTH;

    if (rest < 0)
    {
      rest = 0;
    }

    flash_WriteEnable_fast(flash_no);
    // flash_WaitWhileBusy(flash_no);

    if (Parameter.debug == true || Parameter.flash_verbose == true)
    {
      rtc_print_debug_timestamp();
      shell_fprintf(shell_backend_uart_get_ptr(), SHELL_VT100_COLOR_GREEN, "Read flash addr: 0x%02X, length: %d bytes, rest: %d bytes\n", (addr + offset), length, rest);
    }

    if (flash_no == FLASH1)
    {
      //flash_access(spi_dev, &spi_cfg1, FLASH_READ_CMD, (addr + (offset * sizeof(uint8_t))), (data + (offset * sizeof(uint8_t))), length);
    }
    else if (flash_no == FLASH2)
    {
      //flash_access(spi_dev, &spi_cfg2, FLASH_READ_CMD, (addr + (offset * sizeof(uint8_t))), (data + (offset * sizeof(uint8_t))), length);
    }
    offset += SPI_MAX_BLOCK_LENGTH;

  } while (rest > 0);
}

void flash_read(uint8_t flash_no, uint32_t addr, uint8_t *data, uint32_t len)
{
  uint32_t length = 0; // store original length
  uint32_t offset = 0;
  int32_t rest = len;

  do
  {
    /* Split message in 256 byte tuples. Driver can only handle SPI message length up to 256 bytes */
    if (rest > SPI_MAX_BLOCK_LENGTH)
    {
      length = SPI_MAX_BLOCK_LENGTH;
    }
    else
    {
      length = rest;
    }

    rest -= SPI_MAX_BLOCK_LENGTH;

    if (rest < 0)
    {
      rest = 0;
    }

    flash_WriteEnable(flash_no);
    flash_WaitWhileBusy(flash_no);

    if (Parameter.debug == true || Parameter.flash_verbose == true)
    {
      rtc_print_debug_timestamp();
      shell_fprintf(shell_backend_uart_get_ptr(), SHELL_VT100_COLOR_GREEN, "Read flash addr: 0x%02X, length: %d bytes, rest: %d bytes\n", (addr + offset), length, rest);
    }  
    
    if (flash_no == FLASH1)
    {
   //   //flash_access(spi_dev, &spi_cfg1, FLASH_READ_CMD, (addr + (offset * sizeof(uint8_t))), (data + (offset * sizeof(uint8_t))), length);
    }
    else if (flash_no == FLASH2)
    {
   //   //flash_access(spi_dev, &spi_cfg2, FLASH_READ_CMD, (addr + (offset * sizeof(uint8_t))), (data + (offset * sizeof(uint8_t))), length);
    }

    offset += SPI_MAX_BLOCK_LENGTH;

  } while (rest > 0);
}

uint8_t flash_ValidateDataInMemory(uint8_t flash_no, uint32_t offset, uint32_t addr, uint16_t accumulation_length, uint8_t clear_value)
{
  uint16_t i;
  uint8_t frame[accumulation_length];
  uint32_t sum = 0;

  uint32_t sum_clear_val = clear_value * accumulation_length;

  /* Get one frame of data */
  for (i = 0; i < accumulation_length; i++)
  {
    flash_read(flash_no, offset + (addr + i), &frame[i], 1);
  }

  /* Calculate cross sum */
  for (i = 0; i < accumulation_length; i++)
  {
    sum += (uint16_t)frame[i];
  }

  /* Frame has no valid data if all frame elements has the clear value */
  if (sum == sum_clear_val)
  {
    return false; // frame is clear
  }
  else
  {
    return true; // frame is not clear
  }
}

void flash_MemoryViewer(uint8_t flash_no, uint32_t start_address, uint32_t length)
{
  uint32_t i = 0UL;
  uint8_t data = 0;

  if (length > 0)
  {
    for (i = 0UL; i <= length; i++)
    {
      if ((i % 16) == 0) // Add every 16th line a line break (except at line 0)
      {
        // shell_fprintf(shell_backend_uart_get_ptr(), SHELL_VT100_COLOR_YELLOW, "\n0x%X: ", start_address + i);
        printf("\n0x%X: ", start_address + i);
      }

      flash_read(flash_no, start_address + i, &data, sizeof(data));
      // shell_fprintf(shell_backend_uart_get_ptr(), SHELL_VT100_COLOR_BLUE, "0x%02X ", data);
      printf("0x%02X ", data);
    }
    //  shell_fprintf(shell_backend_uart_get_ptr(), SHELL_VT100_COLOR_BLUE, "\n");
    printf("\n");
  }
}

uint8_t flash_CommunicationTest(uint8_t flash_no)
{
  uint8_t flash_ReadByteOut = 0;
  uint8_t test_pattern = 0x55;

  /* Clear test memory */
  flash_EraseSector_4kB(flash_no, PARAMETER_MEM);

  /* Write test data (checkerboard) */
  uint16_t i = 0;
  for (i = 0; i <= FLASH_CHECKERBOARD_SIZE; i++)
  {
    flash_write(flash_no, PARAMETER_MEM + i, &test_pattern, sizeof(test_pattern));
  }

  /* Verify checkerboard test data */
  for (i = 0; i <= FLASH_CHECKERBOARD_SIZE; i++)
  {
    flash_read(flash_no, PARAMETER_MEM, &flash_ReadByteOut, sizeof(flash_ReadByteOut));
    if (flash_ReadByteOut != 0x55)
    {
      flash_ReadByteOut = 0;
      return false;
    }
    flash_ReadByteOut = 0;
  }
  return true;
}

uint32_t flash_GetLastFrameNumber(const uint8_t flash_no, const uint32_t sub_sector_size, const uint32_t start_address, const uint32_t memory_length, const uint32_t frame_length)
{
  uint32_t FrameNumber = 0UL;
  uint32_t frames_per_sector = 0UL;
  uint32_t addr = 0UL;
  uint32_t data = 0UL;
  uint32_t i = 0UL;
  uint32_t rslt = 0;

  /* Disable data logging while SPI hardware is used for read out flash memory */
  datalog_EnableFlag = false;

  /* Caclulate how much frames fits into one sector */
  frames_per_sector = sub_sector_size / frame_length;

  /* Fast step throuh sector to accelerate finding last frame number */
  for (i = 0UL; i < memory_length; i += sub_sector_size)
  {
    addr = start_address + i;

    flash_read(flash_no, addr, &data, sizeof(data));

    if (addr >= 0xFFFFFF)
    {
      data += 131072;
    }

    if (i > 0)
    {
      rslt = (i / sub_sector_size) * frames_per_sector;

      if (data == rslt)
      {
        FrameNumber += frames_per_sector;
      }
      else
      {
        addr -= sub_sector_size;
        break;
      }
    }
  }

  /* Within sector search for the latest frame number which has a valid number (and not the default flash erase state -> 0xFFFFFFFF)*/
  for (i = 0UL; i < sub_sector_size; i++)
  {
    flash_read(flash_no, (addr + (i * frame_length)), &data, sizeof(data));

    if (data == FrameNumber)
    {
      FrameNumber++;
    }
    else
    {
      //   shell_print(shell_backend_uart_get_ptr(), "Address: %d, Frame: %d, data: %d", (uint32_t)addr, FrameNumber, data);
      break;
    }
  }

  /* Enable data logging again if it is activated */
  if (Parameter.datalogEnable == true && datalog_MemoryFull == false)
  {
    datalog_EnableFlag = true;
  }
  else
  {
    datalog_EnableFlag = false;
  }

  return FrameNumber;
}
