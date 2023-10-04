/**
 * @file stepdetection.c
 * @author Thomas Keilbach | keiltronic GmbH
 * @date 27 Oct 2022
 * @brief This file contains functions to communicate with the pheripherals
 * @version 1.0.0
 */

/*!
 * @defgroup Peripherals
 * @brief This file contains functions to communicate with the pheripherals
 * @{*/

#include "stepdetection.h"

bool step_sequence_running = false;
uint8_t step_sequence_post_timer = 0;
uint8_t step_sequence_pre_timer = 0;
uint16_t step_count_old = 0;

void step_sequence_detection(void) {
  if (!step_sequence_running && (System.Steps >= STEP_SEQUENCE_MIN_STEPS)) {
    step_sequence_running = true;
    if (Parameter.stepdetection_verbose || Parameter.debug) {
    rtc_print_debug_timestamp();
      shell_print(shell_backend_uart_get_ptr(), "Step sequence started");
    }
  } else {
    if (step_count_old == System.Steps) {
      if (step_sequence_pre_timer >= STEP_SEQUENCE_RESET_DELAY) {
        step_sequence_pre_timer = 0;
        System.Steps = 0;
      } else {
        step_sequence_pre_timer++;
      }
    } else {
      step_sequence_pre_timer = 0;
    }
  }

  if (step_sequence_running && (step_count_old == System.Steps)) {
    step_sequence_post_timer++;
  } else {
    step_sequence_post_timer = 0;
  }

  /* Running step sequence finished (no further steps in the past seconds detected) */
  if (step_sequence_running && (step_sequence_post_timer >= STEP_SEQUENCE_RESET_DELAY)) {
    if (Parameter.stepdetection_verbose || Parameter.debug) {
    rtc_print_debug_timestamp();
      shell_print(shell_backend_uart_get_ptr(), "Step sequence finished");
    }
    step_sequence_running = false;
    step_sequence_post_timer = 0;
    System.Steps = 0;
  }

  step_count_old = System.Steps;
}