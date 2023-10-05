/**
 * @file commands.h
 * @author Thomas Keilbach | keiltronic GmbH
 * @date 27 Oct 2022
 * @brief This file contains functions headers to communicate with the pheripherals
 * @version 1.0.0
 */

#ifndef COMMANDS_H
#define COMMANDS_H

#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <zephyr/shell/shell.h>
#include <zephyr/shell/shell_uart.h>
#include <string.h>
#include <stdlib.h>
#include <nrf9160.h>
#include <zephyr/sys/reboot.h>
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
#include <nrfx_twi_twim.h>
#include "test.h"
#include "event_mem.h"

extern bool trace_acc_switch;
extern bool trace_flash;
//extern struct device *gpio_dev;

extern void command_init(void);
extern void factorysettings(void);

#endif