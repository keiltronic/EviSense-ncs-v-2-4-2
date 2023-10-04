/**
 * @file event_mem.h
 * @author Thomas Keilbach | keiltronic GmbH
 * @date 27 Oct 2022
 * @brief This file contains functions headers to communicate with the pheripherals
 * @version 1.0.0
 */

#ifndef EVENT_MEM_H
#define EVENT_MEM_H

#include "protobuf-c.h"
#include "flash.h"
#include "cloud.h"
#include "events.h"

#define EVENT_MEM 0x800000UL        // Start address of memory region
#define EVENT_MEM_LENGTH 0x7FFFFFUL // for 3-byte addressing it is 0xFFFFFFUL, in 4-byte addressing 0x1FFFFFFUL   // Lengts of memory region (multiples of 64kB sector size)
#define EVENT_MAX_OUTSOURCED_MESSAGES 500

typedef struct __attribute__((packed))
{
    uint32_t start_address;
    uint32_t length;
} EVENT_LIST_OF_OUTSOURCED_MESSAGES;

extern void Event_ClearCompleteFlash(void);
extern void Event_StorePackedUsageObjectToFlash(void);
extern uint8_t Event_ReadFromFlash(uint32_t address, uint32_t length, uint8_t **buf);
extern uint8_t event_clearing_in_progress;

extern EVENT_LIST_OF_OUTSOURCED_MESSAGES Event_ListOfOutsourcedMessages[EVENT_MAX_OUTSOURCED_MESSAGES];
extern uint32_t Event_NumberOfOutsourcedMessages;

#endif