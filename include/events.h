/**
 * @file event.h
 * @author Thomas Keilbach | keiltronic GmbH
 * @date 27 Oct 2022
 * @brief This file contains functions headers to communicate with the pheripherals
 * @version 1.0.0
 */

#ifndef EVENTS_H
#define EVENTS_H

#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <stdlib.h>
#include "rtc.h"
#include "aletheia.pb-c.h"
#include "parameter_mem.h"
#include "notification.h"
#include "cloud.h"
#include "epc_mem.h"
#include "event_mem.h"

#define EVENT_MAX_ITEMS_IN_ARRAY 100

extern EventArray__EventArrayEntry my_event_array_entries[EVENT_MAX_ITEMS_IN_ARRAY];
extern EventArray myEventArray;
extern ProtobufCBinaryData binary_data;
extern uint8_t current_location_epc_string[20];
extern uint32_t Event_ItemsInArray;
extern bool event_simulation_in_progress;
extern uint16_t event1statistics_interval_timer;

/* Event array function*/
extern void Event_ClearArray(void);
extern void Event_PrintPreviousEvents(void);
extern uint8_t Event_filter(GenericEvent *NewEvent);
extern void Event_BackwardsTimeStampUpdate(time_t initial_unixtime_ms, uint32_t ms_since_boot);
extern void Event_AddInArray(GenericEvent *NewEvent);

/* Each possible event */
extern void NewEvent0x01(time_t interval_start, time_t interval_end, float pattern_idle, float pattern_moving, float pattern_mopping);                                            // 0x01 - New movement detected
extern void NewEvent0x02(uint32_t mop_id);
extern void NewEvent0x04(uint32_t room_id);                                                                                                                                       // 0x04 - Another room detecteds
extern void NewEvent0x05(uint32_t room_id, uint32_t mop_id);                                                                                                                      // 0x05 - Wrong mop is used in the room
extern void NewEvent0x06(uint32_t room_id, uint32_t mop_id);                                                                                                                      // 0x06 - Same mop used in another room
extern void NewEvent0x07(uint8_t *location_epc, uint8_t epc_len);                                                                                                                 // 0x07 - New location detected
extern void NewEvent0x08(uint32_t notification_id, uint64_t notification_ttl, uint8_t notficiation_pattern_id, uint8_t notification_led_power, uint8_t notification_sound_power); // From Hub to device -> notifications are only used to activate LED and / or sound on the Device remotely from the Hub
extern void NewEvent0x09(void);                                                                                                                                                   // 0x09 - Battery charge low
extern void NewEvent0x0B(void);                                                                                                                                                   // 0x0B - Connection up
extern void NewEvent0x0C(void);                                                                                                                                                   // 0x0C - Connection down
extern void NewEvent0x0D(void);                                                                                                                                                   // 0x0D - Button pressed
extern void NewEvent0x0F(void);                                                                                                                                                   // 0x0F - USB plugged in
extern void NewEvent0x10(void);                                                                                                                                                   // 0x10 - USB plugged off
extern void NewEvent0x11(void);                                                                                                                                                   // 0x11 - Charging started
extern void NewEvent0x12(void);                                                                                                                                                   // 0x12 - Charging stopped
extern void NewEvent0x13(void);                                                                                                                                                   // 0x13 - Power on
extern void NewEvent0x17(uint32_t schock_acc);                                                                                                                                    // 0x17 - Hit shock
extern void NewEvent0x18(uint32_t frame_side);                                                                                                                                    // 0x18 - Frame side flip
extern void NewEvent0x19(uint32_t room_id, uint32_t mop_id);
extern void NewEvent0x1A(uint32_t room_id, uint32_t mop_id);
extern void NewEvent0x1B(void);
extern void NewEvent0x1C(uint32_t mop_id, float sqm_side_0, float sqm_side_1);
extern void NewEvent0xFF(uint32_t msg_id, uint8_t *msg, uint16_t len);

#endif