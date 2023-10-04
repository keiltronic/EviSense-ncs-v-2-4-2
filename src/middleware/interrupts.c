/**
 * @file interrupts.c
 * @author Thomas Keilbach | keiltronic GmbH
 * @date 27 Oct 2022
 * @brief This file contains functions to communicate with the pheripherals
 * @version 1.0.0
 */

/*!
 * @defgroup Peripherals
 * @brief This file contains functions to communicate with the pheripherals
 * @{*/

#include "interrupts.h"

/*!
 *  @brief This is the function description


 */
void ISR_imu_int2(struct device *gpio, struct gpio_callback *cb, uint32_t pins)
{
  // motion_detected = true;
  // motion_reset_counter = 0;
}

/*!
 *  @brief This is the function description


 */
void ISR_imu_int1(struct device *gpio, struct gpio_callback *cb, uint32_t pins)
{
  // step_interrupt_triggered = true;
  // System.TotalSteps++;
  // System.Steps++;
}

/*!
 *  @brief This is the function description


 */
void ISR_mag_int(struct device *gpio, struct gpio_callback *cb, uint32_t pins)
{
}

/*!
 *  @brief This is the function description


 */
void ISR_btn1(struct device *gpio, struct gpio_callback *cb, uint32_t pins)
{
  // if (gpio_pin_get_raw(gpio_dev, GPIO_PIN_BTN1))
  // {
  //   btn1_pressed = false;
  // }
  // else
  // {
  //   btn1_pressed = true;
  // }

  // if (btn1_pressed == false)
  // {
  //   btn1_press_counter++;
  //   btn1_press_timer = 0;

  //   if (Parameter.debug)
  //   {
  //     rtc_print_debug_timestamp();
  //     printk("Interrupt: button 1 pressed, press counter: %d\n", btn1_press_counter);
  //   }
  // }
  // else
  // {
  //   if (Parameter.debug)
  //   {
  //     rtc_print_debug_timestamp();
  //     printk("Interrupt: button 1 released, Press duration: %d ms\n", btn1_press_timer);
  //   }
  // }
}

/*!
 *  @brief This is the function description


 */
void ISR_btn2(struct device *gpio, struct gpio_callback *cb, uint32_t pins)
{
  // if (gpio_pin_get_raw(gpio_dev, GPIO_PIN_BTN2))
  // {
  //   btn2_pressed = false;
  // }
  // else
  // {
  //   btn2_pressed = true;
  // }

  // if (btn2_pressed == false)
  // {
  //   btn2_press_counter++;
  //   btn2_press_timer = 0;

  //   if (Parameter.debug)
  //   {
  //     rtc_print_debug_timestamp();
  //     printk("Interrupt: button 2 pressed, press counter: %d\n", btn2_press_counter);
  //   }
  // }
  // else
  // {
  //   if (Parameter.debug)
  //   {
  //     rtc_print_debug_timestamp();
  //     printk("Interrupt: button 2 released, Press duration: %d ms\n", btn2_press_timer);
  //   }
  // }
}