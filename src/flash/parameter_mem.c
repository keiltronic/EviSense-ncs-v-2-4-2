/**
 * @file parameter_mem.c
 * @author Thomas Keilbach | keiltronic GmbH
 * @date 05 Oct 2023
 * @brief This file contains function to write and read device settings (parameters) to and from the external flash memory
 * @version 2.0.0
 */

/*!
 * @defgroup Memory
 * @brief This file contains function to write and read device settings (parameters) to and from the external flash memory
 * @{*/

#include "parameter_mem.h"

PARAMETER Parameter;

/*!
 * @brief This functions initalize a structure with parameter in RAM.
 */
void Parameter_InitRAM(void)
{
  Parameter.datalog_Interval = 50;
  Parameter.imu_interval = 50;
  Parameter.rfid_interval = 300;
  Parameter.rfid_interval_lifted = 120; // msec
  Parameter.rfid_verbose = false;
  Parameter.rfid_output_power = 27;
  Parameter.rfid_output_power_lifted = 27;
  Parameter.rfid_frequency = 5; // EU =5 US = 1;
  Parameter.datalog_sniffFrame = false;
  Parameter.stepdetection_verbose = false;
  Parameter.led_brightness = 255;
  Parameter.buzzer_duty_cycle = 15;
  Parameter.debug = false;
  Parameter.datalogEnable = false;
  Parameter.acc_noise_thr = 0.10;                      // m/s^2 - Noise threshold for Acc
  Parameter.gyr_noise_thr = 10.0;                      // deg/s  - Noise threshold for Gyr
  Parameter.gyr_spin_thr = 100.0;                      // degrees to rotate handle during S-shape mopping
  Parameter.mag_noise_thr = 90.0;                     // microT - Noise threshold for Mag to remove earth magnetic field strength
  Parameter.frame_handle_angle_thr = 20.0;             // in deg -  threshold to detect frame flip.
  Parameter.floor_handle_angle_mopping_thr_min = 30.0; // in deg -  min threshold to enable mopping detection.
  Parameter.floor_handle_angle_mopping_thr_max = 80.0; // in deg -  max threshold to enable mopping detection.
  Parameter.floor_handle_angle_mopchange_thr = 15.0;  // in deg -  thresholds to enable mop change detection.
  Parameter.min_mopchange_duration = 1LL;              // in sec - time threshold to enable mop change detection.
  Parameter.min_mopframeflip_duration = 5LL;           // in sec - time threshold to enable mop frame flip detection.
  Parameter.angle_smooth_factor = 0.9;                 // exponential filter factor for estimation of inclination angles (close to 0 gives more weight to recent samples)
  Parameter.gyr_smooth_factor = 0.95;                  // exponential filter factor for smoothing Gyro based feature values
  Parameter.min_mopcycle_duration = 0.4;               // sec - min duration of mop cycle is the recurring mopping movement (actually is half of the oscillation)-fast mopping
  Parameter.max_mopcycle_duration = 4.0;               // <3 sec - max duration of mop cycle -slow mopping
  Parameter.mop_width = 0.50;                          // in meters - width of the mop frame
  Parameter.mop_overlap = 1.0;                         // in precentage during mopping
  Parameter.mopcycle_sequence_thr = 1.0;               // num of mop cycles needed to set mopping flag
  Parameter.peakfollower_update_delay = 1LL;           // secs - period in sec to update the signal peak
  Parameter.mop_rfid_detection_thr = 7;               //-- 3-5 number of sequential reads until the mop rfid is confirmed
  Parameter.mopping_coverage_per_mop_thr = 1.0;        // sends info only if some m2 have been mopped;
  Parameter.algo_flag_verbose = false;
  Parameter.algo_verbose = true;
  Parameter.coap_verbose = false;
  Parameter.events_verbose = true;
  Parameter.protobuf_verbose = false;
  Parameter.modem_verbose = false;
  Parameter.cloud_sync_interval_idle = 300;   // seconds
  Parameter.cloud_sync_interval_moving = 600; // seconds
  Parameter.flash_verbose = false;
  Parameter.epc_verbose = true;
  Parameter.epc_raw_verbose = false;
  Parameter.rfid_autoscan = false;
  Parameter.log_unkown_tags = true;
  Parameter.last_seen_locations_auto_reset_time = 5;     // in seconds
  Parameter.last_seen_mop_array_auto_reset_time = 20000; // => 30000=8.3h in seconds
  Parameter.enable_blue_dev_led = true;
  Parameter.notification_verbose = false;
  Parameter.notifications_while_usb_connected = false;
  Parameter.enable_rfid_confirmation_blinking = false;
  Parameter.enable_coveraged_per_mop_notification = true;
  Parameter.max_sqm_coveraged_per_mop = 12.0;
  Parameter.usb_plugin_reset_time = 900; // sec: 15 min
  Parameter.usb_auto_reset_time = 86400; // sec: 24 h
  Parameter.anymotion_duration = 2;
  Parameter.anymotion_thr = 12;
  Parameter.motion_reset_time = 15;
  Parameter.fota_enable = true;
  Parameter.fota_verbose = false;
  Parameter.current_shift_mop_check = true;
  Parameter.fully_charged_indicator_time = 8640000;    // sec (100 days)
  Parameter.battery_charge_termination_current = 20.0; // mA
  Parameter.battery_gauge_sniff_i2c = false;
  Parameter.battery_gauge_charge_temp_min = 0.0;
  Parameter.battery_gauge_charge_temp_max = 45.0;
  Parameter.network_connection_type = LTE_M; // LTE_M NB_IOT
  Parameter.mop_id_refresh_timer = 45;
  Parameter.hit_shock_mag_thr = 2.5;
  Parameter.algocontrol_bymag_det = false;
  Parameter.mag_det_threshold = 120;
  Parameter.mag_det_consecutive_samples = 30; // unit: 100ms
  Parameter.notification_test = false;
  Parameter.rfid_blink_notification = false;
  Parameter.modem_disable = false;
  Parameter.rfid_disable = false;
  Parameter.low_bat_threshold = 3450; // mV
  Parameter.binary_search_verbose = false;
  Parameter.mop_verbose = false;
  Parameter.event1statistics_interval = 60; // sec  
}

/*!
 * @brief This functions reads the the stored parameter setting from external flash memory to the RAM.
 */
void Parameter_PopFlashToRAM(void)
{
//   flash_read(GPIO_PIN_FLASH_CS2, PARAMETER_MEM, &Parameter.parameter_mem_bytes[0], PARAMETER_MEM_RAM_SIZE);
 }

/*!
 * @brief This functions saves the current parameter setting in RAM to external flash memory.
 */
 void Parameter_PushRAMToFlash(void)
 {
//   flash_ClearBlock_4kB(GPIO_PIN_FLASH_CS2, PARAMETER_MEM, PARAMETER_MEM_LENGTH);
//   flash_write(GPIO_PIN_FLASH_CS2, PARAMETER_MEM, &Parameter.parameter_mem_bytes[0], PARAMETER_MEM_RAM_SIZE);
 }

/**
 * @brief This functions prints the current parameter setting on console
 *
 * @param para: Pointer to parameter structure
 */
void Parameter_PrintValues(PARAMETER *para)
{
//   shell_fprintf(shell_backend_uart_get_ptr(), SHELL_VT100_COLOR_DEFAULT, "datalog_Interval = %d\n", para->datalog_Interval);
//   shell_fprintf(shell_backend_uart_get_ptr(), SHELL_VT100_COLOR_DEFAULT, "imu_interval = %d\n", para->imu_interval);
//   shell_fprintf(shell_backend_uart_get_ptr(), SHELL_VT100_COLOR_DEFAULT, "rfid_interval = %d\n", para->rfid_interval);
//   shell_fprintf(shell_backend_uart_get_ptr(), SHELL_VT100_COLOR_DEFAULT, "rfid_verbose = %d\n", para->rfid_verbose);
//   shell_fprintf(shell_backend_uart_get_ptr(), SHELL_VT100_COLOR_DEFAULT, "rfid_output_power = %d\n", para->rfid_output_power);
//   shell_fprintf(shell_backend_uart_get_ptr(), SHELL_VT100_COLOR_DEFAULT, "rfid_output_power_lifted = %d\n", para->rfid_output_power_lifted); 
//   shell_fprintf(shell_backend_uart_get_ptr(), SHELL_VT100_COLOR_DEFAULT, "rfid_frequency = %d\n", para->rfid_frequency);
//   shell_fprintf(shell_backend_uart_get_ptr(), SHELL_VT100_COLOR_DEFAULT, "datalog_sniffFrame = %d\n", para->datalog_sniffFrame);
//   shell_fprintf(shell_backend_uart_get_ptr(), SHELL_VT100_COLOR_DEFAULT, "stepdetection_verbose = %d\n", para->stepdetection_verbose);
//   shell_fprintf(shell_backend_uart_get_ptr(), SHELL_VT100_COLOR_DEFAULT, "led_brightness = %d\n", para->led_brightness);
//   shell_fprintf(shell_backend_uart_get_ptr(), SHELL_VT100_COLOR_DEFAULT, "buzzer_duty_cycle = %d\n", para->buzzer_duty_cycle);
//   shell_fprintf(shell_backend_uart_get_ptr(), SHELL_VT100_COLOR_DEFAULT, "debug = %d\n", para->debug);
//   shell_fprintf(shell_backend_uart_get_ptr(), SHELL_VT100_COLOR_DEFAULT, "datalogEnable = %d\n", para->datalogEnable);
//   shell_fprintf(shell_backend_uart_get_ptr(), SHELL_VT100_COLOR_DEFAULT, "acc_noise_thr = %.2f\n", para->acc_noise_thr);
//   shell_fprintf(shell_backend_uart_get_ptr(), SHELL_VT100_COLOR_DEFAULT, "gyr_noise_thr = %.2f\n", para->gyr_noise_thr);
//   shell_fprintf(shell_backend_uart_get_ptr(), SHELL_VT100_COLOR_DEFAULT, "mag_noise_thr = %.2f\n", para->mag_noise_thr);
//   shell_fprintf(shell_backend_uart_get_ptr(), SHELL_VT100_COLOR_DEFAULT, "gyr_spin_thr = %.2f\n", para->gyr_spin_thr);
//   shell_fprintf(shell_backend_uart_get_ptr(), SHELL_VT100_COLOR_DEFAULT, "frame_handle_angle_thr = %.2f\n", para->frame_handle_angle_thr);
//   shell_fprintf(shell_backend_uart_get_ptr(), SHELL_VT100_COLOR_DEFAULT, "floor_handle_angle_mopping_thr_min = %.2f\n", para->floor_handle_angle_mopping_thr_min);
//   shell_fprintf(shell_backend_uart_get_ptr(), SHELL_VT100_COLOR_DEFAULT, "floor_handle_angle_mopping_thr_max = %.2f\n", para->floor_handle_angle_mopping_thr_max);
//   shell_fprintf(shell_backend_uart_get_ptr(), SHELL_VT100_COLOR_DEFAULT, "floor_handle_angle_mopchange_thr = %.2f\n", para->floor_handle_angle_mopchange_thr);
//   shell_fprintf(shell_backend_uart_get_ptr(), SHELL_VT100_COLOR_DEFAULT, "min_mopchange_duration = %ld\n", (uint32_t)para->min_mopchange_duration);
//   shell_fprintf(shell_backend_uart_get_ptr(), SHELL_VT100_COLOR_DEFAULT, "min_mopframeflip_duration = %ld\n", (uint32_t)para->min_mopframeflip_duration);
//   shell_fprintf(shell_backend_uart_get_ptr(), SHELL_VT100_COLOR_DEFAULT, "angle_smooth_factor = %.2f\n", para->angle_smooth_factor);
//   shell_fprintf(shell_backend_uart_get_ptr(), SHELL_VT100_COLOR_DEFAULT, "gyr_smooth_factor = %.2f\n", para->gyr_smooth_factor);
//   shell_fprintf(shell_backend_uart_get_ptr(), SHELL_VT100_COLOR_DEFAULT, "min_mopcycle_duration = %.2f\n", para->min_mopcycle_duration);
//   shell_fprintf(shell_backend_uart_get_ptr(), SHELL_VT100_COLOR_DEFAULT, "max_mopcycle_duration = %.2f\n", para->max_mopcycle_duration);
//   shell_fprintf(shell_backend_uart_get_ptr(), SHELL_VT100_COLOR_DEFAULT, "mop_width = %.2f\n", para->mop_width);
//   shell_fprintf(shell_backend_uart_get_ptr(), SHELL_VT100_COLOR_DEFAULT, "mop_overlap = %.2f\n", para->mop_overlap);
//   shell_fprintf(shell_backend_uart_get_ptr(), SHELL_VT100_COLOR_DEFAULT, "mopcycle_sequence_thr = %.2f\n", para->mopcycle_sequence_thr);
//   shell_fprintf(shell_backend_uart_get_ptr(), SHELL_VT100_COLOR_DEFAULT, "peakfollower_update_delay = %ld\n", (uint32_t)para->peakfollower_update_delay);
//   shell_fprintf(shell_backend_uart_get_ptr(), SHELL_VT100_COLOR_DEFAULT, "mop_rfid_detection_thr = %ld\n", (uint32_t)para->mop_rfid_detection_thr);
//   shell_fprintf(shell_backend_uart_get_ptr(), SHELL_VT100_COLOR_DEFAULT, "mopping_coverage_per_mop_thr = %.2f\n", para->mopping_coverage_per_mop_thr);
//   shell_fprintf(shell_backend_uart_get_ptr(), SHELL_VT100_COLOR_DEFAULT, "algo_flag_verbose = %d\n", para->algo_flag_verbose);
//   shell_fprintf(shell_backend_uart_get_ptr(), SHELL_VT100_COLOR_DEFAULT, "algo_verbose = %d\n", para->algo_verbose);
//   shell_fprintf(shell_backend_uart_get_ptr(), SHELL_VT100_COLOR_DEFAULT, "coap_verbose = %d\n", para->coap_verbose);
//   shell_fprintf(shell_backend_uart_get_ptr(), SHELL_VT100_COLOR_DEFAULT, "events_verbose = %d\n", para->events_verbose);
//   shell_fprintf(shell_backend_uart_get_ptr(), SHELL_VT100_COLOR_DEFAULT, "protobuf_verbose = %d\n", para->protobuf_verbose);
//   shell_fprintf(shell_backend_uart_get_ptr(), SHELL_VT100_COLOR_DEFAULT, "modem_verbose = %d\n", para->modem_verbose);
//   shell_fprintf(shell_backend_uart_get_ptr(), SHELL_VT100_COLOR_DEFAULT, "cloud_sync_interval_idle = %ld\n", para->cloud_sync_interval_idle);
//   shell_fprintf(shell_backend_uart_get_ptr(), SHELL_VT100_COLOR_DEFAULT, "cloud_sync_interval_moving = %ld\n", para->cloud_sync_interval_moving);
//   shell_fprintf(shell_backend_uart_get_ptr(), SHELL_VT100_COLOR_DEFAULT, "flash_verbose = %ld\n", para->flash_verbose);
//   shell_fprintf(shell_backend_uart_get_ptr(), SHELL_VT100_COLOR_DEFAULT, "epc_verbose = %ld\n", para->epc_verbose);
//   shell_fprintf(shell_backend_uart_get_ptr(), SHELL_VT100_COLOR_DEFAULT, "rfid_autoscan = %ld\n", para->rfid_autoscan);
//   shell_fprintf(shell_backend_uart_get_ptr(), SHELL_VT100_COLOR_DEFAULT, "last_seen_locations_auto_reset_time = %ld\n", para->last_seen_locations_auto_reset_time);
//   shell_fprintf(shell_backend_uart_get_ptr(), SHELL_VT100_COLOR_DEFAULT, "log_unkown_tags = %ld\n", para->log_unkown_tags);
//   shell_fprintf(shell_backend_uart_get_ptr(), SHELL_VT100_COLOR_DEFAULT, "last_seen_mop_array_auto_reset_time = %ld\n", para->last_seen_mop_array_auto_reset_time);
//   shell_fprintf(shell_backend_uart_get_ptr(), SHELL_VT100_COLOR_DEFAULT, "enable_blue_dev_led = %ld\n", para->enable_blue_dev_led);
//   shell_fprintf(shell_backend_uart_get_ptr(), SHELL_VT100_COLOR_DEFAULT, "notification_verbose = %ld\n", para->notification_verbose);
//   shell_fprintf(shell_backend_uart_get_ptr(), SHELL_VT100_COLOR_DEFAULT, "notifications_while_usb_connected = %ld\n", para->notifications_while_usb_connected);
//   shell_fprintf(shell_backend_uart_get_ptr(), SHELL_VT100_COLOR_DEFAULT, "enable_rfid_confirmation_blinking = %ld\n", Parameter.enable_rfid_confirmation_blinking);
//   shell_fprintf(shell_backend_uart_get_ptr(), SHELL_VT100_COLOR_DEFAULT, "max_sqm_coveraged_per_mop = %.2f\n", Parameter.max_sqm_coveraged_per_mop);
//   shell_fprintf(shell_backend_uart_get_ptr(), SHELL_VT100_COLOR_DEFAULT, "usb_plugin_reset_time= %ld\n", Parameter.usb_plugin_reset_time);
//   shell_fprintf(shell_backend_uart_get_ptr(), SHELL_VT100_COLOR_DEFAULT, "usb_auto_reset_time= %ld\n", Parameter.usb_auto_reset_time);
//   shell_fprintf(shell_backend_uart_get_ptr(), SHELL_VT100_COLOR_DEFAULT, "anymotion_duration = %d\n", para->anymotion_duration);
//   shell_fprintf(shell_backend_uart_get_ptr(), SHELL_VT100_COLOR_DEFAULT, "anymotion_thr= %d\n", Parameter.anymotion_thr);
//   shell_fprintf(shell_backend_uart_get_ptr(), SHELL_VT100_COLOR_DEFAULT, "motion_reset_time = %d\n", Parameter.motion_reset_time);
//   shell_fprintf(shell_backend_uart_get_ptr(), SHELL_VT100_COLOR_DEFAULT, "fota_enable = %d\n", Parameter.fota_enable);
//   shell_fprintf(shell_backend_uart_get_ptr(), SHELL_VT100_COLOR_DEFAULT, "fully_charged_indicator_time = %d\n", Parameter.fully_charged_indicator_time);
//   shell_fprintf(shell_backend_uart_get_ptr(), SHELL_VT100_COLOR_DEFAULT, "current_shift_mop_check = %d\n", Parameter.current_shift_mop_check);
//   shell_fprintf(shell_backend_uart_get_ptr(), SHELL_VT100_COLOR_DEFAULT, "battery_charge_termination_current = %f\n", Parameter.battery_charge_termination_current);
//   shell_fprintf(shell_backend_uart_get_ptr(), SHELL_VT100_COLOR_DEFAULT, "battery_gauge_sniff_i2c = %d\n", Parameter.battery_gauge_sniff_i2c);
//   shell_fprintf(shell_backend_uart_get_ptr(), SHELL_VT100_COLOR_DEFAULT, "battery_gauge_charge_temp_min = %f\n", Parameter.battery_gauge_charge_temp_min);
//   shell_fprintf(shell_backend_uart_get_ptr(), SHELL_VT100_COLOR_DEFAULT, "battery_gauge_charge_temp_max = %f\n", Parameter.battery_gauge_charge_temp_max);
//   shell_fprintf(shell_backend_uart_get_ptr(), SHELL_VT100_COLOR_DEFAULT, "network_connection_type = %d\n", Parameter.network_connection_type);
//   shell_fprintf(shell_backend_uart_get_ptr(), SHELL_VT100_COLOR_DEFAULT, "mop_id_refresh_timer = %d\n", Parameter.mop_id_refresh_timer);
//   shell_fprintf(shell_backend_uart_get_ptr(), SHELL_VT100_COLOR_DEFAULT, "hit_shock_mag_thr = %f\n", Parameter.hit_shock_mag_thr);
//   shell_fprintf(shell_backend_uart_get_ptr(), SHELL_VT100_COLOR_DEFAULT, "algocontrol_bymag_det = %d\n", Parameter.algocontrol_bymag_det);
//   shell_fprintf(shell_backend_uart_get_ptr(), SHELL_VT100_COLOR_DEFAULT, "mag_det_threshold = %d\n", Parameter.mag_det_threshold);
//   shell_fprintf(shell_backend_uart_get_ptr(), SHELL_VT100_COLOR_DEFAULT, "mag_det_consecutive_samples = %d\n", Parameter.mag_det_consecutive_samples);
//   shell_fprintf(shell_backend_uart_get_ptr(), SHELL_VT100_COLOR_DEFAULT, "notification_test = %d\n", Parameter.notification_test);
//   shell_fprintf(shell_backend_uart_get_ptr(), SHELL_VT100_COLOR_DEFAULT, "rfid_blink_notification = %d\n", Parameter.rfid_blink_notification);
//   shell_fprintf(shell_backend_uart_get_ptr(), SHELL_VT100_COLOR_DEFAULT, "modem_disable = %d\n", Parameter.modem_disable);
//   shell_fprintf(shell_backend_uart_get_ptr(), SHELL_VT100_COLOR_DEFAULT, "rfid_disable = %d\n", Parameter.rfid_disable);
//   shell_fprintf(shell_backend_uart_get_ptr(), SHELL_VT100_COLOR_DEFAULT, "low_bat_threshold = %d\n", Parameter.low_bat_threshold);
//   shell_fprintf(shell_backend_uart_get_ptr(), SHELL_VT100_COLOR_DEFAULT, "binary_search_verbose = %d\n", Parameter.binary_search_verbose);
//   shell_fprintf(shell_backend_uart_get_ptr(), SHELL_VT100_COLOR_DEFAULT, "epc_raw_verbose = %d\n", Parameter.epc_raw_verbose);
//   shell_fprintf(shell_backend_uart_get_ptr(), SHELL_VT100_COLOR_DEFAULT, "mop_verbose = %d\n", Parameter.mop_verbose);
//   shell_fprintf(shell_backend_uart_get_ptr(), SHELL_VT100_COLOR_DEFAULT, "event1statistics_interval = %d\n", Parameter.event1statistics_interval);
}