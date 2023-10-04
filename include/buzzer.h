/**
 * @file buzzer.h
 * @author Thomas Keilbach | keiltronic GmbH
 * @date 27 Oct 2022
 * @brief This file contains functions headers to communicate with the pheripherals
 * @version 1.0.0
 */

#ifndef BUZZER_H
#define BUZZER_H

#include <zephyr/device.h>
#include "pwm.h"
#include "system_mem.h"
#include "parameter_mem.h"

#define BUZZER_GPIO_PIN   26
#define BUZZER_RES_FREQ   2700
#define BUZZER_DUTY_CYCLE 50

#define BUZZER_OFF        0
#define BUZZER_ON         1
#define BUZZER_BEEP       2

typedef struct {
    uint8_t status;
    uint8_t status_old;
    uint32_t frequency;
    uint32_t duty_cycle;
    uint32_t beep_on_time;
    uint32_t beep_off_time;
    uint32_t delay;
    uint32_t delay_timer;
    uint32_t beep_period_counter;
    uint32_t beep_cycles;
    uint32_t beep_cycle_counter;
    uint32_t burst_repeat_periode;
    uint32_t burst_period_counter;
    uint32_t burst_repeat_cycles;
    uint32_t burst_repeat_counter;
    bool isBeeping;
} BUZZER;

extern BUZZER buzzer;
extern void set_buzzer(BUZZER *buzzer);
extern void buzzer_update(BUZZER *buzzer);

#endif