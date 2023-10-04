/**
 * @file system_mem.h
 * @author Thomas Keilbach | keiltronic GmbH
 * @date 27 Oct 2022
 * @brief This file contains functions headers to communicate with the pheripherals
 * @version 1.0.0
 */

#ifndef SYSTEM_H
#define SYSTEM_H

#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include "flash.h"

/* Status word 1 - input bits */
#define STATUSFLAG_MD 1      // 2^0    Motion detected
#define STATUSFLAG_FS 2      // 2^1    Frame side
#define STATUSFLAG_HS 4      // 2^2    Hit shock detected
#define STATUSFLAG_RM 8      // 2^3    New VMop RFID reading
#define STATUSFLAG_RW 16     // 2^4    New Wall RFID reading
#define STATUSFLAG_RT 32     // 2^5    New Trolley RFID reading
#define STATUSFLAG_RO 64     // 2^6    New Other RFID reading
#define STATUSFLAG_HMP 128   // 2^7    handle_in_mopping_position
#define STATUSFLAG_B2 256    // 2^8    Button2
#define STATUSFLAG_UB 512    // 2^9    UserBtn
#define STATUSFLAG_FFF 1024  // 2^10   Frame_flip_flag
#define STATUSFLAG_MF 2048   // 2^11   MoppingFlag
#define STATUSFLAG_MCF 4096  // 2^12   MopChangeFlag
#define STATUSFLAG_USB 8192  // 2^13   USB connected
#define STATUSFLAG_CHG 16384 // 2^14   Battery Charging
#define STATUSFLAG_FLF 32768 // 2^15   frame_lift_flag

/* Status word 2 - output bits */
#define STATUSFLAG_LB 1        // 2^0    Low battery
#define STATUSFLAG_UR 2        // 2^1    User reward
#define STATUSFLAG_IC 4        // 2^2    Internet connectivity
#define STATUSFLAG_BZ 8        // 2^3    Buzzer
#define STATUSFLAG_LEDR 16     // 2^4    led red
#define STATUSFLAG_LEDG 32     // 2^5    led green
#define STATUSFLAG_LEDB 64     // 2^6    led blue
#define STATUSFLAG_RES2 128    // 2^7    reserved
#define STATUSFLAG_RES3 256    // 2^8    reserved
#define STATUSFLAG_RES4 512    // 2^9    reserved
#define STATUSFLAG_RES5 1024   // 2^10   reserved
#define STATUSFLAG_RES6 2048   // 2^11   reserved
#define STATUSFLAG_RES7 4096   // 2^12   reserved
#define STATUSFLAG_RES8 8192   // 2^13   reserved
#define STATUSFLAG_RES9 16384  // 2^14   reserved
#define STATUSFLAG_RES10 32768 // 2^15   reserved

typedef struct
{
    volatile uint16_t Steps;
    volatile uint16_t StatusInputs;
    volatile uint16_t StatusOutputs;
    volatile uint16_t StatusErrors;
    volatile uint16_t TotalSteps;
    volatile uint8_t ChargeStatus;
    volatile uint8_t ChargeStatus_old;
    volatile uint16_t TemperatureDigit;
    volatile float Temperature;
    volatile uint16_t ConfID;
    volatile uint16_t Logtime;
    volatile uint16_t TotalMoppingTime;
    volatile uint16_t AvgMoppingSpeed;
    volatile uint16_t TotalVMopRFIDs;
    volatile uint16_t TotalRoomRFIDs;
    volatile uint16_t TotalOtherRFIDs;
    volatile uint16_t TotalCoverageHandle;
    volatile uint16_t AvgCoverageMop;
    volatile uint16_t AvgdBmVmops;
    volatile uint16_t AvgdBmWalls;
    volatile uint16_t TotalRooms;
    volatile uint16_t AVGRoomCleanTime;
    volatile uint16_t charger_connected;
    volatile uint16_t charger_connected_old;
    volatile uint16_t charger_connected_trigger;
    volatile uint16_t StatusInputs_old;
    volatile uint16_t StatusOutputs_old;
    volatile uint16_t StatusErrors_old;
    volatile uint32_t datalog_StartFrame;
    volatile uint32_t datalog_EndFrame;
    volatile uint8_t RFID_TransparentMode;
    volatile uint32_t datalogFrameNumber;
    volatile uint32_t MillisecSinceBoot;
    volatile uint8_t RFID_Sniff;
    volatile uint32_t MillisecSincecharger_connected;
    volatile uint32_t MillisecSinceUSBDisconnected;
    volatile uint32_t EventNumber;
    volatile uint8_t boot_complete;
} SYSTEM;

extern volatile SYSTEM System;

extern uint32_t reset_reason;

extern void System_InitRAM(void);
extern void System_CheckStatusFlagChange(void);
extern uint32_t memcheck_heap_freespace(void);
#endif