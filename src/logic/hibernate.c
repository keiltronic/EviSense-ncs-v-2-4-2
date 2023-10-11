/**
 * @file hibernate.c
 * @author Thomas Keilbach | keiltronic GmbH
 * @date 08 Mar 2023
 * @brief This files contains function to set the device in hibernate mode (deep sleep)
 * @version 1.0.0
 */

/*!
 * @defgroup Events
 * @brief This files contains function to set the device in hibernate mode (deep sleep)
 * @{*/

#include "hibernate.h"

bool hibernation_mode = false;

void enter_hibernate(void)
{
    hibernation_mode = true;
    System.StatusInputs |= STATUSFLAG_B2;

    /* Notify user with beep to release the button */
    Notification.next_state = NOTIFICATION_HIBERNATE;

    rfid_power_off();
    imu_enter_sleep();
   
    /* Disable all GPIO pins */
  //  for (int i = 0; i < 32; i++)
  //  {
  //      gpio_pin_configure(gpio_dev, i, GPIO_DISCONNECTED);
  //  }

    /* Set user button to as wake up trigger pin */
    nrf_gpio_cfg_input(WAKEUP_PIN, NRF_GPIO_PIN_PULLUP); // Due to errata 4, Always configure PIN_CNF[n].INPUT before PIN_CNF[n].SENSE.
    nrf_gpio_cfg_sense_set(WAKEUP_PIN, NRF_GPIO_PIN_SENSE_LOW);

    uint32_t cnf = NRF_P0_NS->PIN_CNF[WAKEUP_PIN];
    NRF_P0_NS->PIN_CNF[WAKEUP_PIN] = cnf | (GPIO_PIN_CNF_SENSE_Low << GPIO_PIN_CNF_SENSE_Pos);
    // printk("PIN_CNF register for pin %d is %d \n", WAKEUP_PIN, NRF_P0_NS->PIN_CNF[WAKEUP_PIN]);

  //  lte_lc_power_off();               // Send "AT+CFUN=0"
   // bsd_shutdown();                   // Method to gracefully shutdown the BSD library.
   nrf_modem_lib_shutdown();
    NRF_REGULATORS_NS->SYSTEMOFF = 1; // https://devzone.nordicsemi.com/f/nordic-q-a/47881/how-to-put-nrf9160-dk-in-deep-sleep-mode
}