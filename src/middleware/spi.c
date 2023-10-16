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

struct device *spi_dev;
struct gpio_dt_spec cs1_pin = GPIO_DT_SPEC_GET(DT_ALIAS(cs1), gpios);
struct gpio_dt_spec cs2_pin = GPIO_DT_SPEC_GET(DT_ALIAS(cs2), gpios);

struct spi_config spi_cfg = {
    .operation = SPI_WORD_SET(8) | SPI_OP_MODE_MASTER | SPI_TRANSFER_MSB | SPI_LINES_SINGLE | SPI_MODE_CPOL | SPI_MODE_CPHA,
    .frequency = 8000000,
    .slave = 0,
};

void init_spi(void)
{
    int16_t ret = 0;

    /* Init SPI */
   	spi_dev = DEVICE_DT_GET(DT_ALIAS(spi3));
	if (!device_is_ready(spi_dev)) {
		printk("SPI device %s is not ready\n", spi_dev->name);
		return 0;
	}

    if (spi_dev == NULL)
    {
        rtc_print_debug_timestamp();
        shell_error(shell_backend_uart_get_ptr(), "Could not get SPI_3 device");

        return;
    }

    /* Init chip select gpio pins */
    if (!device_is_ready(cs1_pin.port))
        printk("Could not initialize cs1_pin!\n\r");

    ret = gpio_pin_configure_dt(&cs1_pin, GPIO_OUTPUT_ACTIVE);
    if (ret < 0)
        printk("Could not configure cs1_pin!\n\r");

    if (!device_is_ready(cs2_pin.port))
        printk("Could not initialize cs2_pin!\n\r");

    ret = gpio_pin_configure_dt(&cs2_pin, GPIO_OUTPUT_ACTIVE);
    if (ret < 0)
        printk("Could not configure cs2_pin!\n\r");
}

void uninit_spi(void)
{
    spi_release(spi_dev, &spi_cfg);
}
