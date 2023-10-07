/**
 * @file usb.h
 * @author Thomas Keilbach | keiltronic GmbH
 * @date 27 Oct 2022
 * @brief This file contains functions headers to communicate with the pheripherals
 * @version 1.0.0
 */


#ifndef USB_H
#define USB_H

#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include "battery_gauge.h"
#include "notification.h"

extern void USB_CheckConnectionStatus(void);
extern bool charger_plug_in_while_reboot;

#endif