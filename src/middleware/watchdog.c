/**
 * @file watchdog.c
 * @author Thomas Keilbach | keiltronic GmbH
 * @date 07 Oct 2023
 * @brief This file contains functions to communicate with the pheripherals
 * @version 2.0.0
 */

/*!
 * @defgroup Peripherals
 * @brief This file contains functions to communicate with the pheripherals
 * @{*/
#include "watchdog.h"

int16_t wdt_channel_id;
//static struct device *wdt;
static struct wdt_timeout_cfg wdt_config;

const struct device *const wdt = DEVICE_DT_GET(DT_ALIAS(watchdog0));

/*!
 *  @brief This is the function description
 */
void wdt_reset(void)
{
  wdt_feed(wdt, wdt_channel_id);
}

/*!
 *  @brief This is the function description
 */
void wdt_callback(struct device *wdt_dev, int channel_id)
{
  static bool handled_event;

  if (handled_event)
  {
    return;
  }

  wdt_feed(wdt_dev, channel_id);

  printk("Handled things..ready to reset\n");
  handled_event = true;
}

/*!
 *  @brief This is the function description
 */
void wdt_init(void)
{
  int16_t err = 0;

 // wdt = device_get_binding("WDT");

	if (!device_is_ready(wdt)) {
		printk("%s: device not ready.\n", wdt->name);
		return 0;
	}

  /* Reset SoC when watchdog timer expires. */
  wdt_config.flags = WDT_FLAG_RESET_SOC;

  /* Expire watchdog after 10000 milliseconds. */
  wdt_config.window.min = 0U;
  wdt_config.window.max = 10000U;

  /* Set up watchdog callback. Jump into it when watchdog expired. */
  wdt_config.callback = wdt_callback;

  wdt_channel_id = wdt_install_timeout(wdt, &wdt_config);

  if (wdt_channel_id < 0)
  {
    printk("Watchdog install error\n");
    return;
  }

  //err = wdt_setup(wdt, 0);
  err = wdt_setup(wdt, WDT_OPT_PAUSE_HALTED_BY_DBG);
  if (err < 0)
  {
    printk("Watchdog setup error\n");
    return;
  }

  wdt_reset();

  ///* Feeding watchdog. */
  //	printk("Feeding watchdog %d times\n", WDT_FEED_TRIES);
  //	for (int i = 0; i < WDT_FEED_TRIES; ++i) {
  //		printk("Feeding watchdog...\n");
  //		wdt_feed(wdt, wdt_channel_id);
  //		k_sleep(K_MSEC(50));
  //	}
}