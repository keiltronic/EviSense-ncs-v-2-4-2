/**
 * @file button.h
 * @author Thomas Keilbach | keiltronic GmbH
 * @date 04 Oct 2023
 * @brief This file contains functions headers to communicate with the pheripherals
 * @version 2.0.0
 */

#ifndef BUTTONS_H
#define BUTTONS_H

#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <hal/nrf_regulators.h>
#include "algorithms.h"
#include "battery_gauge.h"
#include "events.h"
#include "hibernate.h"
#include "interrupts.h"
#include "led.h"
#include "hard_reset.h"

#define BATTERY_LEVEL_TIME 1000 // ms
#define HIBERNATE_TIME 5000 // ms

extern struct gpio_dt_spec button0;
extern struct gpio_dt_spec button1;

extern struct gpio_callback button0_cb_data;
extern struct gpio_callback button1_cb_data;

extern void button_init(void);
extern void button_monitor(void);
extern void button0_pressed_cb(const struct device *dev, struct gpio_callback *cb, uint32_t pins);
extern void button1_pressed_cb(const struct device *dev, struct gpio_callback *cb, uint32_t pins);

#endif