/**
 * @file device_mem.h
 * @author Thomas Keilbach | keiltronic GmbH
 * @date 27 Oct 2022
 * @brief This file contains functions headers to communicate with the pheripherals
 * @version 1.0.0
 */

#ifndef DEVICE_MEM_H
#define DEVICE_MEM_H

#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include "flash.h"

#define DEVICE_MEM              0x00000UL    // Start address of memory region
#define DEVICE_MEM_LENGTH       0xFFFFUL     // Lengts of memory region (multiples of 64kB sector size, here one 64k sector)
#define DEVICE_MEM_LENGTH_RAM   32UL         // In byte, must be a value of power of 2  (2^n)

typedef union {
    uint8_t device_mem_bytes[DEVICE_MEM_LENGTH_RAM];

    struct __attribute__((packed)) {
       uint32_t FirmwareMajorVersion;
       uint32_t FirmwareMinorVersion;
       uint32_t FirmwareInternVersion;
       uint32_t HardwareMajorVersion;
       uint32_t HardwareMinorVersion;
       uint32_t OpertingTime;
       uint32_t ChargeCycles;
    };
} DEVICE;

extern DEVICE Device;

extern void Device_InitRAM(void);
extern void Device_PushRAMToFlash(void);
extern void Device_PopFlashToRAM(void);
extern const char *get_firmware_version_string(void);

#endif
