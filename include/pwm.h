/**
 * @file pwm.h
 * @author Thomas Keilbach | keiltronic GmbH
 * @date 27 Oct 2022
 * @brief This file contains functions headers to communicate with the pheripherals
 * @version 1.0.0
 */

#ifndef PWM_H
#define PWM_H

#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <zephyr/drivers/pwm.h>
#include "parameter_mem.h"

//extern const struct pwm_dt_spec sBuzzer;
extern const struct pwm_dt_spec sBuzzer;
extern void pwm_init(void);

#endif