/**
 * @file i2c.h
 * @author Thomas Keilbach | keiltronic GmbH
 * @date 05 Oct 2023
 * @brief This file contains functions headers to communicate with the pheripherals
 * @version 2.0.0
 */

#ifndef I2C_H
#define I2C_H

#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <zephyr/drivers/i2c.h>
#include "parameter_mem.h"

 extern void i2c_init(void);

 #define I2C_SLAVE_COUNT 5

extern const struct device *i2c_dev;
 extern struct i2c_msg msgs[1];

// extern void init_i2c(void);
 extern uint16_t i2c_scanner(uint8_t print_message);
 extern uint8_t i2c_test(void);

#endif