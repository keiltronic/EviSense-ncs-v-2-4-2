/**
 * @file gpio.h
 * @author Thomas Keilbach | keiltronic GmbH
 * @date 06 Oct 2023
 * @brief This file contains functions headers to communicate with the pheripherals
 * @version 1.0.0
 */

#ifndef HARD_RESET_H
#define HARD_RESET_H

#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <zephyr/drivers/gpio.h>

extern struct gpio_dt_spec reset_switch;
extern void hard_reset_init(void);

#endif