/**
 * @file aws.h
 * @author Thomas Keilbach | keiltronic GmbH
 * @date 08 Oct 2023
 * @brief This file contains functions headers to communicate with the pheripherals
 * @version 2.0.0
 */

#ifndef AWS_FOTA_H
#define AWS_FOTA_H

#include <zephyr/kernel.h>
#include <stdio.h>
//#include <bsd.h>
// #include <modem/lte_lc.h>
// #include <modem/at_cmd.h>
// #include <modem/at_notif.h>
// #include <net/mqtt.h>
// #include <net/socket.h>
// #include <modem/bsdlib.h>
#include <net/aws_jobs.h>
#include <net/aws_fota.h>
//#include <dfu/mcuboot.h>
#include <zephyr/sys/reboot.h>
#include <zephyr/sys/byteorder.h>
#include "coap.h"
#include "test.h"

BUILD_ASSERT(!IS_ENABLED(CONFIG_LTE_AUTO_INIT_AND_CONNECT), "This sample does not support auto init and connect");

#define MQTT_KEEPALIVE (CONFIG_MQTT_KEEPALIVE * MSEC_PER_SEC)

#if !defined(CONFIG_CLOUD_CLIENT_ID)
/* Define the length of the IMEI AT COMMAND response buffer */
#define CGSN_RESP_LEN 19
#define IMEI_LEN 15
#define CLIENT_ID_LEN (sizeof(CONFIG_NRF_CLOUD_CLIENT_ID_PREFIX) + IMEI_LEN)
#else
#define CLIENT_ID_LEN sizeof(CONFIG_CLOUD_CLIENT_ID)
#endif

#define AWS_FOTA_PROCESS_IDLE 0
#define AWS_FOTA_PROCESS_INIT 1
#define AWS_FOTA_PROCESS_CONNECT 2
#define AWS_FOTA_PROCESS_RUNNING 3
#define AWS_FOTA_PROCESS_DISCONNECT 4

#define FOTA_CONNECTION_DURATION 600 // 10 min

extern uint8_t aws_fota_process_state;

extern int fds_init(struct mqtt_client *c);
extern void at_configure(void);
extern void aws_fota_cb_handler(struct aws_fota_event *fota_evt);
extern int client_init(struct mqtt_client *client, char *hostname);
extern int client_id_get(char *id_buf,uint32_t len);
extern int update_device_shadow_version(struct mqtt_client *const client);
extern void data_print(uint8_t *prefix, uint8_t *data,uint32_t len);
extern int publish_get_payload(struct mqtt_client *c, uint8_t *write_buf,uint32_t length);
extern void mqtt_evt_handler(struct mqtt_client *const c, const struct mqtt_evt *evt);
extern void broker_init(const char *hostname);
extern void aws_fota_statemachine(void);

extern uint8_t fota_reboot_while_usb_connected;
extern uint8_t fota_is_connected;
extern uint32_t fota_connection_timer;
extern uint8_t fota_download_in_progress;

/* MQTT Broker details. */
extern struct sockaddr_storage broker_storage;

/* File descriptor */
extern struct pollfd fds;

/* Set to true when application should teardown and reboot */
extern bool do_reboot;

/* Set to true when application should reconnect the LTE link*/
extern bool link_offline;

extern struct mqtt_client client;

#endif