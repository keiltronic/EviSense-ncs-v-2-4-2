/**
 * @file gpio.h
 * @author Thomas Keilbach | keiltronic GmbH
 * @date 27 Oct 2022
 * @brief This file contains functions headers to communicate with the pheripherals
 * @version 1.0.0
 */

#ifndef GPIO_H
#define GPIO_H

#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <zephyr/drivers/gpio.h>
#include "interrupts.h"
#include "spi.h"

#define GPIO_PIN_5V_EN          0
#define GPIO_PIN_RST            1 
#define GPIO_PIN_CHG_EN         2
#define GPIO_PIN_LED_EN         3
#define GPIO_PIN_5V_PG          4
#define GPIO_PIN_BAT_ALRT1      5
#define GPIO_PIN_PWR_STAT       6
#define GPIO_PIN_CHRG_STAT      9
#define GPIO_PIN_MAG_INT        10
#define GPIO_PIN_SDA            11
#define GPIO_PIN_SCL            12
#define GPIO_PIN_VUSB_MEASURE   13
#define GPIO_PIN_LED1           14
#define GPIO_PIN_IMU_INT1       15
#define GPIO_PIN_IMU_INT2       16
#define GPIO_PIN_RFID_TRIGGER   17
#define GPIO_PIN_BTN1           18
#define GPIO_PIN_BTN2           19
#define GPIO_PIN_GPIO1          20
#define GPIO_PIN_GPIO2          21
#define GPIO_PIN_GPIO3          24
#define GPIO_PIN_GPIO4          25
#define GPIO_PIN_BUZZER         26
#define GPIO_PIN_FLASH_CS1      27  // datalog
#define GPIO_PIN_FLASH_CS2      31  // parameters, epc records storage

extern struct device *gpio_dev;

extern void gpio_init(void);
extern struct gpio_dt_spec led;

#endif