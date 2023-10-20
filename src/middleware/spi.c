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

const struct device *spi_dev;

// struct spi_cs_control spim_cs = {
// 	.gpio = SPI_CS_GPIOS_DT_SPEC_GET(DT_ALIAS(spi3)),
// 	.delay = 0,
// };

struct spi_config spi_cfg = {
    .operation = SPI_WORD_SET(8) | SPI_OP_MODE_MASTER | SPI_TRANSFER_MSB | SPI_LINES_SINGLE | SPI_MODE_CPOL | SPI_MODE_CPHA,
    .frequency = 8000000,
    .slave = 0,
 //   .cs = &spim_cs,
};

void spi_init(void) {

  /* Init SPI */
  spi_dev = DEVICE_DT_GET(DT_ALIAS(spi3));

  if (spi_dev == NULL) {
      rtc_print_debug_timestamp();
      shell_error(shell_backend_uart_get_ptr(), "Could not get SPI_3 device");
    return;
  }

  // 	if(!device_is_ready(spim_cs.gpio.port)){
	// 	printk("SPI master chip select device not ready!\n");
	// }
}

void spi_deinit(void){
  spi_release(spi_dev, &spi_cfg);
}