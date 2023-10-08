/**
 * @file rfid.h
 * @author Thomas Keilbach | keiltronic GmbH
 * @date 27 Oct 2022
 * @brief This file contains functions headers to communicate with the pheripherals
 * @version 1.0.0
 */
#ifndef RFID_H
#define RFID_H

#include <zephyr/device.h>
#include <zephyr/drivers/uart.h>
#include <zephyr/sys/printk.h>
#include <string.h>
#include "uart.h"
#include "epc_mem.h"

extern struct gpio_dt_spec booster_enable_pin;
extern struct gpio_dt_spec rfid_trigger_pin;

/* Output power */
#define RFID_TX_POWER_N2_DBM        "\nN1,00\r"
#define _N2_DBM                     0
#define RFID_TX_POWER_N1_DBM        "\nN1,01\r"
#define _N1_DBM                     1
#define RFID_TX_POWER_0_DBM         "\nN1,02\r"
#define _0_DBM                      2
#define RFID_TX_POWER_1_DBM         "\nN1,03\r"
#define _1_DBM                      3
#define RFID_TX_POWER_2_DBM         "\nN1,04\r"
#define _2_DBM                      4
#define RFID_TX_POWER_3_DBM         "\nN1,05\r"
#define _3_DBM                      5
#define RFID_TX_POWER_4_DBM         "\nN1,06\r"
#define _4_DBM                      6
#define RFID_TX_POWER_5_DBM         "\nN1,07\r"
#define _5_DBM                      7
#define RFID_TX_POWER_6_DBM         "\nN1,08\r"
#define _6_DBM                      8
#define RFID_TX_POWER_7_DBM         "\nN1,09\r"
#define _7_DBM                      9
#define RFID_TX_POWER_8_DBM         "\nN1,0A\r"
#define _8_DBM                      10
#define RFID_TX_POWER_9_DBM         "\nN1,0B\r"
#define _9_DBM                      11
#define RFID_TX_POWER_10_DBM        "\nN1,0C\r"
#define _10_DBM                     12
#define RFID_TX_POWER_11_DBM        "\nN1,0D\r"
#define _11_DBM                     13
#define RFID_TX_POWER_12_DBM        "\nN1,0E\r"
#define _12_DBM                     14
#define RFID_TX_POWER_13_DBM        "\nN1,0F\r"
#define _13_DBM                     15
#define RFID_TX_POWER_14_DBM        "\nN1,10\r"
#define _14_DBM                     16
#define RFID_TX_POWER_15_DBM        "\nN1,11\r"
#define _15_DBM                     17
#define RFID_TX_POWER_16_DBM        "\nN1,12\r"
#define _16_DBM                     18
#define RFID_TX_POWER_17_DBM        "\nN1,13\r"
#define _17_DBM                     19
#define RFID_TX_POWER_18_DBM        "\nN1,14\r"
#define _18_DBM                     20
#define RFID_TX_POWER_19_DBM        "\nN1,15\r"
#define _19_DBM                     21
#define RFID_TX_POWER_20_DBM        "\nN1,16\r"
#define _20_DBM                     22
#define RFID_TX_POWER_21_DBM        "\nN1,17\r"
#define _21_DBM                     23
#define RFID_TX_POWER_22_DBM        "\nN1,18\r"
#define _22_DBM                     24
#define RFID_TX_POWER_23_DBM        "\nN1,19\r"
#define _23_DBM                     25
#define RFID_TX_POWER_24_DBM        "\nN1,1A\r"
#define _24_DBM                     26
#define RFID_TX_POWER_25_DBM        "\nN1,1B\r"
#define _25_DBM                     27

/* Frequency range & regulations */
#define RFID_FREQ_US                "\nN5,01\r"
#define US                          1
#define RFID_FREQ_TW                "\nN5,02\r"
#define TW                          2
#define RFID_FREQ_CN                "\nN5,03\r"
#define CN                          3
#define RFID_FREQ_CN2               "\nN5,04\r"
#define CN2                         4
#define RFID_FREQ_EU                "\nN5,05\r"
#define EU                          5
#define RFID_FREQ_JP                "\nN5,06\r"
#define JP                          6
#define RFID_FREQ_KR                "\nN5,07\r"
#define KR                          7
#define RFID_FREQ_VN                "\nN5,08\r"
#define VN                          8

/* Functions */
extern void config_RFID(void);extern void RFID_TriggerSingleScan(void);
extern void rfid_trigger_multi_read(void);
extern void RFID_TurnOff(void);
extern void RFID_TurnOn(void);

extern uint8_t RFID_getFWVersion(void);
extern uint8_t RFID_getID(void);
extern void RFID_setOutputPower(int8_t tx_dbm);
extern void RFID_setFrequency(uint8_t freq);
extern int8_t RFID_getOutputPower(void);
extern uint8_t RFID_getFrequency(void);

extern void rfid_power_on(void);
extern void rfid_power_off(void);

extern  uint8_t RFID_autoscan_enabled;
extern  uint8_t RFID_IsOn;
extern  uint8_t RFID_ScanEnable;
extern  uint16_t RFID_SwitchOffDelayTimer;
extern  uint8_t RFID_TriggeredRead;

#endif