/**
 * @file datalog_mem.h
 * @author Thomas Keilbach | keiltronic GmbH
 * @date 19 Oct 2023
 * @brief This file contains functions headers to communicate with the pheripherals
 * @version 2.0.0
 */

#ifndef LOGDATA_MEM_H
#define LOGDATA_MEM_H

#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include "flash.h"
#include <string.h>
#include "imu.h"
#include "rfid.h"
#include "rtc.h"
#include "algorithms.h"

#define DATALOG_MEM 0x0000000UL      // Start address of memory region
#define DATALOG_MEM_LENGTH 0x7FFFFFUL // for 3-byte addressing it is 0xFFFFFFUL, in 4-byte addressing 0x1FFFFFFUL   // Lengts of memory region (multiples of 64kB sector size)
#define DATALOG_FRAME_LENGTH 128UL   // In bytes - must be a value of power of 2  (2^n)

/* Dataframe to store in external log. Frame length of 2^n (256 byte in this case) is mandatory
 * to avoid memory leaks in the external flash memory.
 */
typedef union
{
    uint8_t logging_frame[DATALOG_FRAME_LENGTH];

    struct __attribute__((packed))
    {
        uint32_t FrameNumber;        /* 4 byte */
        uint32_t unixtime;           /* 4 byte */
        uint16_t millisec;           /* 2 byte */
        uint8_t rfid_record_hex[21]; /* 21 bytes */
        uint8_t rfid_record_type;    /* 1 byte */
        int16_t raw_sens_value[9];   /* 18 bytes */
        float floor_handle_angle;    /* 4 byte */
        float frame_handle_angle;    /* 4 byte */
        float Mopping_speed;         /* 4 byte */
        float Coverage_per_Mop;      /* 4 byte */
        uint16_t Mop_cycles;         /* 2 bytes */
        uint8_t Mopping_pattern;     /* 1 byte */
        uint8_t Motion_state;        /* 1 byte */
        uint16_t Total_Steps;        /* 2 bytes */
        uint16_t Battery_voltage;    /* 2 bytes */
        uint16_t ChargeCycle;        /* 2 byte */
        uint16_t Input_status;       /* 2 bytes */
        uint16_t Output_status;      /* 2 bytes */
        uint8_t Error_status;        /* 1 byte */
    };
} LOGFRAME;

extern LOGFRAME DataFrame;

extern void datalog_StoreFrame(void);
extern void datalog_GetData(void);
extern void datalog_CleardatalogAll(void);
extern uint32_t datalog_GetLastFrameNumber(const uint8_t cs_pin);

extern uint8_t datalog_EnableFlag;
extern uint8_t datalog_MemoryFull;
extern uint8_t datalog_EraseActive;
extern uint8_t setaddress_flag;
extern uint8_t datalog_ReadOutisActive;

#endif
