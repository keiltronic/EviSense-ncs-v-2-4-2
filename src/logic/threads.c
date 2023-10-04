/**
 * @file threads.c
 * @author Thomas Keilbach | keiltronic GmbH
 * @date 11 May 2023 mod by TLU
 * @brief This file contains all threads which are genereated by the application
 * @version 2.0.0
 */

/*!
 * @defgroup Threads
 * @brief This file contains all threads which are genereated by the application
 * @{*/
#include "threads.h"

K_THREAD_STACK_DEFINE(notification_stack_area, STACKSIZE_SMALL);
K_THREAD_STACK_DEFINE(imu_stack_area, STACKSIZE_LARGE);
K_THREAD_STACK_DEFINE(rfid_stack_area, STACKSIZE_LARGE);
K_THREAD_STACK_DEFINE(epc_stack_area, STACKSIZE_LARGE);
K_THREAD_STACK_DEFINE(datalog_stack_area, STACKSIZE_LARGE);
K_THREAD_STACK_DEFINE(battery_area, STACKSIZE_SMALL);
K_THREAD_STACK_DEFINE(lte_and_cloud_area, STACKSIZE_LARGE);
K_THREAD_STACK_DEFINE(aws_fota_area, STACKSIZE_LARGE);
K_THREAD_STACK_DEFINE(fetch_time_area, STACKSIZE_SMALL);
K_THREAD_STACK_DEFINE(datalog_readout_area, STACKSIZE_SMALL);
K_THREAD_STACK_DEFINE(autosave_area, STACKSIZE_SMALL);
K_THREAD_STACK_DEFINE(safety_area, STACKSIZE_SMALL);
K_THREAD_STACK_DEFINE(magnet_detection_area, STACKSIZE_SMALL);
K_THREAD_STACK_DEFINE(seconds_loop_area, STACKSIZE_LARGE);

static struct k_thread notification_data;
static struct k_thread imu_data;
static struct k_thread rfid_data;
static struct k_thread epc_data;
static struct k_thread datalog_data;
static struct k_thread battery_data;
static struct k_thread lte_and_cloud_data;
static struct k_thread aws_fota_data;
static struct k_thread fetch_time_data;
static struct k_thread datalog_readout_data;
static struct k_thread autosave_data;
static struct k_thread safety_data;
static struct k_thread magnet_detection_data;
static struct k_thread seconds_loop_data;

k_tid_t tid;

void safety_thread(void *dummy1, void *dummy2, void *dummy3)
{
  // ARG_UNUSED(dummy1);
  // ARG_UNUSED(dummy2);
  // ARG_UNUSED(dummy3);

  // while (1)
  // {
  //   /* Test I2C bus*/
  //   if ((hibernation_mode == false) && (i2c_test() == false)) // test I2C bus if IMU and led driver gets found
  //   {
  //     /* Print error message on terminal*/
  //     rtc_print_debug_timestamp();
  //     shell_fprintf(shell_backend_uart_get_ptr(), SHELL_VT100_COLOR_RED, "ERROR: I2C bus broken. Trying to recover bus and reboot device.\n");

  //     /* Try to recover SDA and SCL line */
  //     nrfx_twi_twim_bus_recover(GPIO_PIN_SCL, GPIO_PIN_SDA);

  //     /* Print error message on terminal*/
  //     rtc_print_debug_timestamp();
  //     shell_fprintf(shell_backend_uart_get_ptr(), SHELL_VT100_COLOR_YELLOW, "I2C bus recovery done. Rebooting device.\n");

  //     k_msleep(1000);

  //     sys_reboot(0);
  //   }

  //   /* Monitor battery temperature
  //       - read out AIN from battery gauge (battery.temperature holds the value)
  //       - if battery temperature gets higher than 45°C (VARTA 2P/LPP datasheet) then disable charging
  //   */
  //   if (battery_gauge_temperature_progress_delay == 0)
  //   {
  //     if (System.charger_connected == true)
  //     {
  //       if ((battery.Temperature >= Parameter.battery_gauge_charge_temp_min) && (battery.Temperature < Parameter.battery_gauge_charge_temp_max))
  //       {
  //         battery_gauge_temperature_controlled_charge_enable = true;

  //         if (battery_charge_enable_notification == true)
  //         {
  //           /* Enable charge circuit */
  //           gpio_pin_set_raw(gpio_dev, GPIO_PIN_CHG_EN, 0);

  //           battery_charge_enable_notification = false;

  //           rtc_print_debug_timestamp();
  //           shell_fprintf(shell_backend_uart_get_ptr(), SHELL_VT100_COLOR_YELLOW, "Enabled charging - battery temperature is valid range: %3.2f°C (0-45°C).\n", battery.Temperature);
  //         }
  //       }
  //       else
  //       {
  //         battery_gauge_temperature_controlled_charge_enable = false;

  //         if (battery_charge_enable_notification == false)
  //         {
  //           /* Disable charge circuit */
  //           gpio_pin_set_raw(gpio_dev, GPIO_PIN_CHG_EN, 1);

  //           battery_charge_enable_notification = true;

  //           if (System.ChargeStatus == true)
  //           {
  //             /* Add event in event array which is send to cloud in next sync interval */
  //             NewEvent0x12(); // charging stopped

  //             rtc_print_debug_timestamp();
  //             shell_fprintf(shell_backend_uart_get_ptr(), SHELL_VT100_COLOR_RED, "Disabled charging - battery temperature is not valid range: %3.2f°C (0-45°C).\n", battery.Temperature);
  //           }
  //           else
  //           {
  //             rtc_print_debug_timestamp();
  //             shell_fprintf(shell_backend_uart_get_ptr(), SHELL_VT100_COLOR_RED, "battery temperature is not valid range for next charge progress: %3.2f°C (0-45°C).\n", battery.Temperature);
  //           }
  //         }
  //       }
  //     }
  //     else
  //     {
  //       /* Enable charge circuit */
  //       gpio_pin_set_raw(gpio_dev, GPIO_PIN_CHG_EN, 0);

  //       battery_charge_enable_notification = false;
  //       battery_gauge_temperature_controlled_charge_enable = true;
  //     }
  //   }
  //   else
  //   {
  //     battery_gauge_temperature_progress_delay--;
  //   }

  //   k_msleep(1000);
  // }
}

void notification_thread(void *dummy1, void *dummy2, void *dummy3)
{
  // ARG_UNUSED(dummy1);
  // ARG_UNUSED(dummy2);
  // ARG_UNUSED(dummy3);

  // while (1)
  // {
  //   if (datalog_ReadOutisActive == false)
  //   {
  //     notification_update();
  //     led_update();
  //     buzzer_update(&buzzer);
  //   }
  //   k_msleep(1);
  // }
}

static void imu_thread(void *dummy1, void *dummy2, void *dummy3)
{
  // ARG_UNUSED(dummy1);
  // ARG_UNUSED(dummy2);
  // ARG_UNUSED(dummy3);

  // while (1)
  // {
  //   if (datalog_ReadOutisActive == false)
  //   {
  //     imu_fetch_data();            // fetch data from IMU (new samples from  accelerometer, gyrometer and magnetometer)
  //     algorithm_execute_process(); // process main algorithm designed bei Dr. Theofanis Lambrou (check if device is moving, mopping; turn on of/rfid reader, etc..)

  //     if (step_interrupt_triggered)
  //     {
  //       if (Parameter.stepdetection_verbose || Parameter.debug)
  //       {
  //         rtc_print_debug_timestamp();
  //         shell_print(shell_backend_uart_get_ptr(), "Step detected. Total step count: %d, step sequence count: %d", System.TotalSteps, System.Steps);
  //       }
  //       step_interrupt_triggered = false;
  //     }

  //     if (trace_imu_flag)
  //     {
  //       trace_imu();
  //     }

  //     if (trace_imu_reduced_flag)
  //     {
  //       trace_imu_reduced();
  //     }
  //   }
  //   k_msleep(Parameter.imu_interval);
  // }
}

void rfid_thread(void *dummy1, void *dummy2, void *dummy3)
{
  // ARG_UNUSED(dummy1);
  // ARG_UNUSED(dummy2);
  // ARG_UNUSED(dummy3);

  // while (1)
  // {
  //   if ((datalog_ReadOutisActive == false) && (battery_low_bat_notification == false) && (event_clearing_in_progress == false))
  //   {
  //     /* Scan only every x seconds to reduce power consumption */
  //     if (RFID_IsOn == true)
  //     {
  //       if (RFID_ScanEnable == true)
  //       {
  //         rfid_trigger_multi_read();
  //         epc_extract_tags_from_buffer();
  //       }
  //     }
  //   }

  //   // rfid reader trigger interval depents on frame lift state
  //   if (frame_lift_flag[0] == 1)  // frame is lifted
  //   {
  //     k_msleep(Parameter.rfid_interval_lifted);
  //   }
  //   else
  //   {
  //     k_msleep(Parameter.rfid_interval);
  //   }
  // }
}

void epc_thread(void *dummy1, void *dummy2, void *dummy3)
{
  // ARG_UNUSED(dummy1);
  // ARG_UNUSED(dummy2);
  // ARG_UNUSED(dummy3);

  // while (1)
  // {
  //   /* Check new tags if the are listed in epc database and check if it is a mop, tag or room tag */
  //   if ((event_clearing_in_progress == false) && ((System.charger_connected == false) || (Parameter.notifications_while_usb_connected == true)))
  //   {
  //     epc_process_tags();
  //   }

  //   k_msleep(1);
  // }
}

void datalog_thread(void *dummy1, void *dummy2, void *dummy3)
{
  // ARG_UNUSED(dummy1);
  // ARG_UNUSED(dummy2);
  // ARG_UNUSED(dummy3);

  // static uint32_t time_based_logging_timer = 0;

  // while (1)
  // {
  //   if ((datalog_EnableFlag == true) && (datalog_ReadOutisActive == false) && (datalog_EraseActive == false))
  //   {
  //     /* Time based logging */
  //     if (time_based_logging_timer > (Parameter.datalog_Interval / 10))
  //     {
  //       datalog_StoreFrame();
  //       time_based_logging_timer = 0;
  //     }
  //     else
  //     {
  //       time_based_logging_timer++;

  //       /* Event based logging - log a new line every time any system flag changed */
  //       System_CheckStatusFlagChange();
  //     }
  //   }
  //   else
  //   {
  //     time_based_logging_timer = 0;
  //   }
  //   k_msleep(10);
  // }
}

void seconds_loop_thread(void *dummy1, void *dummy2, void *dummy3)
{
  ARG_UNUSED(dummy1);
  ARG_UNUSED(dummy2);
  ARG_UNUSED(dummy3);

  while (1)
  {
    // wdt_reset(); // blocks watchdog activation

    // /* Update step detection */
    // if (datalog_ReadOutisActive == false)
    // {
    //   step_sequence_detection();
    //   motion_detection();
    // }

    // /* Clear the last seen room ids after some time */
    // if (last_seen_array_auto_clear_timer >= Parameter.last_seen_locations_auto_reset_time)
    // {
    //   clear_last_seen_room_id_array(EPC_LAST_SEEN_COUNT);
    //   last_seen_array_auto_clear_timer = 0;
    // }
    // else
    // {
    //   last_seen_array_auto_clear_timer++;
    // }

    // /* Clear the last seen mop ids after some time */
    // if (last_seen_mop_auto_clear_timer >= Parameter.last_seen_mop_array_auto_reset_time)
    // {
    //   Mop_ClearLastSeenArray();
    //   last_seen_mop_auto_clear_timer = 0;
    // }
    // else
    // {
    //   last_seen_mop_auto_clear_timer++;
    // }

    // /* Update timer */
    // if (initial_time_update == true)
    // {
    //   coap_last_transmission_timer++;
    // }

    // /* Sequence for notification demo */
    // if (Parameter.notification_test == true)
    // {
    //   update_notification_demo();
    // }

    k_msleep(1000);
  }
}

void magnet_detection_thread(void *dummy1, void *dummy2, void *dummy3)
{
  // ARG_UNUSED(dummy1);
  // ARG_UNUSED(dummy2);
  // ARG_UNUSED(dummy3);

  // while (1)
  // {
  //   /* Automatic device on frame detection */
  //   if (Parameter.algocontrol_bymag_det == 1)
  //   {
  //     if (magnet_detection() > 0)
  //     {
  //       if (algorithm_lock == true)
  //       {
  //         algorithm_lock = false;

  //         rtc_print_debug_timestamp();
  //         shell_fprintf(shell_backend_uart_get_ptr(), SHELL_VT100_COLOR_YELLOW, "Device was placed on frame (magnet detected)\n");
  //       }
  //     }
  //     else if (magnet_detection() == 0)
  //     {
  //       if (algorithm_lock == false)
  //       {
  //         algorithm_lock = true;

  //         rtc_print_debug_timestamp();
  //         shell_fprintf(shell_backend_uart_get_ptr(), SHELL_VT100_COLOR_YELLOW, "Device not installed on frame (no magnet detected)\n");
  //       }
  //     }
  //     else
  //     {
  //       /* do nothing */
  //     }
  //   }

  //   k_msleep(100);
  // }
}

void battery_thread(void *dummy1, void *dummy2, void *dummy3)
{
  // ARG_UNUSED(dummy1);
  // ARG_UNUSED(dummy2);
  // ARG_UNUSED(dummy3);

  // while (1)
  // {
  //   if (datalog_ReadOutisActive == false)
  //   {
  //     USB_CheckConnectionStatus();
  //     battery_gauge_UpdateData();

  //     if (battery_charge_status_delay == 0)
  //     {
  //       battery_gauge_CheckChargeStatus();
  //       battery_charge_status_delay = BATTERY_GAUGE_CHARGE_STATUS_DELAY;
  //     }
  //     else
  //     {
  //       battery_charge_status_delay--;
  //     }
  //     battery_gauge_CheckLowBat();
  //   }
  //   k_msleep(100);
  // }
}

void fetch_time_thread(void *dummy1, void *dummy2, void *dummy3)
{
  // ARG_UNUSED(dummy1);
  // ARG_UNUSED(dummy2);
  // ARG_UNUSED(dummy3);

  // bool time_update_done = 1; // not done yet

  // while (1)
  // {
  //   if (datalog_ReadOutisActive == false)
  //   {
  //     /* Read date and time */
  //     // if (modem.connection_stat == true)
  //     // {
  //     //   if (time_update_done == 1)
  //     //   {
  //     //     time_update_done = rtc_fetch_date_time();
  //     //   }
  //     // }
  //   }
  //   k_msleep(1000);
  // }
}

void mobile_connection_thread(void *dummy1, void *dummy2, void *dummy3)
{
  // ARG_UNUSED(dummy1);
  // ARG_UNUSED(dummy2);
  // ARG_UNUSED(dummy3);

  // int16_t err = 0;

  // /* Turn modem off */
  // lte_lc_power_off();
  // k_msleep(500);

  // // if (Parameter.modem_disable == false)
  // // {
  // //   /* Configure modem to use either LTE-M or NB-IoT */
  // //   if (Parameter.network_connection_type == NB_IOT)
  // //   {
  // //     err = lte_lc_system_mode_set(LTE_LC_SYSTEM_MODE_NBIOT);

  // //     rtc_print_debug_timestamp();
  // //     shell_fprintf(shell_backend_uart_get_ptr(), SHELL_VT100_COLOR_YELLOW, "Note: Device will use NB-IoT connection. It may take several minutes for a NB-IoT connection to be established successfully\n");

  // //     if (err)
  // //     {
  // //       rtc_print_debug_timestamp();
  // //       shell_fprintf(shell_backend_uart_get_ptr(), SHELL_VT100_COLOR_RED, "Error: Setting modem to NB-IoT failed\n");
  // //     }
  // //   }
  // //   else
  // //   {
  // //     err = lte_lc_system_mode_set(LTE_LC_SYSTEM_MODE_LTEM);

  // //     if (err)
  // //     {
  // //       rtc_print_debug_timestamp();
  // //       shell_fprintf(shell_backend_uart_get_ptr(), SHELL_VT100_COLOR_RED, "Error: Setting modem to LTE-M failed\n");
  // //     }
  // //   }
  // //   k_msleep(100);

  // //   /* Turn modem on - it will automatically search for networks*/
  // //   lte_lc_normal();
  // // }

  // while (1)
  // {
  //   // /* Update registration status */
  //   // if (Parameter.modem_disable == false)
  //   // {
  //   //   modem_update_registration_status(); // This function needs 1sec to execute

  //   //   ///////////////////////// MANAGMENT TO SEND DATA TO CLOUD //////////////////////////////////////////

  //   //   /* Send CoAP messages if cloud connection can be establised and device is in IDLE or MOVING state (while MOPPING no data gets send, credentials gets testet in cloud_init function) */
  //   //   if (initial_time_update == true && modem.connection_stat == true && event_simulation_in_progress == false)
  //   //   {
  //   //     System.StatusOutputs |= STATUSFLAG_IC;

  //   //     if ((motion_state[0] == MOVING_STATE) && (coap_last_transmission_timer >= Parameter.cloud_sync_interval_moving))
  //   //     {
  //   //       cloud_SendUsageUpdateObject();
  //   //     }
  //   //     else if ((motion_state[0] == IDLE_STATE) && (coap_last_transmission_timer >= Parameter.cloud_sync_interval_idle))
  //   //     {
  //   //       cloud_SendUsageUpdateObject();
  //   //     }
  //   //   }
  //   //   else
  //   //   {
  //   //     System.StatusOutputs &= ~STATUSFLAG_IC;
  //   //     coap_last_transmission_timer = 0;
  //   //   }

  //   //   /* Manually send the current protobuf if triggered */
  //   //   if ((modem.connection_stat == true) && (trigger_tx == true))
  //   //   {
  //   //     cloud_SendUsageUpdateObject();
  //   //     trigger_tx = false;
  //   //   }

  //   //   /* Connection to FOTA server should be only active for time x after the device was reboot while USB plugged in. */
  //   //   if (fota_is_connected == true)
  //   //   {
  //   //     if ((fota_connection_timer == 0) && (fota_download_in_progress == false))
  //   //     {
  //   //       aws_fota_process_state = AWS_FOTA_PROCESS_DISCONNECT;
  //   //       fota_connection_timer = FOTA_CONNECTION_DURATION;
  //   //       fota_is_connected = false;
  //   //       fota_reboot_while_usb_connected = false;

  //   //       if (pcb_test_is_running == false)
  //   //       {
  //   //         rtc_print_debug_timestamp();
  //   //         shell_fprintf(shell_backend_uart_get_ptr(), SHELL_VT100_COLOR_BLUE, "Automatically disconneced FOTA service from server %d sec after boot\n", FOTA_CONNECTION_DURATION);
  //   //       }
  //   //     }
  //   //     else
  //   //     {
  //   //       fota_connection_timer--;
  //   //     }
  //   //   }
  //   //   else
  //   //   {
  //   //     fota_connection_timer = FOTA_CONNECTION_DURATION;
  //   //   }
  //   // }
  //   k_msleep(1);
  // }
}

void aws_fota_thread(void *dummy1, void *dummy2, void *dummy3)
{
//   /****** AWS FOTA MAIN FUNCTION STUFF ***************************************************************/
//   /* https://developer.nordicsemi.com/nRF_Connect_SDK/doc/1.4.2/nrf/samples/nrf9160/aws_fota/README.html#aws-fota-sample */

//   while (1)
//   {
//     /* Update FOTA process state machine*/
//  //   aws_fota_statemachine();

//     k_msleep(100);
//   }
}

void datalog_readout_thread(void *dummy1, void *dummy2, void *dummy3)
{
  // ARG_UNUSED(dummy1);
  // ARG_UNUSED(dummy2);
  // ARG_UNUSED(dummy3);

  // while (1)
  // {
  //   if (datalog_ReadOutisActive == true)
  //   {
  //     datalog_GetData();
  //   }
  //   k_msleep(1);
  // }
}

void autosave_thread(void *dummy1, void *dummy2, void *dummy3)
{
  // ARG_UNUSED(dummy1);
  // ARG_UNUSED(dummy2);
  // ARG_UNUSED(dummy3);

  // while (1)
  // {
  //   if (datalog_ReadOutisActive == false)
  //   {
  //     k_msleep(3600000);
  //     Device_PushRAMToFlash();

  //     rtc_print_debug_timestamp();
  //     shell_fprintf(shell_backend_uart_get_ptr(), SHELL_VT100_COLOR_DEFAULT, "Autosave time expired. Saved device information in memory.\n");
  //   }
  //   else
  //   {
  //     k_msleep(1000);
  //   }
  // }
}

void init_threads(void)
{
  tid = k_thread_create(&notification_data, notification_stack_area, STACKSIZE_SMALL, notification_thread, NULL, NULL, NULL, K_PRIO_PREEMPT(0), 0, K_NO_WAIT);
  k_thread_name_set(tid, "notification");

  tid = k_thread_create(&imu_data, imu_stack_area, STACKSIZE_LARGE, imu_thread, NULL, NULL, NULL, K_PRIO_PREEMPT(0), 0, K_NO_WAIT);
  k_thread_name_set(tid, "imu");

  tid = k_thread_create(&rfid_data, rfid_stack_area, STACKSIZE_LARGE, rfid_thread, NULL, NULL, NULL, K_PRIO_PREEMPT(0), 0, K_NO_WAIT);
  k_thread_name_set(tid, "rfid");

  tid = k_thread_create(&epc_data, epc_stack_area, STACKSIZE_LARGE, epc_thread, NULL, NULL, NULL, K_PRIO_PREEMPT(0), 0, K_NO_WAIT);
  k_thread_name_set(tid, "epc-thread");

  tid = k_thread_create(&datalog_data, datalog_stack_area, STACKSIZE_LARGE, datalog_thread, NULL, NULL, NULL, K_PRIO_PREEMPT(1), 0, K_NO_WAIT);
  k_thread_name_set(tid, "datalog");

  tid = k_thread_create(&battery_data, battery_area, STACKSIZE_SMALL, battery_thread, NULL, NULL, NULL, K_PRIO_PREEMPT(2), 0, K_NO_WAIT);
  k_thread_name_set(tid, "battery");

  tid = k_thread_create(&lte_and_cloud_data, lte_and_cloud_area, STACKSIZE_LARGE, mobile_connection_thread, NULL, NULL, NULL, K_PRIO_PREEMPT(2), 0, K_MSEC(500));
  k_thread_name_set(tid, "lte-and-cloud");

  tid = k_thread_create(&aws_fota_data, aws_fota_area, STACKSIZE_LARGE, aws_fota_thread, NULL, NULL, NULL, K_PRIO_PREEMPT(2), 0, K_MSEC(3000));
  k_thread_name_set(tid, "aws_fota");

  tid = k_thread_create(&fetch_time_data, fetch_time_area, STACKSIZE_SMALL, fetch_time_thread, NULL, NULL, NULL, K_PRIO_PREEMPT(3), 0, K_NO_WAIT);
  k_thread_name_set(tid, "time-fetch");

  tid = k_thread_create(&datalog_readout_data, datalog_readout_area, STACKSIZE_SMALL, datalog_readout_thread, NULL, NULL, NULL, K_PRIO_PREEMPT(0), 0, K_NO_WAIT);
  k_thread_name_set(tid, "datalog-readout");

  tid = k_thread_create(&autosave_data, autosave_area, STACKSIZE_SMALL, autosave_thread, NULL, NULL, NULL, K_PRIO_PREEMPT(3), 0, K_NO_WAIT);
  k_thread_name_set(tid, "autosave");

  tid = k_thread_create(&safety_data, safety_area, STACKSIZE_SMALL, safety_thread, NULL, NULL, NULL, K_PRIO_PREEMPT(1), 0, K_MSEC(1000));
  k_thread_name_set(tid, "safty-thread");

  tid = k_thread_create(&magnet_detection_data, magnet_detection_area, STACKSIZE_SMALL, magnet_detection_thread, NULL, NULL, NULL, K_PRIO_PREEMPT(1), 0, K_MSEC(1000));
  k_thread_name_set(tid, "magnet-detection-thread");

  tid = k_thread_create(&seconds_loop_data, seconds_loop_area, STACKSIZE_LARGE, seconds_loop_thread, NULL, NULL, NULL, K_PRIO_PREEMPT(1), 0, K_NO_WAIT);
  k_thread_name_set(tid, "seconds-loop");
}
