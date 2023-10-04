/**
 * @file hibernate.h
 * @author Thomas Keilbach | keiltronic GmbH
 * @date 08 Mar 20223
 * @brief This files contains function headers to set the device in hibernate mode (deep sleep)
 * @version 1.0.0
 */

#ifndef HIBERNATE_H
#define HIBERNATE_H

#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <zephyr/drivers/i2c.h>
#include <hal/nrf_regulators.h>
#include <gpio.h>
#include <hal/nrf_gpio.h>
#include <hal/nrf_power.h>
#include <modem/lte_lc.h>
//#include <bsd.h>
#include "uart.h"
#include "imu.h"
#include "spi.h"

// #define WAKEUP_PIN NRF_GPIO_PIN_MAP(0, GPIO_PIN_BTN1 | GPIO_PIN_BTN2)
#define WAKEUP_PIN NRF_GPIO_PIN_MAP(0, GPIO_PIN_BTN2)

extern bool hibernation_mode;

extern void enter_hibernate(void);

#endif