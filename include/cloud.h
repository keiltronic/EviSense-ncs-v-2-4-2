/**
 * @file cloud.h
 * @author Thomas Keilbach | keiltronic GmbH
 * @date 27 Oct 2022
 * @brief This file contains functions headers to communicate with the pheripherals
 * @version 1.0.0
 */

#ifndef CLOUD_H
#define CLOUD_H

#include <zephyr/kernel.h>
#include <stdio.h>
#include <stdlib.h>
#include "aletheia.pb-c.h"
#include "battery_gauge.h"
#include "string.h"
#include "coap.h"
#include "events.h"
#include "algorithms.h"
#include "epc_mem.h"
#include "event_mem.h"

extern UsageUpdate myUsageUpdate;
extern DeviceStatus myDeviceStatus;
extern uint32_t coap_last_transmission_timer;
extern time_t timestamp_last_cloud_transmission;

extern void cloud_SendUsageUpdateObject(void);
extern uint32_t protobuf_EncodeUsageUpdateObject(uint8_t **buf);
extern void cloud_DecodeUsageUpdateProtobuf(uint8_t *message, uint16_t len);
extern void cloud_DecodeHubUpdateProtobuf(uint8_t *message, uint16_t len);
extern void cloud_DecodeDataUpdateProtobuf(uint8_t *message, uint16_t len);

#endif