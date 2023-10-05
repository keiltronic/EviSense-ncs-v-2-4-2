/**
 * @file spi.c
 * @author Thomas Keilbach | keiltronic GmbH
 * @date 27 Oct 2022
 * @brief This file contains functions to communicate with the pheripherals
 * @version 1.0.0
 */

/*!
 * @defgroup Peripherals
 * @brief This file contains functions to communicate with the pheripherals
 * @{*/

#include "spi.h"

// struct device *spi_dev;

// struct spi_config spi_cfg = {
//     .operation = SPI_WORD_SET(8) | SPI_OP_MODE_MASTER | SPI_TRANSFER_MSB | SPI_LINES_SINGLE | SPI_MODE_CPOL | SPI_MODE_CPHA,
//     .frequency = 8000000,
//     .slave = 0,
// };

// void init_spi(void) {

//   /* Init SPI */
//   spi_dev = device_get_binding("SPI_3");

//   if (spi_dev == NULL) {
//     if (Parameter.debug) {
//       rtc_print_debug_timestamp();
//       shell_error(shell_backend_uart_get_ptr(), "Could not get SPI_3 device");
//     }
//     return;
//   }
// }

// void uninit_spi(void){
//   spi_release(spi_dev, &spi_cfg);
// }
