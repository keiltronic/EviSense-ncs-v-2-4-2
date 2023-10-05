/**
 * @file notification.c
 * @author Thomas Keilbach | keiltronic GmbH
 * @date 05 Jun 2022
 * @brief This files combines all user notifications (led and buzzer behaviour)
 * @version 1.0.0
 */

/*!
 * @defgroup Notifications
 * @brief This files contains function to communicate with the modem firmware
 * @{*/
#include "notification.h"

NOTIFICATION Notification;
uint8_t ActionMatrixArray[255];

uint32_t notification_demo_delay = 0;
uint32_t notification_demo_timer = 0;
uint8_t notification_test_state = 0;

void notification_init(void)
{
  /* Init notification priority state machine*/
  Notification.current_state = NOTIFICATION_IDLE;
  Notification.next_state = NOTIFICATION_IDLE;
  Notification.current_priority = NOTIFICATION_PRIORITY_LEVEL_LOWEST;
  Notification.next_priority = NOTIFICATION_PRIORITY_LEVEL_LOWEST;
}

void notification_init_action_matrix(void)
{
  /* Set all entries to default value */
  memset(ActionMatrixArray, 0x00, 255);

  /* Init matrix with default settings - Action matrix index refers to event number, e.g. ActionMatrixArray[EventID] = NOTIFICATION_ID */
  ActionMatrixArray[0x01] = 0;                 // New movement pattern detected
  ActionMatrixArray[0x02] = NOTIFICATION_0x09; // Detects Mop change vis mop EPC
  ActionMatrixArray[0x03] = 0;
  ActionMatrixArray[0x04] = 0;                 // Another room detected
  ActionMatrixArray[0x05] = 0;                 // NOTIFICATION_0x07;  //Detects wrong mop color/type when entering a room
  ActionMatrixArray[0x06] = 0;                 // NOTIFICATION_0x07;  //Dirty mop (used in another room) when entering a room
  ActionMatrixArray[0x07] = NOTIFICATION_0x08; // New location tag detected
  ActionMatrixArray[0x09] = NOTIFICATION_0x06; // Battery charge low
  ActionMatrixArray[0x0B] = 0;                 // Connection up
  ActionMatrixArray[0x0C] = 0;                 // Connection down
  ActionMatrixArray[0x0D] = NOTIFICATION_0x03; // Button pressed
  ActionMatrixArray[0x0F] = 0;                 // USB plugged in
  ActionMatrixArray[0x10] = 0;                 // USB plugged out
  ActionMatrixArray[0x11] = 0;                 // Charging started
  ActionMatrixArray[0x12] = 0;                 // Charging stopped
  ActionMatrixArray[0x13] = NOTIFICATION_0x02; // Power on
  ActionMatrixArray[0x17] = 0;                 // Hit shock
  ActionMatrixArray[0x18] = 0;                 // Frame side flip
  ActionMatrixArray[0x19] = NOTIFICATION_0x07; // Inappropriate mop is used in the room in mopping state
  ActionMatrixArray[0x1A] = 0;                 // Inappropriate mop is near the room
  ActionMatrixArray[0x1B] = 0;                 // NOTIFICATION_0x09;  //Detects mop change based only on motion
}

void notification_set_priority(uint8_t level)
{
  Notification.next_priority = level;
}

/*!
 * @brief This is the function description
 * @note Priorities will be the following - from low (blue) to high (red):
 * 1. Blue blink: Confirmation of wall tag reading if mop is right
 * 2. Green: Confirmation of fresh mop installation
 * 3. Yellow: user should flip the frame because one side has reached the Max_sqm/2
 * 4. Red: user must change the mop (wrong mop, used mop, Max_sqm is reached)
 *
 * @param: none
 * @return: Returns true (1) if next priority level is qual or higher than the current priority, else false (0)
 */
uint8_t notification_check_priority(void)
{
  if (Notification.current_priority <= Notification.next_priority)
  {
    Notification.current_priority = Notification.next_priority;
    return 1;
  }
  else
  {
    return 0;
  }
}

void notification_update(void)
{
  /* Statemachine for executing next user indicator */
  switch (Notification.current_state)
  {

  case NOTIFICATION_IDLE:
    break;

  case NOTIFICATION_0x01:

    /* Set priority level for this user notification */
    notification_set_priority(NOTIFICATION_PRIORITY_LEVEL_RED);

    /* Check if this user notification can be triggered or if currently another one with higher priority is running. In this case skip executing this user notification*/
    if (notification_check_priority())
    {
      if ((System.charger_connected == false) || (Parameter.notifications_while_usb_connected == true))
      {
        rgb_led.blink_on_time = 200;  // ms
        rgb_led.blink_off_time = 200; // ms
        rgb_led.pos_slope = 0;        // ms
        rgb_led.neg_slope = 0;        // ms
        rgb_led.repeats = 4;
        rgb_led.red_value = 255;
        rgb_led.green_value = 0;
        rgb_led.blue_value = 0;
        rgb_led.brightness_value = 255;
        led_next_state = FLASH;

        buzzer.status = BUZZER_BEEP;
        buzzer.frequency = BUZZER_RES_FREQ;
        buzzer.duty_cycle = Parameter.buzzer_duty_cycle;
        buzzer.beep_on_time = 1200;
        buzzer.beep_off_time = 1;
        buzzer.delay = 0;
        buzzer.beep_cycles = 1;
        buzzer.burst_repeat_periode = 0;
        buzzer.burst_repeat_cycles = 1;
        set_buzzer(&buzzer);

        /* Print debug messages if enabled */
        if ((Parameter.notification_verbose == true) && (pcb_test_is_running == false))
        {
          rtc_print_debug_timestamp();
          shell_fprintf(shell_backend_uart_get_ptr(), SHELL_VT100_COLOR_MAGENTA, "Triggered notification 0x01\n");
        }
      }
    }
    else
    {
      /* Print debug messages if enabled */
      if ((Parameter.notification_verbose == true) && (pcb_test_is_running == false))
      {
        rtc_print_debug_timestamp();
        shell_fprintf(shell_backend_uart_get_ptr(), SHELL_VT100_COLOR_RED, "Could not trigger notification 0x01 since a higher priority user notification is running (0x%02X).\n", Notification.current_priority);
      }
    }

    /* Unblock indicator by switching to idle */
    Notification.next_state = NOTIFICATION_IDLE;
    break;

  case NOTIFICATION_0x02:

    /* Set priority level for this user notification */
    notification_set_priority(NOTIFICATION_PRIORITY_LEVEL_GREEN);

    /* Check if this user notification can be triggered or if currently another one with higher priority is running. In this case skip executing this user notification*/
    if (notification_check_priority())
    {
      if ((System.charger_connected == false) || (Parameter.notifications_while_usb_connected == true))
      {
        rgb_led.blink_on_time = 500; // ms
        rgb_led.blink_off_time = 1;  // ms
        rgb_led.pos_slope = 0;       // ms
        rgb_led.neg_slope = 0;       // ms
        rgb_led.repeats = 1;
        rgb_led.red_value = 0;
        rgb_led.green_value = 255;
        rgb_led.blue_value = 0;
        rgb_led.brightness_value = 10;
        led_next_state = FLASH;

        /* Print debug messages if enabled */
        if ((Parameter.notification_verbose == true) && (pcb_test_is_running == false))
        {
          rtc_print_debug_timestamp();
          shell_fprintf(shell_backend_uart_get_ptr(), SHELL_VT100_COLOR_MAGENTA, "Triggered notification 0x02\n");
        }
      }
    }
    else
    {
      /* Print debug messages if enabled */
      if ((Parameter.notification_verbose == true) && (pcb_test_is_running == false))
      {
        rtc_print_debug_timestamp();
        shell_fprintf(shell_backend_uart_get_ptr(), SHELL_VT100_COLOR_RED, "Could not trigger notification 0x02 since a higher priority user notification is running (0x%02X).\n", Notification.current_priority);
      }
    }

    /* Unblock indicator by switching to idle */
    Notification.next_state = NOTIFICATION_IDLE;
    break;

  case NOTIFICATION_0x03:

    /* Set priority level for this user notification */
    notification_set_priority(NOTIFICATION_PRIORITY_LEVEL_BLUE);

    /* Check if this user notification can be triggered or if currently another one with higher priority is running. In this case skip executing this user notification*/
    if (notification_check_priority())
    {
      if ((System.charger_connected == false) || (Parameter.notifications_while_usb_connected == true))
      {
        rgb_led.blink_on_time = 200;  // ms
        rgb_led.blink_off_time = 200; // ms
        rgb_led.pos_slope = 0;        // ms
        rgb_led.neg_slope = 0;        // ms
        rgb_led.repeats = 2;
        rgb_led.red_value = 0;
        rgb_led.green_value = 0;
        rgb_led.blue_value = 255;
        rgb_led.brightness_value = 255;
        led_next_state = FLASH;

        /* Print debug messages if enabled */
        if ((Parameter.notification_verbose == true) && (pcb_test_is_running == false))
        {
          rtc_print_debug_timestamp();
          shell_fprintf(shell_backend_uart_get_ptr(), SHELL_VT100_COLOR_MAGENTA, "Triggered notification 0x03\n");
        }
      }
    }
    else
    {
      /* Print debug messages if enabled */
      if ((Parameter.notification_verbose == true) && (pcb_test_is_running == false))
      {
        rtc_print_debug_timestamp();
        shell_fprintf(shell_backend_uart_get_ptr(), SHELL_VT100_COLOR_RED, "Could not trigger notification 0x03 since a higher priority user notification is running (0x%02X).\n", Notification.current_priority);
      }
    }

    /* Unblock indicator by switching to idle */
    Notification.next_state = NOTIFICATION_IDLE;
    break;

  case NOTIFICATION_0x04:

    /* Set priority level for this user notification */
    notification_set_priority(NOTIFICATION_PRIORITY_LEVEL_BLUE);

    /* Check if this user notification can be triggered or if currently another one with higher priority is running. In this case skip executing this user notification*/
    if (notification_check_priority())
    {
      if ((System.charger_connected == false) || (Parameter.notifications_while_usb_connected == true))
      {
        rgb_led.blink_on_time = 60000; // ms
        rgb_led.blink_off_time = 1;    // ms
        rgb_led.pos_slope = 0;         // ms
        rgb_led.neg_slope = 0;         // ms
        rgb_led.repeats = 1;
        rgb_led.red_value = 255;
        rgb_led.green_value = 0;
        rgb_led.blue_value = 0;
        rgb_led.brightness_value = 255;
        led_next_state = FLASH;

        /* Print debug messages if enabled */
        if ((Parameter.notification_verbose == true) && (pcb_test_is_running == false))
        {
          rtc_print_debug_timestamp();
          shell_fprintf(shell_backend_uart_get_ptr(), SHELL_VT100_COLOR_MAGENTA, "Triggered notification 0x04\n");
        }
      }
    }
    else
    {
      /* Print debug messages if enabled */
      if ((Parameter.notification_verbose == true) && (pcb_test_is_running == false))
      {
        rtc_print_debug_timestamp();
        shell_fprintf(shell_backend_uart_get_ptr(), SHELL_VT100_COLOR_RED, "Could not trigger notification 0x04 since a higher priority user notification is running (0x%02X).\n", Notification.current_priority);
      }
    }
    /* Unblock indicator by switching to idle */
    Notification.next_state = NOTIFICATION_IDLE;
    break;

  case NOTIFICATION_0x05:

    /* Set priority level for this user notification */
    notification_set_priority(NOTIFICATION_PRIORITY_LEVEL_BLUE);

    /* Check if this user notification can be triggered or if currently another one with higher priority is running. In this case skip executing this user notification*/
    if (notification_check_priority())
    {
      if ((System.charger_connected == false) || (Parameter.notifications_while_usb_connected == true))
      {
        rgb_led.blink_on_time = 300000; // ms
        rgb_led.blink_off_time = 1;     // ms
        rgb_led.pos_slope = 0;          // ms
        rgb_led.neg_slope = 0;          // ms
        rgb_led.repeats = 1;
        rgb_led.red_value = 0;
        rgb_led.green_value = 0;
        rgb_led.blue_value = 255;
        rgb_led.brightness_value = 255;
        led_next_state = FLASH;

        /* Print debug messages if enabled */
        if ((Parameter.notification_verbose == true) && (pcb_test_is_running == false))
        {
          rtc_print_debug_timestamp();
          shell_fprintf(shell_backend_uart_get_ptr(), SHELL_VT100_COLOR_MAGENTA, "Triggered notification 0x05\n");
        }
      }
    }
    else
    {
      /* Print debug messages if enabled */
      if ((Parameter.notification_verbose == true) && (pcb_test_is_running == false))
      {
        rtc_print_debug_timestamp();
        shell_fprintf(shell_backend_uart_get_ptr(), SHELL_VT100_COLOR_RED, "Could not trigger notification 0x05 since a higher priority user notification is running (0x%02X).\n", Notification.current_priority);
      }
    }

    /* Unblock indicator by switching to idle */
    Notification.next_state = NOTIFICATION_IDLE;
    break;

  case NOTIFICATION_0x06:

    /* Set priority level for this user notification */
    notification_set_priority(NOTIFICATION_PRIORITY_LEVEL_YELLOW);

    /* Check if this user notification can be triggered or if currently another one with higher priority is running. In this case skip executing this user notification*/
    if (notification_check_priority())
    {
      if ((System.charger_connected == false) || (Parameter.notifications_while_usb_connected == true))
      {
        rgb_led.blink_on_time = 1000; // ms
        rgb_led.blink_off_time = 1;   // ms
        rgb_led.pos_slope = 1000;     // ms
        rgb_led.neg_slope = 1000;     // ms
        rgb_led.repeats = 1;
        rgb_led.red_value = 255;
        rgb_led.green_value = 255;
        rgb_led.blue_value = 255;
        rgb_led.brightness_value = 50;
        led_next_state = FLASH;

        /* Print debug messages if enabled */
        if ((Parameter.notification_verbose == true) && (pcb_test_is_running == false))
        {
          rtc_print_debug_timestamp();
          shell_fprintf(shell_backend_uart_get_ptr(), SHELL_VT100_COLOR_MAGENTA, "Triggered notification 0x06\n");
        }
      }
    }
    else
    {
      /* Print debug messages if enabled */
      if ((Parameter.notification_verbose == true) && (pcb_test_is_running == false))
      {
        rtc_print_debug_timestamp();
        shell_fprintf(shell_backend_uart_get_ptr(), SHELL_VT100_COLOR_RED, "Could not trigger notification 0x06 since a higher priority user notification is running (0x%02X).\n", Notification.current_priority);
      }
    }

    /* Unblock indicator by switching to idle */
    Notification.next_state = NOTIFICATION_IDLE;
    break;

  case NOTIFICATION_0x07:

    /* Set priority level for this user notification */
    notification_set_priority(NOTIFICATION_PRIORITY_LEVEL_RED);

    /* Check if this user notification can be triggered or if currently another one with higher priority is running. In this case skip executing this user notification*/
    if (notification_check_priority())
    {
      if ((System.charger_connected == false) || (Parameter.notifications_while_usb_connected == true))
      {
        rgb_led.blink_on_time = 20000; // ms
        rgb_led.blink_off_time = 1;    // ms
        rgb_led.pos_slope = 0;         // ms
        rgb_led.neg_slope = 0;         // ms
        rgb_led.repeats = 1;
        rgb_led.red_value = 255;
        rgb_led.green_value = 0;
        rgb_led.blue_value = 0;
        rgb_led.brightness_value = 255;
        led_next_state = FLASH;

        /* Print debug messages if enabled */
        if ((Parameter.notification_verbose == true) && (pcb_test_is_running == false))
        {
          rtc_print_debug_timestamp();
          shell_fprintf(shell_backend_uart_get_ptr(), SHELL_VT100_COLOR_MAGENTA, "Triggered notification 0x07\n");
        }
      }
    }
    else
    {
      /* Print debug messages if enabled */
      if ((Parameter.notification_verbose == true) && (pcb_test_is_running == false))
      {
        rtc_print_debug_timestamp();
        shell_fprintf(shell_backend_uart_get_ptr(), SHELL_VT100_COLOR_RED, "Could not trigger notification 0x07 since a higher priority user notification is running (0x%02X).\n", Notification.current_priority);
      }
    }

    /* Unblock indicator by switching to idle */
    Notification.next_state = NOTIFICATION_IDLE;
    break;

  case NOTIFICATION_0x08:

    /* Set priority level for this user notification */
    notification_set_priority(NOTIFICATION_PRIORITY_LEVEL_BLUE);

    /* Check if this user notification can be triggered or if currently another one with higher priority is running. In this case skip executing this user notification*/
    if (notification_check_priority())
    {
      if ((System.charger_connected == false) || (Parameter.notifications_while_usb_connected == true))
      {
        rgb_led.blink_on_time = 180; // ms
        rgb_led.blink_off_time = 5;  // ms
        rgb_led.pos_slope = 0;       // ms
        rgb_led.neg_slope = 0;       // ms
        rgb_led.repeats = 1;
        rgb_led.red_value = 0;
        rgb_led.green_value = 0;
        rgb_led.blue_value = 255;
        rgb_led.brightness_value = 255;
        led_next_state = FLASH;

        /* Print debug messages if enabled */
        if ((Parameter.notification_verbose == true) && (pcb_test_is_running == false))
        {
          rtc_print_debug_timestamp();
          shell_fprintf(shell_backend_uart_get_ptr(), SHELL_VT100_COLOR_MAGENTA, "Triggered notification 0x08\n");
        }
      }
    }
    else
    {
      /* Print debug messages if enabled */
      if ((Parameter.notification_verbose == true) && (pcb_test_is_running == false))
      {
        rtc_print_debug_timestamp();
        shell_fprintf(shell_backend_uart_get_ptr(), SHELL_VT100_COLOR_RED, "Could not trigger notification 0x08 since a higher priority user notification is running (0x%02X).\n", Notification.current_priority);
      }
    }

    /* Unblock indicator by switching to idle */
    Notification.next_state = NOTIFICATION_IDLE;
    break;

  case NOTIFICATION_0x09:

    /* Set priority level for this user notification */
    notification_set_priority(NOTIFICATION_PRIORITY_LEVEL_GREEN);

    /* Check if this user notification can be triggered or if currently another one with higher priority is running. In this case skip executing this user notification*/
    if (notification_check_priority())
    {
      if ((System.charger_connected == false) || (Parameter.notifications_while_usb_connected == true))
      {
        rgb_led.blink_on_time = 500;  // ms
        rgb_led.blink_off_time = 500; // ms
        rgb_led.pos_slope = 0;        // ms
        rgb_led.neg_slope = 0;        // ms
        rgb_led.repeats = 5;
        rgb_led.red_value = 0;
        rgb_led.green_value = 255;
        rgb_led.blue_value = 0;
        rgb_led.brightness_value = 255;
        led_next_state = FLASH;

        /* Print debug messages if enabled */
        if ((Parameter.notification_verbose == true) && (pcb_test_is_running == false))
        {
          rtc_print_debug_timestamp();
          shell_fprintf(shell_backend_uart_get_ptr(), SHELL_VT100_COLOR_MAGENTA, "Triggered notification 0x09\n");
        }
      }
    }
    else
    {
      /* Print debug messages if enabled */
      if ((Parameter.notification_verbose == true) && (pcb_test_is_running == false))
      {
        rtc_print_debug_timestamp();
        shell_fprintf(shell_backend_uart_get_ptr(), SHELL_VT100_COLOR_RED, "Could not trigger notification 0x09 since a higher priority user notification is running (0x%02X).\n", Notification.current_priority);
      }
    }

    /* Unblock indicator by switching to idle */
    Notification.next_state = NOTIFICATION_IDLE;
    break;

  case NOTIFICATION_0x0A:

    /* Set priority level for this user notification */
    notification_set_priority(NOTIFICATION_PRIORITY_LEVEL_YELLOW);

    /* Check if this user notification can be triggered or if currently another one with higher priority is running. In this case skip executing this user notification*/
    if (notification_check_priority())
    {
      if ((System.charger_connected == false) || (Parameter.notifications_while_usb_connected == true))
      {
        rgb_led.blink_on_time = 200;  // ms
        rgb_led.blink_off_time = 200; // ms
        rgb_led.pos_slope = 0;        // ms
        rgb_led.neg_slope = 0;        // ms
        rgb_led.repeats = 8;
        rgb_led.red_value = 255;
        rgb_led.green_value = 255;
        rgb_led.blue_value = 0;
        rgb_led.brightness_value = 60;

        led_next_state = FLASH;

        /* Print debug messages if enabled */
        if ((Parameter.notification_verbose == true) && (pcb_test_is_running == false))
        {
          rtc_print_debug_timestamp();
          shell_fprintf(shell_backend_uart_get_ptr(), SHELL_VT100_COLOR_MAGENTA, "Triggered notification 0x0A\n");
        }
      }
    }
    else
    {
      /* Print debug messages if enabled */
      if ((Parameter.notification_verbose == true) && (pcb_test_is_running == false))
      {
        rtc_print_debug_timestamp();
        shell_fprintf(shell_backend_uart_get_ptr(), SHELL_VT100_COLOR_RED, "Could not trigger notification 0x0A since a higher priority user notification is running (0x%02X).\n", Notification.current_priority);
      }
    }

    /* Unblock indicator by switching to idle */
    Notification.next_state = NOTIFICATION_IDLE;
    break;

  case NOTIFICATION_HIBERNATE:

    rgb_led.red_value = 0;
    rgb_led.green_value = 0;
    rgb_led.blue_value = 0;
    rgb_led.brightness_value = 100;
    led_next_state = OFF;

    buzzer.status = BUZZER_BEEP;
    buzzer.frequency = BUZZER_RES_FREQ;
    buzzer.duty_cycle = Parameter.buzzer_duty_cycle;
    buzzer.beep_on_time = 1000;
    buzzer.beep_off_time = 1;
    buzzer.delay = 0;
    buzzer.beep_cycles = 1;
    buzzer.burst_repeat_periode = 0;
    buzzer.burst_repeat_cycles = 1;
    set_buzzer(&buzzer);

    /* Unblock indicator by switching to idle */
    Notification.next_state = NOTIFICATION_IDLE;
    break;

  case NOTIFICATION_LOWBAT:

    /* Set priority level for this user notification */
    notification_set_priority(NOTIFICATION_PRIORITY_LEVEL_RED);

    /* Check if this user notification can be triggered or if currently another one with higher priority is running. In this case skip executing this user notification*/
    if (notification_check_priority())
    {
      if ((System.charger_connected == false) || (Parameter.notifications_while_usb_connected == true))
      {
        rgb_led.blink_on_time = 1000;  // ms
        rgb_led.blink_off_time = 1000; // ms
        rgb_led.pos_slope = 500;       // ms
        rgb_led.neg_slope = 500;       // ms
        rgb_led.repeats = 0;
        rgb_led.red_value = 255;
        rgb_led.green_value = 255;
        rgb_led.blue_value = 255;
        rgb_led.brightness_value = 30;
        led_next_state = FLASH;

        /* Print debug messages if enabled */
        if ((Parameter.notification_verbose == true) && (pcb_test_is_running == false))
        {
          rtc_print_debug_timestamp();
          shell_fprintf(shell_backend_uart_get_ptr(), SHELL_VT100_COLOR_MAGENTA, "Triggered notification 'Low bat'\n");
        }
      }
    }
    else
    {
      /* Print debug messages if enabled */
      if ((Parameter.notification_verbose == true) && (pcb_test_is_running == false))
      {
        rtc_print_debug_timestamp();
        shell_fprintf(shell_backend_uart_get_ptr(), SHELL_VT100_COLOR_RED, "Could not trigger notification 'Low bat' since a higher priority user notification is running (0x%02X).\n", Notification.current_priority);
      }
    }

    /* Unblock indicator by switching to idle */
    Notification.next_state = NOTIFICATION_IDLE;
    break;

  case NOTIFICATION_CHARGING:

    /* Set priority level for this user notification */
    if (notification_check_priority())
    {
      if ((System.charger_connected == false) || (Parameter.notifications_while_usb_connected == true))
      {
        Notification.current_priority = NOTIFICATION_PRIORITY_LEVEL_LOWEST; // Set priority to lowest level to allow new user notification
        Notification.next_priority = NOTIFICATION_PRIORITY_LEVEL_LOWEST;
      }
      else
      {
        notification_set_priority(NOTIFICATION_PRIORITY_LEVEL_CHARGING);
      }

      rgb_led.red_value = 255;
      rgb_led.green_value = 255;
      rgb_led.blue_value = 255;
      rgb_led.brightness_value = 50;
      led_next_state = ON;

      /* Print debug messages if enabled */
      if ((Parameter.notification_verbose == true) && (pcb_test_is_running == false))
      {
        rtc_print_debug_timestamp();
        shell_fprintf(shell_backend_uart_get_ptr(), SHELL_VT100_COLOR_MAGENTA, "Triggered notification 'Charging'\n");
      }
    }
    else
    {
      /* Print debug messages if enabled */
      if ((Parameter.notification_verbose == true) && (pcb_test_is_running == false))
      {
        rtc_print_debug_timestamp();
        shell_fprintf(shell_backend_uart_get_ptr(), SHELL_VT100_COLOR_RED, "Could not trigger notification 'charging' since a higher priority user notification is running (0x%02X).\n", Notification.current_priority);
      }
    }

    /* Unblock indicator by switching to idle */
    Notification.next_state = NOTIFICATION_IDLE;
    break;

  case NOTIFICATION_CHARGING_STOPPED:

    /* Set priority level for this user notification */
    if (notification_check_priority())
    {
      if ((System.charger_connected == false) || (Parameter.notifications_while_usb_connected == true))
      {
        Notification.current_priority = NOTIFICATION_PRIORITY_LEVEL_LOWEST; // Set priority to lowest level to allow new user notification
        Notification.next_priority = NOTIFICATION_PRIORITY_LEVEL_LOWEST;
      }
      else
      {
        notification_set_priority(NOTIFICATION_PRIORITY_LEVEL_RED);
      }

      rgb_led.blink_on_time = 1000; // ms
      rgb_led.blink_off_time = 500; // ms
      rgb_led.pos_slope = 500;      // ms
      rgb_led.neg_slope = 500;      // ms
      rgb_led.repeats = 0;
      rgb_led.red_value = 255;
      rgb_led.green_value = 255;
      rgb_led.blue_value = 0;
      rgb_led.brightness_value = 30;
      led_next_state = FLASH;

      /* Print debug messages if enabled */
      if ((Parameter.notification_verbose == true) && (pcb_test_is_running == false))
      {
        rtc_print_debug_timestamp();
        shell_fprintf(shell_backend_uart_get_ptr(), SHELL_VT100_COLOR_MAGENTA, "Triggered notification 'Charging stopped (temperature)'\n");
      }
    }
    else
    {
      /* Print debug messages if enabled */
      if ((Parameter.notification_verbose == true) && (pcb_test_is_running == false))
      {
        rtc_print_debug_timestamp();
        shell_fprintf(shell_backend_uart_get_ptr(), SHELL_VT100_COLOR_RED, "Could not trigger notification 'charging stopped' since a higher priority user notification is running (0x%02X).\n", Notification.current_priority);
      }
    }

    /* Unblock indicator by switching to idle */
    Notification.next_state = NOTIFICATION_IDLE;
    break;

  case NOTIFICATION_FULLY_CHARGED:

    /* Set priority level for this user notification */
    if (notification_check_priority())
    {
      if ((System.charger_connected == false) || (Parameter.notifications_while_usb_connected == true))
      {
        Notification.current_priority = NOTIFICATION_PRIORITY_LEVEL_LOWEST; // Set priority to lowest level to allow new user notification
        Notification.next_priority = NOTIFICATION_PRIORITY_LEVEL_LOWEST;
      }
      else
      {
        notification_set_priority(NOTIFICATION_PRIORITY_LEVEL_GREEN);
      }

      rgb_led.red_value = 0;
      rgb_led.green_value = 255;
      rgb_led.blue_value = 0;
      rgb_led.brightness_value = 100;
      led_next_state = ON;

      /* Print debug messages if enabled */
      if ((Parameter.notification_verbose == true) && (pcb_test_is_running == false))
      {
        rtc_print_debug_timestamp();
        shell_fprintf(shell_backend_uart_get_ptr(), SHELL_VT100_COLOR_MAGENTA, "Triggered notification 'Fully charged'\n");
      }
    }
    else
    {
      {
        /* Print debug messages if enabled */
        if ((Parameter.notification_verbose == true) && (pcb_test_is_running == false))
        {
          rtc_print_debug_timestamp();
          shell_fprintf(shell_backend_uart_get_ptr(), SHELL_VT100_COLOR_RED, "Could not trigger notification 'Low bat' since a higher priority user notification is running (0x%02X).\n", Notification.current_priority);
        }
      }
    }

    /* Unblock indicator by switching to idle */
    Notification.next_state = NOTIFICATION_IDLE;
    break;

  case NOTIFICATION_BATTERY_LEVEL:

    /* Set priority level for this user notification */
    notification_set_priority(NOTIFICATION_PRIORITY_LEVEL_RED);

    /* Check if this user notification can be triggered or if currently another one with higher priority is running. In this case skip executing this user notification*/
    if (notification_check_priority())
    {
      if ((System.charger_connected == false) || (Parameter.notifications_while_usb_connected == true))
      {
        // if (battery.StateOfCharge > BATTERY_GOOD)
        // {                            /* green LED */
        //   rgb_led.blink_on_time = 10000; // ms
        //   rgb_led.blink_off_time = 1;    // ms
        //   rgb_led.pos_slope = 0;         // ms
        //   rgb_led.neg_slope = 0;         // ms
        //   rgb_led.repeats = 1;
        //   rgb_led.red_value = 0;
        //   rgb_led.green_value = 255;
        //   rgb_led.blue_value = 0;
        //   rgb_led.brightness_value = 255;
        //   led_next_state = FLASH;
        // }
        // else if (battery.StateOfCharge > BATTERY_LOW)
        // {                            /* yellow LED */
        //   rgb_led.blink_on_time = 10000; // ms
        //   rgb_led.blink_off_time = 1;    // ms
        //   rgb_led.pos_slope = 0;         // ms
        //   rgb_led.neg_slope = 0;         // ms
        //   rgb_led.repeats = 1;
        //   rgb_led.red_value = 100;
        //   rgb_led.green_value = 255;
        //   rgb_led.blue_value = 0;
        //   rgb_led.brightness_value = 255;
        //   led_next_state = FLASH;
        // }
        // else
        // {                            /* red LED */
        //   rgb_led.blink_on_time = 10000; // ms
        //   rgb_led.blink_off_time = 1;    // ms
        //   rgb_led.pos_slope = 0;         // ms
        //   rgb_led.neg_slope = 0;         // ms
        //   rgb_led.repeats = 1;
        //   rgb_led.red_value = 255;
        //   rgb_led.green_value = 0;
        //   rgb_led.blue_value = 0;
        //   rgb_led.brightness_value = 255;
        //   led_next_state = FLASH;
        // }

        /* Print debug messages if enabled */
        if ((Parameter.notification_verbose == true) && (pcb_test_is_running == false))
        {
          rtc_print_debug_timestamp();
          shell_fprintf(shell_backend_uart_get_ptr(), SHELL_VT100_COLOR_MAGENTA, "Triggered notification 'Battery level'\n");
        }
      }
    }
    else
    {
      /* Print debug messages if enabled */
      if ((Parameter.notification_verbose == true) && (pcb_test_is_running == false))
      {
        rtc_print_debug_timestamp();
        shell_fprintf(shell_backend_uart_get_ptr(), SHELL_VT100_COLOR_RED, "Could not trigger notification 'Battery level' since a higher priority user notification is running (0x%02X).\n", Notification.current_priority);
      }
    }

    /* Unblock indicator by switching to idle */
    Notification.next_state = NOTIFICATION_IDLE;
    break;

  case NOTIFICATION_GENERAL_RFID_CONFIRMATION:

    /* Set priority level for this user notification */
    notification_set_priority(NOTIFICATION_PRIORITY_LEVEL_BLUE);

    /* Check if this user notification can be triggered or if currently another one with higher priority is running. In this case skip executing this user notification*/
    if (notification_check_priority())
    {
      if ((System.charger_connected == false) || (Parameter.notifications_while_usb_connected == true))
      {
        rgb_led.blink_on_time = 30; // ms
        rgb_led.blink_off_time = 10; // ms
        rgb_led.pos_slope = 0;      // ms
        rgb_led.neg_slope = 0;      // ms
        rgb_led.repeats = 1;
        rgb_led.red_value = 0;
        rgb_led.green_value = 0;
        rgb_led.blue_value = 255;
        rgb_led.brightness_value = 255;
        led_next_state = FLASH;

        /* Print debug messages if enabled */
        if ((Parameter.notification_verbose == true) && (pcb_test_is_running == false))
        {
          rtc_print_debug_timestamp();
          shell_fprintf(shell_backend_uart_get_ptr(), SHELL_VT100_COLOR_MAGENTA, "Triggered notification 'General RFID confirmation'\n");
        }
      }
    }
    else
    {
      /* Print debug messages if enabled */
      if ((Parameter.notification_verbose == true) && (pcb_test_is_running == false))
      {
        rtc_print_debug_timestamp();
        shell_fprintf(shell_backend_uart_get_ptr(), SHELL_VT100_COLOR_RED, "Could not trigger notification 'General RFID confirmation' since a higher priority user notification is running (0x%02X).\n", Notification.current_priority);
      }
    }

    /* Unblock indicator by switching to idle */
    Notification.next_state = NOTIFICATION_IDLE;
    break;

  case NOTIFICATION_GENERAL_RFID_CONFIRMATION_SHORT:

    /* Set priority level for this user notification */
    notification_set_priority(NOTIFICATION_PRIORITY_LEVEL_BLUE);

    /* Check if this user notification can be triggered or if currently another one with higher priority is running. In this case skip executing this user notification*/
    if (notification_check_priority())
    {
      if ((System.charger_connected == false) || (Parameter.notifications_while_usb_connected == true))
      {
        rgb_led.blink_on_time = 30; // ms
        rgb_led.blink_off_time = 1; // ms
        rgb_led.pos_slope = 0;      // ms
        rgb_led.neg_slope = 0;      // ms
        rgb_led.repeats = 1;
        rgb_led.red_value = 0;
        rgb_led.green_value = 0;
        rgb_led.blue_value = 255;
        rgb_led.brightness_value = 255;
        led_next_state = FLASH;

        /* Print debug messages if enabled */
        if ((Parameter.notification_verbose == true) && (pcb_test_is_running == false))
        {
          rtc_print_debug_timestamp();
          shell_fprintf(shell_backend_uart_get_ptr(), SHELL_VT100_COLOR_MAGENTA, "Triggered notification 'General RFID confirmation'\n");
        }
      }
    }
    else
    {
      /* Print debug messages if enabled */
      if ((Parameter.notification_verbose == true) && (pcb_test_is_running == false))
      {
        rtc_print_debug_timestamp();
        shell_fprintf(shell_backend_uart_get_ptr(), SHELL_VT100_COLOR_RED, "Could not trigger notification 'General RFID confirmation' since a higher priority user notification is running (0x%02X).\n", Notification.current_priority);
      }
    }

    /* Unblock indicator by switching to idle */
    Notification.next_state = NOTIFICATION_IDLE;
    break;

  case NOTIFICATION_SIDE_MAX_SQM_COVERAGE_REACHED:

    /* Set priority level for this user notification */
    notification_set_priority(NOTIFICATION_PRIORITY_LEVEL_YELLOW);

    /* Check if this user notification can be triggered or if currently another one with higher priority is running. In this case skip executing this user notification*/
    if (notification_check_priority())
    {
      if ((System.charger_connected == false) || (Parameter.notifications_while_usb_connected == true))
      {
        rgb_led.blink_on_time = 250;  // ms
        rgb_led.blink_off_time = 250; // ms
        rgb_led.pos_slope = 0;        // ms
        rgb_led.neg_slope = 0;        // ms
        rgb_led.repeats = 20;
        rgb_led.red_value = 200;
        rgb_led.green_value = 255;
        rgb_led.blue_value = 0;
        rgb_led.brightness_value = 255;
        led_next_state = FLASH;
        flash_repeat_counter = 0;

        /* Print debug messages if enabled */
        if ((Parameter.notification_verbose == true) && (pcb_test_is_running == false))
        {
          rtc_print_debug_timestamp();
          shell_fprintf(shell_backend_uart_get_ptr(), SHELL_VT100_COLOR_MAGENTA, "Triggered notification 'Max square meter coverage for current mop side reached'.\n");
        }
      }
    }
    else
    {
      /* Print debug messages if enabled */
      if ((Parameter.notification_verbose == true) && (pcb_test_is_running == false))
      {
        rtc_print_debug_timestamp();
        shell_fprintf(shell_backend_uart_get_ptr(), SHELL_VT100_COLOR_RED, "Could not trigger notification 'Max square meter coverage for current mop side reached' since a higher priority user notification is running (0x%02X).\n", Notification.current_priority);
      }
    }

    /* Unblock indicator by switching to idle */
    Notification.next_state = NOTIFICATION_IDLE;
    break;

  case NOTIFICATION_MAX_SQM_COVERAGE_REACHED:

    /* Set priority level for this user notification */
    notification_set_priority(NOTIFICATION_PRIORITY_LEVEL_YELLOW);

    /* Check if this user notification can be triggered or if currently another one with higher priority is running. In this case skip executing this user notification*/
    if (notification_check_priority())
    {
      if ((System.charger_connected == false) || (Parameter.notifications_while_usb_connected == true))
      {
        rgb_led.blink_on_time = 20000; // ms
        rgb_led.blink_off_time = 1;    // ms
        rgb_led.pos_slope = 0;         // ms
        rgb_led.neg_slope = 0;         // ms
        rgb_led.repeats = 1;
        rgb_led.red_value = 200;
        rgb_led.green_value = 255;
        rgb_led.blue_value = 0;
        rgb_led.brightness_value = 255;
        led_next_state = FLASH;
        flash_repeat_counter = 0;

        /* Print debug messages if enabled */
        if ((Parameter.notification_verbose == true) && (pcb_test_is_running == false))
        {
          rtc_print_debug_timestamp();
          shell_fprintf(shell_backend_uart_get_ptr(), SHELL_VT100_COLOR_MAGENTA, "Triggered notification 'Max square meter coverage reached'.\n");
        }
      }
    }
    else
    {
      /* Print debug messages if enabled */
      if ((Parameter.notification_verbose == true) && (pcb_test_is_running == false))
      {
        rtc_print_debug_timestamp();
        shell_fprintf(shell_backend_uart_get_ptr(), SHELL_VT100_COLOR_RED, "Could not trigger notification 'Max square meter coverage reached' since a higher priority user notification is running (0x%02X).\n", Notification.current_priority);
      }
    }

    /* Unblock indicator by switching to idle */
    Notification.next_state = NOTIFICATION_IDLE;
    break;

  case NOTIFICATION_SAME_MOP_ALREADY_USED:

    /* Set priority level for this user notification */
    notification_set_priority(NOTIFICATION_PRIORITY_LEVEL_YELLOW);

    /* Check if this user notification can be triggered or if currently another one with higher priority is running. In this case skip executing this user notification*/
    if (notification_check_priority())
    {
      if ((System.charger_connected == false) || (Parameter.notifications_while_usb_connected == true))
      {
        rgb_led.blink_on_time = 500;  // ms
        rgb_led.blink_off_time = 500; // ms
        rgb_led.pos_slope = 0;        // ms
        rgb_led.neg_slope = 0;        // ms
        rgb_led.repeats = 30;
        rgb_led.red_value = 255;
        rgb_led.green_value = 0;
        rgb_led.blue_value = 0;
        rgb_led.brightness_value = 255;
        led_next_state = FLASH;

        /* Print debug messages if enabled */
        if ((Parameter.notification_verbose == true) && (pcb_test_is_running == false))
        {
          rtc_print_debug_timestamp();
          shell_fprintf(shell_backend_uart_get_ptr(), SHELL_VT100_COLOR_MAGENTA, "Triggered notification 'Same mop already used'.\n");
        }
      }
    }
    else
    {
      /* Print debug messages if enabled */
      if ((Parameter.notification_verbose == true) && (pcb_test_is_running == false))
      {
        rtc_print_debug_timestamp();
        shell_fprintf(shell_backend_uart_get_ptr(), SHELL_VT100_COLOR_RED, "Could not trigger notification 'Same mop already used' since a higher priority user notification is running (0x%02X).\n", Notification.current_priority);
      }
    }

    /* Unblock indicator by switching to idle */
    Notification.next_state = NOTIFICATION_IDLE;
    break;

  case NOTIFICATION_CLEAR:

    /* Set priority level for this user notification */
    notification_set_priority(NOTIFICATION_PRIORITY_LEVEL_HIGHEST);

    /* Check if this user notification can be triggered or if currently another one with higher priority is running. In this case skip executing this user notification*/
    if (notification_check_priority())
    {
      if ((System.charger_connected == false) || (Parameter.notifications_while_usb_connected == true))
      {
        led_next_state = OFF;

        /* Print debug messages if enabled */
        if ((Parameter.notification_verbose == true) && (pcb_test_is_running == false))
        {
          rtc_print_debug_timestamp();
          shell_fprintf(shell_backend_uart_get_ptr(), SHELL_VT100_COLOR_MAGENTA, "Triggered notification 'Clear'\n");
        }
      }
    }
    else
    {
      /* Print debug messages if enabled */
      if ((Parameter.notification_verbose == true) && (pcb_test_is_running == false))
      {
        rtc_print_debug_timestamp();
        shell_fprintf(shell_backend_uart_get_ptr(), SHELL_VT100_COLOR_RED, "Could not trigger notification 'Clear' since a higher priority user notification is running (0x%02X).\n", Notification.current_priority);
      }
    }

    /* Unblock indicator by switching to idle */
    Notification.next_state = NOTIFICATION_IDLE;
    break;

  default:
    /* Unblock indicator by switching to idle */
    Notification.next_state = NOTIFICATION_IDLE;
    break;
  }

  /* Set new state for notification state machine */
  Notification.current_state = Notification.next_state;
}

void update_notification_demo(void)
{
  if (notification_demo_delay >= 10)
  {
    switch (notification_test_state)
    {

    case 0:
      if (notification_demo_timer == 1)
      {
        Notification.next_state = NOTIFICATION_0x09;
      }

      if (notification_demo_timer >= 20)
      {
        notification_test_state++;
        notification_demo_timer = 0;
      }
      break;

    case 1:

      if (notification_demo_timer == 1)
      {
        Notification.next_state = NOTIFICATION_0x08;
      }

      if (notification_demo_timer >= 1)
      {
        notification_test_state++;
        notification_demo_timer = 0;
      }
      break;

    case 2:
      if (notification_demo_timer == 1)
      {
        Notification.next_state = NOTIFICATION_0x08;
      }

      if (notification_demo_timer >= 1)
      {
        notification_test_state++;
        notification_demo_timer = 0;
      }
      break;

    case 3:
      if (notification_demo_timer == 1)
      {
        Notification.next_state = NOTIFICATION_0x08;
      }

      if (notification_demo_timer >= 1)
      {
        notification_test_state++;
        notification_demo_timer = 0;
      }
      break;

    case 4:
      if (notification_demo_timer == 1)
      {
        Notification.next_state = NOTIFICATION_0x08;
      }

      if (notification_demo_timer >= 1)
      {
        notification_test_state++;
        notification_demo_timer = 0;
      }
      break;

    case 5:
      if (notification_demo_timer == 1)
      {
        Notification.next_state = NOTIFICATION_0x08;
      }

      if (notification_demo_timer >= 1)
      {
        notification_test_state++;
        notification_demo_timer = 0;
      }
      break;

    case 6:
      if (notification_demo_timer == 1)
      {
        Notification.next_state = NOTIFICATION_0x08;
      }

      if (notification_demo_timer >= 1)
      {
        notification_test_state++;
        notification_demo_timer = 0;
      }
      break;

    case 7:
      if (notification_demo_timer == 1)
      {
        Notification.next_state = NOTIFICATION_0x08;
      }

      if (notification_demo_timer >= 1)
      {
        notification_test_state++;
        notification_demo_timer = 0;
      }
      break;

    case 8:
      if (notification_demo_timer == 1)
      {
        Notification.next_state = NOTIFICATION_0x08;
      }

      if (notification_demo_timer >= 1)
      {
        notification_test_state++;
        notification_demo_timer = 0;
      }
      break;

    case 9:
      if (notification_demo_timer == 1)
      {
        Notification.next_state = NOTIFICATION_0x08;
      }

      if (notification_demo_timer >= 1)
      {
        notification_test_state++;
        notification_demo_timer = 0;
      }
      break;

    case 10:
      if (notification_demo_timer == 1)
      {
        Notification.next_state = NOTIFICATION_0x08;
      }

      if (notification_demo_timer >= 1)
      {
        notification_test_state++;
        notification_demo_timer = 0;
      }
      break;

    case 11:
      if (notification_demo_timer == 1)
      {
        Notification.next_state = NOTIFICATION_0x07;
      }

      if (notification_demo_timer >= 30)
      {
        notification_test_state++;
        notification_demo_timer = 0;
      }
      break;

    case 12:
      if (notification_demo_timer == 1)
      {
        Notification.next_state = NOTIFICATION_SAME_MOP_ALREADY_USED;
      }

      if (notification_demo_timer >= 30)
      {
        notification_test_state++;
        notification_demo_timer = 0;
      }
      break;

    case 13:
      if (notification_demo_timer == 1)
      {
        Notification.next_state = NOTIFICATION_SIDE_MAX_SQM_COVERAGE_REACHED;
      }

      if (notification_demo_timer >= 10)
      {
        notification_test_state++;
        notification_demo_timer = 0;
      }
      break;

    case 14:

      if (notification_demo_timer == 1)
      {
        Notification.next_state = NOTIFICATION_MAX_SQM_COVERAGE_REACHED;
      }

      if (notification_demo_timer >= 30)
      {
        notification_test_state = 0;
        notification_demo_timer = 0;
      }
      break;

    default:
      break;
    }

    notification_demo_timer++;
  }
  else
  {
    notification_demo_delay++;
  }
}