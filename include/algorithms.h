/**
 * @file algorithm.h
 * @author Thomas Keilbach | keiltronic GmbH
 * @date 27 Oct 2022
 * @brief This file contains functions headers to communicate with the pheripherals
 * @version 1.0.0
 */

#ifndef ALGORITHMS_H
#define ALGORITHMS_H

#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <math.h>
#include "parameter_mem.h"
#include "datalog_mem.h"
#include "imu.h"
#include "epc_mem.h"
#include "rtc.h"
#include "events.h"
#include "epc_mem.h"
#include "cloud.h"
#include "test.h"

#define PI                3.141593
#define EARTH_GRAVITY     9.80665 //  m/s^2
#define NF                10 //samples for moving average filter (0.25secs)

#define MOP_RESET_STATE     0
#define NEW_CHIPPED_MOP     1
#define NEW_UNCHIPPED_MOP   2

#define IDLE_STATE          0
#define MOVING_STATE        1
#define MOPPING_STATE       2

extern void init_algorithms(void);
extern void algo_reset_variables(void);
extern void algo_fAddArrayElement(float *array, float sample, uint16_t array_length);
extern float algo_fmean(float *array, uint16_t array_length);
extern float algo_fsum(float *array, uint16_t array_length);
extern void algorithm_execute_process(void);
extern void update_average_usage_numbers(void);
extern float standard_deviation(float *array, int length);

extern uint8_t mob_max_sqm_reached;
extern uint8_t mob_max_side1_sqm_reached;
extern uint8_t mob_max_side2_sqm_reached;
extern float mopping_speed;
extern float mopping_coverage_per_mop;
extern float mopping_coverage_side1;
extern float mopping_coverage_side2;
extern float floor_handle_angle[2];
extern float frame_handle_angle[2];
extern uint16_t mopcycles;
extern uint8_t mopping_pattern[2];
extern uint8_t motion_state[2];
extern uint8_t Frame_side[2];
extern uint8_t Mop_on_floor_after_Change_Flag;
extern uint8_t frame_lift_flag[2];

extern uint32_t mop_rfid_readings;
extern uint32_t mop_null_readings;
extern uint8_t MoppingFlag[2];
extern int64_t Newmop_RFID_tsp;
extern uint32_t Newmop_RFID_readings;
extern uint8_t Newmop_RFID_flag;
extern uint8_t Prevmop_RFID_replaced_flag;
extern int64_t Mopping_start_tsp;
extern uint8_t Mopping_motion_gyr_flag;

extern uint32_t time_in_idle_state;
extern uint32_t time_in_moving_state;
extern uint32_t time_in_mopping_state;

extern double idle_time_in_percentage;
extern double moving_time_in_percentage;
extern double mopping_time_in_percentage;

extern uint8_t Flag_SameMopAlreadyUsedNotification;
extern uint8_t chipped_mop_installed;
extern uint8_t algorithm_lock;
extern uint32_t Total_mops_used;

#endif
