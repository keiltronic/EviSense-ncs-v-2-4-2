/**
 * @file stepdetection.h
 * @author Thomas Keilbach | keiltronic GmbH
 * @date 07 Oct 2023
 * @brief This file contains functions headers to communicate with the pheripherals
 * @version 2.0.0
 */


#ifndef STEPDETECTION_H
#define STEPDETECTION_H

#include <zephyr/device.h>
#include <zephyr/shell/shell.h>
#include <zephyr/sys/printk.h>
#include "system_mem.h"
#include "parameter_mem.h"
#include "rtc.h"


#define STEP_SEQUENCE_MIN_STEPS       5
#define STEP_SEQUENCE_RESET_DELAY     3 // seconds

extern void step_sequence_detection(void);

#endif