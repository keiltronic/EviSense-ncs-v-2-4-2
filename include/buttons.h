/**
 * @file button.h
 * @author Thomas Keilbach | keiltronic GmbH
 * @date 27 Oct 2022
 * @brief This file contains functions headers to communicate with the pheripherals
 * @version 1.0.0
 */

#ifndef BUTTONS_H
#define BUTTONS_H

#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <zephyr/drivers/i2c.h>
#include <hal/nrf_regulators.h>
#include "i2c.h"
#include "algorithms.h"
#include "battery_gauge.h"
#include "imu.h"
#include "events.h"
#include "hibernate.h"
#include "interrupts.h"

// // #include <gpio.h>
// // #include <hal/nrf_regulators.h>
// // #include <hal/nrf_gpio.h>
// // #include <hal/nrf_power.h>
// // #include <modem/lte_lc.h>
// // #include <bsd.h>

// #define BATTERY_LEVEL_TIME 1000 // ms
// #define BATTERY_LEVEL_TIME 1000 // ms
// #define HIBERNATE_TIME 5000      // ms

// extern uint16_t btn1_press_timer;
// extern uint16_t btn2_press_timer;
// extern uint8_t btn1_pressed;
// extern uint8_t btn2_pressed;
// extern uint8_t btn1_beep;
// extern uint8_t btn2_beep;
// extern volatile uint16_t btn1_press_counter;
// extern volatile uint16_t btn2_press_counter;
// extern void button_monitor(void);

extern struct gpio_dt_spec button0;
extern struct gpio_dt_spec button1;

extern struct gpio_callback button0_cb_data;
extern struct gpio_callback button1_cb_data;

extern void button0_pressed(const struct device *dev, struct gpio_callback *cb, uint32_t pins);
extern void button1_pressed(const struct device *dev, struct gpio_callback *cb, uint32_t pins);

extern void init_button(void);

#endif