/**
 * @file buzzer.c
 * @author Thomas Keilbach | keiltronic GmbH
 * @date 27 Oct 2022
 * @brief This file contains functions to communicate with the pheripherals
 * @version 1.0.0
 */

/*!
 * @defgroup Peripherals
 * @brief This file contains functions to communicate with the pheripherals
 * @{*/

#include "buzzer.h"

BUZZER buzzer = {BUZZER_OFF, BUZZER_OFF, 2700UL, 50UL, 0UL, 0UL, 0UL, 0UL, 0UL, 0UL, 0UL, 0UL, 0UL, 0UL, 0UL, false};

/*!
 *  @brief This is the function description
 */
void set_buzzer(BUZZER *buzzer) {

  if (buzzer->status_old != buzzer->status) {

    uint32_t periode = 0UL;
    uint32_t pulse_width = 0UL;

    periode = 1000000UL / buzzer->frequency;              // Convert Hz into µs
    pulse_width = (periode * buzzer->duty_cycle) / 100UL; // Convert duty_cycle in percentage [%] into µs

    switch (buzzer->status) {

    case BUZZER_OFF:

      pwm_pin_set_usec(pwm_dev, BUZZER_GPIO_PIN, periode, 0UL, 0UL);
      System.StatusOutputs &= ~STATUSFLAG_BZ; // Create status entry
      break;

    case BUZZER_ON:

      pwm_pin_set_usec(pwm_dev, BUZZER_GPIO_PIN, periode, pulse_width, 0UL);
      System.StatusOutputs |= STATUSFLAG_BZ; // Create status entry
      break;

    case BUZZER_BEEP:

      buzzer->isBeeping = true;
      System.StatusOutputs |= STATUSFLAG_BZ; // Create status entry
      break;

    default:
      break;
    }

    /* If LED setting changes, store frame in log */
  }
  buzzer->status_old = buzzer->status;
}

/*!
 *  @brief This is the function description
 */
void buzzer_beep_function(BUZZER *buzzer) {

  if (buzzer->beep_period_counter >= (buzzer->beep_on_time + buzzer->beep_off_time)) {
    buzzer->beep_period_counter = 0UL;
    buzzer->beep_cycle_counter++;
  } else if (buzzer->beep_period_counter >= buzzer->beep_on_time) {
    buzzer->status = BUZZER_OFF;
    set_buzzer(buzzer);
  } else if (((buzzer->beep_period_counter >= 0UL) && (buzzer->beep_period_counter < buzzer->beep_on_time))) {
    buzzer->status = BUZZER_ON;
    set_buzzer(buzzer);
  }

  buzzer->beep_period_counter++;
}

/*!
 *  @brief This is the function description
 */
void buzzer_update(BUZZER *buzzer) {

  if (buzzer->isBeeping == true) {

    if (buzzer->beep_cycles == 0UL) { // beep forever
      buzzer_beep_function(buzzer);

    } else { // just beep a few times

      if (buzzer->delay_timer > buzzer->delay) {
        if (buzzer->beep_cycle_counter < buzzer->beep_cycles) {
          buzzer_beep_function(buzzer);
        } else {
          buzzer->status = BUZZER_OFF;
          buzzer->beep_cycle_counter = 0UL;
          buzzer->delay_timer = 0UL;
          buzzer->isBeeping = false;
          set_buzzer(buzzer);
        }
      } else {
        buzzer->delay_timer++;
      }
    }
  }
}