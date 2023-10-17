/**
 * @file spi.c
 * @author Thomas Keilbach | keiltronic GmbH
 * @date 17 Oct 2023
 * @brief This file contains functions to communicate with the pheripherals
 * @version 2.0.0
 */

/*!
 * @defgroup Peripherals
 * @brief This file contains functions to communicate with the pheripherals
 * @{*/

#include "spi.h"

struct device *spi_dev;

struct spi_cs_control spim_cs1 = {
	.gpio = SPI_CS_GPIOS_DT_SPEC_GET(DT_NODELABEL(mt25ql256_0)),
	.delay = 0,
};

struct spi_cs_control spim_cs2 = {
	.gpio = SPI_CS_GPIOS_DT_SPEC_GET(DT_NODELABEL(mt25ql256_1)),
	.delay = 0,
};

struct spi_config spi_cfg1 = {
    .operation = SPI_WORD_SET(8) | SPI_OP_MODE_MASTER | SPI_TRANSFER_MSB | SPI_LINES_SINGLE | SPI_MODE_CPOL | SPI_MODE_CPHA,
    .frequency = 8000000,
	.slave = 0,
	.cs = &spim_cs1,
};

struct spi_config spi_cfg2 = {
    .operation = SPI_WORD_SET(8) | SPI_OP_MODE_MASTER | SPI_TRANSFER_MSB | SPI_LINES_SINGLE | SPI_MODE_CPOL | SPI_MODE_CPHA,
    .frequency = 8000000,
	.slave = 0,
	.cs = &spim_cs2,
};

void spi_init(void)
{
	spi_dev = DEVICE_DT_GET(DT_ALIAS(spi3));
	if(!device_is_ready(spi_dev)) {
		printk("SPI master device not ready!\n");
	}

	if(!device_is_ready(spim_cs1.gpio.port)){
		printk("SPI master chip select 1 device not ready!\n");
	}

    if(!device_is_ready(spim_cs2.gpio.port)){
		printk("SPI master chip select 2 device not ready!\n");
	}
}

void uninit_spi(void)
{
    spi_release(spi_dev, &spi_cfg1);
    spi_release(spi_dev, &spi_cfg2);
}
