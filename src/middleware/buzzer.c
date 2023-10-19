/**
 * @file buzzer.c
 * @author Thomas Keilbach | keiltronic GmbH
 * @date 05 Oct 2023
 * @brief This file contains functions to communicate with the pheripherals
 * @version 2.0.0
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
void set_buzzer(BUZZER *buzzer)
{

  if (buzzer->status_old != buzzer->status)
  {
    switch (buzzer->status)
    {

    case BUZZER_OFF:

      pwm_set_pulse_dt(&sBuzzer, 0);
      System.StatusOutputs &= ~STATUSFLAG_BZ; // Create status entry
      break;

    case BUZZER_ON:

      pwm_set_dt(&sBuzzer, PWM_HZ(buzzer->frequency), ((PWM_HZ(buzzer->frequency) / 100) * buzzer->duty_cycle));
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
void buzzer_beep_function(BUZZER *buzzer)
{

  if (buzzer->beep_period_counter >= (buzzer->beep_on_time + buzzer->beep_off_time))
  {
    buzzer->beep_period_counter = 0UL;
    buzzer->beep_cycle_counter++;
  }
  else if (buzzer->beep_period_counter >= buzzer->beep_on_time)
  {
    buzzer->status = BUZZER_OFF;
    set_buzzer(buzzer);
  }
  else if (((buzzer->beep_period_counter >= 0UL) && (buzzer->beep_period_counter < buzzer->beep_on_time)))
  {
    buzzer->status = BUZZER_ON;
    set_buzzer(buzzer);
  }

  buzzer->beep_period_counter++;
}

/*!
 *  @brief This is the function description
 */
void buzzer_update(BUZZER *buzzer)
{

  if (buzzer->isBeeping == true)
  {

    if (buzzer->beep_cycles == 0UL)
    { // beep forever
      buzzer_beep_function(buzzer);
    }
    else
    { // just beep a few times

      if (buzzer->delay_timer > buzzer->delay)
      {
        if (buzzer->beep_cycle_counter < buzzer->beep_cycles)
        {
          buzzer_beep_function(buzzer);
        }
        else
        {
          buzzer->status = BUZZER_OFF;
          buzzer->beep_cycle_counter = 0UL;
          buzzer->delay_timer = 0UL;
          buzzer->isBeeping = false;
          set_buzzer(buzzer);
        }
      }
      else
      {
        buzzer->delay_timer++;
      }
    }
  }
}