/**
 * @file button.c
 * @author Thomas Keilbach | keiltronic GmbH
 * @date 27 Oct 2022
 * @brief This file contains functions to communicate with the pheripherals
 * @version 1.0.0
 */

/*!
 * @defgroup Peripherals
 * @brief This file contains functions to communicate with the pheripherals
 * @{*/
#include "buttons.h"

#define BTN0_NODE DT_ALIAS(btn0)
#define BTN1_NODE DT_ALIAS(btn1)

bool btn0_status = false;
bool btn1_status = false;

struct gpio_dt_spec button0 = GPIO_DT_SPEC_GET(BTN0_NODE, gpios);
struct gpio_dt_spec button1 = GPIO_DT_SPEC_GET(BTN1_NODE, gpios);

struct gpio_callback button0_cb_data;
struct gpio_callback button1_cb_data;

/*!
 * @brief tbd
 * @details tbd
 */
void button_init(void)
{
  int16_t ret = 0;

  ret = gpio_pin_interrupt_configure_dt(&button0, GPIO_INT_EDGE_BOTH);
  ret = gpio_pin_interrupt_configure_dt(&button1, GPIO_INT_EDGE_BOTH);

  /* Button 0 */
  if (!device_is_ready(button0.port))
  {
    return;
  }
  ret = gpio_pin_configure_dt(&button0, GPIO_INPUT);
  if (ret < 0)
  {
    return;
  }

  /* Button 1 */
  if (!device_is_ready(button1.port))
  {
    return;
  }
  ret = gpio_pin_configure_dt(&button1, GPIO_INPUT);
  if (ret < 0)
  {
    return;
  }

  gpio_init_callback(&button0_cb_data, button0_pressed, BIT(button0.pin));
  gpio_init_callback(&button1_cb_data, button1_pressed, BIT(button1.pin));

  gpio_add_callback(button0.port, &button0_cb_data);
  gpio_add_callback(button1.port, &button1_cb_data);
}

/*!
 * @brief tbd
 * @details tbd
 */
void button0_pressed(const struct device *dev, struct gpio_callback *cb, uint32_t pins)
{
  static uint64_t btn0_start_time = 0;
  static uint64_t btn0_stop_time = 0;
  static uint64_t btn0_cycles_spent = 0;
  static uint64_t btn0_press_time_ns = 0;
  static float btn0_press_time_ms = 0.0;

  btn0_status = (bool)gpio_pin_get_dt(&button0);

  if (btn0_status == true)
  {
    System.StatusInputs |= STATUSFLAG_UB;

    /* Capture kernel time stamp to measure press duration */
    btn0_start_time = k_cycle_get_32();
  }
  else
  {
    if (btn0_start_time > 0)
    {
      /* Capture final kernel time stamp */
      btn0_stop_time = k_cycle_get_32();

      /* Compute how long the binary search needed */
      btn0_cycles_spent = btn0_stop_time - btn0_start_time;

      /* Convert ticks into nanoseconds and milliseconds */
      btn0_press_time_ns = k_cyc_to_ns_floor64(btn0_cycles_spent);
      btn0_press_time_ms = ((float)btn0_press_time_ns / 1000000.0);

      printk("Button 0 press time: %.2fms\r\n", btn0_press_time_ms);
    }
  }
}

/*!
 * @brief tbd
 * @details tbd
 */
void button1_pressed(const struct device *dev, struct gpio_callback *cb, uint32_t pins)
{
  static uint64_t btn1_start_time = 0;
  static uint64_t btn1_stop_time = 0;
  static uint64_t btn1_cycles_spent = 0;
  static uint64_t btn1_press_time_ns = 0;
  static float btn1_press_time_ms = 0.0;

  btn1_status = (bool)gpio_pin_get_dt(&button1);
  gpio_pin_toggle_dt(&led);

  if (btn1_status == true)
  {
    System.StatusInputs |= STATUSFLAG_UB;

    /* Add event in event array which is send to cloud in next sync interval */
    NewEvent0x0D();

    /* Capture kernel time stamp to measure press duration */
    btn1_start_time = k_cycle_get_32();
  }
  else
  {
    if (btn1_start_time > 0)
    {
      /* Capture final kernel time stamp */
      btn1_stop_time = k_cycle_get_32();

      /* Compute how long the binary search needed */
      btn1_cycles_spent = btn1_stop_time - btn1_start_time;

      /* Convert ticks into nanoseconds and milliseconds */
      btn1_press_time_ns = k_cyc_to_ns_floor64(btn1_cycles_spent);
      btn1_press_time_ms = ((float)btn1_press_time_ns / 1000000.0);

      /* Trigger */
      printk("Button 1 press time: %.2fms\r\n", btn1_press_time_ms);

      if (btn1_press_time_ms >= HIBERNATE_TIME)
      {
        printk("Enter hibernate mode\r\n");
        // enter_hibernate();
        /* Do a hard reboot */
        // shell_fprintf(shell_backend_uart_get_ptr(), SHELL_VT100_COLOR_DEFAULT, "Device hard reboot via user button\n", battery.Voltage);
        // Device_PushRAMToFlash();
        // Notification.next_state = NOTIFICATION_HIBERNATE;
        // lte_lc_power_off();
        // k_msleep(1000); // Delay the reboot to give the system enough time to o<uput the debug message on console
        // gpio_pin_set_raw(gpio_dev, GPIO_PIN_RST, 1);
      }
      else if (btn1_press_time_ms >= BATTERY_LEVEL_TIME)
      {
      //  if ((System.charger_connected == false) || (Parameter.notifications_while_usb_connected == true))
      //  {
          printk("Show battery level\r\n");
          //       /* Show battery level with led */
          //       Notification.next_state = NOTIFICATION_BATTERY_LEVEL;

          //       rtc_print_debug_timestamp();
          //       shell_fprintf(shell_backend_uart_get_ptr(), SHELL_VT100_COLOR_DEFAULT, "Show battery level indicator (led), battery voltage= %4.2fmV\n", battery.Voltage);
     //   }
      }
    }
  }
}