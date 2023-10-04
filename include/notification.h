/**
 * @file notification.h
 * @author Thomas Keilbach | keiltronic GmbH
 * @date 27 Oct 2022
 * @brief This file contains functions headers to communicate with the pheripherals
 * @version 1.0.0
 */


#ifndef NOTIFICATION_H
#define NOTIFICATION_H

#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include "led.h"
#include "buzzer.h"
#include "battery_gauge.h"

#define NOTIFICATION_IDLE 0

/* User notifications from cloud */
#define NOTIFICATION_0x01 1
#define NOTIFICATION_0x02 2
#define NOTIFICATION_0x03 3
#define NOTIFICATION_0x04 4
#define NOTIFICATION_0x05 5
#define NOTIFICATION_0x06 6
#define NOTIFICATION_0x07 7
#define NOTIFICATION_0x08 8
#define NOTIFICATION_0x09 9
#define NOTIFICATION_0x0A 10

/* User notifications from firmware */
#define NOTIFICATION_LOWBAT 20
#define NOTIFICATION_CHARGING 21
#define NOTIFICATION_CHARGING_STOPPED 22
#define NOTIFICATION_FULLY_CHARGED 23
#define NOTIFICATION_BATTERY_LEVEL 24
#define NOTIFICATION_CLEAR 25
#define NOTIFICATION_HIBERNATE 26
#define NOTIFICATION_GENERAL_RFID_CONFIRMATION 27
#define NOTIFICATION_MAX_SQM_COVERAGE_REACHED 28
#define NOTIFICATION_SIDE_MAX_SQM_COVERAGE_REACHED 29
#define NOTIFICATION_SAME_MOP_ALREADY_USED 30
#define NOTIFICATION_GENERAL_RFID_CONFIRMATION_SHORT 31

/* Notification prioritiy level */
#define NOTIFICATION_PRIORITY_LEVEL_LOWEST 0    // blue led color / idle
#define NOTIFICATION_PRIORITY_LEVEL_BLUE 0      // blue led color / idle
#define NOTIFICATION_PRIORITY_LEVEL_GREEN 3     // green led color
#define NOTIFICATION_PRIORITY_LEVEL_YELLOW 1    // yellow led color
#define NOTIFICATION_PRIORITY_LEVEL_RED 2       // red led color
#define NOTIFICATION_PRIORITY_LEVEL_WHITE 4     // white led color
#define NOTIFICATION_PRIORITY_LEVEL_CHARGING 5     // white led color
#define NOTIFICATION_PRIORITY_LEVEL_HIGHEST 10   // gloabl led off
typedef struct __attribute__((packed))
{
    int8_t current_state;
    int8_t next_state;
    int8_t current_priority;
    int8_t next_priority;
} NOTIFICATION;

extern NOTIFICATION Notification;
extern uint8_t ActionMatrixArray[255];
extern uint32_t notification_demo_delay;
extern uint32_t notification_demo_timer;
extern uint8_t notification_test_state;

extern void notification_init(void);
extern void notification_init_action_matrix(void);
extern void notification_update(void);
extern void notification_set_priority(uint8_t level);
extern void update_notification_demo(void);

#endif