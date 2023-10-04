/**
 * @file parameter_mem.h
 * @author Thomas Keilbach | keiltronic GmbH
 * @date 27 Oct 2022
 * @brief This file contains functions headers to communicate with the pheripherals
 * @version 1.0.0
 */

#ifndef PARAMETER_H
#define PARAMETER_H

#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include "flash.h"

#define PARAMETER_MEM 0x10000UL       // Start address of memory region
#define PARAMETER_MEM_LENGTH 0xFFFFUL // Lengts of memory region (multiples of 64kB sector size, here one 64kB sector)
#define PARAMETER_MEM_RAM_SIZE 256UL  // In byte, must be a value of power of 2  (2^n)

#define LTE_M 0
#define NB_IOT 1
typedef union
{
  uint8_t parameter_mem_bytes[PARAMETER_MEM_RAM_SIZE];

  struct __attribute__((packed))
  {
    int16_t datalog_Interval;
    int16_t imu_interval;
    int16_t rfid_interval;
    uint32_t rfid_interval_lifted;
    int16_t rfid_verbose;
    uint8_t datalog_sniffFrame;
    uint8_t stepdetection_verbose;
    uint8_t debug;
    uint8_t rfid_output_power;
    uint8_t rfid_output_power_lifted;
    int16_t rfid_frequency;
    uint8_t led_brightness;
    uint8_t buzzer_duty_cycle;
    uint8_t datalogEnable;
    float acc_noise_thr; // m/s^2 - Noise threshold for Acc
    float gyr_noise_thr; // deg/s  - Noise threshold for Gyr
    float gyr_spin_thr;
    float mag_noise_thr;                      // microT - Noise threshold for Mag to remove earth magnetic field strength
    float frame_handle_angle_thr;             // in deg -  threshold to detect frame flip.
    float floor_handle_angle_mopping_thr_min; // in deg -  min threshold to enable mopping detection.
    float floor_handle_angle_mopping_thr_max; // in deg -  max threshold to enable mopping detection.
    float floor_handle_angle_mopchange_thr;   // in deg -  thresholds to enable mop change detection.
    int64_t min_mopchange_duration;           // in sec - time threshold to enable mop change detection.
    int64_t min_mopframeflip_duration;        // in sec - time threshold to enable mop frame flip detection.
    float angle_smooth_factor;                // exponential filter factor for estimation of inclination angles (close to 0 gives more weight to recent samples)
    float gyr_smooth_factor;                  // exponential filter factor for smoothing Gyro based feature values
    float min_mopcycle_duration;              //>0.4 sec - min duration of mop cycle is the recurring mopping movement (actually is half of the ?scillation)-fast mopping
    float max_mopcycle_duration;              //<3 sec - max duration of mop cycle -slow mopping
    float mop_width;                          // in meters - width of the mop frame
    float mop_overlap;                        // in precentage during mopping
    float mopcycle_sequence_thr;              // num of mop cycles needed to set mopping flag
    int64_t peakfollower_update_delay;        // secs - period in sec to update the signal peak
    int32_t mop_rfid_detection_thr;            // 3-5 number of sequential reads until the mop rfid is confirmed
    float mopping_coverage_per_mop_thr;       // sends info only if some m2 have been mopped;
    uint8_t algo_flag_verbose;
    uint8_t algo_verbose;
    uint8_t coap_verbose;
    uint8_t events_verbose;
    uint8_t protobuf_verbose;
    uint32_t cloud_sync_interval_idle;
    uint32_t cloud_sync_interval_moving;
    uint8_t modem_verbose;
    uint8_t flash_verbose;
    uint8_t rfid_autoscan;
    uint8_t epc_verbose;
    uint32_t last_seen_locations_auto_reset_time;
    uint8_t log_unkown_tags;
    uint32_t last_seen_mop_array_auto_reset_time;
    uint8_t enable_blue_dev_led;
    uint8_t notification_verbose;
    uint8_t notifications_while_usb_connected;
    uint8_t enable_rfid_confirmation_blinking;
    float max_sqm_coveraged_per_mop;
    uint8_t enable_coveraged_per_mop_notification;
    uint32_t usb_plugin_reset_time;
    uint32_t usb_auto_reset_time;
    uint16_t anymotion_duration;
    uint16_t anymotion_thr;
    uint32_t motion_reset_time;
    uint8_t fota_enable;
    uint32_t fully_charged_indicator_time;
    uint8_t current_shift_mop_check;
    uint8_t fota_verbose;
    float battery_charge_termination_current;
    uint8_t battery_gauge_sniff_i2c;
    float battery_gauge_charge_temp_min;
    float battery_gauge_charge_temp_max;
    uint8_t network_connection_type;
    uint32_t mop_id_refresh_timer;
    float hit_shock_mag_thr;
    uint8_t algocontrol_bymag_det;
    uint8_t mag_det_threshold;
    uint8_t mag_det_consecutive_samples;
    uint8_t notification_test;
    uint8_t rfid_blink_notification;
    uint8_t modem_disable;
    uint8_t rfid_disable;
    uint32_t low_bat_threshold;
    uint8_t epc_raw_verbose;
    uint8_t binary_search_verbose;
    uint8_t protobuf_enable;
    uint8_t mop_verbose;
    uint16_t event1statistics_interval;
  };
} PARAMETER;

extern void Parameter_InitRAM(void);
extern void Parameter_PushRAMToFlash(void);
extern void Parameter_PopFlashToRAM(void);
extern void Parameter_PrintValues(PARAMETER *para);

extern PARAMETER Parameter;
#endif
