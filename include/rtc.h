/**
 * @file rtc.h
 * @author Thomas Keilbach | keiltronic GmbH
 * @date 27 Oct 2022
 * @brief This file contains functions headers to communicate with the pheripherals
 * @version 1.0.0
 */


#ifndef RTC_H
#define RTC_H

#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <zephyr/shell/shell.h>
#include <zephyr/shell/shell_uart.h>
#include <time.h>
#include "parameter_mem.h"
#include "datalog_mem.h"
#include "system_mem.h"
#include <date_time.h>

#define DEFAULT_UNIX_TIME 1609459200000LL


extern struct tm *ptm;
extern time_t unixtime;
extern time_t unixtime_ms;
extern uint16_t millisec;
extern uint8_t initial_time_update;

extern void init_rtc(void);
extern void rtc_print_timestamp(void);
extern void rtc_print_time(void);
extern void rtc_print_date(void);
extern void rtc_print_weekday(void);
extern void rtc_print_timezone(void);
extern void rtc_print_unixtime(uint64_t unixtime);
extern void rtc_print_unixtime_ms(uint64_t unixtime, uint16_t millisec);
extern bool rtc_fetch_date_time(void);
extern void rtc_print_debug_timestamp(void);

#endif