/**
 * @file device_mem.c
 * @author Thomas Keilbach | keiltronic GmbH
 * @date 05 Oct 2023
 * @brief This file contains function to write and read device specific data to and from the external flash memory
 * @version 2.0.0
 */

/*!
 * @defgroup Memory
 * @brief This file contains function to write and read device specific data to and from the external flash memory
 * @{*/

#include "device_mem.h"

DEVICE Device;

uint16_t Device_StructSizeInByte = 0;
uint16_t Device_StructMemberQuantity = 0;

/*!
 * @brief This functions initalize a structure with device data in RAM.
 */
void Device_InitRAM(void)
{
  Device_StructSizeInByte = sizeof(DEVICE);
  Device_StructMemberQuantity = Device_StructSizeInByte / sizeof(uint32_t);

  Device.FirmwareMajorVersion = 2UL;
  Device.FirmwareMinorVersion = 5UL;
  Device.FirmwareInternVersion = 4UL;
  Device.HardwareMajorVersion = 2UL;
  Device.HardwareMinorVersion = 3UL;
  Device.OpertingTime = 0UL;
  Device.ChargeCycles = 0UL;
}

/*!
 * @brief This functions saves the current device structure in RAM to external flash memory.
 */
void Device_PushRAMToFlash(void) {
 // flash_ClearBlock_4kB(GPIO_PIN_FLASH_CS2, DEVICE_MEM, DEVICE_MEM_LENGTH);
  //flash_write(GPIO_PIN_FLASH_CS2, DEVICE_MEM, &Device.device_mem_bytes[0], DEVICE_MEM_LENGTH_RAM);
}

/*!
 * @brief This functions reads the the stored device structure from external flash memory to the RAM.
 */
void Device_PopFlashToRAM(void) {
//  flash_read(GPIO_PIN_FLASH_CS2, DEVICE_MEM, &Device.device_mem_bytes[0], DEVICE_MEM_LENGTH_RAM);
}

/*!
 * @brief This functions returns the firmware version as an ASCII string
 * @return Pointer to char array
 */
const char *get_firmware_version_string(void)
{
  static char string[12];

  memset(string, 0, 12);
  sprintf(string, "v%d.%d.%d", Device.FirmwareMajorVersion, Device.FirmwareMinorVersion, Device.FirmwareInternVersion);

  return string;
}
