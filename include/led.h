/**
 * @file led.h
 * @author Thomas Keilbach | keiltronic GmbH
 * @date 27 Oct 2022
 * @brief This file contains functions headers to communicate with the pheripherals
 * @version 1.0.0
 */

#ifndef LED_H
#define LED_H

#include <zephyr/device.h>
#include <zephyr/drivers/i2c.h>
#include <zephyr/devicetree.h>
#include "gpio.h"
#include "i2c.h"
#include "datalog_mem.h"
#include "parameter_mem.h"

/* The device tree node idedntify for the "led0" alias*/
#define LED0_NODE DT_ALIAS(led0)
#define I2C2_NODE DT_NODELABEL(lp5009)

#define DEVICE_CONFIG0 0x00
#define DEVICE_CONFIG1 0x01
#define LED_CONFIG0 0x02
#define BANK_BRIGHTNESS 0x03
#define BANK_A_COLOR 0x04 // green
#define BANK_B_COLOR 0x05 // red
#define BANK_C_COLOR 0x06 // blue
#define LED0_BRIGHTNESS 0x07
#define LED1_BRIGHTNESS 0x08
#define LED2_BRIGHTNESS 0x09
#define LED3_BRIGHTNESS 0x0A
#define OUT0_COLOR 0x0B
#define OUT1_COLOR 0x0C
#define OUT2_COLOR 0x0D
#define OUT3_COLOR 0x0E
#define OUT4_COLOR 0x0F
#define OUT5_COLOR 0x10
#define OUT6_COLOR 0x11
#define OUT7_COLOR 0x12
#define OUT8_COLOR 0x13
#define OUT9_COLOR 0x14
#define OUT10_COLOR 0x15
#define OUT11_COLOR 0x16
#define RESET 0x17

typedef struct
{
    uint8_t brightness_addr;
    uint8_t red_addr;
    uint8_t green_addr;
    uint8_t blue_addr;
    uint8_t red_value;
    uint8_t green_value;
    uint8_t blue_value;
    uint8_t brightness_value;
    uint32_t pos_slope;
    uint32_t neg_slope;
    uint32_t blink_on_time;
    uint32_t blink_off_time;
    uint32_t repeats;
} RGB_LED;

enum mode
{
    IDLE = 0,
    OFF,
    ON,
    FLASH,
    FLASHING
};

extern uint8_t led_next_state;
extern uint32_t flash_repeat_counter;
extern struct gpio_dt_spec led;

extern void led_init(void);
extern void led_update(void);
extern void led_set_rgb_brightness(uint8_t brightness);
extern void led_set_rgb_color(uint8_t red, uint8_t green, uint8_t blue);

#endif