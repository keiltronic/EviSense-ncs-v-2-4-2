/**
 * @file interrupts.h
 * @author Thomas Keilbach | keiltronic GmbH
 * @date 27 Oct 2022
 * @brief This file contains functions headers to communicate with the pheripherals
 * @version 1.0.0
 */

#ifndef INTERRUPTS_H
#define INTERRUPTS_H

#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <zephyr/drivers/gpio.h>
#include "imu.h"
#include "system_mem.h"
#include "buttons.h"
#include "pwm.h"

extern void ISR_imu_int1(struct device *gpio, struct gpio_callback *cb, uint32_t pins);
extern void ISR_imu_int2(struct device *gpio, struct gpio_callback *cb, uint32_t pins);
extern void ISR_mag_int(struct device *gpio, struct gpio_callback *cb, uint32_t pins);
extern void ISR_btn1(struct device *gpio, struct gpio_callback *cb, uint32_t pins);
extern void ISR_btn2(struct device *gpio, struct gpio_callback *cb, uint32_t pins);

#endif