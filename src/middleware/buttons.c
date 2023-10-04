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

#define BTN0_NODE	DT_ALIAS(btn0) 
#define BTN1_NODE	DT_ALIAS(btn1) 

bool btn0_status = false;
bool btn1_status = false;

struct gpio_dt_spec button0 = GPIO_DT_SPEC_GET(BTN0_NODE, gpios);
struct gpio_dt_spec button1 = GPIO_DT_SPEC_GET(BTN1_NODE, gpios);

struct gpio_callback button0_cb_data;
struct gpio_callback button1_cb_data;

void init_button(void){
  
     int16_t ret = 0;

    ret = gpio_pin_interrupt_configure_dt(&button0, GPIO_INT_EDGE_BOTH);
    ret = gpio_pin_interrupt_configure_dt(&button1, GPIO_INT_EDGE_BOTH);

  /* Button 0 */
    if (!device_is_ready(button0.port)) {
    return;
  }
  ret = gpio_pin_configure_dt(&button0, GPIO_INPUT);
  if (ret < 0) {
    printk("Button 0 not ready, skip rest!\r\n");
    return;
  }

/* Button 1 */
    if (!device_is_ready(button1.port)) {
    return;
  }
  ret = gpio_pin_configure_dt(&button1, GPIO_INPUT);
  if (ret < 0) {

    printk("Button 1 not ready!\r\n");
    return;
  }

  gpio_init_callback(&button0_cb_data, button0_pressed, BIT(button0.pin)); 	
  gpio_init_callback(&button1_cb_data, button1_pressed, BIT(button1.pin)); 	

  gpio_add_callback(button0.port, &button0_cb_data);
gpio_add_callback(button1.port, &button1_cb_data);
}


void button0_pressed(const struct device *dev, struct gpio_callback *cb, uint32_t pins)
{
    btn0_status = (bool) gpio_pin_get_dt(&button0); 
    printk("btn0: %d\r\n", btn0_status);
}

void button1_pressed(const struct device *dev, struct gpio_callback *cb, uint32_t pins)
{
    btn1_status = (bool) gpio_pin_get_dt(&button1); 
    printk("btn1: %d\r\n", btn1_status);
   
   gpio_pin_toggle_dt(&led);    
}

// uint8_t btn1_pressed = false;
// uint8_t btn2_pressed = false;
// uint8_t btn1_pressed_old = false;
// uint8_t btn2_pressed_old = false;
// uint8_t btn1_beep = false;
// uint8_t btn2_beep = false;
// uint16_t btn1_press_timer = 0;
// uint16_t btn2_press_timer = 0;
// volatile uint16_t btn1_press_counter = 0;
// volatile uint16_t btn2_press_counter = 0;

// /*!
//  *  @brief This is the function description
//  */
// void button_monitor(void)
// {
//   /* Check if any button is pressed (interrupt) and count press time*/
//   if (btn1_pressed == true)
//   {
//     btn1_press_timer++;
//   }

//   if (btn2_pressed == true)
//   {
//     btn2_press_timer++;
//   }

//   /* ########### INTERNAL BUTTON ########################################################## */

//   if (btn1_pressed == true && btn1_pressed_old == false)
//   {
//     /* Not used */
//   }

//   btn1_pressed_old = btn1_pressed;

//   /* ########### USER BUTTON ########################################################## */

//   /* Activate blue rgb led while user button (btn2) is pressed and not USB is connected */
//   if (btn2_pressed == true && btn2_pressed_old == false)
//   {
//     System.StatusInputs |= STATUSFLAG_UB;

//     /* Add event in event array which is send to cloud in next sync interval */
//     NewEvent0x0D();

//   }

//   /* Check for how long the user button is pressed an trigger actions depending on press duration */
//   if (btn2_press_timer == BATTERY_LEVEL_TIME)
//   {
//     if ((System.charger_connected == false) || (Parameter.notifications_while_usb_connected == true))
//     {
//       /* Show battery level with led */
//       Notification.next_state = NOTIFICATION_BATTERY_LEVEL;

//       rtc_print_debug_timestamp();
//       shell_fprintf(shell_backend_uart_get_ptr(), SHELL_VT100_COLOR_DEFAULT, "Show battery level indicator (led), battery voltage= %4.2fmV\n", battery.Voltage);
//     }
//   }
//   else if (btn2_press_timer == HIBERNATE_TIME)
//   {
//     //enter_hibernate();
//     /* Do a hard reboot */
//       shell_fprintf(shell_backend_uart_get_ptr(), SHELL_VT100_COLOR_DEFAULT, "Device hard reboot via user button\n", battery.Voltage);
//       Device_PushRAMToFlash();
//       Notification.next_state = NOTIFICATION_HIBERNATE;
//      // lte_lc_power_off();
//       k_msleep(1000); // Delay the reboot to give the system enough time to o<uput the debug message on console
//       gpio_pin_set_raw(gpio_dev, GPIO_PIN_RST, 1);
//   }

//   btn2_pressed_old = btn2_pressed;
// }