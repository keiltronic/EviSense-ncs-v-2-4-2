/**
 * @file threads.h
 * @author Thomas Keilbach | keiltronic GmbH
 * @date 27 Oct 2022
 * @brief This file contains functions headers to communicate with the pheripherals
 * @version 1.0.0
 */

#ifndef THREADS_H
#define THREADS_H

#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include "led.h"
#include "imu.h"
#include "system_mem.h"
#include "rfid.h"
#include "datalog_mem.h"
#include "stepdetection.h"
#include "notification.h"
#include "battery_gauge.h"
#include "uart.h"
#include "pwm.h"
#include "coap.h"
#include "usb.h"
#include "datalog_mem.h"
#include "aws_fota.h"
#include "cloud.h"
#include "test.h"
#include "hibernate.h"

/* size of stack area used by each thread */
#define STACKSIZE_LARGE 3072
#define STACKSIZE_SMALL 1024

extern void init_threads(void);
#endif
