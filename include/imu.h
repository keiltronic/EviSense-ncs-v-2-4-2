/**
 * @file imu.h
 * @author Thomas Keilbach | keiltronic GmbH
 * @date 27 Oct 2022
 * @brief This file contains functions headers to communicate with the pheripherals
 * @version 1.0.0
 */

#ifndef IMU_H
#define IMU_H

#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include "bmi160.h"
#include "bmi160_defs.h"
#include "bmm150.h"
#include "bmm150_defs.h"
#include "stdio.h"
#include "i2c.h"

#define GRAVITY_EARTH           9.81

extern struct bmi160_dev bmi160;
extern struct bmm150_dev bmm150;

extern struct bmi160_sensor_data accel;
extern struct bmi160_sensor_data gyro;

extern volatile uint8_t imu_IsInitialized;
extern volatile uint8_t trace_imu_flag;
extern volatile uint8_t trace_imu_reduced_flag;
extern volatile uint8_t step_interrupt_triggered;
extern volatile uint8_t motion_detected;
extern volatile uint32_t motion_reset_counter;

extern void init_imu(void);
extern void imu_fetch_data(void);
extern void trace_imu(void);
extern void trace_imu_reduced(void);
extern void motion_detection(void);
extern float gyro_lsb_to_dps(int16_t val);
extern float acc_lsb_to_ms2(int16_t val);
extern void imu_enter_sleep(void);

extern void user_delay_ms(uint32_t period, void *intf_ptr);
extern int8_t user_bmi160_i2c_read(uint8_t dev_id, uint8_t reg_addr, uint8_t *reg_data, uint16_t len);
extern int8_t user_bmi160_i2c_write(uint8_t dev_id, uint8_t reg_addr, uint8_t *reg_data, uint16_t len);
extern int8_t bmm150_aux_read(uint8_t id, uint8_t reg_addr, uint8_t *reg_data, uint16_t len);
extern int8_t bmm150_aux_write(uint8_t id, uint8_t reg_addr, uint8_t *reg_data, uint16_t len);
extern int8_t magnet_detection(void);

#endif