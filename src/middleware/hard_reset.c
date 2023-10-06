/**
 * @file gpio.c
 * @author Thomas Keilbach | keiltronic GmbH
 * @date 06 Oct 2023
 * @brief This file contains functions to communicate the low voltage detector ic
 * @version 1.0.0
 */
#include "hard_reset.h"

struct gpio_dt_spec reset_switch = GPIO_DT_SPEC_GET_OR(DT_NODELABEL(reset_switch), gpios, {0});

/*!
 * @brief tbd
 * @note tbd
 */
void hard_reset_init(void)
{
	int err;

	if (!gpio_is_ready_dt(&reset_switch))
	{
		return;
	}

	err = gpio_pin_configure_dt(&reset_switch, GPIO_OUTPUT_INACTIVE);
	if (err != 0)
	{
		return;
	}
}