/**
 * @file led.c
 * @author Thomas Keilbach | keiltronic GmbH
 * @date 05 Jun 2022
 * @brief This files provides a set of API functions to control the LED
 * @details This API use Zephyr I2C driver to control the Texas Instruments LP5009/12 led driver
 * @version 1.0.0
 */

/*!
 * @defgroup Peripherals
 * @brief This file contains functions to communicate with the pheripherals
 * @{*/

#include "led.h"

struct gpio_dt_spec led = GPIO_DT_SPEC_GET(LED0_NODE, gpios);

// RGB_LED led = {BANK_BRIGHTNESS, BANK_B_COLOR, BANK_A_COLOR, BANK_C_COLOR, 255, 0, 0, 0, 0, 0, 0, 0, 0};
// uint8_t led_next_state;
// uint8_t led_current_state;

// uint8_t led_flash_enabled = false;
// uint32_t flash_repeat_counter = 0;
// uint32_t flash_period_counter = 0;
// float slope = 0.0;
// float current_brightness = 0.0;

// /*!
//  *  @brief This is the function description


//  */
// void init_led(void)
// {
//   uint8_t data[2];

//   /* Chip enable */
//   data[0] = DEVICE_CONFIG0;
//   data[1] = 0b01000000;
//   i2c_write(i2c_dev, data, 2, (uint16_t)LED_ADDR);

//   data[0] = DEVICE_CONFIG1;
//   data[1] = 0b00010000;
//   i2c_write(i2c_dev, data, 2, (uint16_t)LED_ADDR);

//   data[0] = LED_CONFIG0;
//   data[1] = 0x07;
//   i2c_write(i2c_dev, data, 2, (uint16_t)LED_ADDR);

//   data[0] = BANK_A_COLOR; // green
//   data[1] = 0;
//   i2c_write(i2c_dev, data, 2, (uint16_t)LED_ADDR);

//   data[0] = BANK_B_COLOR; // red
//   data[1] = 0;
//   i2c_write(i2c_dev, data, 2, (uint16_t)LED_ADDR);

//   data[0] = BANK_C_COLOR; // blue
//   data[1] = 0;
//   i2c_write(i2c_dev, data, 2, (uint16_t)LED_ADDR);
// }

// /*!
//  *  @brief This is the function description


//  */
// void led_set_rgb_color(uint8_t red, uint8_t green, uint8_t blue)
// {
//   uint8_t data[2];

//   data[0] = BANK_A_COLOR; // green
//   data[1] = green;
//   i2c_write(i2c_dev, data, 2, (uint16_t)LED_ADDR);

//   data[0] = BANK_B_COLOR; // red
//   data[1] = red;
//   i2c_write(i2c_dev, data, 2, (uint16_t)LED_ADDR);

//   data[0] = BANK_C_COLOR; // blue
//   data[1] = blue;
//   i2c_write(i2c_dev, data, 2, (uint16_t)LED_ADDR);
// }

// /*!
//  *  @brief This is the function description


//  */
// void led_set_rgb_brightness(uint8_t brightness)
// {
//   uint8_t data[2];

//   data[0] = BANK_BRIGHTNESS;
//   data[1] = brightness;
//   i2c_write(i2c_dev, data, 2, (uint16_t)LED_ADDR);
// }

// /*!
//  *  @brief This is the function description


//  */
// void led_update(void)
// {
//   switch (led_current_state)
//   {
//   case IDLE:
//     break;

//   case OFF:
//     led_set_rgb_brightness(0);
//     flash_repeat_counter = 0;
//     flash_period_counter = 0;
//     current_brightness = 0.0;
//     led_next_state = IDLE;
//     Notification.current_priority = NOTIFICATION_PRIORITY_LEVEL_LOWEST; // Set priority to lowest level to allow new user notification
//     Notification.next_priority = NOTIFICATION_PRIORITY_LEVEL_LOWEST;
//     break;

//   case ON:
//     led_set_rgb_color(led.red_value, led.green_value, led.blue_value);
//     led_set_rgb_brightness(led.brightness_value);
//     led_next_state = IDLE;
//     break;

//   case FLASH:
//     flash_period_counter = 0;
//     flash_repeat_counter = 0;
//     current_brightness = 0.0;
//     led_next_state = FLASHING;

//   case FLASHING:

//     if ((led.repeats == 0) || (flash_repeat_counter < led.repeats))
//     {
//       if (flash_period_counter >= (led.blink_on_time + led.blink_off_time)) // complete periode elapsed
//       {
//         flash_period_counter = 0;
//         if (led.repeats != 0)
//         {
//           flash_repeat_counter++;
//         }
//       }
//       else if (flash_period_counter >= led.blink_on_time)
//       {                                                      // off-phase
//         if (flash_period_counter == (led.blink_on_time + 1)) // For not spamming hte I2C bus color and brightnes value should be sent only once a start of periode
//         {
//           led_set_rgb_brightness(0);
//         }
//       }
//       else if ((flash_period_counter >= 0) && (flash_period_counter < led.blink_on_time))
//       {                                               // on-phase
//         if (led.pos_slope == 0 && led.neg_slope == 0) // no slope - turn immediatelly on
//         {
//           if (flash_period_counter == 1) // For not spamming hte I2C bus color and brightnes value should be sent only once a start of periode
//           {
//             led_set_rgb_color(led.red_value, led.green_value, led.blue_value);
//             led_set_rgb_brightness(led.brightness_value);
//           }
//         }
//         else
//         {
//           slope = (float)Parameter.led_brightness / (float)led.pos_slope;

//           if (flash_period_counter < led.pos_slope) // ramping up
//           {
//             if (flash_period_counter == 1)
//             {
//               led_set_rgb_color(led.red_value, led.green_value, led.blue_value);
//               led_set_rgb_brightness(0);
//               current_brightness = 0.0;
//             }

//             current_brightness += slope;
//             led.brightness_value = (uint8_t)current_brightness;

//             if (led.brightness_value <= Parameter.led_brightness)
//             {
//               led_set_rgb_brightness(led.brightness_value);
//             }
//           }
//           else if (flash_period_counter > (led.blink_on_time - led.neg_slope)) // ramping down
//           {
//             if (flash_period_counter == led.pos_slope + 1)
//             {
//               led_set_rgb_brightness(Parameter.led_brightness);
//               current_brightness = (float)Parameter.led_brightness;
//             }

//             current_brightness -= slope;
//             led.brightness_value = (uint8_t)current_brightness;

//             if (led.brightness_value >= 0)
//             {
//               led_set_rgb_brightness(led.brightness_value);
//             }
//           }
//         }
//       }
//       flash_period_counter++;
//     }
//     else
//     {
//       led_next_state = OFF;
//     }
//     break;

//   default:
//     break;
//   }

//   led_current_state = led_next_state;
// }
