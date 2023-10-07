/**
 * @file watchdog.h
 * @author Thomas Keilbach | keiltronic GmbH
 * @date 27 Oct 2022
 * @brief This file contains functions headers to communicate with the pheripherals
 * @version 1.0.0
 */


#ifndef WATCHDOG_H
#define WATCHDOG_H

#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <zephyr/drivers/watchdog.h>
#include <zephyr/devicetree.h>

extern void wdt_init(void);
extern void wdt_reset(void);

#endif