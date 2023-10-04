/**
 * @file cloud.h
 * @author Thomas Keilbach | keiltronic GmbH
 * @date 27 Oct 2022
 * @brief This file contains functions headers to communicate with the pheripherals
 * @version 1.0.0
 */

#ifndef COAP_H
#define COAP_H

#include <zephyr/kernel.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <zephyr/sys/printk.h>
#include <zephyr/sys/byteorder.h>
#include <zephyr/random/rand32.h>

#include <modem/lte_lc.h>
//#include <modem/modem_key_mgmt.h>
#include <zephyr/net/coap.h>
#include <zephyr/net/socket.h>
#include <zephyr/net/net_mgmt.h>
#include <zephyr/net/net_ip.h>
#include <zephyr/net/udp.h>
#include <zephyr/net/coap.h>

#include <zephyr/net/tls_credentials.h>
#include <nrfx.h>
//#include <modem/nrf_socket.h>

#include "parameter_mem.h"
#include "rtc.h"
#include "aws_fota.h"

#define MAX_COAP_MSG_LEN 1024
#define BLOCK_WISE_TRANSFER_SIZE_GET 2048
#define COAP_BLOCK_SIZE COAP_BLOCK_128
#define PSK_TAG 2

extern uint16_t next_token;
extern int16_t err;
extern uint8_t trigger_tx;
extern time_t time_since_last_cloud_transmission;
extern time_t timestamp_last_cloud_transmission;

extern int16_t send_coap_request(uint8_t method, uint8_t *message, uint16_t len);
extern int16_t start_coap_client(void);
extern void convert_ASCII_text_in_hexadecimal_string(char *destination, char *source);
extern int16_t process_large_coap_reply(void);
extern void wait(void);
extern void prepare_fds(void);

#endif