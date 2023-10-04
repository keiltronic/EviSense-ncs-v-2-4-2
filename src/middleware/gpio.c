/**
 * @file gpio.c
 * @author Thomas Keilbach | keiltronic GmbH
 * @date 27 Oct 2022
 * @brief This file contains functions to communicate with the pheripherals
 * @version 1.0.0
 */

/*!
 * @defgroup Peripherals
 * @brief This file contains functions to communicate with the pheripherals
 * @{*/
#include "gpio.h"

struct device *gpio_dev;
struct gpio_callback gpio_cb_acc_int1, gpio_cb_acc_int2, gpio_cb_btn1, gpio_cb_btn2, gpio_cb_mag_int;

void init_gpio(void)
{
  /* Bind to GPIO port */
  gpio_dev = device_get_binding("GPIO_0");

  if (gpio_dev == NULL)
  {
    //shell_print(shell_backend_uart_get_ptr(), "Could not get GPIO_O device");
   printk("Could not get GPIO_O device");
    return;
  }

  /* Configure GPIOs output pins */
  gpio_pin_configure(gpio_dev, GPIO_PIN_5V_EN, GPIO_OUTPUT);
  gpio_pin_configure(gpio_dev, GPIO_PIN_RST, GPIO_OUTPUT);
  gpio_pin_configure(gpio_dev, GPIO_PIN_GPIO1, GPIO_OUTPUT);
  gpio_pin_configure(gpio_dev, GPIO_PIN_CHG_EN, GPIO_OUTPUT);
  gpio_pin_configure(gpio_dev, GPIO_PIN_LED_EN, GPIO_OUTPUT);
  gpio_pin_configure(gpio_dev, GPIO_PIN_LED1, GPIO_OUTPUT);
  gpio_pin_configure(gpio_dev, GPIO_PIN_RFID_TRIGGER, GPIO_OUTPUT);
  gpio_pin_configure(gpio_dev, GPIO_PIN_BUZZER, GPIO_OUTPUT);
  gpio_pin_configure(gpio_dev, GPIO_PIN_FLASH_CS1, GPIO_OUTPUT);
  gpio_pin_configure(gpio_dev, GPIO_PIN_FLASH_CS2, GPIO_OUTPUT);

  /* Configure GPIOs input pins */
  gpio_pin_configure(gpio_dev, GPIO_PIN_BAT_ALRT1, GPIO_INPUT);
  gpio_pin_configure(gpio_dev, GPIO_PIN_PWR_STAT, GPIO_INPUT);
  gpio_pin_configure(gpio_dev, GPIO_PIN_CHRG_STAT, GPIO_INPUT);
  gpio_pin_configure(gpio_dev, GPIO_PIN_VUSB_MEASURE, GPIO_INPUT);
  gpio_pin_configure(gpio_dev, GPIO_PIN_IMU_INT1, GPIO_INPUT);
  gpio_pin_configure(gpio_dev, GPIO_PIN_IMU_INT2, GPIO_INPUT);
  gpio_pin_configure(gpio_dev, GPIO_PIN_MAG_INT, GPIO_INPUT);
  gpio_pin_configure(gpio_dev, GPIO_PIN_BTN1, GPIO_INPUT);
  gpio_pin_configure(gpio_dev, GPIO_PIN_BTN2, GPIO_INPUT);

  /* Set default states */
  gpio_pin_set_raw(gpio_dev, GPIO_PIN_5V_EN, 0);
  gpio_pin_set_raw(gpio_dev, GPIO_PIN_RST, 0);  
  gpio_pin_set_raw(gpio_dev, GPIO_PIN_GPIO1, 0);
  gpio_pin_set_raw(gpio_dev, GPIO_PIN_CHG_EN, 0);
  gpio_pin_set_raw(gpio_dev, GPIO_PIN_LED_EN, 1);
  gpio_pin_set_raw(gpio_dev, GPIO_PIN_LED1, 1);
  gpio_pin_set_raw(gpio_dev, GPIO_PIN_RFID_TRIGGER, 1);
  gpio_pin_set_raw(gpio_dev, GPIO_PIN_BUZZER, 0);
  gpio_pin_set_raw(gpio_dev, GPIO_PIN_FLASH_CS1, 1);
  gpio_pin_set_raw(gpio_dev, GPIO_PIN_FLASH_CS2, 1);

  /* Configure interrupt pins */
  gpio_init_callback(&gpio_cb_acc_int1, ISR_imu_int1, BIT(GPIO_PIN_IMU_INT1));
  gpio_add_callback(gpio_dev, &gpio_cb_acc_int1);
  gpio_pin_interrupt_configure(gpio_dev, GPIO_PIN_IMU_INT1, GPIO_INT_EDGE_RISING);

  gpio_init_callback(&gpio_cb_acc_int2, ISR_imu_int2, BIT(GPIO_PIN_IMU_INT2));
  gpio_add_callback(gpio_dev, &gpio_cb_acc_int2);
  gpio_pin_interrupt_configure(gpio_dev, GPIO_PIN_IMU_INT2, GPIO_INT_EDGE_FALLING);

  gpio_init_callback(&gpio_cb_mag_int, ISR_mag_int, BIT(GPIO_PIN_MAG_INT));
  gpio_add_callback(gpio_dev, &gpio_cb_mag_int);
  gpio_pin_interrupt_configure(gpio_dev, GPIO_PIN_MAG_INT, GPIO_INT_EDGE_RISING);

  gpio_init_callback(&gpio_cb_btn1, ISR_btn1, BIT(GPIO_PIN_BTN1));
  gpio_add_callback(gpio_dev, &gpio_cb_btn1);
  gpio_pin_interrupt_configure(gpio_dev, GPIO_PIN_BTN1, GPIO_INT_EDGE_BOTH);

  gpio_init_callback(&gpio_cb_btn2, ISR_btn2, BIT(GPIO_PIN_BTN2));
  gpio_add_callback(gpio_dev, &gpio_cb_btn2);
  gpio_pin_interrupt_configure(gpio_dev, GPIO_PIN_BTN2, GPIO_INT_EDGE_BOTH);
}