/**
 * @file test.h
 * @author Thomas Keilbach | keiltronic GmbH
 * @date 27 Oct 2022
 * @brief This file contains functions headers to communicate with the pheripherals
 * @version 1.0.0
 */


#ifndef TEST_H
#define TEST_H

#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <string.h>
#include <stdlib.h>
#include <nrf9160.h>
#include <nrfx_twi_twim.h>
#include "imu.h"
#include "flash.h"
#include "device_mem.h"
#include "spi.h"
#include "i2c.h"
#include "rfid.h"
#include "datalog_mem.h"
#include "buzzer.h"
#include "notification.h"
#include "battery_gauge.h"
#include "rtc.h"
#include "epc_mem.h"
#include "adc.h"
#include "threads.h"
#include "events.h"
#include "aletheia.pb-c.h"
#include "cloud.h"
#include "watchdog.h"
#include "modem.h"
#include "coap.h"
#include "test.h"
#include "algorithms.h"

extern uint8_t test_pcb(void);
extern uint8_t suppress_rfid_command_charcaters;
extern uint8_t rfid_ok;
extern uint8_t pcb_test_is_running;

#endif
