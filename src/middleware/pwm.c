/**
 * @file pwm.c
 * @author Thomas Keilbach | keiltronic GmbH
 * @date 19 Oct 2023
 * @brief This file contains functions to communicate with the pheripherals
 * @version 2.0.0
 */

/*!
 * @defgroup Peripherals
 * @brief This file contains functions to communicate with the pheripherals
 * @{*/
#include "pwm.h"

const struct pwm_dt_spec sBuzzer = PWM_DT_SPEC_GET(DT_ALIAS(buzzerpwm));

void pwm_init(void) {
}


/* Set pwm:
  pwm_set_dt(&sBuzzer, PWM_HZ(atoi(argv[1])), PWM_HZ(atoi(argv[1])) / 2);
   
   turn pwm off:

  pwm_set_pulse_dt(&sBuzzer, 0);
   
   */