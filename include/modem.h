/**
 * @file modem.h
 * @author Thomas Keilbach | keiltronic GmbH
 * @date 11 Oct 2023
 * @brief This file contains functions headers to communicate with the pheripherals
 * @version 2.0.0
 */

#ifndef MODEM_H
#define MODEM_H

#include <zephyr/kernel.h>
#include <stdio.h>
#include <stdlib.h>
#include <zephyr/drivers/uart.h>
#include <string.h>
#include <zephyr/random/rand32.h>
#include <modem/nrf_modem_lib.h>
//#include <net/mqtt.h>
//#include <net/socket.h>
#include <modem/lte_lc.h>
//#include <modem/at_cmd.h>
#include <nrf_modem_at.h>
#include <modem/at_cmd_parser.h>
#include "events.h"
#include "rtc.h"
#include "modem.h"
#include "test.h"
#include "string.h"

typedef struct
{
  uint8_t mode; // 0 - idle, 1 - connected
  uint8_t connection_stat;
  uint8_t stat;
  char tac[10];
  char cell_id[12];
  char active_time[10];
  char tau[10];
  char IMEI[18]; // Unique modem ID
  int16_t RSSI;
  char UICCID[40]; // Unique SIM card ID
  char oper[8];    // operator name in the alphanumeric format or a string of MCC and MNC
  uint8_t format;
  uint8_t AcT; // connection methode 7 = LTE-M, 9 = NB-IoT
  uint8_t band;
  uint8_t temp;
  char version[20]; // modem firmware version
  enum lte_lc_nw_reg_status registration_status[2];
} MODEM;

extern MODEM modem;
extern void modem_init(void);
extern void modem_initial_setup(void);
extern void modem_update_information(void);
extern void modem_print_settings(void);
extern const char *modem_get_imei(void);
extern void modem_update_registration_status(void);
extern const char *modem_get_iccid(void);

extern char modem_at_recv_buf[500];
extern int8_t get_signal_quality(void);
extern int16_t get_signal_strength(void);
#endif