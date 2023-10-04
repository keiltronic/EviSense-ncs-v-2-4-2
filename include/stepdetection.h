/**
 * @file stepdetection.h
 * @author Thomas Keilbach | keiltronic GmbH
 * @date 27 Oct 2022
 * @brief This file contains functions headers to communicate with the pheripherals
 * @version 1.0.0
 */


#ifndef STEPDETECTION_H
#define STEPDETECTION_H

#include <zephyr/device.h>
#include <zephyr/sys/printk.h>
#include "system_mem.h"

#define STEP_SEQUENCE_MIN_STEPS       5
#define STEP_SEQUENCE_RESET_DELAY     3 // seconds

extern void step_sequence_detection(void);

#endif