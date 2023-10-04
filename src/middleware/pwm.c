/**
 * @file pwm.c
 * @author Thomas Keilbach | keiltronic GmbH
 * @date 27 Oct 2022
 * @brief This file contains functions to communicate with the pheripherals
 * @version 1.0.0
 */

/*!
 * @defgroup Peripherals
 * @brief This file contains functions to communicate with the pheripherals
 * @{*/
#include "pwm.h"

struct device *pwm_dev;

void init_pwm(void) {
  pwm_dev = device_get_binding("PWM_0");
  if (!pwm_dev) {
   if (Parameter.debug) {
    shell_error(shell_backend_uart_get_ptr(), "device_get_binding() PWM0 failed");
    }
  }
}
