/**
 * @file usb.c
 * @author Thomas Keilbach | keiltronic GmbH
 * @date 07 Oct 2023
 * @brief This file contains functions to communicate with the pheripherals
 * @version 2.0.0
 */

/*!
 * @defgroup Peripherals
 * @brief This file contains functions to communicate with the pheripherals
 * @{*/
#include "usb.h"

bool charger_plug_in_while_reboot = false;

void USB_PluggedIn(void)
{
  battery_gauge_UpdateData();

  rfid_power_off();
  RFID_TurnOff();
  if ((Total_mops_used >= 1) && (mopping_coverage_per_mop > 1.0))
  {
    NewEvent0x1C(current_room_to_mop_mapping.current_mop_id, mopping_coverage_side1, mopping_coverage_side2);
  }

  /* Add event in event array which is send to cloud in next sync interval */
  NewEvent0x0F(); // USB plugged in

  /* Add event in event array which is send to cloud in next sync interval */
  NewEvent0x11(); // charging started

  /* Update user notification led */
  Notification.next_state = NOTIFICATION_CHARGING;
  battery_low_bat_notification = false;
  System.StatusInputs |= STATUSFLAG_CHG; // Create status entry
  System.MillisecSincecharger_connected = 0;
  motion_state[0] = IDLE_STATE;

  /* Force logic to send data immediately */
  // coap_last_transmission_timer = Parameter.cloud_sync_interval_idle + Parameter.cloud_sync_interval_moving;

  Device_PushRAMToFlash(); // store operating time
}

void USB_Unplugged(void)
{
  System.TotalSteps = 0;
  System.EventNumber = 0;
  battery_avoid_multiple_notifications = false;
  battery_charge_status_delay = BATTERY_GAUGE_CHARGE_STATUS_DELAY;

  System.MillisecSinceUSBDisconnected = 0;
  System.MillisecSincecharger_connected = 0;
  last_seen_mop_auto_clear_timer = 0;
  charger_plug_in_while_reboot = false;

  Notification.next_state = NOTIFICATION_CLEAR;

  algo_reset_variables();
  EPC_Clear_last_seen();
  reset_room_to_mop_mapping();

  /* Clear list of last seen mobs */
  Mop_ClearLastSeenArray();

  //   if (aws_fota_process_state != AWS_FOTA_PROCESS_IDLE)
  //   {
  //     aws_fota_process_state = AWS_FOTA_PROCESS_DISCONNECT;
  //   }

 Device_PushRAMToFlash(); // store operating time

  k_msleep(10);

  /* Clear all stored events from previous shift */
  Event_ClearCompleteFlash(); // clears all stored events in external flash

  /* Add event in event array which is send to cloud in next sync interval */
  NewEvent0x10(); // USB plugged off
  NewEvent0x12(); // charging stopped
}

void USB_CheckConnectionStatus(void)
{
  uint16_t vusb_digit = 0;
  vusb_digit = adc_sample(0);

  /* Check if USB is plugged in based on battery voltage measurement */
  if (vusb_digit >= VUSB_THRES_DIGIT && vusb_digit <= 4095)
  {
    System.charger_connected = true;
    System.StatusInputs |= STATUSFLAG_USB; // Create status entry
  }
  else
  {
    System.charger_connected = false;
    System.StatusInputs &= ~STATUSFLAG_USB; // Create status entry
  }

  /* Detected new USB plug in event */
  if (System.charger_connected_old == false && System.charger_connected == true)
  {
    USB_PluggedIn();
  }

  /* Detected USB unplug event */
  if (System.charger_connected_old == true && System.charger_connected == false)
  {
    USB_Unplugged();
  }

  /* Auto reboot after device was connected to the charger */
  if ((System.charger_connected == true) && (charger_plug_in_while_reboot == false))
  {
    if (System.MillisecSincecharger_connected > (Parameter.usb_plugin_reset_time * 1000))
    {
      rtc_print_debug_timestamp();
      shell_fprintf(shell_backend_uart_get_ptr(), SHELL_VT100_COLOR_YELLOW, "Auto reset after the device was connected to the charger (delay: %d sec)\n", Parameter.usb_plugin_reset_time);

      /* Do a hard reboot */
      Device_PushRAMToFlash();
      // lte_lc_power_off();
      nrf_modem_lib_shutdown();

      k_msleep(1000); // Delay the reboot to give the system enough time to o<uput the debug message on console
      gpio_pin_set_dt(&reset_switch, 1);
    }
  }

  /* Frequent auto reboot if the device is continously connected with the charger */
  if ((System.charger_connected == true) && (charger_plug_in_while_reboot == true))
  {
    if (System.MillisecSinceBoot > (Parameter.usb_auto_reset_time * 1000))
    {
      rtc_print_debug_timestamp();
      shell_fprintf(shell_backend_uart_get_ptr(), SHELL_VT100_COLOR_YELLOW, "Device is connected for more than %d sec to the charger. Auto reboot.\n", Parameter.usb_auto_reset_time);

      /* Do a hard reboot */
      Device_PushRAMToFlash();
      lte_lc_power_off();

      k_msleep(1000); // Delay the reboot to give the system enough time to o<uput the debug message on console
      gpio_pin_set_dt(&reset_switch, 1);
    }
  }

  System.charger_connected_old = System.charger_connected;
}